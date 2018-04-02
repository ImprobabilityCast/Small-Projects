# Script to fill a directory with hard links of each file in
# another directory.
#!/bin/bash

if [ $# -ne 2 ]; then
	printf "Script to fill a directory with hard links of each"
	printf " file in another directory.\n\n"
	printf "Usage:\t ./dlink.sh SRC DEST\n"
	printf "\t Where SRC is the directory full of files to link\n"
	printf "\t And DEST is the directory to fill with links\n"
	exit 1
else
	SRC_DIR="$1"
	END_POS=$(( ${#SRC_DIR} - 1 ))
	if [ ${SRC_DIR:END_POS:1} != "/" ]; then
		SRC_DIR="$SRC_DIR/"
	fi
	
	DEST_DIR="$2"
	END_POS=$(( ${#DEST_DIR} - 1 ))
	if [ ${DEST_DIR:END_POS:1} != "/" ]; then
		DEST_DIR="$DEST_DIR/"
	fi
	
	printf "Creating symbolic links.\n"
	for FILE in $SRC_DIR*; do
		if [[ -f $FILE ]]; then
			ln -vs "$FILE" "$DEST_DIR"
		fi
	done
fi
