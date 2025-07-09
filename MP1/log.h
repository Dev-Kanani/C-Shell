#ifndef LOG_H
#define LOG_H

#include "all_directories.h"

void log_command(char *command, int *flag, int execute_flag);
void log_execute(char *newCommand, int number);

#endif