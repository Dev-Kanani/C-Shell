#include "all_directories.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

int input_redirect_to_file(char *file_name, int *file_fd)
{
    *file_fd = open(file_name, O_RDONLY);
    if (*file_fd < 0)
    {
        perror("Failed to open file");
        // exit(EXIT_FAILURE);
    }

    int duplicate_stdin = dup(0);
    if (duplicate_stdin == -1)
    {
        perror("Error in duplicating stdin");
        close(*file_fd);
        // exit(1);
    }

    int input_changed = dup2(*file_fd, 0);
    if (input_changed == -1)
    {
        perror("Failed to redirect input");
        close(*file_fd);
        // exit(1);
    }

    return duplicate_stdin;
}

void input_back_to_normal(int duplicate_stdin, int *file_fd)
{
    fflush(stdin);
    if (dup2(duplicate_stdin, 0) == -1)
    {
        perror("Failed to restore stdin");
        close(*file_fd);
        // exit(1);
    }
    close(duplicate_stdin);
}

int output_redirect_to_file(char *file_name, int *file_fd, int mode)
{
    if (mode == 0)
    {
        *file_fd = open(file_name, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    }
    else if (mode == 1)
    {
        *file_fd = open(file_name, O_WRONLY | O_CREAT | O_APPEND, 0644);
    }

    if (*file_fd < 0)
    {
        perror("Failed to open file");
        // exit(EXIT_FAILURE);
    }

    int duplicate_stdout = dup(1);
    if (duplicate_stdout == -1)
    {
        perror("Error in duplicating stdout");
        close(*file_fd);
        // exit(1);
    }

    int output_changed = dup2(*file_fd, 1);
    if (output_changed == -1)
    {
        perror("Failed to redirect output");
        close(*file_fd);
        // exit(1);
    }

    return duplicate_stdout;
}

void output_back_to_normal(int duplicate_stdout, int *file_fd)
{
    fflush(stdout);
    if (dup2(duplicate_stdout, 1) == -1)
    {
        perror("Failed to restore stdout");
        close(*file_fd);
        // exit(1);
    }
    close(duplicate_stdout);
}

char **tokenise_the_input_redirect(char *command, int *num_tokens)
{
    char **args = malloc(MAX_SIZE_OF_NAMES * sizeof(char *));
    char *token;
    const char delim[] = "<";
    int index = 0;

    token = strtok(command, delim);
    while (token != NULL)
    {
        if (strlen(token) > 0)
        {
            args[index] = malloc(strlen(token) + 1);
            strcpy(args[index], token);
            index++;
        }
        token = strtok(NULL, delim);
    }

    args[index] = NULL;
    *num_tokens = index;
    return args;
}

char **tokenise_the_output_redirect(char *command, int *num_tokens)
{
    char **args = malloc(MAX_SIZE_OF_NAMES * sizeof(char *));
    char *token;
    const char delim[] = ">";
    int index = 0;

    token = strtok(command, delim);
    while (token != NULL)
    {
        if (strlen(token) > 0)
        {
            args[index] = malloc(strlen(token) + 1);
            strcpy(args[index], token);
            index++;
        }
        token = strtok(NULL, delim);
    }

    args[index] = NULL;
    *num_tokens = index;
    return args;
}

char **parse_command(char *command)
{
    char **args = malloc(MAX_SIZE_OF_NAMES * sizeof(char *));
    char *token;
    int index = 0;

    token = strtok(command, " ");
    while (token != NULL)
    {
        args[index] = malloc(strlen(token) + 1);
        strcpy(args[index], token);
        index++;
        token = strtok(NULL, " ");
    }

    args[index] = NULL;
    return args;
}