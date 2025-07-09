#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "aliases.h"

extern char *alias_names[MAX_ALIASES];
extern char *alias_commands[MAX_ALIASES];
extern int alias_count;

typedef struct
{
    char function_name[MAX_ALIASES];
    char function_body[MAX_ALIASES];
} FunctionDef;

extern FunctionDef functions[MAX_FUNCTIONS];
extern int function_count;

char *trim_whitespace(char *str)
{
    char *end;

    while (isspace((unsigned char)*str))
        str++;

    if (*str == 0)
        return str;

    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end))
        end--;

    end[1] = '\0';

    return str;
}

void load_myshrc()
{
    memset(functions, 0, sizeof(functions));
    function_count = 0;

    for (int i = 0; i < alias_count; i++)
    {
        free(alias_names[i]);
        free(alias_commands[i]);
    }
    memset(alias_names, 0, sizeof(alias_names));
    memset(alias_commands, 0, sizeof(alias_commands));
    alias_count = 0;

    FILE *file = fopen("/home/devkanani/Desktop/DEV/SEM 3/Operating Systems and Networks/MiniProject-1/Part B/MP1/.myshrc", "r");
    if (!file)
    {
        perror("Error opening .myshrc");
        return;
    }

    char line[MAX_LINE_LENGTH];
    char function_name[MAX_ALIASES];
    char function_body[MAX_ALIASES] = {0};
    int in_function = 0;

    while (fgets(line, sizeof(line), file) != NULL)
    {
        // printf("[DEBUG] Reading line: %s", line);
        if (strstr(line, "alias") == line)
        {
            char *alias_name = strtok(line + 6, "=");
            char *command = strtok(NULL, "\n");

            command++;
            command[strlen(command) - 1] = '\0';

            if (alias_name && command)
            {
                alias_name = trim_whitespace(alias_name);
                command = trim_whitespace(command);

                alias_names[alias_count] = strdup(alias_name);
                alias_commands[alias_count] = strdup(command);
                alias_count++;
                // printf("[DEBUG] Loaded alias: %s -> %s\n", alias_name, command);
            }
        }

        // printf("%s\n", line);

        if (strstr(line, "() {"))
        {
            // printf("FUNCT\n");
            sscanf(line, "%s", function_name);
            in_function = 1;
            continue;
        }

        if (in_function)
        {
            if (strstr(line, "}"))
            {
                in_function = 0;
                strcpy(functions[function_count].function_name, function_name);
                strcpy(functions[function_count].function_body, function_body);
                function_count++;
                memset(function_body, 0, sizeof(function_body)); 
            }
            else
            {
                strcat(function_body, line); 
            }
        }
    }

    fclose(file);
}

void create_alias(char *alias_definition)
{
    char *equal_sign = strchr(alias_definition, '=');
    if (!equal_sign)
    {
        printf("Invalid alias format. Use: alias name=command\n");
        return;
    }

    *equal_sign = '\0';
    char *alias_name = alias_definition;
    char *command = equal_sign + 1;

    alias_name = trim_whitespace(alias_name);
    command = trim_whitespace(command);

    if (alias_name && command)
    {
        alias_names[alias_count] = strdup(alias_name);
        alias_commands[alias_count] = strdup(command);
        alias_count++;
        printf("Created alias: %s -> %s\n", alias_name, command);

        save_alias_to_file(alias_name, command);
        // printf("Alias '%s' created.\n", alias_name);
    }
    else
    {
        printf("Invalid alias format. Use: alias name=command\n");
    }
}

void save_alias_to_file(char *alias_name, char *command)
{
    FILE *file = fopen("/home/devkanani/Desktop/DEV/SEM 3/Operating Systems and Networks/MiniProject-1/Part B/MP1/.myshrc", "a");
    if (!file)
    {
        perror("Error opening .myshrc for writing");
        return;
    }

    fprintf(file, "\nalias %s='%s'\n", alias_name, command);
    fclose(file);
    // printf("[DEBUG] Alias '%s=%s' saved to .myshrc\n", alias_name, command);
}

char *resolve_alias(char *cmd)
{
    static char resolved_command[MAX_LINE_LENGTH];

    char *first_word = strtok(cmd, " ");

    for (int i = 0; i < alias_count; i++)
    {
        // printf("[DEBUG] Checking if %s matches alias %s\n", first_word, alias_names[i]);
        if (strcmp(first_word, alias_names[i]) == 0)
        {
            // printf("[DEBUG] Command %s resolved to %s\n", first_word, alias_commands[i]);
            strcpy(resolved_command, alias_commands[i]);

            char *rest_of_cmd = strtok(NULL, "\n");
            if (rest_of_cmd != NULL)
            {
                strcat(resolved_command, " ");
                strcat(resolved_command, rest_of_cmd);
            }

            return resolved_command;
        }
    }

    return cmd;
}

#define MAX_SIZE 1024
