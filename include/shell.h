#ifndef SHELL_H
#define SHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <readline/readline.h>
#include <readline/history.h>

#define MAX_LEN 1024
#define MAXARGS 100
#define ARGLEN 100
#define PROMPT "myshell> "
#define HISTORY_SIZE 50

/* Function declarations */
char* read_cmd(char* prompt);
char** tokenize(char* cmdline);
int execute(char* arglist[]);
int handle_builtin(char** arglist);

void internal_history_add(const char* cmdline);
void internal_history_show();
char* internal_history_get(int index);

#endif
