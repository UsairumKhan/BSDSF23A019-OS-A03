#include "shell.h"
#include <errno.h>

static char internal_history[HISTORY_SIZE][MAX_LEN];
static int internal_history_count = 0;

/* For background job tracking */
typedef struct {
    pid_t pid;
    char command[MAX_LEN];
} Job;

static Job jobs[MAX_JOBS];
static int job_count = 0;

/* ------------------ Read Command ------------------ */
char* read_cmd(char* prompt) {
    char* line = readline(prompt);
    if (line == NULL) return NULL;

    if (strlen(line) > 0) {
        add_history(line);
        internal_history_add(line);
    }
    return line;
}

/* ------------------ Tokenize ------------------ */
char** tokenize(char* cmdline) {
    if (cmdline == NULL || cmdline[0] == '\0') return NULL;

    char** arglist = (char**)malloc(sizeof(char*) * (MAXARGS + 1));
    for (int i = 0; i < MAXARGS + 1; i++) {
        arglist[i] = (char*)malloc(sizeof(char) * ARGLEN);
        bzero(arglist[i], ARGLEN);
    }

    char* cp = cmdline;
    char* start;
    int len, argnum = 0;

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

/* ------------------ Built-in Commands ------------------ */
int handle_builtin(char** arglist) {
    if (arglist == NULL || arglist[0] == NULL) return 0;

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
        printf("  cd <dir>      - Change directory\n");
        printf("  exit          - Exit the shell\n");
        printf("  help          - Show help\n");
        printf("  jobs          - Show background jobs\n");
        printf("  history       - Show command history\n");
        printf("  !n            - Execute nth command\n");
        return 1;
    } else if (strcmp(arglist[0], "history") == 0) {
        internal_history_show();
        return 1;
    } else if (strcmp(arglist[0], "jobs") == 0) {
        remove_finished_jobs();
        show_jobs();
        return 1;
    }

    return 0;
}

/* ------------------ Internal History ------------------ */
void internal_history_add(const char* cmdline) {
    if (cmdline == NULL || strlen(cmdline) == 0)
        return;
    if (internal_history_count < HISTORY_SIZE) {
        strncpy(internal_history[internal_history_count], cmdline, MAX_LEN - 1);
        internal_history_count++;
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

/* ------------------ Background Job Functions ------------------ */
void add_job(pid_t pid, const char* cmd) {
    if (job_count < MAX_JOBS) {
        jobs[job_count].pid = pid;
        strncpy(jobs[job_count].command, cmd, MAX_LEN - 1);
        job_count++;
    } else {
        printf("Job limit reached!\n");
    }
}

void remove_finished_jobs() {
    int i = 0;
    while (i < job_count) {
        pid_t result = waitpid(jobs[i].pid, NULL, WNOHANG);
        if (result == 0) {
            i++;
        } else {
            for (int j = i; j < job_count - 1; j++)
                jobs[j] = jobs[j + 1];
            job_count--;
        }
    }
}

void show_jobs() {
    if (job_count == 0)
        printf("No background jobs.\n");
    else {
        for (int i = 0; i < job_count; i++)
            printf("[%d] %d %s\n", i + 1, jobs[i].pid, jobs[i].command);
    }
}
