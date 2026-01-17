#!/bin/bash
EX=./target/release/recover-from-failure
cargo build -r
sudo chown root $EX && sudo chmod a+s $EX
$EX "/dev/mmcblk0p1"
sudo umount "/dev/mmcblk0p1"

