#include "shell.h"

int main() {
    char* cmdline;
    char** arglist;

    while ((cmdline = read_cmd(PROMPT, stdin)) != NULL) {

        // Handle command recall: !n
        if (cmdline[0] == '!' && strlen(cmdline) > 1) {
            int index = atoi(cmdline + 1);
            char* recalled = get_history_command(index);
            if (recalled) {
                printf("%s\n", recalled);
                free(cmdline);
                cmdline = strdup(recalled); // Reuse recalled command
            } else {
                printf("No such command in history.\n");
                free(cmdline);
                continue;
            }
        }

        if ((arglist = tokenize(cmdline)) != NULL) {

            // Add command to history
            add_history(cmdline);

            // Handle built-ins or execute external command
            if (!handle_builtin(arglist)) {
                execute(arglist);
            }

            // Free memory
            for (int i = 0; arglist[i] != NULL; i++)
                free(arglist[i]);
            free(arglist);
        }

        free(cmdline);
    }

    printf("\nmyshell exited.\n");
    return 0;
}
