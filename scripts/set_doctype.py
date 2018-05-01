from sys import argv
from os import listdir

def print_help():
    print(
"""Replaces the first line of every *.html file in a directory with '<!DOCTYPE html>'
USAGE: set_doctype.py DIRECTORY ... [ DIRECTORY ]

DIRECTORY: place full of files to modify""")

def main():
    if (len(argv) < 2):
        print_help()
    else:
        for place in argv[1:]:
            if place[-1] != '\\' or place[-1] != '/':
                place += '/'
            dir_list = listdir(place)
            i = 1
            for name in dir_list:
                if name.lower().endswith(".html"):
                    print("processing", place, " file : ", i, end='\r')
                    i += 1
                    with open(place + name, "rb+") as f:
                        f.readline()
                        buf = b'<!DOCTYPE html>\n' + f.read()
                        f.seek(0)
                        f.write(buf)
                        f.truncate()


if __name__ == "__main__":
    main()
