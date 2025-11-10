#ifndef SHELL_H
#define SHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_LEN 1024
#define MAXARGS 100
#define ARGLEN 100
#define PROMPT "myshell> "
#define HISTORY_SIZE 50  // store last 50 commands

// Function declarations
char* read_cmd(char* prompt, FILE* fp);
char** tokenize(char* cmdline);
int execute(char* arglist[]);
int handle_builtin(char** arglist);
void add_history(const char* cmdline);
void show_history();
char* get_history_command(int index);

#endif
