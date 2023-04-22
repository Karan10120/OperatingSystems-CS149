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
#include <time.h>

#define MAX_COMMAND_LENGTH 30

//HASHTABLE CODE
struct nlist { /* table entry: */
    struct timespec starttime; /* starttime */
    struct timespec finishtime;/* finishtime */
    int index; /* index // this is the line index in the input text file */
    int pid; /* pid  // the process id. you can use the pid result of wait to lookup in the hashtable */
    char *command; // command. This is good to store for when you decide to restart a command */
    struct nlist *next; /* next entry in chain */
};

 #define HASHSIZE 101
 static struct nlist *hashtab[HASHSIZE]; /* pointer table */

/* This is the hash function: form hash value for string s */
/* TODO change to: unsigned hash(int pid) */
/* TODO modify to hash by pid . /*
/* You can use a simple hash function: pid % HASHSIZE */
unsigned hash(int pid)
{
//    unsigned hashval;
//     for (hashval = 0; *s != '\0'; s++)
//       hashval = *s + 31 * hashval;
    return ((unsigned)(pid % HASHSIZE));
}

/* lookup: look for s in hashtab */
/* TODO change to lookup by pid: struct nlist *lookup(int pid) */
/* TODO modify to search by pid, you won't need strcmp anymore */
/* This is traversing the linked list under a slot of the hash table. The array position to look in is returned by the hash function */
struct nlist *lookup(int pid)
{
    struct nlist *np;
    for (np = hashtab[hash(pid)]; np != NULL; np = np->next)
        if (pid == np->pid)
          return np; /* found */
    return NULL; /* not found */
}


char *strdup2(char *);


/* insert: put (name, defn) in hashtab */
/* TODO: change this to insert in hash table the info for a new pid and its command */
/* TODO: change signature to: struct nlist *insert(char *command, int pid, int index). */
/* This insert returns a nlist node. Thus when you call insert in your main function  */
/* you will save the returned nlist node in a variable (mynode). */
/* Then you can set the starttime and finishtime from your main function: */
/* mynode->starttime = starttime; mynode->finishtime = finishtime; */
struct nlist *insert(char *command, int pid, int index) {

    struct nlist *np;
    unsigned hashval;

    if ((np = lookup(pid)) == NULL) {       //linkedlist not found
        np = (struct nlist *) malloc(sizeof(*np));
        if (np == NULL || (np->command = strdup2(command)) == NULL) {
            return NULL;
        }
        np->pid = pid;
        np->index = index;
        hashval = hash(pid);
        np->next = hashtab[hashval];
        hashtab[hashval] = np;
    }
    //free maybe?
    return np;
}

/** You might need to duplicate the command string to ensure you don't overwrite the previous command each time a new line is read from the input file.
Or you might not need to duplicate it. It depends on your implementation. **/
char *strdup2(char *s) /* make a duplicate of s */
{
    char *p;
    p = (char *) malloc(strlen(s)+1); /* +1 for ’\0’ */
    if (p != NULL)
       strcpy(p, s);
    return p;
}
//End of HASHTABLE CODE






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
    int index = 0;


    //reads through the file line by line or until EOF signal
    while (fgets(line, MAX_COMMAND_LENGTH, stdin) != NULL) {

        //increment command line count
        index++;

        struct timespec starttime;
        clock_gettime(CLOCK_MONOTONIC, &starttime);

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
            printf("Starting command %d: child %d pid of parent %d\n",  index, getpid(), getppid());
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
        else if (pid > 0) { //Parent Process

            struct nlist *entry_new = insert(line, (int)pid, index);
            //entry_new->starttime = starttime;
            //entry->command = line;
            entry_new->starttime = starttime;
        }
    }


            //Waiting for all child processes
    while ((pid = wait(&status)) >= 0) {
        if (pid > 0) {
            struct timespec finishtime;
            //memset(&finishtime, 0, sizeof(finishtime));
            clock_gettime(CLOCK_MONOTONIC, &finishtime);
            struct nlist *entry = lookup(pid);
            entry->finishtime = finishtime;

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
            float elapsed =  (float)(finishtime.tv_sec - entry->starttime.tv_sec);
            //fprintf(fp, "Start Time: %.1f\n", (double)entry->starttime.tv_sec);
            //fprintf(fp, "Finish Time: %.1f\n", (double)entry->finishtime.tv_sec);
            fprintf(fp, "Finished at %.ld, runtime duration %.f\n", entry->finishtime.tv_sec, elapsed);
            //fprintf(fp, "Elapsed: %.1f\n", elapsed);
            fclose(fp);

            if (elapsed > 10) {
                struct timespec starttime;
                clock_gettime(CLOCK_MONOTONIC, &starttime);
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
                    //printf("RESTARTING\n");
                    printf("Starting command %d: child %d pid of parent %d\n",  index, getpid(), getppid());
                    fflush(stdout);
                    close(fd_out);

                    //Error file
                    sprintf(filenameerr, "%d.err", getpid());
                    fd_err = open(filenameerr, O_RDWR | O_CREAT | O_APPEND, 0777);
                    dup2(fd_err, 2);
                    //printf("RESTARTING\n");
                    //fflush(stderr);

                    execvp(args[0], args);

                    //If exec fails
                    fprintf(stderr, "Could not execute command: %s\n", args[0]);
                    fflush(stderr);
                    close(fd_err);
                    exit(2);
                }
                else if (pid > 0) { //Parent Process

                    struct nlist *entry_new = insert(line, (int)pid, index);
                    //entry_new->starttime = starttime;
                    //entry->command = line;
                    entry_new->starttime = starttime;
                }
            }
        }


    }

    exit(0);
}
