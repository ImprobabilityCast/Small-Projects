<?php
// Script to fill a directory with symbolic links of each file in
// another directory.

if ($argc != 3) {
	print("Script to fill a directory with symbolic links of each");
	print(" file in another directory.\n\n");
	print("Usage:\t php dlink.php SRC DEST\n");
	print("\t Where SRC is the directory full of files to link (Must be absolute)\n");
	print("\t And DEST is the directory to fill with links (Must be absolute)\n");
} else {
	$SRC_DIR = str_replace("/", "\\", $argv[1]);
	$SRC_DIR = try_add_end_slash($SRC_DIR);
	
	$DEST_DIR = str_replace("/", "\\", $argv[2]);
	$DEST_DIR = try_add_end_slash($DEST_DIR);

	print("Creating symbolic links. Path to directories must be absolute!\n");
	foreach (scandir($SRC_DIR) as $FILE) {
		if (!is_dir($FILE)) {
			print(shell_exec("mklink ". escapeshellarg("$DEST_DIR$FILE")
				. " " . escapeshellarg("$SRC_DIR$FILE")));
		}
	}
}

function try_add_end_slash($str) {
	$last = strlen($str) - 1;
	if (strrpos($str, '\\') != $last) {
		$str = $str . '\\';
	}
	return $str;
}

?>
