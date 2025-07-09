#ifndef NEONATE_H
#define NEONATE_H

#include "all_directories.h"

int get_latest_pid();
void enable_nonblocking_input();
void disable_nonblocking_input();
int check_key_press();
void neonate(int time_arg);

#endif