#ifndef PARSE_H
#define PARSE_H

#include "all_directories.h"

void parse_commands(char *input, char *newHomeDirectory, char *previousDirectory, char command_greater_than_2[]);
void execute_the_sub_command(char *command, int is_background, char *newHomeDirectory, char *previousDirectory, int *counter_of_and, char command_greater_than_2[]);

#endif