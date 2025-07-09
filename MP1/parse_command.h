// #ifndef PARSE_H
// #define PARSE_H

// #include "all_directories.h"
// #define MAX_PROCESSES 100

// // typedef struct
// // {
// //     char command[MAX_SIZE];
// //     pid_t pid;
// //     char state[10]; // "Running" or "Stopped"
// // } ProcessInfo;

// void parse_commands(char *input, char *newHomeDirectory, char *previousDirectory, char command_greater_than_2[]);
// void execute_the_sub_command(char *command, int is_background, char *newHomeDirectory, char *previousDirectory, int *counter_of_and, char command_greater_than_2[]);
// void handle_background_processes();
// void print_activities();
// void handle_sigtstp(int sig);
// void handle_sigquit();
// void handle_sigint(int sig);
// void ping(pid_t pid, int signal_number);
// void add_process(const char *command, pid_t pid, const char *state);
// void update_process_status();
// int fg_command(int pid);
// void ping(int pid, int signal);
// ProcessInfo *finding_in_the_proc_file(int pid);

// #endif

#ifndef PARSE_COMMAND_H
#define PARSE_COMMAND_H

#include "all_directories.h"

void parse_commands(char *input, char *newHomeDirectory, char *previousDirectory, char command_greater_than_2[]);
void execute_the_sub_command(char *command, int is_background, char *newHomeDirectory, char *previousDirectory, int *counter_of_and, char command_greater_than_2[]);
void handle_background_processes();
void print_activities();
void handle_sigtstp(int sig);
void handle_sigquit();
void handle_sigint(int sig);
void ping(pid_t pid, int signal_number);
void add_process(const char *command, pid_t pid, const char *state);
void update_process_status();
int fg_command(int pid);
int bg_command(int pid);
// ProcessInfo *finding_in_the_proc_file(int pid);
void handle_IO_redirection(char *command, int is_background, char *newHomeDirectory, char *previousDirectory, int *counter_of_and, char command_greater_than_2[]);
void solve_pipes(char *command_sequence, int is_background, char *newHomeDirectory, char *previousDirectory, int *counter_of_and, char command_greater_than_2[]);
// ProcessInfo *finding_in_the_proc_file(int pid); // Declaration only, defined elsewhere

#endif // PARSE_COMMAND_H
