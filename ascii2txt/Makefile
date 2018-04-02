CXXFLAGS = -std=c++11 -Wall
LDFLAGS = -Os -shared-libgcc

ascii2txt.exe: main.o
	g++ $(CXXFLAGS) $(LDFLAGS) main.cpp -o ascii2txt.exe
	
main.o: main.cpp
	g++ $(CXXFLAGS) -c main.cpp

.PHONY: clean

clean:
	rm -vf main.o
