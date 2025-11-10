#include "shell.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int execute(char* arglist[]) {
    int status;
    int cpid = fork();

    switch (cpid) {
        case -1:
            perror("fork failed");
            exit(1);
        case 0: /* child */
            execvp(arglist[0], arglist);
            perror("Command not found");
            exit(1);
        default: /* parent */
            waitpid(cpid, &status, 0);
            return 0;
    }
}
