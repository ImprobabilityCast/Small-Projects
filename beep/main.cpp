#include <stdio.h>
#include <iostream>
#if       _WIN32_WINNT < 0x0500
  #undef  _WIN32_WINNT
  #define _WIN32_WINNT   0x0501
#endif
#include <windows.h>

int WinMain(HINSTANCE h, HINSTANCE h2, LPSTR whoKnows, int it)
{
    Beep(0x320, 200);
    return 0;
}
