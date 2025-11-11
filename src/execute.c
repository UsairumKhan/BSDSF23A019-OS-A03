#include "shell.h"

/* ---------------------------- */
/* IF / ELSE HANDLER */
int handle_if_else(char* cmdline) {
    if (strncmp(cmdline, "if ", 3) != 0)
        return 0;

    char *then_ptr = strstr(cmdline, " then ");
    char *else_ptr = strstr(cmdline, " else ");
    char *fi_ptr = strstr(cmdline, " fi");

    if (!then_ptr || !fi_ptr)
        return 0;

    char cond[256] = {0}, then_cmd[256] = {0}, else_cmd[256] = {0};

    strncpy(cond, cmdline + 3, then_ptr - (cmdline + 3));

    if (else_ptr)
        strncpy(then_cmd, then_ptr + 6, else_ptr - (then_ptr + 6));
    else
        strncpy(then_cmd, then_ptr + 6, fi_ptr - (then_ptr + 6));

    if (else_ptr)
        strncpy(else_cmd, else_ptr + 6, fi_ptr - (else_ptr + 6));

    /* Run condition */
    char** cond_args = tokenize(cond);
    int status = fork();
    if (status == 0) {
        execvp(cond_args[0], cond_args);
        perror("Command not found");
        exit(1);
    }
    int exit_status;
    waitpid(status, &exit_status, 0);

    for (int i = 0; cond_args && cond_args[i]; i++) free(cond_args[i]);
    free(cond_args);

    if (WEXITSTATUS(exit_status) == 0) {
        char** then_args = tokenize(then_cmd);
        execute(then_args, then_cmd);
        for (int i = 0; then_args && then_args[i]; i++) free(then_args[i]);
        free(then_args);
    } else if (strlen(else_cmd) > 0) {
        char** else_args = tokenize(else_cmd);
        execute(else_args, else_cmd);
        for (int i = 0; else_args && else_args[i]; i++) free(else_args[i]);
        free(else_args);
    }

    return 1;
}

/* ---------------------------- */
/* EXECUTE FUNCTION (previous version with I/O, background, pipes, etc.) */
int execute(char* arglist[], char* fullcmd) {
    if (handle_if_else(fullcmd))
        return 0;

    int background = 0;
    for (int i = 0; arglist[i] != NULL; i++) {
        if (strcmp(arglist[i], "&") == 0) {
            background = 1;
            arglist[i] = NULL;
            break;
        }
    }

    int in_redirect = -1, out_redirect = -1;
    char* input_file = NULL;
    char* output_file = NULL;
    for (int i = 0; arglist[i] != NULL; i++) {
        if (strcmp(arglist[i], "<") == 0) {
            input_file = arglist[i + 1];
            arglist[i] = NULL;
        } else if (strcmp(arglist[i], ">") == 0) {
            output_file = arglist[i + 1];
            arglist[i] = NULL;
        }
    }

    int pipe_index = -1;
    for (int i = 0; arglist[i] != NULL; i++) {
        if (strcmp(arglist[i], "|") == 0) {
            pipe_index = i;
            arglist[i] = NULL;
            break;
        }
    }

    int status;
    int cpid = fork();

    if (cpid == 0) {
        if (input_file) {
            in_redirect = open(input_file, O_RDONLY);
            dup2(in_redirect, STDIN_FILENO);
            close(in_redirect);
        }
        if (output_file) {
            out_redirect = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            dup2(out_redirect, STDOUT_FILENO);
            close(out_redirect);
        }
        execvp(arglist[0], arglist);
        perror("Command not found");
        exit(1);
    }
    else if (cpid > 0) {
        if (!background)
            waitpid(cpid, &status, 0);
        else
            printf("[Running in background: PID %d]\n", cpid);
    }
    else {
        perror("fork failed");
    }

    return 0;
}
