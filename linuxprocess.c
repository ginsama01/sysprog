#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<dirent.h>

#define MAX_LEN 1024

int main(int argc, char** argv) {

    printf("%-10s %-10s %s\n", "PID", "PPID", "Commandline");

    // all process appear here
    struct dirent* dirent;
    DIR* dir = opendir("/proc");
    if (dir == NULL) {
        return 1;
    }
    
    int pid;
    
    while ((dirent = readdir(dir)) != NULL) {
        // check if d_name is number, it's is process id
        if (pid = atoi(dirent->d_name)) {
            char* process_path = (char*)malloc(MAX_LEN);
            char* cmdline = (char*)malloc(MAX_LEN);
            char* comm = (char*)malloc(MAX_LEN);
            int ppid;
            FILE* fin;
            
            // open cmdline
            sprintf(process_path, "/proc/%d/cmdline", pid);
            if (fin = fopen(process_path, "r")) {
                fscanf(fin, "%s", cmdline);
                fclose(fin);
            }

            // open stat file
            sprintf(process_path, "/proc/%d/stat", pid);
            char state;
            if (fin = fopen(process_path, "r")) {
                fscanf(fin, "%d %s %c %d", &pid, comm, &state, &ppid);
                fclose(fin);
            }

            if (strlen(cmdline) == 0) { // mean the process is a zombie, use The filename of the executable
                printf("%-10d %-10d %s\n", pid, ppid, comm);
            } else {
                printf("%-10d %-10d %s\n", pid, ppid, cmdline);
            }
        }
    }

    closedir(dir);

}