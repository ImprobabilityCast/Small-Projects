CXX = g++.exe
AR = ar.exe
LD = g++.exe
WINDRES = windres.exe

CFLAGS = -Wall -fexceptions
LDFLAGS = -s -flto -Os -Wl,--gc-sections

OBJDIR = obj

OBJ = $(OBJDIR)\\res.o $(OBJDIR)\\main.o
OUT = bin\\beep.exe


$(OUT): $(OBJ)
	cmd /c if not exist bin md bin
	$(LD) -o $(OUT) $(OBJ) $(LDFLAGS) -mwindows

$(OBJDIR)\\res.o: res.rc
	cmd /c if not exist $(OBJDIR) md $(OBJDIR)
	$(WINDRES) -i res.rc -J rc -o $(OBJDIR)\\res.o -O coff

$(OBJDIR)\\main.o: main.cpp
	cmd /c if not exist $(OBJDIR) md $(OBJDIR)
	$(CXX) $(CFLAGS) -c main.cpp -o $(OBJDIR)\\main.o

clean:
	cmd /c del /f $(OBJ) $(OUT)

.PHONY: clean
