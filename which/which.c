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
        int found = 1;
        
        while (token && found) {
            DIR* dir = opendir(token);
            dirPtr = readdir(dir);
            while (dirPtr && found) {
                found = strcmp(dirPtr->d_name, name);
                dirPtr = readdir(dir);
            }
            closedir(dir);
            *(token-1) = ';';
            if (found) {
                token = strtok(NULL, ";");
            }
        }
        
        if (found == 0) {
            if (token[strlen(token) - 1] == '\\') {
                printf("%s%s\n", token, name);
            } else {
                printf("%s\\%s\n", token, name);
            }
        } else {
            printf("which: no '%s' in (%s)", name, path);
        }
    }
    return 0;
}
