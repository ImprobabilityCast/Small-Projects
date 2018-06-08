#include "ComParser.h"

#define DEBUG
#ifdef DEBUG
    #include <iostream>
#endif // DEBUG

#include <stdio.h>

ComParser::ComParser() {
    Process(GetCommandLineA());
}

ComParser::ComParser(char* argStr) {
    Process(argStr);
}

void ComParser::Process(char* str) {
    //count space chars
    size_t index=0;
    size_t siz=strlen(str);
    size_t cnt=0;
    while (index < siz) {
        if (str[index] == '"') {
            //skip whitespaces inside quotes
            do { index++; }
            while (index < siz && str[index] != '"');
            index++;
            //count whitespaces outside quotes
            if (index < siz && str[index] == ' ') {
                //if there is more than one whitespace char between args
                // only count it as one
                cnt++;
                while (index < siz && str[index] == ' ') {
                    str[index] = '\xff';
                    index++;
                }
            }
        } else {
            if (str[index] == ' ') {
                cnt++;
                while(index < siz && str[index] == ' ') {
                    str[index] = '\xff';
                    index++;
                }
            }
            index++;
        }
    }
    argcb = cnt+1;
    argvb = new (std::nothrow) char*[argcb];
    if (argvb == nullptr) {
        MessageBoxA(NULL,
                    "Memory could not be allocated for command line parser",
                     NULL, MB_OK|MB_ICONERROR|MB_APPLMODAL);
        std::terminate();
    }
    //size_t pos=0;
    argvb[0] = strtok(str, "\xff");
    for (int i=1; i<argcb; i++)
        argvb[i] = strtok(NULL, "\xff");
    #ifdef DEBUG
    for (int i=0; i<argcb; i++) {
        printf(argvb[i]);
        printf("\n");
    }
    #endif
}

ComParser::~ComParser() {
    delete[] argvb;
}

///\brief Gets the command line argument specified by index
const char* ComParser::GetArgAt(int index) {
    if (index > argcb)
        throw std::out_of_range("index out of bounds");
    if (argvb[index][0] == '"') {
    #ifdef DEBUG
        std::cout << argvb[index] << std::endl;
        argvb[index][strlen(argvb[index])-1]='\0';
        std::cout << argvb[index] << std::endl;
        argvb[index]++;
        std::cout << argvb[index] << std::endl;
    #else
        argvb[index][strlen(argvb[index])-1]='\0';
        argvb[index]++;
    #endif // DEBUG
    }
    return argvb[index];
}

///\brief Gets the number of command line arguments
///\return The number of command line arguments
int ComParser::GetArgc() { return argcb; }
