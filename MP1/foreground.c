#include "all_directories.h"

void foreground(char *command, char *newHomeDirectory, char *previousDirectory, char command_greater_than_2[])
{
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

    char currentDirectory[MAX_SIZE_OF_NAMES];
    getcwd(currentDirectory, MAX_SIZE_OF_NAMES);

    int index = 1;

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    if (strcmp("hop", args[0]) == 0)
    {
        if (args[1] == NULL)
        {
            char temp[1000] = {0};
            getcwd(temp, sizeof(temp));

            strcpy(previousDirectory, temp);

            if (chdir(newHomeDirectory) != 0)
            {
                perror("chdir");
            }

            printf("%s\n", newHomeDirectory);
        }
        else
        {
            while (args[index] != NULL)
            {
                char *yo = args[index++];

                if (strcmp(yo, "..") == 0 || strcmp(yo, ".") == 0)
                {
                    char temp1[1000] = {0};
                    getcwd(temp1, sizeof(temp));

                    if (strcmp(yo, ".") != 0)
                        strcpy(previousDirectory, temp1);

                    if (chdir(yo) != 0)
                        perror("chdir");

                    char temp[1000] = {0};
                    getcwd(temp, sizeof(temp));

                    printf("%s\n", temp);
                }
                else if (yo[0] == '~')
                {
                    char temp1[1000] = {0};
                    getcwd(temp1, sizeof(temp));
                    strcpy(previousDirectory, temp1);

                    char path[MAX_SIZE];
                    if (strlen(yo) == 1)
                    {
                        strcpy(path, newHomeDirectory);
                    }
                    else
                    {
                        snprintf(path, sizeof(path), "%s%s", newHomeDirectory, yo + 1);
                    }

                    if (chdir(path) != 0)
                        perror("chdir");

                    char temp[1000] = {0};
                    getcwd(temp, sizeof(temp));

                    printf("%s\n", temp);
                }
                else if (strcmp(yo, "-") == 0)
                {
                    char temp1[1000] = {0};
                    getcwd(temp1, sizeof(temp));

                    if (strlen(previousDirectory) == 0)
                    {
                        printf("OLDPWD not set\n");
                        return;
                    }

                    if (chdir(previousDirectory) != 0)
                        perror("chdir");

                    char temp[1000] = {0};
                    getcwd(temp, sizeof(temp));

                    printf("%s\n", temp);

                    strcpy(previousDirectory, temp1);
                }
                else
                {
                    char temp1[1000] = {0};
                    getcwd(temp1, sizeof(temp));
                    strcpy(previousDirectory, temp1);

                    if (chdir(yo) != 0)
                        perror("chdir");

                    char temp[1000] = {0};
                    getcwd(temp, sizeof(temp));

                    printf("%s\n", temp);
                }
            }
        }
    }
    else if (strcmp("reveal", args[0]) == 0)
    {
        int a_found = 0;
        int l_found = 0;

        int pathIndex = 1;
        while (args[pathIndex] != NULL && args[pathIndex][0] == '-')
        {
            if (args[2] == NULL)
            {
                for (int p = 1; args[pathIndex][p] != '\0'; p++)
                {
                    if (args[1][p] == 'a')
                        a_found = 1;
                    if (args[1][p] == 'l')
                        l_found = 1;
                }

                if (a_found == 0 && l_found == 0)
                {
                }
                else
                {
                    pathIndex++;
                }
                
                break;
            }
            else
            {
                for (int p = 1; args[pathIndex][p] != '\0'; p++)
                {
                    if (args[1][p] == 'a')
                        a_found = 1;
                    if (args[1][p] == 'l')
                        l_found = 1;
                }

                pathIndex++;
            }
        }

        // printf("Pathindex %d\n", pathIndex);

        char *pathToReveal;

        if (args[pathIndex] == NULL)
        {
            pathToReveal = currentDirectory;
            // strcmp(pathToReveal, currentDirectory);
        }
        else if (strcmp(args[pathIndex], "-") == 0)
        {
            // printf("HI\n");

            if (strlen(previousDirectory) == 0)
            {
                printf("OLDPWD not set\n");
                return;
            }

            pathToReveal = previousDirectory;

            // strcmp(pathToReveal, previousDirectory);
        }
        else if (args[pathIndex][0] == '~')
        {
            static char expandedPath[MAX_SIZE];
            snprintf(expandedPath, sizeof(expandedPath), "%s%s", newHomeDirectory, args[pathIndex] + 1);
            pathToReveal = expandedPath;
            // strcmp(pathToReveal, expandedPath);
        }
        else
        {
            pathToReveal = args[pathIndex];
            // strcmp(pathToReveal, args[pathIndex]);
        }

        reveal(pathToReveal, a_found, l_found);
    }
    else if (strcmp(args[0], "log") == 0)
    {
        int log_flag = 1;

        if (args[1] == NULL)
        {
        }
        else if (strcmp(args[1], "purge") == 0)
        {

            if (remove(LOG_FILE_PATH) != 0)
            {
                printf("Error deleting file!\n");
            }
            else
            {
                printf("File deleted successfully.\n");
            }
        }
        else if (strcmp(args[1], "execute") == 0)
        {
            char newCommand[4096];

            int number = atoi(args[2]);

            log_execute(newCommand, number);

            // strcat(newCommand, " ; ");
            // strcat(newCommand, "usleep(100)");

            // printf("%s\n", newCommand);
            // // pr

            parse_commands(newCommand, newHomeDirectory, previousDirectory, command_greater_than_2);
        }
    }
    else if (strcmp(args[0], "proclore") == 0)
    {
        if (args[1] == NULL)
        {
            int process_id = getpid();

            get_process_info(process_id, currentDirectory, newHomeDirectory);
        }
        else
        {
            int process_id = atoi(args[1]);
            get_process_info(process_id, currentDirectory, newHomeDirectory);
        }
    }
    else if (strcmp(args[0], "seek") == 0)
    {
        if (args[1] == NULL)
        {
            printf("Invalid Input\n");
            return;
        }

        int e_flag = 0;
        int d_flag = 0;
        int f_flag = 0;

        int check = 1;

        for (int p = 1; p < i; p++)
        {
            if (args[p][0] == '-')
            {
                if (args[p][1] == 'e')
                    e_flag = 1;
                if (args[p][1] == 'd')
                    d_flag = 1;
                if (args[p][1] == 'f')
                    f_flag = 1;

                check++;
            }
        }

        if (d_flag && f_flag)
        {
            printf("Invalid flags!\n");
            return;
        }

        if (e_flag)
        {
            char file_name[1000] = {0};
            strcpy(file_name, args[check]);

            check++;

            int file_count = 0;
            int dir_count = 0;

            char something[1000] = {0};

            if (args[check])
            {
                if (args[check][0] == '~' || args[check][0] == '.')
                {
                    char path[MAX_SIZE];

                    if (strlen(args[check]) == 1)
                    {
                        strcpy(path, newHomeDirectory);
                    }
                    else
                    {
                        snprintf(path, sizeof(path), "%s%s", newHomeDirectory, args[check] + 1);
                    }

                    search_directory(path, file_name, currentDirectory, newHomeDirectory, d_flag, f_flag, e_flag, &file_count, &dir_count, something);
                }
            }
            else
                search_directory(currentDirectory, file_name, currentDirectory, newHomeDirectory, d_flag, f_flag, e_flag, &file_count, &dir_count, something);

            if (d_flag)
            {
                if (dir_count == 1)
                {
                    char temp[1000] = {0};
                    getcwd(temp, 1000);

                    strcpy(previousDirectory, temp);

                    if (chdir(something) != 0)
                    {
                        perror("chdir");
                    }

                    if (strncmp(something, newHomeDirectory, strlen(newHomeDirectory)) == 0)
                    {
                        printf("%s\n", something + strlen(newHomeDirectory));
                    }
                    else
                    {
                        printf(".%s\n", something);
                    }
                }
                else if (dir_count == 0)
                {
                    printf("No Directory Found\n");
                }
                else
                {
                    printf("More than 1 Directory found\n");
                }
            }
            else if (f_flag)
            {
                if (file_count == 1)
                {
                    FILE *p;
                    char buffer[1000];

                    p = fopen(something, "r");
                    if (p == NULL)
                    {
                        printf("Error: Could not open file %s\n", something);
                    }

                    while (fgets(buffer, sizeof(buffer), p) != NULL)
                    {
                        printf("%s", buffer);
                    }

                    printf("\n");

                    fclose(p);
                }
                else if (file_count == 0)
                {
                    printf("No File Found\n");
                }
                else
                {
                    printf("More than 1 file found\n");
                }
            }
            else
            {
                if (dir_count == 1 && file_count == 0)
                {
                    char temp[1000] = {0};
                    getcwd(temp, 1000);

                    strcpy(previousDirectory, temp);

                    if (chdir(something) != 0)
                    {
                        perror("chdir");
                    }

                    if (strncmp(something, newHomeDirectory, strlen(newHomeDirectory)) == 0)
                    {
                        printf("%s\n", something + strlen(newHomeDirectory));
                    }
                    else
                    {
                        printf(".%s\n", something);
                    }
                }
                else if (file_count == 1 && dir_count == 0)
                {
                    FILE *p;
                    char buffer[1000];

                    p = fopen(something, "r");
                    if (p == NULL)
                    {
                        printf("Error: Could not open file %s\n", something);
                    }

                    while (fgets(buffer, sizeof(buffer), p) != NULL)
                    {
                        printf("%s", buffer);
                    }

                    printf("\n");

                    fclose(p);
                }
                else
                {
                    printf("No effect\n");
                }
            }
        }
        else
        {
            char file_name[1000] = {0};
            strcpy(file_name, args[check]);

            check++;

            int file_count = 0;
            int dir_count = 0;

            char something[1000] = {0};

            if (args[check])
            {
                if (args[check][0] == '~' || args[check][0] == '.')
                {
                    char path[MAX_SIZE];

                    if (strlen(args[check]) == 1)
                    {
                        strcpy(path, newHomeDirectory);
                    }
                    else
                    {
                        snprintf(path, sizeof(path), "%s%s", newHomeDirectory, args[check] + 1);
                    }

                    search_directory(path, file_name, currentDirectory, newHomeDirectory, d_flag, f_flag, e_flag, &file_count, &dir_count, something);
                }
            }
            else
                search_directory(currentDirectory, file_name, currentDirectory, newHomeDirectory, d_flag, f_flag, e_flag, &file_count, &dir_count, something);

            if ((dir_count) + (file_count) == 0)
                printf("No match found!\n");
        }
    }
    else if (strcmp(args[0], "sleep") == 0)
    {
        if (args[2])
            printf("Invalid command\n");

        sleep(atoi(args[1]));

        clock_gettime(CLOCK_MONOTONIC, &end);

        double time_spent = (end.tv_sec - start.tv_sec) +
                            (end.tv_nsec - start.tv_nsec) / 1E9;

        // printf("QWERUWEIFWEIFUIWEUF\n");

        if ((int)time_spent > 2)
        {
            int index_args = 1;
            strcpy(command_greater_than_2, args[0]);
            while (args[index_args])
            {
                strcat(command_greater_than_2, " ");
                strcat(command_greater_than_2, args[index_args++]);
            }

            strcat(command_greater_than_2, "s");
        }
    }
    else
    {
        int new_foreground_fork = fork();

        if (new_foreground_fork < 0)
        {
            perror("fork failed\n");
        }
        else if (new_foreground_fork == 0)
        {
            if (execvp(args[0], args) < 0)
            {
                printf("ERROR : '%s' is not a valid command\n", args[0]);
            }
            else
            {
                perror("Command not Found");
            }
        }
        else
        {
            wait(NULL);

            clock_gettime(CLOCK_MONOTONIC, &end);

            double time_spent = (end.tv_sec - start.tv_sec) +
                                (end.tv_nsec - start.tv_nsec) / 1E9;

            if ((int)time_spent > 2)
            {
                int index_args = 1;
                strcpy(command_greater_than_2, args[0]);
                while (args[index_args])
                {
                    strcat(command_greater_than_2, " ");
                    strcat(command_greater_than_2, args[index_args++]);
                }

                strcat(command_greater_than_2, "s");
            }
        }
    }
}