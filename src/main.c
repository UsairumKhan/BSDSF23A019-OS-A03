#include "shell.h"

int main() {
    char* cmdline;
    char** arglist;

    while ((cmdline = read_cmd(PROMPT)) != NULL) {
        if (cmdline[0] == '!' && strlen(cmdline) > 1) {
            int index = atoi(cmdline + 1);
            char* recalled = internal_history_get(index);
            if (recalled) {
                printf("%s\n", recalled);
                free(cmdline);
                cmdline = strdup(recalled);
            } else {
                printf("No such command in history.\n");
                free(cmdline);
                continue;
            }
        }

        if ((arglist = tokenize(cmdline)) != NULL) {
            if (!handle_builtin(arglist))
                execute(arglist);

            for (int i = 0; arglist[i] != NULL; i++)
                free(arglist[i]);
            free(arglist);
        }

        free(cmdline);
    }

    printf("\nmyshell exited.\n");
    return 0;
}
