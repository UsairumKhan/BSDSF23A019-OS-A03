#include "shell.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* Internal (assignment) history storage */
static char internal_history[HISTORY_SIZE][MAX_LEN];
static int internal_history_count = 0;

/* ----------------------------
   read_cmd - wrapper for readline()
   ---------------------------- */
char* read_cmd(char* prompt) {
    char* line = readline(prompt);   /* readline allocates the returned string */

    if (line == NULL)                /* Ctrl+D or EOF */
        return NULL;

    if (strlen(line) > 0) {
        /* Add to readline history (so up/down works) */
        add_history(line);

        /* Also save to our internal history (for 'history' builtin and !n) */
        internal_history_add(line);
    }

    return line; /* caller must free line when done */
}

/* ----------------------------
   tokenize - split input into argv-like array
   Caller must free the returned char** and each string.
   ---------------------------- */
char** tokenize(char* cmdline) {
    if (cmdline == NULL || cmdline[0] == '\0' || cmdline[0] == '\n') {
        return NULL;
    }

    char** arglist = (char**)malloc(sizeof(char*) * (MAXARGS + 1));
    for (int i = 0; i < MAXARGS + 1; i++) {
        arglist[i] = (char*)malloc(sizeof(char) * ARGLEN);
        bzero(arglist[i], ARGLEN);
    }

    char* cp = cmdline;
    char* start;
    int len;
    int argnum = 0;

    while (*cp != '\0' && argnum < MAXARGS) {
        while (*cp == ' ' || *cp == '\t') cp++; /* skip whitespace */
        if (*cp == '\0') break;
        start = cp;
        len = 1;
        while (*++cp != '\0' && !(*cp == ' ' || *cp == '\t')) len++;
        strncpy(arglist[argnum], start, len);
        arglist[argnum][len] = '\0';
        argnum++;
    }

    if (argnum == 0) {
        for (int i = 0; i < MAXARGS + 1; i++) free(arglist[i]);
        free(arglist);
        return NULL;
    }

    arglist[argnum] = NULL;
    return arglist;
}

/* ----------------------------
   Built-in command handler
   Return 1 if command was handled (built-in), 0 otherwise.
   ---------------------------- */
int handle_builtin(char** arglist) {
    if (arglist == NULL || arglist[0] == NULL)
        return 0;

    /* exit */
    if (strcmp(arglist[0], "exit") == 0) {
        printf("Exiting myshell...\n");
        exit(0);
    }

    /* cd */
    else if (strcmp(arglist[0], "cd") == 0) {
        if (arglist[1] == NULL) {
            fprintf(stderr, "cd: expected argument\n");
        } else if (chdir(arglist[1]) != 0) {
            perror("cd failed");
        }
        return 1;
    }

    /* help */
    else if (strcmp(arglist[0], "help") == 0) {
        printf("Built-in commands:\n");
        printf("  cd <dir>   - Change directory\n");
        printf("  exit       - Exit the shell\n");
        printf("  help       - Show this help message\n");
        printf("  jobs       - Show background jobs (not yet implemented)\n");
        printf("  history    - Show command history (internal)\n");
        printf("  !n         - Re-execute nth command from history\n");
        return 1;
    }

    /* jobs (placeholder) */
    else if (strcmp(arglist[0], "jobs") == 0) {
        printf("Job control not yet implemented.\n");
        return 1;
    }

    /* history builtin (uses internal history) */
    else if (strcmp(arglist[0], "history") == 0) {
        internal_history_show();
        return 1;
    }

    return 0; /* not a built-in */
}

/* ----------------------------
   internal_history_add - keep an internal circular/shifted buffer
   ---------------------------- */
void internal_history_add(const char* cmdline) {
    if (cmdline == NULL || strlen(cmdline) == 0)
        return;

    if (internal_history_count < HISTORY_SIZE) {
        strncpy(internal_history[internal_history_count], cmdline, MAX_LEN - 1);
        internal_history[internal_history_count][MAX_LEN - 1] = '\0';
        internal_history_count++;
    } else {
        /* remove oldest (shift left) and append new command at the end */
        for (int i = 1; i < HISTORY_SIZE; i++) {
            strncpy(internal_history[i - 1], internal_history[i], MAX_LEN);
        }
        strncpy(internal_history[HISTORY_SIZE - 1], cmdline, MAX_LEN - 1);
        internal_history[HISTORY_SIZE - 1][MAX_LEN - 1] = '\0';
    }
}

/* ----------------------------
   internal_history_show - print internal history with numbers
   ---------------------------- */
void internal_history_show() {
    for (int i = 0; i < internal_history_count; i++) {
        printf("%d %s\n", i + 1, internal_history[i]);
    }
}

/* ----------------------------
   internal_history_get - return pointer to internal history string
   index is 1-based.
   ---------------------------- */
char* internal_history_get(int index) {
    if (index < 1 || index > internal_history_count)
        return NULL;
    return internal_history[index - 1];
}
