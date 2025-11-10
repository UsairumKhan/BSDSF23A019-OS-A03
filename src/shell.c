#include "shell.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static char history[HISTORY_SIZE][MAX_LEN];
static int history_count = 0;

// ----------------------------
// Read a command line from user
// ----------------------------
char* read_cmd(char* prompt, FILE* fp) {
    printf("%s", prompt);
    char* cmdline = (char*) malloc(sizeof(char) * MAX_LEN);
    int c, pos = 0;

    while ((c = getc(fp)) != EOF) {
        if (c == '\n') break;
        cmdline[pos++] = c;
    }

    if (c == EOF && pos == 0) {
        free(cmdline);
        return NULL; // Handle Ctrl+D
    }

    cmdline[pos] = '\0';
    return cmdline;
}

// ----------------------------
// Tokenize command into arguments
// ----------------------------
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
        while (*cp == ' ' || *cp == '\t') cp++;
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

// ----------------------------
// Handle built-in commands
// ----------------------------
int handle_builtin(char** arglist) {
    if (arglist[0] == NULL)
        return 0;

    if (strcmp(arglist[0], "exit") == 0) {
        printf("Exiting myshell...\n");
        exit(0);
    }

    else if (strcmp(arglist[0], "cd") == 0) {
        if (arglist[1] == NULL)
            fprintf(stderr, "cd: expected argument\n");
        else if (chdir(arglist[1]) != 0)
            perror("cd failed");
        return 1;
    }

    else if (strcmp(arglist[0], "help") == 0) {
        printf("Built-in commands:\n");
        printf("  cd <dir>   - Change directory\n");
        printf("  exit       - Exit the shell\n");
        printf("  help       - Show this help message\n");
        printf("  jobs       - Show background jobs (not yet implemented)\n");
        printf("  history    - Show command history\n");
        printf("  !n         - Re-execute nth command from history\n");
        return 1;
    }

    else if (strcmp(arglist[0], "jobs") == 0) {
        printf("Job control not yet implemented.\n");
        return 1;
    }

    else if (strcmp(arglist[0], "history") == 0) {
        show_history();
        return 1;
    }

    return 0;
}

// ----------------------------
// Add command to history
// ----------------------------
void add_history(const char* cmdline) {
    if (cmdline == NULL || strlen(cmdline) == 0)
        return;

    if (history_count < HISTORY_SIZE) {
        strcpy(history[history_count++], cmdline);
    } else {
        // Shift commands up (remove oldest)
        for (int i = 1; i < HISTORY_SIZE; i++)
            strcpy(history[i - 1], history[i]);
        strcpy(history[HISTORY_SIZE - 1], cmdline);
    }
}

// ----------------------------
// Show history
// ----------------------------
void show_history() {
    for (int i = 0; i < history_count; i++) {
        printf("%d %s\n", i + 1, history[i]);
    }
}

// ----------------------------
// Get specific history command
// ----------------------------
char* get_history_command(int index) {
    if (index < 1 || index > history_count)
        return NULL;
    return history[index - 1];
}
