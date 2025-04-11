#include "all_directories.h"

void execute_the_sub_command(char *command, int is_background, char *newHomeDirectory, char *previousDirectory, int *counter_of_and, char command_greater_than_2[])
{
    if (strlen(command) == 0)
        return;

    if (is_background && (*counter_of_and) > 0)
    {
        // printf("HERE\n");

        // printf("%s\n", command);

        (*counter_of_and)--;

        // printf("HERE\n");

        int pid = fork();

        if (pid < 0)
        {
            perror("Fork failed");
            return;
        }
        else if (pid == 0)
        {
            setpgid(0, 0);

            // printf("HERE\n");

            pid_t fork_inside_child = fork();

            if (fork_inside_child < 0)
            {
                perror("Fork inside child failed");
                exit(EXIT_FAILURE);
            }
            else if (fork_inside_child == 0)
            {
                printf("[%d] ", job_number++);
                int polo;

                polo = getpid();

                printf("%d\n", polo);

                char temp[MAX_SIZE];

                strcpy(temp, command);

                char *args[MAX_SIZE / 2 + 1];
                char *token = strtok(temp, " \t\n");
                int i = 0;

                while (token != NULL)
                {
                    // printf("%s\n", token);
                    args[i++] = token;
                    token = strtok(NULL, " \t\n");
                }

                args[i] = NULL;

                // printf("HERE\n");

                for (int w = 0; w < i; w++)
                {
                    // printf("Arg ; %s\n", args[w]);
                }

                if (execvp(args[0], args) < 0)
                {
                    perror("Exec failed");
                    exit(EXIT_FAILURE);
                }
            }
            else
            {
                wait(NULL);

                // printf("iajfoisd\n");

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

                printf("%s with PID %d exited normally\n", args[0], fork_inside_child);
                exit(EXIT_SUCCESS);
            }

            // printf("iuefhiwu efiwheuifhwiewihe iwhedifu whiw fchw\n");
        }
        else
        {
            // printf("HERE\n");

            job_number++;
        }
    }
    else
    {
        foreground(command, newHomeDirectory, previousDirectory, command_greater_than_2);
    }
}

void parse_commands(char *input, char *newHomeDirectory, char *previousDirectory, char command_greater_than_2[])
{
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

        sub_command = strtok_r(command_sequence, "&", &saveptr2);

        while (sub_command != NULL)
        {
            while (*sub_command == ' ' || *sub_command == '\t')
                sub_command++;

            execute_the_sub_command(sub_command, is_background, newHomeDirectory, previousDirectory, counter_of_and, command_greater_than_2);

            sub_command = strtok_r(NULL, "&", &saveptr2);
        }

        command_sequence = strtok_r(NULL, ";", &saveptr1);
    }
}