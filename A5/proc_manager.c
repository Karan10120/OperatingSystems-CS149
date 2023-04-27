/**
 * Description: This program executes commands in parallel with corresponding
 * output and error files for each process and restarts if the process takes longer
 * than 2 seconds to execute.
 * Author names: Luc Tang, Karan Gandhi
 * Author emails: luc.tang@sjsu.edu, karan.gandhi@sjsu.edu
 * Last modified date: 04/26/2023
 * Creation date: 04/21/2023
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
    int index; /* index: this is the line index in the input text file */
    int pid; /* pid: the process id. you can use the pid result of wait to lookup in the hashtable */
    char **words_array; // command: This is good to store for when you decide to restart a command */
    char *first_word; // first word in the command
    char *command;      // full length command
    struct nlist *next; /* next entry in chain */
};

 #define HASHSIZE 101
 static struct nlist *hashtab[HASHSIZE]; /* pointer table */

/* This is the hash function: form hash value for string s */
/* You can use a simple hash function: pid % HASHSIZE */
unsigned hash(int pid)
{
    return ((unsigned)(pid % HASHSIZE));
}

/* lookup: look for s in hashtab */
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
/* This insert returns a nlist node. Thus when you call insert in your main function  */
/* you will save the returned nlist node in a variable (mynode). */
/* Then you can set the starttime and finishtime from your main function: */
/* mynode->starttime = starttime; mynode->finishtime = finishtime; */
struct nlist *insert(char *command, int pid, int index) {

    struct nlist *np;
    unsigned hashval;
    // if process exists in hashtable
    if ((np = lookup(pid)) == NULL) {       //linkedlist not found
        np = (struct nlist *) malloc(sizeof(*np));
        // check if malloc did not work
        if (np == NULL) {
            return NULL;
        }
        char *cmd = strdup2(command);
        np->command = cmd;
        // assume 10 words each at most 30 chars
        np->words_array = (char **)malloc(10 * sizeof(char *));
        // allocating memory for array
        for (int i = 0; i < 10; i++) {
            np->words_array[i] = (char *)malloc(30 * sizeof(char));
        }

        int arg_count = 0;

        np->words_array[arg_count++] = strtok(cmd, " ");
        //Separates words by space and places word pointers in array
        while ( np->words_array[arg_count] != NULL) {
            np->words_array[arg_count] = strtok(NULL, " ");
            arg_count++;
        }
        np->words_array[arg_count] = NULL;

        np->pid = pid;
        np->index = index;
        hashval = hash(pid);
        np->first_word = np->words_array[0];
        np->next = hashtab[hashval];
        hashtab[hashval] = np;
    }
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

void print_hashtable() {
    printf("\nHashtable contents:\n");
    for (int i = 0; i < HASHSIZE; i++) {
        struct nlist *np;
        for (np = hashtab[i]; np != NULL; np = np->next) {
            printf("Slot %d:\n", i);
            printf("  PID: %d\n", np->pid);
            printf("  Index: %d\n", np->index);
            fflush(stdout);
            printf("  First word: %s\n", np->first_word);
            printf("  Command: %s\n", np->command);
            printf("  Command arr: ");
            for (int j = 0; np->words_array[j] != NULL; j++) {
                printf("%s ", np->words_array[j]);
            }
            printf("\n");
            printf("  Start time: %ld.%ld\n", np->starttime.tv_sec, np->starttime.tv_nsec);
            printf("  Finish time: %ld.%ld\n", np->finishtime.tv_sec, np->finishtime.tv_nsec);
            fflush(stdout);
        }
    }
}

// char *words_array_to_string(char **words_array) {
//     int length = 0;
//     for (int i = 0; words_array[i] != NULL; i++) {
//         length += strlen(words_array[i]) + 1; // +1 for space or null terminator
//     }

//     char *result = (char *)malloc(length * sizeof(char));
//     result[0] = '\0';

//     for (int i = 0; words_array[i] != NULL; i++) {
//         strcat(result, words_array[i]);
//         if (words_array[i + 1] != NULL) {
//             strcat(result, " ");
//         }
//     }

//     result[strcspn(result, "\n")] = 0;

//     return result;
// }


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
        char *line_copy = strdup2(line);

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
        else if (pid > 0) { //Parent Process: inserts into hashtable
            printf("inserted line: %s", line_copy);
            struct nlist *entry_new = insert(line_copy, (int)pid, index);
            entry_new->starttime = starttime;
        }
    }


    //Waiting for all child processes
    while ((pid = wait(&status)) >= 0) {
        struct timespec finishtime;
        //memset(&finishtime, 0, sizeof(finishtime));
        clock_gettime(CLOCK_MONOTONIC, &finishtime);
        //child process
        if (pid > 0) {
            struct nlist *entry = lookup(pid);
            printf("lookup: %d, %s", pid, entry->command);
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
            fprintf(fp, "Finished at %.ld, runtime duration %.f\n", entry->finishtime.tv_sec, elapsed);
            //fprintf(fp, "Elapsed: %.1f\n", elapsed);
            fclose(fp);


            struct timespec new_starttime;
            //process takes longer than 2 seconds
            if (elapsed > 2) {

                clock_gettime(CLOCK_MONOTONIC, &new_starttime);
                //forking and checking for error
                if ((pid = fork()) < 0) {
                    printf("fork error");
                    exit(1);
                } else if (pid == 0) { //child process
                    //Out file
                    sprintf(filenameout, "%d.out", getpid());
                    fd_out = open(filenameout, O_RDWR | O_CREAT | O_APPEND, 0777);
                    dup2(fd_out, 1);
                    printf("RESTARTING\n");
                    printf("Starting command %d: child %d pid of parent %d\n",  entry->index, getpid(), getppid());
                    fflush(stdout);
                    close(fd_out);

                    //Error file
                    sprintf(filenameerr, "%d.err", getpid());
                    fd_err = open(filenameerr, O_RDWR | O_CREAT | O_APPEND, 0777);
                    dup2(fd_err, 2);
                    printf("RESTARTING\n");

                    printf("%s", *(entry->words_array));
                    execvp(entry->words_array[0], entry->words_array);

                    //If exec fails
                    fprintf(stderr, "Could not execute command: %s\n", entry->words_array[0]);
                    fflush(stderr);
                    close(fd_err);
                    exit(2);
                }
                else if (pid > 0) { //Parent Process
                    // char *concatenated_cmd = words_array_to_string(entry->words_array);
                    // printf("concat: %s", concatenated_cmd);
                    struct nlist *entry_new = insert(entry->command, (int)pid, entry->index);
                    entry_new->starttime = new_starttime;
                }
            } else {    //takes less than 2 seconds
                sprintf(filenameerr, "%d.err", pid);
                fp = fopen(filenameerr, "a");
                fprintf(fp, "spawning too fast\n");
                fclose(fp);
            }
        }
    }
    // print_hashtable();
    exit(0);
}
