#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int is_directory(const char *path) {
   struct stat path_stat;
   stat(path, &path_stat);
   return S_ISDIR(path_stat.st_mode);
}

void scan_file(char* path, char* file_name) {
    struct dirent* dirent;
    DIR* dir = opendir(path);
    if (dir == NULL) {
        return;
    }
    while ((dirent = readdir(dir)) != NULL) {
        char* child_path = (char*)malloc(2 + strlen(path) + strlen(dirent->d_name));
        strcpy(child_path, path);
        if (path[strlen(path) - 1] != '/') {
            strcat(child_path, "/");
        }
        strcat(child_path, dirent->d_name);
        if (is_directory(child_path)) {
            if (strcmp(dirent->d_name, ".") == 0 || strcmp(dirent->d_name, "..") == 0) {
                continue;
            }
            scan_file(child_path, file_name);
        } else {
            if (strstr(dirent->d_name, file_name) != NULL) {
                printf("%s\n", child_path);
            }
        }
    }
}

int main(int argc, char** argv) {
    if (argc < 3) {
        return 0;
    }
    scan_file(argv[1], argv[2]);
}