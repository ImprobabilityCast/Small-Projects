#!/bin/bash
EX=./target/debug/recover-from-failure
DEVICE="/dev/mmcblk0p1"
cargo build
sudo chown root $EX && sudo chmod a+s $EX
$EX resume $DEVICE
source ./change_date.bash
