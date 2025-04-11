#ifndef REVEAL_H
#define REVEAL_H

#include "all_directories.h"

#define GREEN "\033[0;32m"
#define BLUE "\033[0;34m"
#define WHITE "\033[0;37m"
#define RESET "\033[0m"

void print_permissions(int mode);
void print_color(struct stat fileStat);
void print_file_details(struct stat fileStat, char *fileName);
void reveal(char *path, int showAll, int showLong);

#endif
