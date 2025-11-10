#include "shell.h"

int execute(char* arglist[], int background) {
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork failed");
        return 1;
    }

    if (pid == 0) {
        /* Child process */
        if (background) {
            setpgid(0, 0);  // detach from terminal
        }
        execvp(arglist[0], arglist);
        perror("Command not found");
        exit(1);
    } else {
        /* Parent process */
        if (background) {
            printf("[Background PID %d] %s\n", pid, arglist[0]);
            add_job(pid, arglist[0]);
        } else {
            waitpid(pid, NULL, 0);
        }
    }

    return 0;
}
