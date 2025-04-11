#define _POSIX_C_SOURCE 199309L
#include "all_directories.h"
#include "log.h"
#include "queue.h"
#include "reveal.h"

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

int main()
{

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

        if (strncmp(newHomeDirectory, currentWorkingDirectory, strlen(newHomeDirectory)) == 0)
        {
            if (command_greater_than_2[0] != '\0')
                printf("<%s@%s:~%s %s> ", userName, system_Name, currentWorkingDirectory + strlen(newHomeDirectory), command_greater_than_2);
            else
                printf("<%s@%s:~%s> ", userName, system_Name, currentWorkingDirectory + strlen(newHomeDirectory));
        }
        else
        {
            if (command_greater_than_2 != '\0')
                printf("<%s@%s:%s %s> ", userName, system_Name, currentWorkingDirectory, command_greater_than_2);
            else
                printf("<%s@%s:%s> ", userName, system_Name, currentWorkingDirectory);
        }

        command_greater_than_2[0] = '\0';

        char inputCommand[MAX_SIZE];
        fgets(inputCommand, MAX_SIZE, stdin);
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

        parse_commands(inputCommand, newHomeDirectory, previousDirectory, command_greater_than_2);
    }
}
