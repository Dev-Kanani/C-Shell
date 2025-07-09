#include "all_directories.h"
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *alias_names[MAX_ALIASES];
char *alias_commands[MAX_ALIASES];
int alias_count = 0;

extern pid_t foreground_pid;

typedef struct
{
    char command[MAX_SIZE];
    pid_t pid;
    char state[10]; // "Running" or "Stopped"
} ProcessInfo;

extern ProcessInfo processes[MAX_PROCESSES];
extern int process_count;

typedef struct
{
    char function_name[MAX_SIZE];
    char function_body[MAX_SIZE];
} FunctionDef;

FunctionDef functions[MAX_FUNCTIONS];
int function_count = 0;

void add_process(const char *command, pid_t pid, const char *state)
{
    if (process_count < MAX_PROCESSES)
    {
        strcpy(processes[process_count].command, command);
        processes[process_count].pid = pid;
        strcpy(processes[process_count].state, state);

        // printf("%s\n", processes[process_count].state);
        process_count++;
    }
}

void update_process_status()
{
    for (int i = 0; i < process_count; i++)
    {
        char proc_file[256];
        char buffer[512];
        snprintf(proc_file, sizeof(proc_file), "/proc/%d/status", processes[i].pid);

        FILE *f = fopen(proc_file, "r");
        if (f == NULL)
        {
            strcpy(processes[i].state, "Stopped");
            continue;
        }

        while (fgets(buffer, sizeof(buffer), f))
        {
            if (strncmp(buffer, "State:", 6) == 0)
            {
                char state = buffer[7];

                switch (state)
                {
                case 'R': // Running
                    strcpy(processes[i].state, "Running");
                    break;
                case 'S':                                  // Sleeping
                case 'D':                                  // Disk sleep (usually waiting for IO)
                    strcpy(processes[i].state, "Running"); // Treat sleeping as still running
                    break;
                case 'T': // Stopped (usually by a signal like SIGSTOP)
                    strcpy(processes[i].state, "Stopped");
                    break;
                case 'Z': // Zombie
                    strcpy(processes[i].state, "Zombie");
                    break;
                default:
                    strcpy(processes[i].state, "Unknown");
                    break;
                }
                break;
            }
        }

        fclose(f);
    }
}

volatile sig_atomic_t sigint_flag = 0;
volatile sig_atomic_t sigtstp_flag = 0;

void handle_sigint(int sig)
{
    sigint_flag = 1;
}

void handle_sigtstp(int sig)
{
    sigtstp_flag = 1;
}

ProcessInfo *finding_in_the_proc_file(int pid)
{
    char proc_file[256];
    char buffer[512];
    snprintf(proc_file, sizeof(proc_file), "/proc/%d/status", pid);

    FILE *f = fopen(proc_file, "r");
    if (f == NULL)
    {
        return NULL;
    }

    ProcessInfo *found = malloc(sizeof(ProcessInfo));
    found->pid = pid;
    strcpy(found->state, "Not found");
    strcpy(found->command, "Not found");

    while (fgets(buffer, sizeof(buffer), f) != NULL)
    {
        if (strncmp(buffer, "Name:", 5) == 0)
        {
            sscanf(buffer, "Name:\t%255s", found->command);
        }
        if (strncmp(buffer, "State:", 6) == 0)
        {
            found->state[0] = buffer[7];
            found->state[1] = '\0';     
        }
    }

    if (found->state[0] == 'T')
    {
        strcpy(found->state, "Stopped");
    }
    if (found->state[0] == 'R')
    {
        strcpy(found->state, "Running 1");
    }
    if (found->state[0] == 'S')
    {
        strcpy(found->state, "Running 2");
    }
    if (found->state[0] == 'Z')
    {
        strcpy(found->state, "Zombie");
    }

    fclose(f);

    if (strcmp(found->state, "Not found") == 0 || strcmp(found->command, "Not found") == 0)
    {
        printf("Process information not found for PID: %d\n", pid);
        free(found);
        return NULL;
    }

    return found;
}

int fg_command(int pid)
{
    signal(SIGINT, handle_sigint);
    signal(SIGTSTP, handle_sigtstp);

    ProcessInfo *found = finding_in_the_proc_file(pid);

    if (!found)
    {
        printf("No process with PID %d found in background\n", pid);
        return -1;
    }

    foreground_pid = pid;

    if (kill(pid, SIGCONT) == -1)
    {
        printf("Failed to send SIGCONT: %s\n", strerror(errno));
        return -1;
    }

    ProcessInfo *temp2 = finding_in_the_proc_file(pid);
    while (temp2 != NULL)
    {
        sleep(1); // Wait a second before checking again

        printf("Process state: %s\n", temp2->state);

        // Check if Ctrl+C (SIGINT) or Ctrl+Z (SIGTSTP) was pressed
        if (sigint_flag)
        {
            printf("SIGINT detected (Ctrl+C). Breaking the loop...\n");
            kill(temp2->pid, SIGINT);
            sigint_flag = 0;
            break;
        }

        if (sigtstp_flag)
        {
            printf("SIGTSTP detected (Ctrl+Z). Breaking the loop...\n");
            kill(temp2->pid, SIGTSTP);
            sigtstp_flag = 0;
            break;
        }

        if (strcmp(temp2->state, "Stopped") == 0 || strcmp(temp2->state, "Zombie") == 0)
            break;

        temp2 = finding_in_the_proc_file(pid);
    }

    foreground_pid = -1;

    return 0;
}

int bg_command(int pid)
{
    ProcessInfo *found = finding_in_the_proc_file(pid);

    printf("1 : %s\n", found->command);
    printf("2 : %s\n", found->state);

    if (!found)
    {
        printf("No process with PID %d found in background\n", pid);
        return -1;
    }

    if (kill(found->pid, SIGCONT) == -1)
    {
        printf("Failed to send SIGCONT: %s\n", strerror(errno));
        return -1;
    }

    strcpy(found->state, "Running");

    // int status;
    // waitpid(found->pid, &status, WUNTRACED); // Wait for the process to finish or be stopped

    foreground_pid = -1; // No process in the foreground anymore

    return 0;
}

char *trim_leading_whitespace(char *str)
{
    while (isspace(*str))
    {
        str++;
    }
    return str;
}

char *get_first_word(char *str)
{
    str = trim_leading_whitespace(str);
    char *end_of_first_word = strchr(str, ' ');
    if (end_of_first_word != NULL)
    {
        *end_of_first_word = '\0';
    }
    return str;
}

void handle_IO_redirection(char *command, int is_background, char *newHomeDirectory, char *previousDirectory, int *counter_of_and, char command_greater_than_2[])
{

    // printf("HERE 1\n");

    int fd_in = 0, fd_out = 0;
    int duplicate_in = -1, duplicate_out = -1;

    char **args;
    int num_tokens;

    int length = 0;

    int flag = 0;

    if (strstr(command, ">>") != NULL)
    {
        args = tokenise_the_output_redirect(command, &num_tokens);
        // printf("ARGS 0 : %s\n", args[0]);
        char new[MAX_SIZE_OF_NAMES] = {0};
        strcpy(new, args[1]);
        // printf("ARGS 1 : %s\n", args[1]);
        char *file_name = strtok(new, " ");
        length = strlen(file_name);
        duplicate_out = output_redirect_to_file(file_name, &fd_out, 1);

        flag = 1;
    }
    else if (strstr(command, ">") != NULL)
    {
        args = tokenise_the_output_redirect(command, &num_tokens);
        // printf("ARGS 0 : %s\n", args[0]);

        char new[MAX_SIZE_OF_NAMES] = {0};
        strcpy(new, args[1]);
        // printf("ARGS 1 : %s\n", args[1]);
        char *file_name = strtok(new, " ");
        length = strlen(file_name);
        duplicate_out = output_redirect_to_file(file_name, &fd_out, 0);

        flag = 1;
    }

    if (strstr(command, "<") != NULL)
    {
        args = tokenise_the_input_redirect(command, &num_tokens);
        // printf("ARGS 0 : %s\n", args[0]);
        char new[MAX_SIZE_OF_NAMES] = {0};
        strcpy(new, args[1]);
        // printf("ARGS 2 : %s\n", args[1]);
        char *file_name = strtok(new, " ");
        length = strlen(file_name);
        duplicate_in = input_redirect_to_file(file_name, &fd_in);

        flag = 1;
    }

    // printf("asdasdasda\n");

    if (!flag)
    {
        char *saveptr1, *saveptr2;
        char *sub_command;

        sub_command = strtok_r(command, "&", &saveptr2);

        // printf("GEKNFJASD\n");

        while (sub_command != NULL)
        {
            while (*sub_command == ' ' || *sub_command == '\t')
                sub_command++;

            // printf("Sub Command:%sahfuihdifsu\n", sub_command);

            if (strlen(sub_command) == 0)
            {
                sub_command = strtok_r(NULL, "&", &saveptr2);
                continue;
            }

            int flago = 0;

            char first_command[MAX_SIZE_OF_NAMES][MAX_SIZE_OF_NAMES];

            int index = 0;

            for (int i = 0; i < function_count; i++)
            {
                if (strncmp(sub_command, functions[i].function_name, strlen(functions[i].function_name) - 2) == 0)
                {
                    // printf("Sub Command : %s\n", sub_command);
                    // printf("Function : %s\n", functions[i].function_name);
                    // printf("Function Body :\n%s", functions[i].function_body);

                    char *line = strtok(functions[i].function_body, "\n");
                    while (line != NULL)
                    {
                        char *first_word = get_first_word(line);
                        char final_1[MAX_SIZE_OF_NAMES] = {0};
                        // if (*first_word != '\0')
                        // { // Make sure it's not an empty line
                        //   // printf("First word: %s\n", first_word);
                        // }

                        // strcat(first_word, " ");
                        // strcat(first_word, sub_command);

                        // first_word[strlen(first_word) - 1] = '\0';

                        strcat(final_1, first_word);
                        strcat(final_1, " ");
                        strcat(final_1, sub_command + strlen(functions[i].function_name) - 1);
                        // strcat(first_word, sub_command);
                        final_1[strlen(final_1)] = '\0';
                        // printf("First word : %s\n", final_1);
                        strcpy(first_command[index], final_1);
                        // printf("First Command 1 : %s\n", first_command[index]);
                        // execute_the_sub_command(final_1, is_background, newHomeDirectory, previousDirectory, counter_of_and, command_greater_than_2);
                        index++;
                        // execute_the_sub_command(final_1, is_background, newHomeDirectory, previousDirectory, counter_of_and, command_greater_than_2);
                        line = strtok(NULL, "\n");
                    }
                    // execute_the_sub_command(functions[i].function_body, is_background, newHomeDirectory, previousDirectory, counter_of_and, command_greater_than_2);
                    flago = 1;
                }
            }

            if (!flago)
            {
                // printf("Here\n");

                char temp[1000] = {0};
                strcpy(temp, sub_command);
                char temp2[1000] = {0};
                strcpy(temp2, sub_command);
                char temp3[1000] = {0};
                strcpy(temp3, sub_command);
                char *resolved = resolve_alias(temp2);
                char *first = strtok(temp3, " ");

                // printf("Resolved : %s\n", resolved);

                // for (int i = 0; i < function_count; i++)
                // {
                //     if(strcmp(first, functions[i].function_name) == 0)
                //     {
                //         char *resolved = functions[i].function_body;
                //         char *first = strtok(functions[i].function_body, " ");
                //         break;
                //     }
                // }

                // printf("temp : %s\n", temp);
                // printf("resolved : %s\n", resolved);
                // printf("first : %s\n", first);

                if (strcmp(resolved, first) == 0)
                {
                    // printf("Here 1\n");
                    execute_the_sub_command(sub_command, is_background, newHomeDirectory, previousDirectory, counter_of_and, command_greater_than_2);
                }
                else
                    execute_the_sub_command(resolved, is_background, newHomeDirectory, previousDirectory, counter_of_and, command_greater_than_2);
            }
            else
            {
                for (int i = 0; i < index; i++)
                {
                    // printf("First Command : %s\n", first_command[i]);
                    execute_the_sub_command(first_command[i], is_background, newHomeDirectory, previousDirectory, counter_of_and, command_greater_than_2);
                }
            }

            // char *resolved = resolve_function(first);

            sub_command = strtok_r(NULL, "&", &saveptr2);
        }

        return;
    }

    // printf("Command : %s\n", coopied_command);
    // printf("Length : %d\n", length);
    strcat(args[0], " ");
    strcat(args[0], args[1] + length + 1);

    // printf("FINAL ARGS : %s\n", args[0]);

    char **command_args = parse_command(args[0]);

    char final_command[MAX_SIZE_OF_NAMES] = {0};

    int q = 0;
    while (command_args[q] != NULL)
    {
        strcat(final_command, command_args[q]);
        if (command_args[q + 1] != NULL)
            strcat(final_command, " ");
        q++;
    }

    char hi[MAX_SIZE_OF_NAMES] = {0};
    strcpy(hi, final_command);

    char *saveptr1, *saveptr2;
    char *sub_command;

    sub_command = strtok_r(hi, "&", &saveptr2);

    // printf("GEKNFJASD\n");

    while (sub_command != NULL)
    {
        while (*sub_command == ' ' || *sub_command == '\t')
            sub_command++;

        // printf("Sub Command: %s\n", sub_command);

        // char *resolved = resolve_function(first);
        char temp[1000] = {0};
        strcpy(temp, sub_command);
        char *resolved = resolve_alias(sub_command);
        char *first = strtok(sub_command, " ");

        // printf("Resolved : %s\n", resolved);

        if (strcmp(resolved, first) == 0)
            execute_the_sub_command(temp, is_background, newHomeDirectory, previousDirectory, counter_of_and, command_greater_than_2);
        else
            execute_the_sub_command(resolved, is_background, newHomeDirectory, previousDirectory, counter_of_and, command_greater_than_2);

        sub_command = strtok_r(NULL, "&", &saveptr2);
    }

    // printf("HI : %s\n", hi);

    // if (strstr(final_command, "&") != NULL)
    // {
    //     is_background = 1;
    //     char *temp;
    //     temp = strtok(final_command, "&");
    //     strcpy(hi, temp);
    //     hi[strlen(hi) - 1] = '\0';
    // }

    // // printf("%s asdad\n", hi);

    // // CHANGE THIS & CASE

    // // printf("FINAL COMMAND : %s\n", final_command);
    // *counter_of_and = 1;

    // execute_the_sub_command(hi, is_background, newHomeDirectory, previousDirectory, counter_of_and, command_greater_than_2);

    if (duplicate_in != -1)
    {
        input_back_to_normal(duplicate_in, &fd_in);
    }
    if (duplicate_out != -1)
    {
        output_back_to_normal(duplicate_out, &fd_out);
    }
}

void solve_pipes(char *command_sequence, int is_background, char *newHomeDirectory, char *previousDirectory, int *counter_of_and, char command_greater_than_2[])
{
    char *commands[MAX_SIZE_OF_NAMES]; 
    int num_commands = 0;
    int fd_in = 0, pipe_fds[2];
    pid_t pid;

    char *saveptr;
    char *current_command = strtok_r(command_sequence, "|", &saveptr);
    while (current_command != NULL)
    {
        commands[num_commands++] = trim_whitespace(current_command);
        current_command = strtok_r(NULL, "|", &saveptr);
    }

    for (int i = 0; i < num_commands; i++)
    {
        // printf("Processing command %d: %s\n", i + 1, commands[i]);

        // pipe for all but the last command
        if (i < num_commands - 1 && pipe(pipe_fds) == -1)
        {
            perror("pipe");
            // exit(EXIT_FAILURE);
        }

        if ((pid = fork()) == -1)
        {
            perror("fork");
            // exit(EXIT_FAILURE);
        }

        if (pid == 0)
        {
            if (fd_in != 0)
            {
                dup2(fd_in, STDIN_FILENO); // Set input to fd_in from previous command
                close(fd_in);
            }

            if (i < num_commands - 1)
            {
                dup2(pipe_fds[1], STDOUT_FILENO); // Redirect output to pipe for next command
                close(pipe_fds[1]);
                close(pipe_fds[0]);
            }

            handle_IO_redirection(commands[i], is_background, newHomeDirectory, previousDirectory, counter_of_and, command_greater_than_2);

            exit(EXIT_FAILURE);
        }
        else
        {
            if (!is_background)
            {
                waitpid(pid, NULL, 0);
            }

            if (i < num_commands - 1)
            {
                close(pipe_fds[1]);
                fd_in = pipe_fds[0]; // Pass read end as input for the next command
            }
        }
    }

    if (is_background)
    {
        printf("Background process: PID = %d\n", pid);
    }

    printf("Finished executing piped commands.\n");
}

void execute_the_sub_command(char *command, int is_background, char *newHomeDirectory, char *previousDirectory, int *counter_of_and, char command_greater_than_2[])
{
    if (strlen(command) == 0)
        return;

    if (is_background && (*counter_of_and) > 0)
    {
        (*counter_of_and)--;

        int pipefd[2];
        if (pipe(pipefd) == -1)
        {
            perror("Pipe failed");
            return;
        }

        int pid = fork();

        if (pid < 0)
        {
            perror("Fork failed");
            return;
        }
        else if (pid == 0)
        {
            setpgid(0, 0);

            pid_t fork_inside_child = fork();

            if (fork_inside_child < 0)
            {
                perror("Fork inside child failed");
                exit(EXIT_FAILURE);
            }
            else if (fork_inside_child == 0)
            {

                // printf("[%d] ", job_number++);
                // int polo;

                // polo = getpid();

                // printf("%d\n", polo);

                close(pipefd[0]); // Close read end of pipe in the grandchild
                pid_t grandchild_pid = getpid();
                write(pipefd[1], &grandchild_pid, sizeof(grandchild_pid)); // Send PID to parent
                close(pipefd[1]);                                          // Close write end after sending

                char temp[MAX_SIZE];
                strcpy(temp, command);
                char *args[MAX_SIZE / 2 + 1];
                char *token = strtok(temp, " \t\n");
                int i = 0;
                while (token != NULL)
                {
                    args[i++] = token;
                    token = strtok(NULL, " \t\n");
                }
                args[i] = NULL;

                if (execvp(args[0], args) < 0)
                {
                    perror("Exec failed");
                    exit(EXIT_FAILURE);
                }
            }
            else
            {
                wait(NULL);

                char temp[MAX_SIZE];

                strcpy(temp, command);

                char *args[MAX_SIZE / 2 + 1];
                char *token = strtok(temp, " \t\n");
                int i = 0;

                while (token != NULL)
                {
                    args[i++] = token;
                    token = strtok(NULL, " \t\n");
                }

                args[i] = NULL;

                // printf("Foreground pid : %d\n", foreground_pid);
                // printf("Forkk inside child pid : %d\n", fork_inside_child);

                if (foreground_pid != fork_inside_child)
                    printf("%s with PID %d exited normally\n", args[0], fork_inside_child);

                exit(EXIT_SUCCESS);
            }
        }
        else
        {
            // wait(NULL);
            close(pipefd[1]); // Close write end of pipe

            // Wait for the grandchild's PID
            pid_t background_pid;
            read(pipefd[0], &background_pid, sizeof(background_pid));
            close(pipefd[0]);

            // printf("BACKGROUND PID : %d\n", background_pid);
            strcat(command, " &");
            add_process(command, background_pid, "Running");

            printf("[%d] %d\n", job_number++, background_pid);

            // exit(EXIT_SUCCESS);
        }
    }
    else
    {
        // printf("fyugyg\n");

        // printf("Command : %s\n", command);

        foreground(command, newHomeDirectory, previousDirectory, command_greater_than_2);
    }
}

void parse_commands(char *input, char *newHomeDirectory, char *previousDirectory, char command_greater_than_2[])
{
    load_myshrc();

    // printf("Function Count : %d\n", function_count);
    // printf("Alias Count : %d\n", alias_count);

    // for (int i = 0; i < function_count; i++)
    // {
    //     printf("%s\n%s", functions[i].function_name, functions[i].function_body);
    // }

    char *saveptr1, *saveptr2;
    char *command_sequence, *sub_command;

    command_sequence = strtok_r(input, ";", &saveptr1);

    while (command_sequence != NULL)
    {
        while (*command_sequence == ' ' || *command_sequence == '\t')
            command_sequence++;

        int is_background = 0;

        int command_length = strlen(command_sequence);

        int *counter_of_and = malloc(sizeof(int *));
        *(counter_of_and) = 0;

        for (int p = 0; p < (command_length); p++)
        {
            if (command_sequence[p] == '&')
            {
                is_background = 1;
                (*counter_of_and)++;
            }
        }

        int pipe_redirection = 0, input_redirection = 0, output_redirection = 0;

        if (strstr(command_sequence, "|") != NULL)
        {
            pipe_redirection = 1;
        }
        if (strstr(command_sequence, "<") != NULL)
        {
            input_redirection = 1;
        }
        if (strstr(command_sequence, ">") != NULL)
        {
            output_redirection = 1;
        }

        if (pipe_redirection)
        {
            char *answer;

            solve_pipes(command_sequence, is_background, newHomeDirectory, previousDirectory, counter_of_and, command_greater_than_2);
        }
        else
        {
            printf("Command Sequence : %s\n", command_sequence);
            handle_IO_redirection(command_sequence, is_background, newHomeDirectory, previousDirectory, counter_of_and, command_greater_than_2);
        }
        // else
        // {

        //     /////////////////////////////////////////////////////////////////////

        //     sub_command = strtok_r(command_sequence, "&", &saveptr2);

        //     while (sub_command != NULL)
        //     {
        //         while (*sub_command == ' ' || *sub_command == '\t')
        //             sub_command++;

        //         // printf("Sub Command: %s\n", sub_command);

        //         // char *resolved = resolve_function(first);
        //         char temp[1000] = {0};
        //         strcpy(temp, sub_command);
        //         char *resolved = resolve_alias(sub_command);
        //         char *first = strtok(sub_command, " ");

        //         // printf("Resolved : %s\n", resolved);

        //         if (strcmp(resolved, first) == 0)
        //             execute_the_sub_command(temp, is_background, newHomeDirectory, previousDirectory, counter_of_and, command_greater_than_2);
        //         else
        //             execute_the_sub_command(resolved, is_background, newHomeDirectory, previousDirectory, counter_of_and, command_greater_than_2);

        //         sub_command = strtok_r(NULL, "&", &saveptr2);
        //     }

        command_sequence = strtok_r(NULL, ";", &saveptr1);

        //     // free(counter_of_and);
        // }
    }
}

void print_activities()
{
    update_process_status();
    for (int i = 0; i < process_count; i++)
    {
        ProcessInfo *temp = finding_in_the_proc_file(processes[i].pid);
        if (temp == NULL)
            continue;

        printf("[%d] : %s - %s\n", processes[i].pid, processes[i].command, processes[i].state);
    }
}

void ping(int pid, int signal)
{
    if (signal < 0)
    {
        printf("Input signal is not correct\n");
        return;
    }

    int final_signal_number = signal % 32;

    if (kill(pid, final_signal_number) == 0)
    {
        printf("Sent signal %d to process with pid %d\n", final_signal_number, pid);
    }
    else
    {

        if (errno == ESRCH)
        {
            printf("No such process found\n");
        }
        else
        {
            perror("Error sending signal");
        }
    }
}
