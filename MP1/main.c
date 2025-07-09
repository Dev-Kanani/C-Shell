#define _POSIX_C_SOURCE 199309L
#include "all_directories.h"
#include "log.h"
#include "reveal.h"

void removeSpaces(const char *str, char *temp)
{
    int i = 0, j = 0;

    while (str[i])
    {
        if (str[i] != ' ')
        {
            temp[j++] = str[i];
        }
        i++;
    }

    temp[j] = '\0';
}

pid_t foreground_pid = -1;

typedef struct pqwe
{
    char command[MAX_SIZE];
    pid_t pid;
    char state[10]; // "Running" or "Stopped"
} ProcessInfo;

ProcessInfo processes[MAX_PROCESSES];
int process_count = 0;

int job_number = 0;
char LOG_FILE_PATH[4096];

char *strtok_r(char *str, const char *delim, char **saveptr)
{
    if (str == NULL)
    {
        str = *saveptr;
    }

    if (str == NULL || *str == '\0')
    {
        *saveptr = NULL;
        return NULL;
    }

    char *token_start = str + strspn(str, delim);

    if (*token_start == '\0')
    {
        *saveptr = NULL;
        return NULL;
    }

    char *token_end = strpbrk(token_start, delim);

    if (token_end == NULL)
    {
        *saveptr = NULL;
    }
    else
    {
        *token_end = '\0';
        *saveptr = token_end + 1;
    }

    return token_start;
}

void sigint_handler(int signal)
{
    if (signal == SIGINT)
    {
        if (foreground_pid != -1)
        {
            // Send SIGINT to the foreground process
            // ping(foreground_pid, 9);
            if (kill(foreground_pid, SIGINT) == 0)
            {
                printf("\nForeground process with PID %d interrupted by SIGINT\n", foreground_pid);
                update_process_status();
            }
            else
            {
                perror("Failed to send SIGINT");
            }

            // Reset foreground_pid to indicate no active foreground process
            foreground_pid = -1;
        }
        else
        {
            printf("\n");
        }
    }
}

void sigtstp_handler(int signal)
{
    if (signal == 20)
    {
        if (foreground_pid != -1)
        {
            // Send SIGTSTP to the foreground process
            if (kill(foreground_pid, 19) == 0)
            {
                printf("\nForeground process with PID %d stopped by SIGTSTP\n", foreground_pid);
                // Optionally update the process list or handle status changes
                update_process_status();
                foreground_pid = -1;
            }
            else
            {
                perror("Failed to send SIGTSTP");
            }
        }
        else
        {
            printf("\n");
        }
    }
}

void sigquit_handler(int signal)
{
    for (int i = 0; i < process_count; i++)
    {
        ProcessInfo *temp = finding_in_the_proc_file(processes[i].pid);

        if (temp == NULL)
            continue;
        // Send SIGKILL to terminate each process
        if (kill(processes[i].pid, SIGKILL) == 0)
        {
            printf("Sent SIGKILL to process with PID %d\n", processes[i].pid);
        }
        else
        {
            perror("Failed to send SIGKILL");
        }
    }

    process_count = 0;

    printf("Logging out of shell, all processes killed\n");

    exit(EXIT_SUCCESS);
}

void set_signal_action(void)
{
    struct sigaction act;

    // bzero(&act, sizeof(act));
    // act.sa_handler = handle_sigchld;
    // sigaction(SIGCHLD, &act, NULL);

    // SIGINT (Ctrl-C)
    // memset(&act, 0, sizeof(act));
    bzero(&act, sizeof(act));
    act.sa_handler = sigint_handler;
    sigaction(SIGINT, &act, NULL);

    // SIGTSTP (Ctrl-Z)
    // memset(&act, 0, sizeof(act));
    bzero(&act, sizeof(act));
    act.sa_handler = sigtstp_handler;
    sigaction(SIGTSTP, &act, NULL);

    // SIGQUIT (Ctrl-\)
    // memset(&act, 0, sizeof(act));
    bzero(&act, sizeof(act));
    act.sa_handler = sigquit_handler;
    sigaction(SIGQUIT, &act, NULL);
}

int main()
{
    // signal(SIGCHLD, handle_background_processes);

    // setup_signal_handler(); // Setup SIGCHLD handler
    // setup_sigchld_handler();
    // signal(SIGINT, sigint_handler);   // Ctrl-C
    // signal(SIGQUIT, sigquit_handler); // Ctrl-D
    // signal(SIGTSTP, sigtstp_handler); // Ctrl-Z
    set_signal_action();

    char system_Name[MAX_SIZE_OF_NAMES];
    char *userName;
    char previousDirectory[MAX_SIZE] = {0};

    userName = getenv("USER");

    gethostname(system_Name, MAX_SIZE_OF_NAMES);

    char newHomeDirectory[MAX_SIZE_OF_NAMES];
    getcwd(newHomeDirectory, MAX_SIZE_OF_NAMES);

    strcpy(LOG_FILE_PATH, newHomeDirectory);
    strcat(LOG_FILE_PATH, "/");
    strcat(LOG_FILE_PATH, "log.txt");

    char command_greater_than_2[1000] = {0};
    command_greater_than_2[0] = '\0';

    while (1)
    {
        char currentWorkingDirectory[MAX_SIZE_OF_NAMES];
        getcwd(currentWorkingDirectory, MAX_SIZE_OF_NAMES);

        usleep(5000);

        if (foreground_pid != -1)
        {
            int status;
            pid_t result = waitpid(foreground_pid, &status, WUNTRACED);
            if (result > 0)
            {
                if (WIFEXITED(status) || WIFSIGNALED(status))
                {
                    foreground_pid = -1;
                }
                else if (WIFSTOPPED(status))
                {
                    // Foreground process was stopped (e.g., via SIGTSTP)
                    printf("\nForeground process with PID %d stopped\n", foreground_pid);
                    foreground_pid = -1;
                }
            }
            continue;
        }

        if (strncmp(newHomeDirectory, currentWorkingDirectory, strlen(newHomeDirectory)) == 0)
        {
            if (command_greater_than_2[0] != '\0')
                printf("<%s@%s:~%s %s> ", userName, system_Name, currentWorkingDirectory + strlen(newHomeDirectory), command_greater_than_2);
            else
                printf("<%s@%s:~%s> ", userName, system_Name, currentWorkingDirectory + strlen(newHomeDirectory));
        }
        else
        {
            if (command_greater_than_2[0] != '\0')
                printf("<%s@%s:%s %s> ", userName, system_Name, currentWorkingDirectory, command_greater_than_2);
            else
                printf("<%s@%s:%s> ", userName, system_Name, currentWorkingDirectory);
        }

        command_greater_than_2[0] = '\0';

        char inputCommand[MAX_SIZE];

        if (fgets(inputCommand, MAX_SIZE, stdin) == NULL)
        {
            if (feof(stdin))
                sigquit_handler(SIGQUIT);

            continue;
        }

        inputCommand[strlen(inputCommand) - 1] = '\0';

        if (strcmp(inputCommand, "") == 0)
        {
            continue;
        }

        int printing = 1;

        if (strstr(inputCommand, "log"))
        {
            int execute_flag = 0;

            if (strstr(inputCommand, "execute"))
            {
                printing = 0;
                execute_flag = 1;
            }
            if (strstr(inputCommand, "purge"))
            {
                printing = 0;
            }

            log_command(inputCommand, &printing, execute_flag);
        }
        else
        {
            int execute_flag = 0;
            printing = 0;

            log_command(inputCommand, &printing, execute_flag);
        }

        char temporary_checking_pipe[MAX_SIZE_OF_NAMES];
        removeSpaces(inputCommand, temporary_checking_pipe);

        if (temporary_checking_pipe[0] == '|' || temporary_checking_pipe[strlen(temporary_checking_pipe) - 1] == '|' || strstr(temporary_checking_pipe, "&|") != NULL || strstr(temporary_checking_pipe, "&&") != NULL || strstr(temporary_checking_pipe, "|&") != NULL)
        {
            printf("Error in input\n");
            continue;
        }

        int flag = 0;

        // char temporary_checking_pipe[MAX_SIZE_OF_NAMES];
        removeSpaces(inputCommand, temporary_checking_pipe);

        if (strstr(temporary_checking_pipe, "||") != NULL)
        {
            flag = 1;
            int final = 0;
            if (flag)
            {
                // print_debug(inputCommand);
                for (int i = 0; i < strlen(inputCommand) - 1; i++)
                {
                    // printf("%c\n", inputCommand[i]);
                    if (inputCommand[i] == '|')
                    {
                        if (inputCommand[i + 1] == '|')
                        {
                        }
                        else
                        {
                            while (inputCommand[++i] == ' ')
                            {
                            }

                            if (inputCommand[i] == '|')
                            {
                                printf("Error space between pipes\n");
                                final = 1;
                                break;
                            }
                        }
                    }
                }
            }

            if (final)
                continue;

            char temp2[MAX_SIZE_OF_NAMES];
            strcpy(temp2, inputCommand);
            // printf("Temp2 : %s\n");
            char *commands = strtok(temp2, "||");
            // printf("COmmand : %s\n", commands);
            strcpy(inputCommand, commands);
            inputCommand[strlen(inputCommand) - 1] = '\0';
        }

        // printf("%s\n", inputCommand);

        parse_commands(inputCommand, newHomeDirectory, previousDirectory, command_greater_than_2);
    }
}
