/**
 * Description: This program executes commands in parallel with corresponding
 * output and error files for process.
 * Author names: Luc Tang, Karan Gandhi
 * Author emails: luc.tang@sjsu.edu, karan.gandhi@sjsu.edu
 * Last modified date: 03/23/2023
 * Creation date: 03/20/2023
 **/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>

#define MAX_COMMAND_LENGTH 30

/**
 * This function counts and prints names for one or more files.
 * Assumption:
 * 1. The command and parameters are at most 30 characters.
 * 2. Atmost 100 commands.
 * 3. Each line contains a new command.
 * 4. Each command is separated by a new line.
 * 5. Input file contains only valid characters.
 * 6. Assume a command has a maximum of two parameters.
 *
 * Input parameters: stdin
 * Returns: Output and error files for each process
**/

int main() {
    char line[MAX_COMMAND_LENGTH + 1];
    char *args[100];    // assume 100 commands max
    pid_t pid;
    int status;
    FILE *fp;
    char filenameout[10], filenameerr[10];
    int fd_out, fd_err;
    int cmd_count = 0;

    //reads through the file line by line or until EOF signal
    while (fgets(line, MAX_COMMAND_LENGTH, stdin) != NULL) {

        //increment command line count
        cmd_count++;
        //forking and checking for error
        if ((pid = fork()) < 0) {
            printf("fork error");
            exit(1);
        } else if (pid == 0) { //child process
            //removes new line character from line
            line[strcspn(line, "\n")] = 0;

            int arg_count = 0;
            args[arg_count++] = strtok(line, " ");
            //Separates words by space and places word pointers in array
            while ( args[arg_count] != NULL) {
                args[arg_count] = strtok(NULL, " ");
                arg_count++;
            }
            args[arg_count] = NULL;

            //Out file
            sprintf(filenameout, "%d.out", getpid()); 
            fd_out = open(filenameout, O_RDWR | O_CREAT | O_APPEND, 0777);
            dup2(fd_out, 1);
            printf("Starting command %d: child %d pid of parent %d\n", cmd_count, getpid(), getppid());
            fflush(stdout);
            close(fd_out);

            //Error file
            sprintf(filenameerr, "%d.err", getpid());
            fd_err = open(filenameerr, O_RDWR | O_CREAT | O_APPEND, 0777);
            dup2(fd_err, 2);

            execvp(args[0], args);

            //If exec fails
            fprintf(stderr, "Could not execute command: %s\n", args[0]);
            fflush(stderr);
            close(fd_err);
            exit(2);
        }
        //Waiting for all child processes
        while ((pid = wait(&status)) > 0) { //Parent Process
            sprintf(filenameerr, "%d.err", pid);
            fp = fopen(filenameerr, "a");
            //Normal termination with exit code
            if (WIFEXITED(status)) {
                fprintf(fp, "Exited with exit code = %d\n", WEXITSTATUS(status));
            } else if (WIFSIGNALED(status)) {   //Abnormal termination with signal
                fprintf(fp, "Killed with signal %d\n", WTERMSIG(status));
            }
            fclose(fp);

            sprintf(filenameout, "%d.out", pid);
            fp = fopen(filenameout, "a");
            fprintf(fp, "Finished child %d pid of parent %d\n", pid, getpid());
            fclose(fp);
        }
    }
    exit(0);
}
