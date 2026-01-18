#!/bin/bash

echo "copying last modified dates..."

SRC_DIR=./tmp_mnt/
DST_DIR=./out
sudo mount $DEVICE $SRC_DIR

for src in "$SRC_DIR"DCIM/**/*; do
    [ -f "$src" ] || continue

    base=$(basename "$src")
    dst="$DST_DIR/$base"
    if [ -f "$dst" ]; then
        sudo touch -r "$src" "$dst"
    fi
done
sudo umount $DEVICE
