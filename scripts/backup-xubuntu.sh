#!/bin/bash
# script to backup home directory
XUBUNTU_ARC=xubuntu-$(date +%F-%H-%M).tar.gz
XUBUNTU_DIR="/home/xubuntu"
EXCLUSIONS=(
	--exclude=*.o
	--exclude=$XUBUNTU_ARC
	--exclude=$XUBUNTU_DIR/.local/share/Trash
	--exclude=$XUBUNTU_DIR/.thumbnails
	--exclude=$XUBUNTU_DIR/.cache
	--exclude=$XUBUNTU_DIR/.dbus
	--exclude=$XUBUNTU_DIR/.config/xpad/server
	--exclude=$XUBUNTU_DIR/.config/Mousepad
	--exclude=$XUBUNTU_DIR/Downloads/*.gz
	--exclude=$XUBUNTU_DIR/Downloads/*.zip
	--exclude=$XUBUNTU_DIR/Downloads/*.deb
)
XUBUNTU_ARC=xubuntu-$(date +%F-%H-%M).tar.gz

printf "Backup of '$XUBUNTU_DIR' will begin momentarily.\n\n"
echo "tar -I/usr/bin/pigz ${EXCLUSIONS[*]} -cpvf $XUBUNTU_ARC $XUBUNTU_DIR"
sleep 2s

tar -I/usr/bin/pigz ${EXCLUSIONS[*]} -cpvf $XUBUNTU_ARC $XUBUNTU_DIR

printf "Backup complete.\n\n"
sleep 2s