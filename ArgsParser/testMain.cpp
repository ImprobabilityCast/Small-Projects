#if defined(UNICODE) && !defined(_UNICODE)
    #define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
    #define UNICODE
#endif

//#define _WIN32_WINNT 0x0500
#if       _WIN32_WINNT < 0x0500
  #undef  _WIN32_WINNT
  #define _WIN32_WINNT   0x0500
#endif

#include <iostream>
#include <tchar.h>
#include <windows.h>

#include "ComParser.h"

int WinMain (HINSTANCE hThisInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR lpszArgument, //this one!!!
                     int nCmdShow)
{
    ComParser parser;
    for(int i=0; i<parser.GetArgc(); ++i)
        std::cout << parser.GetArgAt(i) << '\n' << std::endl;
    return 0;
}


