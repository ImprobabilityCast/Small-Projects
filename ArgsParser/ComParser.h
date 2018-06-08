#ifndef COMPARSER_H
#define COMPARSER_H

#include <cstring>
#include <windows.h>
#include <algorithm>
#include <stdexcept>

///\brief Class for parsing command line args
class ComParser{
private:
    int argcb;
    char** argvb;
    void Process(char* str);
public:
    ComParser();
    ComParser(char* argStr);
    ~ComParser();
    ///\brief Gets the command line argument specified by index
    const char* GetArgAt(int index);
    ///\brief Gets the number of command line arguments
    ///\return The number of command line arguments
    int GetArgc();
};

#endif // COMPARSER_H
