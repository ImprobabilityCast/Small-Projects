#!/bin/bash
# my script to backup my http server's configuration and
# document root

DIRECTORIES="/var/www/html /etc/apache2"
ARCHIVE=http-$(date +%F-%H-%M).tar.gz

COUNTER=7
while [ $COUNTER -gt 0 ]; do
	printf "Backup of all the content in $DIRECTORIES will start in $COUNTER"
	sleep 1s
	printf "\33[2K\r"
	let COUNTER=COUNTER-1
done

sudo -u xubuntu tar -I/usr/bin/pigz -vlpcf /home/xubuntu/Desktop/$ARCHIVE $DIRECTORIES

printf "\nBackup complete.\n"
sleep 2s