#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <termios.h>
#include <sys/select.h>

volatile sig_atomic_t terminate_program = 0;

int get_latest_pid()
{
    DIR *dir;
    struct dirent *entry;
    struct stat statbuf;
    int latest_pid = 0;
    char path[256];

    dir = opendir("/proc");
    if (dir == NULL)
    {
        perror("opendir");
        exit(EXIT_FAILURE);
    }

    while ((entry = readdir(dir)) != NULL)
    {
        snprintf(path, sizeof(path), "/proc/%s", entry->d_name);

        if (stat(path, &statbuf) == 0 && S_ISDIR(statbuf.st_mode))
        {
            int pid = atoi(entry->d_name);
            if (pid > 0 && pid > latest_pid)
            {
                latest_pid = pid;
            }
        }
    }

    closedir(dir);
    return latest_pid;
}

void enable_raw_mode()
{
    struct termios term;
    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag &= ~(ICANON | ECHO); // Disable canonical mode and echo
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

void disable_raw_mode()
{
    struct termios term;
    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag |= (ICANON | ECHO); // Enable canonical mode and echo
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

int check_key_press()
{
    struct timeval tv = {0L, 0L}; 
    fd_set fds;

    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds); // Set stdin in the file descriptor set

    int result = select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv);
    if (result > 0)
    {
        char c;
        if (read(STDIN_FILENO, &c, 1) > 0 && c == 'x')
        {
            return 1;
        }
    }
    return 0;
}

void neonate(int time_arg)
{
    enable_raw_mode();

    while (!terminate_program)
    {
        int latest_pid = get_latest_pid();
        printf("Latest PID: %d\n", latest_pid);
        fflush(stdout);

        for (int i = 0; i < time_arg && !terminate_program; i++)
        {
            usleep(1000000);       // Sleep for 100ms
            if (check_key_press()) // Check if 'x' is pressed
            {
                printf("Terminating...\n");
                terminate_program = 1;
                break;
            }
        }
    }

    disable_raw_mode();
}