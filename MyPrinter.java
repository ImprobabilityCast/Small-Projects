import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.print.Printable;
import java.awt.print.PageFormat;
import java.io.*;
import java.util.Deque;
import java.util.ArrayDeque;

class MyPrinter implements Printable {

    private static int LINE_HEIGHT = 12;

    /**
     * Index of the last page in the current file.
     */
    private int lastPage;

    /**
     * Keep track of a page during mutiple calls to print
     * with the same index.
     */
    private int previousIndex;
    private Deque<String> previousPage;

    /**
     * Holds all the lines of the current file.
     */
    private Deque<String> lines;

    /**
     * List of files to print.
     */
    private Deque<String> fileNames;

    /*
     * Constructors
     */

    public MyPrinter(String... files) {
        this.lastPage = -1;
        this.previousIndex = -1;
        this.lines = new ArrayDeque<>();
        this.fileNames = new ArrayDeque<>();
        this.previousPage = new ArrayDeque<>();
        for (String f : files) {
            this.fileNames.addLast(f);
        }
    }

    /**
     * Reads the file specified by fileName into buffer.
     */
    private static void fillBuffer(Deque<String> buffer, String fileName) {
        BufferedReader reader = null;
        try {
            reader = new BufferedReader(new FileReader(fileName));
            String line = reader.readLine();
            while (line != null) {
                buffer.addLast(line);
                line = reader.readLine();
            }
            reader.close();
        } catch (IOException e) {
            System.err.println(e.getMessage());
            if (reader != null) {
                try {
                    reader.close();
                } catch (IOException e2) {
                    System.err.println(e2.getMessage());
                }
            }
        }
    }

    @Override
    public int print(Graphics g, PageFormat pf, int pageIndex) {
        Graphics2D g2d = (Graphics2D) g;
        g2d.translate(pf.getImageableX(), pf.getImageableY());
        Deque<String> page;

        if (pageIndex == this.previousIndex) {
            page = this.previousPage;
        } else {
            page = this.lines;
            this.previousPage.clear();
            this.previousIndex = pageIndex;

            if (pageIndex > this.lastPage) {
                if (this.fileNames.size() > 0) {
                    MyPrinter.fillBuffer(this.lines, this.fileNames.pollFirst());
                    this.lastPage += ((this.lines.size() * MyPrinter.LINE_HEIGHT)
                            / pf.getImageableHeight()) + 1;
                } else {
                    return Printable.NO_SUCH_PAGE;
                }
            }
        }

        int pos = MyPrinter.LINE_HEIGHT;
        Deque<String> holder = new ArrayDeque<>();
        while (page.size() > 0 && pos < pf.getImageableHeight()) {
            String s = page.pollFirst();
            holder.addLast(s);
            g2d.drawString(s, 0, pos);
            pos += MyPrinter.LINE_HEIGHT;
        }
        this.previousPage = holder;

        return Printable.PAGE_EXISTS;
    }
}
