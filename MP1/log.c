#include "all_directories.h"
#include "log.h"

#define MAX_LOGS 15
#define MAX_COMMAND_LENGTH 1000

void log_command(char *command, int *print, int execute_flag)
{
    FILE *p = fopen(LOG_FILE_PATH, "r");

    char *logs_command[MAX_LOGS];
    for (int i = 0; i < MAX_LOGS; i++)
    {
        logs_command[i] = (char *)malloc(MAX_COMMAND_LENGTH * sizeof(char));
        if (logs_command[i] == NULL)
        {
            printf("Memory allocation failed!\n");
            for (int j = 0; j < i; j++)
            {
                free(logs_command[j]);
            }
            return;
        }
    }

    int index = 0;

    if (p != NULL)
    {
        char s[MAX_COMMAND_LENGTH];
        while (fgets(s, MAX_COMMAND_LENGTH, p) != NULL && index < MAX_LOGS)
        {
            strcpy(logs_command[index], s);
            index++;
        }
        fclose(p);
    }
    else
    {
        p = fopen(LOG_FILE_PATH, "w");
    }

    char lastCommand[MAX_COMMAND_LENGTH] = "";

    if (index > 0)
    {
        strcpy(lastCommand, logs_command[index - 1]);
    }

    int len = strlen(lastCommand);
    if (len > 0 && lastCommand[len - 1] == '\n')
    {
        lastCommand[len - 1] = '\0';
    }

    int is_same_as_last_command = (strcmp(lastCommand, command) == 0);

    if (!is_same_as_last_command)
    {
        if ((*print) == 0 && execute_flag == 0) // Print karvanu nathi etle logs command modify
        {
            if (index == MAX_LOGS)
            {
                free(logs_command[0]);

                for (int i = 1; i < MAX_LOGS; i++)
                {
                    logs_command[i - 1] = logs_command[i];
                }

                logs_command[MAX_LOGS - 1] = (char *)malloc(MAX_COMMAND_LENGTH * sizeof(char));

                if (logs_command[MAX_LOGS - 1] == NULL)
                {
                    printf("Memory allocation failed for the new command!\n");
                    return;
                }

                strcpy(logs_command[MAX_LOGS - 1], command);
            }
            else
            {
                strcpy(logs_command[index], command);
                index++;
            }
        }
    }

    p = fopen(LOG_FILE_PATH, "w");
    if (p == NULL)
    {
        printf("Error opening file for writing!\n");
        return;
    }

    for (int i = 0; i < index; i++)
    {
        int len = strlen(logs_command[i]);
        if (len > 0 && logs_command[i][len - 1] == '\n')
        {
            if (*print)
            {
                printf("%s", logs_command[i]);
            }
            fprintf(p, "%s", logs_command[i]);
            // else // print nathi karvanu
            // {
            // }

            // if (execute_flag == 1 && i == index - 1)
            // {
            // }
            // fprintf(p, "%s", logs_command[i]);
            // else
            // fprintf(p, "%s", logs_command[i]);
        }
        else
        {
            if (*print)
            {
                printf("%s\n", logs_command[i]);
            }
            fprintf(p, "%s\n", logs_command[i]);
            // if (execute_flag == 1 && i == index - 1)
            // {
            // }
            // fprintf(p, "%s", logs_command[i]);
            // else
            // fpr.intf(p, "%s\n", logs_command[i]);
        }
        free(logs_command[i]);
    }

    fclose(p);
}

void log_execute(char *newCommand, int number)
{
    FILE *p = fopen(LOG_FILE_PATH, "r");

    char *logs_command[MAX_LOGS];
    for (int i = 0; i < MAX_LOGS; i++)
    {
        logs_command[i] = (char *)malloc(MAX_COMMAND_LENGTH * sizeof(char));
        if (logs_command[i] == NULL)
        {
            printf("Memory allocation failed!\n");

            for (int j = 0; j < i; j++)
            {
                free(logs_command[j]);
            }
            return;
        }
    }

    int index = 0;

    if (p != NULL)
    {
        char s[MAX_COMMAND_LENGTH];
        while (fgets(s, MAX_COMMAND_LENGTH, p) != NULL && index < MAX_LOGS)
        {
            strcpy(logs_command[index], s);
            index++;
        }
        fclose(p);
    }
    else
    {
        p = fopen(LOG_FILE_PATH, "w");
    }

    if (number <= index)
    {
        strcpy(newCommand, logs_command[index - number]);
    }

    newCommand[strlen(newCommand) - 1] = '\0';
}
