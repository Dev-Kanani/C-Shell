#include "all_directories.h"
#include "reveal.h"

void print_permissions(int mode)
{
    if (S_ISDIR(mode))
        printf("d");
    else
        printf("-");

    if (mode & S_IRUSR)
        printf("r");
    else
        printf("-");

    if (mode & S_IWUSR)
        printf("w");
    else
        printf("-");

    if (mode & S_IXUSR)
        printf("x");
    else
        printf("-");

    if (mode & S_IRGRP)
        printf("r");
    else
        printf("-");

    if (mode & S_IWGRP)
        printf("w");
    else
        printf("-");

    if (mode & S_IXGRP)
        printf("x");
    else
        printf("-");

    if (mode & S_IROTH)
        printf("r");
    else
        printf("-");

    if (mode & S_IWOTH)
        printf("w");
    else
        printf("-");

    if (mode & S_IXOTH)
        printf("x");
    else
        printf("-");
}

void print_color(struct stat fileStat)
{
    if (S_ISDIR(fileStat.st_mode))
        printf(BLUE);

    else if (fileStat.st_mode & S_IXUSR)
        printf(GREEN);

    else
        printf(WHITE);
}

void print_file_details(struct stat fileStat, char *fileName)
{
    print_permissions(fileStat.st_mode);

    printf(" %ld", fileStat.st_nlink);
    printf(" %s", getpwuid(fileStat.st_uid)->pw_name);
    printf(" %s", getgrgid(fileStat.st_gid)->gr_name);
    printf(" %5ld", fileStat.st_size);
    char time[20];
    strftime(time, sizeof(time), "%b %d %H:%M", localtime(&fileStat.st_mtime));
    printf(" %s", time);
    print_color(fileStat);
    printf(" %s\n", fileName);
    printf(RESET);
}

int compare(const void *a, const void *b)
{
    struct dirent *entryA = *(struct dirent **)a;
    struct dirent *entryB = *(struct dirent **)b;
    return strcmp(entryA->d_name, entryB->d_name);
}

void reveal(char *path, int showAll, int showLong)
{
    struct dirent *entry;
    struct dirent **entryList;
    struct stat fileStat;
    DIR *dir = opendir(path);
    int fileCount = 0;
    int totalBlocks = 0;

    if (dir == NULL)
    {
        perror("opendir");
        return;
    }

    while ((entry = readdir(dir)) != NULL)
    {
        if (!showAll && entry->d_name[0] == '.')
        {
        continue;
        }

        char fullPath[1024];
        snprintf(fullPath, sizeof(fullPath), "%s/%s", path, entry->d_name);

        if (stat(fullPath, &fileStat) < 0)
        {
            perror("stat");
            continue;
        }

        fileCount++;
        totalBlocks += fileStat.st_blocks;
    }

    printf("total %d\n", totalBlocks / 2);

    rewinddir(dir);

    entryList = malloc(fileCount * sizeof(struct dirent *));
    if (entryList == NULL)
    {
        perror("malloc");
        closedir(dir);
        return;
    }

    int i = 0;
    while ((entry = readdir(dir)) != NULL)
    {
        if (!showAll && entry->d_name[0] == '.')
        {
            continue;
        }

        entryList[i] = entry;
        i++;
    }

    qsort(entryList, fileCount, sizeof(struct dirent *), compare);

    for (i = 0; i < fileCount; i++)
    {
        char fullPath[1024];
        snprintf(fullPath, sizeof(fullPath), "%s/%s", path, entryList[i]->d_name);

        if (stat(fullPath, &fileStat) < 0)
        {
            perror("stat");
            continue;
        }

        if (showLong)
        {
            print_file_details(fileStat, entryList[i]->d_name);
        }
        else
        {
            print_color(fileStat);
            printf("%s\n", entryList[i]->d_name);
            printf(RESET);
        }
    }

    free(entryList);
    closedir(dir);
}