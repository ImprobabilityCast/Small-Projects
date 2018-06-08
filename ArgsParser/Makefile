SHELL:=/bin/bash

bin/testMain.exe: obj/testMain.o bin/libComParser.a
	@mkdir -p $(@D)
	g++ -s -shared-libgcc -o bin/testMain.exe obj/testMain.o -L./bin -lComParser

bin/libComParser.a: obj/libComParser.o
	@mkdir -p $(@D)
	ar rcs bin/libComParser.a obj/libComParser.o

obj/libComParser.o: ComParser.cpp ComParser.h
	@mkdir -p $(@D)
	g++ -O2 -std=c++11 -c ComParser.cpp -o obj/libComParser.o
	
obj/testMain.o: testMain.cpp
	@mkdir -p $(@D)
	g++ -O2 -std=c++11 -c testMain.cpp -o obj/testMain.o
	
.PHONY: clean
clean:
	if [ -f obj/testMain.o ]; then rm ./obj/testMain.o; fi
	if [ -f obj/libComParser.o ]; then rm ./obj/libComParser.o; fi
	if [ -f bin/testMain.exe ]; then rm ./bin/testMain.exe; fi
	if [ -f bin/libComParser.a ]; then rm ./bin/libComParser.a; fi
	if [ -d obj ]; then rmdir ./obj; fi
	if [ -d bin ]; then rmdir ./bin; fi
