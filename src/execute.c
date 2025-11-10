#include "shell.h"

int execute(char* arglist[]) {
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork failed");
        return 1;
    }

    if (pid == 0) {  // Child process
        int in_redirect = -1, out_redirect = -1, append_redirect = -1;

        // Detect redirection symbols
        for (int i = 0; arglist[i] != NULL; i++) {
            if (strcmp(arglist[i], "<") == 0) {
                in_redirect = i;
            } else if (strcmp(arglist[i], ">") == 0) {
                out_redirect = i;
            } else if (strcmp(arglist[i], ">>") == 0) {
                append_redirect = i;
            }
        }

        // Handle input redirection (<)
        if (in_redirect != -1 && arglist[in_redirect + 1] != NULL) {
            int fd = open(arglist[in_redirect + 1], O_RDONLY);
            if (fd < 0) {
                perror("Input file error");
                exit(1);
            }
            dup2(fd, STDIN_FILENO);
            close(fd);
            arglist[in_redirect] = NULL;  // terminate arglist for exec
        }

        // Handle output redirection (>)
        if (out_redirect != -1 && arglist[out_redirect + 1] != NULL) {
            int fd = open(arglist[out_redirect + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd < 0) {
                perror("Output file error");
                exit(1);
            }
            dup2(fd, STDOUT_FILENO);
            close(fd);
            arglist[out_redirect] = NULL;
        }

        // Handle output append (>>)
        if (append_redirect != -1 && arglist[append_redirect + 1] != NULL) {
            int fd = open(arglist[append_redirect + 1], O_WRONLY | O_CREAT | O_APPEND, 0644);
            if (fd < 0) {
                perror("Append file error");
                exit(1);
            }
            dup2(fd, STDOUT_FILENO);
            close(fd);
            arglist[append_redirect] = NULL;
        }

        execvp(arglist[0], arglist);
        perror("Command not found");
        exit(1);
    } 
    else {  // Parent process
        waitpid(pid, NULL, 0);
    }

    return 0;
}
