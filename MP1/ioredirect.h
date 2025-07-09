#ifndef IOREDIRECT_H
#define IOREDIRECT_H

#include "all_directories.h"

void output_back_to_normal(int duplicate_stdout, int *file_fd);
int output_redirect_to_file(char *file_name, int *file_fd, int mode);
char **parse_command(char *command);
char **tokenise_the_input_redirect(char *command, int *num_tokens);
char **tokenise_the_output_redirect(char *command, int *num_tokens);
int input_redirect_to_file(char *file_name, int *file_fd);
void input_back_to_normal(int duplicate_stdin, int *file_fd);

#endif
