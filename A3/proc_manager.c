#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>

#define MAX_COMMAND_LENGTH 30

int main() {
    char line[MAX_COMMAND_LENGTH + 1];
    char *args[100];    // assume 100 commands max
    pid_t pid;
    int status;
    FILE *fp;
    char filenameout[10], filenameerr[10];
    int fd_out, fd_err;

    // reads through the file line by line / until interrupt signal
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
            // creating output and error files
            // char filenameout[10], filenameerr[10];
            // int fd_out, fd_err;


            sprintf(filenameout, "%d.out", getpid()); 
            fd_out = open(filenameout, O_RDWR | O_CREAT | O_APPEND, 0777);
            dup2(fd_out, 1);
            close(fd_out);

            execvp(args[0], args);

            sprintf(filenameerr, "%d.err", getpid());
            fd_err = open(filenameerr, O_RDWR | O_CREAT | O_APPEND, 0777);
            dup2(fd_err, 2);
            close(fd_err);
            
            printf("couldn't execute: %s\n", args[0]);
            exit(2);
        }
        /* parent */
        while ((pid = wait(&status)) > 0) {
            sprintf(filenameerr, "%d.err", pid);
            fp = fopen(filenameerr, "a");
            fprintf(fp, "\nexited with exit code = %d\n", WEXITSTATUS(status));
            // status = wait(NULL);
            fclose(fp);
        }
    }
    exit(0);
}