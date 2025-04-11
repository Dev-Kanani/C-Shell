#include "all_directories.h"

void get_process_info(int pid, char *currentDirectory, char *newHomeDirectory)
{
    char path[256], buffer[256], state[3] = {0}, exec_path[256];
    int vm_size = 0, pgrp = 0;

    snprintf(path, sizeof(path), "/proc/%d/status", pid);
    FILE *status_file = fopen(path, "r");
    if (!status_file)
    {
        perror("Failed to open status file");
        return;
    }

    while (fgets(buffer, sizeof(buffer), status_file))
    {
        sscanf(buffer, "State:\t%s", state);
        sscanf(buffer, "VmSize:\t%d", &vm_size);
        sscanf(buffer, "Tgid:\t%d", &pgrp);
    }
    fclose(status_file);

    int fg_pgrp = tcgetpgrp(STDIN_FILENO); 

    if (pgrp == fg_pgrp)
    {
        strcat(state, "+");
    }

    snprintf(path, sizeof(path), "/proc/%d/exe", pid);
    ssize_t len = readlink(path, exec_path, sizeof(exec_path) - 1);
    if (len != -1)
    {
        exec_path[len] = '\0';

        if (strncmp(exec_path, newHomeDirectory, strlen(newHomeDirectory)) == 0)
        {
            snprintf(exec_path, sizeof(exec_path), "~%s", exec_path + strlen(newHomeDirectory));
        }
    }
    else
    {
        strcpy(exec_path, "Unknown");
    }

    printf("pid : %d\nProcess Status : %s\nProcess Group : %d\nVirtual Memory : %d kB\nExecutable Path : %s\n",
           pid, state, pgrp, vm_size, exec_path);
}