#ifndef ALIASES_H
#define ALIASES_H

#include <ctype.h>
#include <stdio.h>
#include <string.h>

#define MAX_ALIASES 1000
#define MAX_LINE_LENGTH 1024
#define MAX_FUNCTIONS 100

// extern char *alias_names[MAX_ALIASES];
// extern char *alias_commands[MAX_ALIASES];
// extern int alias_count;

char *trim_whitespace(char *str);
void load_myshrc();
void create_alias(char *alias_definition);
void save_alias_to_file(char *alias_name, char *command);
char *resolve_alias(char *cmd);
char *resolve_function(char *cmd);

#endif // ALIASES_H
