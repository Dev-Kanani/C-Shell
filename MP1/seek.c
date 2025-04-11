#include "all_directories.h"

void search_directory(char *dir_path, char *name, char *currentDirectory, char *homedirectory, int d_flag, int f_flag, int e_flag, int *file_count, int *dir_count, char *something)
{
    struct dirent *entry;
    DIR *dp = opendir(dir_path);

    if (dp == NULL)
    {
        return;
    }

    while ((entry = readdir(dp)) != NULL)
    {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        {
            continue;
        }

        struct stat statbuf;

        if (d_flag == 0 && f_flag == 0)
        {
            char path[1000] = {0};
            strcat(path, dir_path);
            strcat(path, "/");
            strcat(path, entry->d_name);

            stat(path, &statbuf);

            if (strncmp(entry->d_name, name, strlen(name)) == 0)
            {

                if (S_ISDIR(statbuf.st_mode))
                {
                    (*dir_count)++;
                }
                else if (!S_ISDIR(statbuf.st_mode))
                {
                    (*file_count)++;
                }

                if (strncmp(dir_path, homedirectory, strlen(homedirectory)) == 0)
                {
                    if (e_flag)
                    {
                        if (S_ISDIR(statbuf.st_mode) && access(path, X_OK) != 0)
                        {
                            printf("Missing permissions for task!\n");
                            closedir(dp);
                            return;
                        }
                        else if (!S_ISDIR(statbuf.st_mode) && access(path, R_OK) != 0)
                        {
                            printf("Missing permissions for task!\n");
                            closedir(dp);
                            return;
                        }
                    }

                    if (S_ISDIR(statbuf.st_mode))
                    {
                        if (!e_flag)
                            printf(BLUE "~%s", dir_path + strlen(homedirectory));
                        else
                            strcpy(something, dir_path);

                        if (!e_flag)
                            printf(BLUE "/%s\n", entry->d_name);

                        printf(RESET);
                    }
                    else if (!S_ISDIR(statbuf.st_mode))
                    {
                        if (!e_flag)
                            printf(BLUE "~%s", dir_path + strlen(homedirectory));
                        else
                            strcpy(something, dir_path);

                        if (!e_flag)
                            printf(GREEN "/%s\n", entry->d_name);

                        printf(RESET);
                    }

                    strcat(something, "/");
                    strcat(something, entry->d_name);
                }
                else
                {
                    if (e_flag)
                    {
                        if (S_ISDIR(statbuf.st_mode) && access(path, X_OK) != 0)
                        {
                            printf("Missing permissions for task!\n");
                            closedir(dp);
                            return;
                        }
                        else if (!S_ISDIR(statbuf.st_mode) && access(path, R_OK) != 0)
                        {
                            printf("Missing permissions for task!\n");
                            closedir(dp);
                            return;
                        }
                    }

                    if (S_ISDIR(statbuf.st_mode))
                    {
                        if (!e_flag)
                            printf(BLUE "%s", dir_path);
                        else
                            strcpy(something, dir_path);

                        if (!e_flag)
                            printf(BLUE "/%s\n", entry->d_name);
                    }
                    else if (!S_ISDIR(statbuf.st_mode))
                    {
                        if (!e_flag)
                            printf(BLUE "%s", dir_path);
                        else
                            strcpy(something, dir_path);

                        if (!e_flag)
                            printf(GREEN "/%s\n", entry->d_name);
                    }

                    strcat(something, "/");
                    strcat(something, entry->d_name);
                }

                printf(RESET);
            }

            if (S_ISDIR(statbuf.st_mode))
            {
                if (e_flag)
                {
                    if (S_ISDIR(statbuf.st_mode) && access(path, X_OK) != 0)
                    {
                        printf("Missing permissions for task!\n");
                        closedir(dp);
                        return;
                    }
                    else if (!S_ISDIR(statbuf.st_mode) && access(path, R_OK) != 0)
                    {
                        printf("Missing permissions for task!\n");
                        closedir(dp);
                        return;
                    }
                }

                search_directory(path, name, currentDirectory, homedirectory, d_flag, f_flag, e_flag, file_count, dir_count, something);
            }
        }
        else
        {
            char path[1000] = {0};
            strcat(path, dir_path);
            strcat(path, "/");
            strcat(path, entry->d_name);

            char dir_path_without_name[1000] = {0};
            strncpy(dir_path_without_name, path, strlen(path) - strlen(entry->d_name));

            if (stat(path, &statbuf) == 0)
            {

                if (e_flag)
                {
                    if (S_ISDIR(statbuf.st_mode) && access(path, X_OK) != 0)
                    {
                        printf("Missing permissions for task!\n");
                        closedir(dp);
                        return;
                    }
                    else if (!S_ISDIR(statbuf.st_mode) && access(path, R_OK) != 0)
                    {
                        printf("Missing permissions for task!\n");
                        closedir(dp);
                        return;
                    }
                }

                if (S_ISDIR(statbuf.st_mode) && d_flag)
                {
                    if (strncmp(entry->d_name, name, strlen(name)) == 0)
                    {
                        if (strncmp(path, homedirectory, strlen(homedirectory)) == 0)
                        {
                            if (!e_flag)
                                printf(BLUE "~%s", dir_path_without_name + strlen(homedirectory));
                            else
                                strcpy(something, path);
                        }
                        else
                        {
                            if (!e_flag)
                                printf(BLUE "%s", dir_path_without_name);
                            else
                                strcpy(something, path);
                        }

                        if (!e_flag)
                            printf(BLUE "%s" RESET "\n", entry->d_name);

                        printf(RESET);

                        (*dir_count)++;
                    }
                }
                else if (!S_ISDIR(statbuf.st_mode) && f_flag)
                {
                    if (strncmp(entry->d_name, name, strlen(name)) == 0)
                    {
                        if (strncmp(path, homedirectory, strlen(homedirectory)) == 0)
                        {
                            if (!e_flag)
                                printf(BLUE "~%s", dir_path_without_name + strlen(homedirectory));
                            else
                                strcpy(something, path);
                        }
                        else
                        {
                            if (!e_flag)
                                printf(BLUE "%s", dir_path_without_name);
                            else
                                strcpy(something, path);
                        }

                        if (!e_flag)
                            printf(GREEN "%s" RESET "\n", entry->d_name);

                        printf(RESET);

                        (*file_count)++;
                    }
                }
            }

            if (S_ISDIR(statbuf.st_mode))
            {
                if (e_flag)
                {
                    if (S_ISDIR(statbuf.st_mode) && access(path, X_OK) != 0)
                    {
                        printf("Missing permissions for task!\n");
                        closedir(dp);
                        return;
                    }
                    else if (!S_ISDIR(statbuf.st_mode) && access(path, R_OK) != 0)
                    {
                        printf("Missing permissions for task!\n");
                        closedir(dp);
                        return;
                    }
                }

                search_directory(path, name, currentDirectory, homedirectory, d_flag, f_flag, e_flag, file_count, dir_count, something);
            }
        }
    }

    closedir(dp);
}