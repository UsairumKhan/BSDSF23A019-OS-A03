#include "shell.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int main() {
    char* cmdline;
    char** arglist;

    while ((cmdline = read_cmd(PROMPT)) != NULL) {

        /* Handle command recall: !n using internal history */
        if (cmdline[0] == '!' && strlen(cmdline) > 1) {
            int index = atoi(cmdline + 1);
            char* recalled = internal_history_get(index);
            if (recalled) {
                printf("%s\n", recalled);
                free(cmdline);
                cmdline = strdup(recalled); /* duplicate for tokenization and later free */
            } else {
                printf("No such command in history.\n");
                free(cmdline);
                continue;
            }
        }

        if ((arglist = tokenize(cmdline)) != NULL) {

            /* handle builtins first */
            if (!handle_builtin(arglist)) {
                execute(arglist);
            }

            /* free allocated arglist memory */
            for (int i = 0; arglist[i] != NULL; i++)
                free(arglist[i]);
            free(arglist);
        }

        free(cmdline); /* free string returned by readline() or strdup() */
    }

    printf("\nmyshell exited.\n");
    return 0;
}
