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
    int cmd_count = 0;

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
        // increment command line count;
        cmd_count++;

        if ((pid = fork()) < 0) {
            printf("fork error");
            exit(1);

        } else if (pid == 0) { //child

            sprintf(filenameout, "%d.out", getpid()); 
            fd_out = open(filenameout, O_RDWR | O_CREAT | O_APPEND, 0777);
            dup2(fd_out, 1);

            printf("Starting command %d: child %d pid of parent %d\n", cmd_count, getpid(), getppid());
            fflush(stdout);
            close(fd_out);

            execvp(args[0], args);
            // TODO: increment cmd_count somewhere
            //       print couldn't execute to .err instead of .out
            //       fix print starting command line to .out file
            //fprintf(fp, "starting command %d: child %d PID of parent %d", cmd_count, getpid(), getppid());
        //Problem: blank line enter is returning a WIFSIGNALED, causing it to print Killed with sig statement
            sprintf(filenameerr, "%d.err", getpid());
            fd_err = open(filenameerr, O_RDWR | O_CREAT | O_APPEND, 0777);
            dup2(fd_err, 2);
            close(fd_err);
            
            printf("Could not execute command: %s\n", args[0]);
            fflush(stderr);
            exit(2);
        }


        /* parent */
        while ((pid = wait(&status)) > 0) {
            sprintf(filenameerr, "%d.err", pid);
            fp = fopen(filenameerr, "a");
            if (WIFEXITED(status)) {
                fprintf(fp, "Exited with exit code = %d\n", WEXITSTATUS(status));
            } else if (WIFSIGNALED(status)) {
                fprintf(fp, "Killed with signal %d\n", WTERMSIG(status));
            }
            fclose(fp);

            sprintf(filenameout, "%d.out", pid);
            fp = fopen(filenameout, "a");
            fprintf(fp, "Finished child %d PID of parent %d\n", pid, getpid());
            fclose(fp);
        }
    }
    exit(0);
}
