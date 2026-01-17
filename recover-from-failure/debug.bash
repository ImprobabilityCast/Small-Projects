#!/bin/bash
EX=./target/debug/recover-from-failure
cargo build
sudo chown root $EX && sudo chmod a+s $EX
$EX "/dev/mmcblk0p1"
sudo umount "/dev/mmcblk0p1"

