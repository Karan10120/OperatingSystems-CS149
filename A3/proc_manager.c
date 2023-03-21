#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#define MAX_COMMAND_LENGTH 30

int main() {
    char line[30];
    char *args[100];
    pid_t pid;
    int status;

    while (fgets(line, MAX_COMMAND_LENGTH, stdin) != NULL)
    {
        line[strcspn(line, "\n")] = 0;

        int arg_count = 0;
        args[arg_count++] = strtok(line, " ");
        while ( args[arg_count] != NULL) {
            args[arg_count] = strtok(NULL, " ");
            arg_count++;
        }
        args[arg_count] = NULL;

        if ((pid = fork()) < 0) {
            printf("fork error");
            exit(1);
        } else if (pid == 0) { /* child */
            execvp(args[0], args);
            printf("couldn't execute: %s\n", args[0]);
            exit(2);
        }
        /* parent */
        status = wait(NULL);
        while (status != -1) {
            fprintf(stderr, "\nexited with exit code = %d\n", WEXITSTATUS(status));
            status = wait(NULL);
        }
    }
    exit(0);
}