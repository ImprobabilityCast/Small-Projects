#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main (int argc, char** argv) {
    if (argc != 2) {
        printf("USAGE: which <thing to look for in the PATH>");
    } else {
        char* path = getenv("PATH");
        char* name = argv[1];
        char* token = strtok(path, ";");
        struct dirent* dirPtr;

        while (token) {
            DIR* dir = opendir(token);
            dirPtr = readdir(dir);
            while (dirPtr) {
                if (NULL == strcmp(dirPtr->d_name, name)) {
                    if (token[strlen(token) - 1] == '\\') {
                        printf("%s%s\n", token, name);
                    } else {
                        printf("%s\\%s\n", token, name);
                    }
                    return 0;
                }
                dirPtr = readdir(dir);
            }
            closedir(dir);
            *(token-1) = ';';
            token = strtok(NULL, ";");
        }

        printf("which: no '%s' in (%s)", name, path);
    }
    return 0;
}
