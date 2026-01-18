// attempt to detect corruption by checking for 512 byte aligned chunks of all zeros

extern crate sys_mount;

use std::sync::mpsc::{self, SyncSender, Sender, Receiver};
use std::sync::atomic::{AtomicI64, Ordering};
use std::sync::{Arc};
use std::{thread, time};
use std::env::*;
use std::fs::{File, DirEntry, OpenOptions, read_dir, create_dir_all};
use std::io::{self, BufReader, StdoutLock};
use std::io::prelude::*;
use std::io::{SeekFrom};
use std::iter::{self};
use std::path::{PathBuf};

use sys_mount::*;


const CHUNKSIZE: u64 = 512;
const MAX_ATTEMPTS: u64 = 5;
// Limit the memory the program uses
const MAX_WORKER_QUEUE_SIZE: usize = 1_000_000;
const MAX_WRITER_QUEUE_SIZE: usize = 1_000_000;

enum TaskMsg {
    Norm(IOTask),
    End,
}

#[derive(Debug)]
struct IOTask {
    attempts: u64,
    file: PathBuf,
    chunks: Vec<FileChunk>,
}

#[derive(Debug)]
struct FileChunk {
    offset: u64,
    len: u64,
    data: Vec<u8>,
}

#[derive(Clone, Debug)]
struct RecoveryInfo {
    tmp_dir: PathBuf,
    out_dir: PathBuf,
    device: PathBuf,
    progress_counter: Arc::<AtomicI64>,
    resume: bool,
}

impl FileChunk {
    fn no_data_clone(&self) -> FileChunk {
        FileChunk {
            offset: self.offset,
            len: self.len,
            data: Vec::<u8>::new(),
        }
    }

    fn all_zeros(&self) -> bool {
        self.data.iter().all(|&b| b == 0)
    }
}

impl IOTask {
    fn new(file: DirEntry) -> IOTask {
        let meta = file.metadata().unwrap();
        IOTask {
            attempts: 0,
            file: file.path(),
            chunks: vec![FileChunk { offset: 0, len: meta.len(), data: Vec::<u8>::new() }],
        }
    }
}

fn try_read(progress_counter: &Arc<AtomicI64>, task: &mut IOTask, stdout: &mut StdoutLock) -> io::Result<()> {
    let mut new_chunks = Vec::<FileChunk>::new();
    let mut f = File::open(&mut task.file)?;
    for chunk in &task.chunks {
        //println!("{:?}", chunk);
        let num_chunks = chunk.len / CHUNKSIZE + u64::from((chunk.len % CHUNKSIZE) > 0);
        progress_counter.fetch_add((num_chunks - 1) as i64, Ordering::Relaxed);
        let mut buf = vec![vec![0u8; CHUNKSIZE as usize]; num_chunks as usize];
        f.seek(SeekFrom::Start(chunk.offset))?;
        let mut reader = f.take(chunk.len);
        // split file into chunks
        let mut idx = 0;
        writeln!(stdout, "\nreading file: {} len: {}MiB", task.file.to_str().unwrap(), num_chunks * CHUNKSIZE / (1024 * 1024))?;
        while let Ok(_) = reader.read(&mut buf[idx]) && idx + 1 < buf.len() {
            idx += 1;
        }
        write!(stdout, "{} - attempt: {} slices: {:8} left: {:12}\r", task.file.to_str().unwrap(), task.attempts, buf.len(), progress_counter.load(Ordering::Relaxed))?;

        let mut idx = u64::MAX;
        new_chunks.extend(buf.drain(..).map(|b| {
            idx = idx.wrapping_add(1);
            FileChunk {
                offset: idx * CHUNKSIZE + chunk.offset,
                len: b.len() as u64,
                data: b,
            }
        }));
        f = reader.into_inner();
    }
    //println!("new_chunks: {}", new_chunks.len());
    task.chunks = new_chunks;
    Ok(())
}

fn file_list(starting_dir: &PathBuf) -> impl iter::Iterator<Item = DirEntry> {
    read_dir(starting_dir).unwrap().flat_map(|p| {
        let present = p.unwrap();
        if present.file_type().unwrap().is_dir() {
            file_list(&present.path()).collect::<Vec<DirEntry>>()
        } else {
            vec![present]
        }
    })
}

fn worker(info: RecoveryInfo, rx: Receiver<TaskMsg>, read_tx: Sender<IOTask>, write_tx: SyncSender<TaskMsg>) {
    for msg in rx {
        match &msg {
            TaskMsg::Norm(io_task) => {
                // retry large swathes of zeros
                let retry_task = IOTask {
                    attempts: io_task.attempts + 1,
                    file: io_task.file.clone(),
                    chunks: io_task.chunks.iter().filter_map(|c| {
                        if c.all_zeros() {
                            Some(c.no_data_clone())
                        } else {
                            None
                        }
                    }).collect::<Vec<FileChunk>>(),
                };
                let attempts = retry_task.attempts;
                let num_retry_chunks = retry_task.chunks.len();
                //eprintln!("found {}", (io_task.chunks.len() - num_retry_chunks));
                info.progress_counter.fetch_sub((io_task.chunks.len() - num_retry_chunks) as i64, Ordering::Relaxed);
                if num_retry_chunks > 0 && let Err(e) = read_tx.send(retry_task) {
                    if attempts < MAX_ATTEMPTS {
                        eprintln!("{}", e.to_string());
                    }
                    break;
                }
                if let Err(e) = write_tx.send(msg) {
                    eprintln!("{}", e.to_string());
                    break;
                }
            },
            TaskMsg::End => break,
        }
    }

    println!("worker thread exiting");
    let _ = write_tx.send(TaskMsg::End);
}

fn remount(info: &RecoveryInfo, mounty: &mut Mount) -> io::Result<()> {
    mounty.unmount(UnmountFlags::empty())?;
    std::thread::sleep(time::Duration::from_secs(5)); // wait in case remounting quickly doesn't work
    *mounty = Mount::builder().mount(&info.device, &info.tmp_dir)?;
    Ok(())
}

fn read_thread(info: RecoveryInfo, rx: Receiver<IOTask>, worker_tx: SyncSender<TaskMsg>, mut mounty: Mount) {
    let mut attempt_state: u64 = 0;
    let mut stdout_lock = io::stdout().lock();
    for mut io_task in rx {
        if io_task.attempts > attempt_state {
            attempt_state += 1;
            if attempt_state >= MAX_ATTEMPTS {
                break;
            }
            if let Err(e) = remount(&info, &mut mounty) {
                eprintln!("{}", e.to_string());
                break;
            }
        }
        if let Err(e) = try_read(&info.progress_counter, &mut io_task, &mut stdout_lock) {
            eprintln!("failed to read: {:?}", io_task.file);
            eprintln!("{}", e.to_string());
        }
        // Blocks if worker queue is full
        if let Err(e) = worker_tx.send(TaskMsg::Norm(io_task)) {
            eprintln!("{}", e.to_string());
            break;
        }
    }
    if let Err(e) = mounty.unmount(UnmountFlags::empty()) {
        eprintln!("{}", e.to_string());
    }
    let _ = write!(stdout_lock, "\nread thread exiting\n");

    let _ = worker_tx.send(TaskMsg::End);
}

fn write_thread(info: RecoveryInfo, rx: Receiver<TaskMsg>) {
    let mut sleep_counter = 0;
    for task in rx {
        match task {
            TaskMsg::Norm(mut io_task) => {
                let mut new_path = info.out_dir.clone();
                new_path.push(io_task.file.file_name().unwrap());
                io_task.file = new_path;
                let write_zeros = !io_task.file.as_path().exists() && io_task.attempts == 0;

                if let Ok(mut f) = OpenOptions::new().create(true).write(true).open(io_task.file) {
                    for chunk in io_task.chunks.iter().filter(|c| { write_zeros || !c.all_zeros() }) {
                        if let Err(e) = f.seek(SeekFrom::Start(chunk.offset)) {
                            eprintln!("{}", e.to_string());
                        } else if let Err(e) = f.write(&chunk.data) {
                            eprintln!("{}", e.to_string());
                        }
                    }
                }
            },
            TaskMsg::End => break,
        }
        // let me still use my pc
        sleep_counter = (sleep_counter + 1) % 100;
        if sleep_counter == 0 {
            thread::sleep(time::Duration::from_secs(2));
        }
    }
    println!("write thread exiting");
}

fn start_from_scratch(info: &RecoveryInfo) -> Vec<IOTask> {
    file_list(&info.tmp_dir).filter_map(|f| {
        if let Some(s) = f.path().extension() {
            let slow = std::ffi::OsStr::to_ascii_lowercase(s);
            if slow == "jpg" || slow == "mov" {
                return Some(IOTask::new(f))
            }
        }
        None
    }).collect::<Vec<_>>()
}

impl RecoveryInfo {
    fn new(device: PathBuf, resume: bool) -> RecoveryInfo {
        RecoveryInfo {
            tmp_dir: PathBuf::from("./tmp_mnt"),
            out_dir: PathBuf::from("./out"),
            device: device,
            progress_counter: Arc::new(AtomicI64::new(0)),
            resume: resume,
        }
    }

    fn start(info: RecoveryInfo) -> io::Result<()> {
        let (read_tx, read_rx) = mpsc::channel::<IOTask>();
        let (to_worker_tx, to_worker_rx) = mpsc::sync_channel::<TaskMsg>(MAX_WORKER_QUEUE_SIZE);
        let (write_tx, write_rx) = mpsc::sync_channel::<TaskMsg>(MAX_WRITER_QUEUE_SIZE);

        println!("{:?}", info);
    
        create_dir_all(&info.out_dir)?;
        create_dir_all(&info.tmp_dir)?;
        let mounty = Mount::builder().mount(&info.device, &info.tmp_dir)?;

        // intial pass
        for io_task in if info.resume {
            resume(&info)
        } else {
            start_from_scratch(&info)
        } {
            info.progress_counter.fetch_add(io_task.chunks.len() as i64, Ordering::Relaxed);
            read_tx.send(io_task).unwrap();
        }

        if info.progress_counter.load(Ordering::Relaxed) > 0 {
            let writer_info = info.clone();
            let worker_info = info.clone();
            let read_info = info.clone();
            
            let writer_th = thread::spawn(move || write_thread(writer_info, write_rx));
            let worker_th = thread::spawn(move || worker(worker_info, to_worker_rx, read_tx, write_tx));
            let read_th = thread::spawn(move || read_thread(read_info, read_rx, to_worker_tx, mounty));

            writer_th.join().unwrap();
            worker_th.join().unwrap();
            read_th.join().unwrap();
        }

        Ok(())
    }
}

fn resume(recovery: &RecoveryInfo) -> Vec<IOTask> {
    let file_list_vec = file_list(&recovery.tmp_dir).collect::<Vec<DirEntry>>();
    println!("Resuming...");
    read_dir(&recovery.out_dir).unwrap().filter_map(move |p| {
        let present = p.unwrap();
        let meta = present.metadata().unwrap();
        if let Ok(f) = OpenOptions::new().read(true).open(present.path()) {
            let mut reader = BufReader::new(f);
            let num_chunks = meta.len() / CHUNKSIZE + u64::from((meta.len() % CHUNKSIZE) > 0);
            let mut buf = vec![vec![0u8; CHUNKSIZE as usize]; num_chunks as usize];

            let mut idx = 0;
            while reader.read(&mut buf[idx]).is_ok() && idx + 1 < buf.len() {
                idx += 1;
            }

            let mut counter = -(CHUNKSIZE as i64);
            let io_task = IOTask {
                attempts: 0,
                file: file_list_vec.iter().find(|d| d.file_name() == present.file_name()).unwrap().path(),
                chunks: buf.drain(..).filter_map(|c| {
                    counter += CHUNKSIZE as i64;
                    let fc = FileChunk { offset: counter as u64, len: c.len() as u64, data: c };
                    if fc.all_zeros() {
                        Some(fc.no_data_clone())
                    } else {
                        None
                    }
                }).collect::<_>(),
            };
            if io_task.chunks.len() > 0 {
                return Some(io_task);
            }
        }
        None
    }).collect::<Vec<_>>()
}

fn main() {
    let device = PathBuf::from(args().last().unwrap());
    let resume = args().len() > 2 && args().nth(1).unwrap() == "resume";
    let recovery = RecoveryInfo::new(device, resume);

    if let Err(err) = RecoveryInfo::start(recovery) {
        println!("{}", "could not start");
        eprintln!("{}", err.to_string());
    }
}
