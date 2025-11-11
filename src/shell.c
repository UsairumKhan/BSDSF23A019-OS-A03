#include "shell.h"

/* ----------------------------
   Internal history
   ---------------------------- */
static char internal_history[HISTORY_SIZE][MAX_LEN];
static int internal_history_count = 0;

/* Readline wrapper */
char* read_cmd(char* prompt) {
    char* line = readline(prompt);
    if (line == NULL)
        return NULL;
    if (strlen(line) > 0) {
        add_history(line);
        internal_history_add(line);
    }
    return line;
}

/* Tokenizer */
char** tokenize(char* cmdline) {
    if (cmdline == NULL || cmdline[0] == '\0' || cmdline[0] == '\n')
        return NULL;

    char** arglist = malloc(sizeof(char*) * (MAXARGS + 1));
    for (int i = 0; i < MAXARGS + 1; i++) {
        arglist[i] = malloc(ARGLEN);
        bzero(arglist[i], ARGLEN);
    }

    char* token = strtok(cmdline, " \t");
    int i = 0;
    while (token != NULL && i < MAXARGS) {
        strncpy(arglist[i++], token, ARGLEN - 1);
        token = strtok(NULL, " \t");
    }
    arglist[i] = NULL;
    return arglist;
}

/* Built-ins */
int handle_builtin(char** arglist) {
    if (arglist == NULL || arglist[0] == NULL)
        return 0;

    if (strcmp(arglist[0], "exit") == 0) {
        printf("Exiting myshell...\n");
        exit(0);
    } else if (strcmp(arglist[0], "cd") == 0) {
        if (arglist[1] == NULL)
            fprintf(stderr, "cd: expected argument\n");
        else if (chdir(arglist[1]) != 0)
            perror("cd failed");
        return 1;
    } else if (strcmp(arglist[0], "help") == 0) {
        printf("Built-in commands:\n");
        printf("  cd <dir>   - Change directory\n");
        printf("  exit       - Exit the shell\n");
        printf("  help       - Show help\n");
        printf("  history    - Show command history\n");
        printf("  !n         - Re-execute nth history command\n");
        return 1;
    } else if (strcmp(arglist[0], "history") == 0) {
        internal_history_show();
        return 1;
    }
    return 0;
}

/* Internal history */
void internal_history_add(const char* cmdline) {
    if (!cmdline || !*cmdline) return;
    if (internal_history_count < HISTORY_SIZE) {
        strncpy(internal_history[internal_history_count++], cmdline, MAX_LEN - 1);
    } else {
        for (int i = 1; i < HISTORY_SIZE; i++)
            strncpy(internal_history[i - 1], internal_history[i], MAX_LEN);
        strncpy(internal_history[HISTORY_SIZE - 1], cmdline, MAX_LEN - 1);
    }
}

void internal_history_show() {
    for (int i = 0; i < internal_history_count; i++)
        printf("%d %s\n", i + 1, internal_history[i]);
}

char* internal_history_get(int index) {
    if (index < 1 || index > internal_history_count)
        return NULL;
    return internal_history[index - 1];
}
