#!/bin/bash
EX=./target/release/recover-from-failure
DEVICE="/dev/mmcblk0p1"
cargo build -r
sudo chown root $EX && sudo chmod a+s $EX
$EX resume $DEVICE
source ./change_date.bash
