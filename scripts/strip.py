import sys
import re
import os

def main ():
	# remove '/* */' comments
	stripMultiLine = re.compile("/\*(.|[\n\r]+\s+\*)+")
	# remove '//' comments
	stripSingleLine = re.compile("(\r?\n{1}(\t| )*//)(.*[a-z0-9]i*|[,\.':])*")
	# convert tabs to 4 spaces
	tabs = re.compile("\t")
	stripLines = re.compile("\n\s*\n");

	conversionList = []
	
	if len(sys.argv) == 2 and sys.argv[1][:2] == "*.":
		ext = re.compile(".*\." + sys.argv[1][2:])
		for f in os.scandir():
			if ext.match(f.name) != None:
				conversionList.append(f.name)
	else:
		conversionList = sys.argv[1:]
	
	for name in conversionList:
		dot = name.rfind(".")
		outName = name[:dot] + "NoComments" + name[dot:];
		print("Converting: " + name + " to " + outName)

		with open(name, "r") as f:
			txt = f.read()
			txt = stripMultiLine.sub("", txt)
			txt = stripSingleLine.sub("", txt)
			txt = stripLines.sub("\n", txt)
			txt = tabs.sub("    ", txt)
			f.seek(0)
			with open(outName, "w") as out:
				out.write(txt)


if __name__ == "__main__":
	main()