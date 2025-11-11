#include "shell.h"

/* Helper for pipeline execution */
static void execute_pipeline(char*** commands, int num_cmds, int background);

/* ------------------------------------------------------------------
   execute(): detects pipes and dispatches accordingly
   ------------------------------------------------------------------ */
int execute(char* arglist[]) {
    int background = 0;
    int pipe_count = 0;

    // Detect background and pipe symbols
    for (int i = 0; arglist[i] != NULL; i++) {
        if (strcmp(arglist[i], "|") == 0)
            pipe_count++;
        else if (strcmp(arglist[i], "&") == 0) {
            background = 1;
            arglist[i] = NULL;
        }
    }

    // ---------- Case 1: No pipes ----------
    if (pipe_count == 0) {
        pid_t pid = fork();

        if (pid == 0) {
            // --- Handle I/O redirection ---
            for (int i = 0; arglist[i] != NULL; i++) {
                if (strcmp(arglist[i], "<") == 0) {
                    int fd = open(arglist[i + 1], O_RDONLY);
                    if (fd < 0) { perror("Input file"); exit(1); }
                    dup2(fd, STDIN_FILENO);
                    close(fd);
                    arglist[i] = NULL;
                } else if (strcmp(arglist[i], ">") == 0) {
                    int fd = open(arglist[i + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
                    if (fd < 0) { perror("Output file"); exit(1); }
                    dup2(fd, STDOUT_FILENO);
                    close(fd);
                    arglist[i] = NULL;
                } else if (strcmp(arglist[i], ">>") == 0) {
                    int fd = open(arglist[i + 1], O_WRONLY | O_CREAT | O_APPEND, 0644);
                    if (fd < 0) { perror("Append file"); exit(1); }
                    dup2(fd, STDOUT_FILENO);
                    close(fd);
                    arglist[i] = NULL;
                }
            }

            execvp(arglist[0], arglist);
            perror("Command not found");
            exit(1);
        } 
        else if (pid > 0) {
            if (!background)
                waitpid(pid, NULL, 0);
            else
                printf("[Running in background: PID %d]\n", pid);
        } 
        else {
            perror("fork failed");
        }

        return 0;
    }

    // ---------- Case 2: Pipeline detected ----------
    int num_cmds = pipe_count + 1;
    char*** commands = malloc(sizeof(char**) * num_cmds);

    int cmd_start = 0;
    for (int i = 0; i <= pipe_count; i++) {
        commands[i] = malloc(sizeof(char*) * (MAXARGS + 1));
        int j = 0;

        while (arglist[cmd_start] != NULL && strcmp(arglist[cmd_start], "|") != 0) {
            commands[i][j++] = arglist[cmd_start++];
        }
        commands[i][j] = NULL;
        cmd_start++;  // skip '|'
    }

    execute_pipeline(commands, num_cmds, background);

    // Cleanup
    for (int i = 0; i < num_cmds; i++)
        free(commands[i]);
    free(commands);

    return 0;
}

/* ------------------------------------------------------------------
   execute_pipeline(): run piped commands
   ------------------------------------------------------------------ */
static void execute_pipeline(char*** commands, int num_cmds, int background) {
    int pipefds[2 * (num_cmds - 1)];

    // Create all required pipes
    for (int i = 0; i < num_cmds - 1; i++)
        if (pipe(pipefds + i * 2) < 0)
            perror("pipe error");

    for (int i = 0; i < num_cmds; i++) {
        pid_t pid = fork();

        if (pid == 0) {
            // Connect pipe ends
            if (i > 0)
                dup2(pipefds[(i - 1) * 2], STDIN_FILENO);
            if (i < num_cmds - 1)
                dup2(pipefds[i * 2 + 1], STDOUT_FILENO);

            // Close all pipe FDs
            for (int j = 0; j < 2 * (num_cmds - 1); j++)
                close(pipefds[j]);

            execvp(commands[i][0], commands[i]);
            perror("exec failed");
            exit(1);
        }
    }

    // Parent closes all pipe FDs
    for (int i = 0; i < 2 * (num_cmds - 1); i++)
        close(pipefds[i]);

    // Wait for children if foreground
    if (!background)
        for (int i = 0; i < num_cmds; i++)
            wait(NULL);
    else
        printf("[Pipeline running in background]\n");
}
