/**
 * Description: This program executes commands in parallel with corresponding
 * output and error files for each process and restarts if the process takes longer
 * than 2 seconds to execute.
 * Author names: Luc Tang, Karan Gandhi
 * Author emails: luc.tang@sjsu.edu, karan.gandhi@sjsu.edu
 * Last modified date: 04/28/2023
 * Creation date: 04/21/2023
 **/

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>

//LinkedList Implementation

typedef struct command_struct {
    char **commands_array;      //array of command strings
    int index;                  //index of command number
    int PID;                    //pid process of node
    struct timespec starttime;  //starttime of node
    struct command_struct* nextCommandPtr;      //pointer to next node in Linkedlist
} CommandNode;

//this function creates a CommandNode given the attributes of the struct
void CreateCommandNode(CommandNode* thisNode, char **cmd, int ind, int pid, CommandNode* nextCmd) {
    thisNode->commands_array = cmd;
    thisNode->index = ind;
    thisNode->PID = pid;
    thisNode->nextCommandPtr = nextCmd;
    return;
}

//Inserts a Command Node after a given node
//Use to from a LinkedList starting from head node
void InsertCommandAfter(CommandNode* thisNode, CommandNode* newNode) {
    CommandNode* tmpNext = NULL;

    tmpNext = thisNode->nextCommandPtr;
    thisNode->nextCommandPtr = newNode;
    newNode->nextCommandPtr = tmpNext;
    return;
}

//Acessor to get the pointer to the next CommandNode
CommandNode* GetNextCommand(CommandNode* thisNode) {
    return thisNode->nextCommandPtr;
}

//Searches for CommandNode in LinkedList by pid of a node
CommandNode* FindCommand(CommandNode* head, int pid) {
    CommandNode* current = NULL;
    current = head;
    while (current != NULL) {
        if (current->PID == pid) { return current; }
        current = current->nextCommandPtr;
    }

    return NULL;
}
//end of LinkedList Implementation

//Function to create tokens of words based upon a given command line
//Words are then assigned in array to return
char** tokenize_line (char* line, int* count) {
    char* token = strtok(line, " ");
    char** words = (char**)calloc(21, sizeof(char*));
    //int count = 0;

    while (token!= NULL && (*count) < 20) {
        words[(*count)] = strdup(token);
        token = strtok(NULL, " ");
        (*count)++;
    }
    words[(*count)] = NULL;     //set last element in array to null
    return words;
}

//Frees all the Command Nodes contained in a LinkedList with head node
void free_linked_list(CommandNode* head) {
    CommandNode* current = NULL;
    current = head;
    CommandNode* next = NULL;
    //loops until no more nodes in the list
    while (current != NULL) {
        for (int i = 0; i < 21; i++) {
            free(current->commands_array[i]); // free the memory for each string in the array
        }
        free(current->commands_array); // free the memory for the array itself
        next = current->nextCommandPtr;
        free(current);
        current = next;
    }
}

/**
 * This function reads commands and spawns processes to run them. Restarting those processes that take more than 2 seconds.
 * Assumption:
 * 1. The command and parameters are at most 20 characters.
 * 2. Each line contains a new command.
 * 3. Each command is separated by a new line.
 * 4. Input file contains only valid characters.
 *
 * Input parameters: stdin
 * Returns: Output and error files for each process
**/
int main(void) {
    int index = 0;
    char line[101];
    CommandNode* head = NULL;
    CommandNode* current = NULL;
    char filenameout[10], filenameerr[10];
    int fd_out, fd_err;

    //reads through the file line by line or until EOF signal
    while (fgets(line, 100, stdin)){
        index++;
        line[strcspn(line, "\n")] = '\0';                   //Remove trailing new line characters
        int* count = calloc(1, sizeof(int*));
        char** cmds_array = tokenize_line(line, count);     //tokenize the input line into words and assign into char** array
        //saving start time for command
        struct timespec start;
        clock_gettime(CLOCK_MONOTONIC, &start);

        pid_t pid = fork(); // create a child process

        if (pid == 0) { // child process

            //file outputs and errs
            sprintf(filenameout, "%d.out", getpid());
            fd_out = open(filenameout, O_RDWR | O_CREAT | O_APPEND, 0777);
            dup2(fd_out, 1);
            printf("Starting command %d: child %d pid of parent %d\n", index, getpid(), getppid());
            fflush(stdout);

            sprintf(filenameerr, "%d.err", getpid());
            fd_err = open(filenameerr, O_RDWR | O_CREAT | O_APPEND, 0777);
            dup2(fd_err, 2);


            // execute the command using execvp() and if fails to execute print error message
            if (execvp(cmds_array[0], cmds_array) <= 0) {
            fprintf(stderr, "Error: Failed to execute command: %s\n", cmds_array[0]);
            }
        }
        else if (pid > 0) { // parent process

            CommandNode* node = (CommandNode*)malloc(sizeof(CommandNode));
            CreateCommandNode(node, cmds_array, index, pid, NULL);
            if (head == NULL) {         //case where this no head yet in LinkedList
                head = node;
                current = node;
            } else {                    //case when head exists and we add to after the current node
                InsertCommandAfter(current, node);
            }
            CommandNode* entry_new = FindCommand(head, pid);    //search for node by pid in linked list
            entry_new->starttime = start;                       //assign value of node starttime
        }
        else { // error occurred
            fprintf(stderr, "Error Forking\n");
            exit(2);
        }
    }

    int status;
    int childPID;

    //waiting for all child processes to completed
    while ((childPID = wait(&status)) > 0) {
        double elapsed;

        if (childPID > 0) {         //ensure only entered by parent process

            struct timespec finish; //record endtime for child process
            clock_gettime(CLOCK_MONOTONIC, &finish);
            CommandNode* entry = FindCommand(head, childPID);

            //file outputs and errs
            sprintf(filenameout, "%d.out", childPID);
            fd_out = open(filenameout, O_RDWR | O_CREAT | O_APPEND, 0777);
            dup2(fd_out, 1);

            sprintf(filenameerr, "%d.err", childPID);
            fd_err = open(filenameerr, O_RDWR | O_CREAT | O_APPEND, 0777);
            dup2(fd_err, 2);

            elapsed = (double)(finish.tv_sec - (entry->starttime.tv_sec));
            fprintf(stdout, "Finished child %d pid of parent %d\n", childPID, getpid());
            fprintf(stdout, "Finished at %ld, runtime duration %.1f\n", finish.tv_sec, elapsed);
            fflush(stdout);

            if (WIFEXITED(status)) {             //Normal termination with exit code
                fprintf(stderr, "Exited with exit code = %d\n", WEXITSTATUS(status));
            } else if (WIFSIGNALED(status)) {   //Abnormal termination with signal
                fprintf(stderr, "Killed with signal %d\n", WTERMSIG(status));
            }

            //Checks if process duration is less than 2 seconds
            if (elapsed <= 2.0) {
                fprintf(stderr, "Spawning too fast\n");
            } else if (elapsed > 2.0) { //process duration is greater than 2 seconds and need to restart
                char** cmds_array = entry->commands_array;
                int new_index = entry->index;
                struct timespec start;
                clock_gettime(CLOCK_MONOTONIC, &start);     //save starttime of restarted process

                pid_t pid = fork(); // create a child process

                if (pid == 0) { // child process

                    //file outputs and errs
                    sprintf(filenameout, "%d.out", getpid());
                    fd_out = open(filenameout, O_RDWR | O_CREAT | O_APPEND, 0777);
                    dup2(fd_out, 1);
                    printf("RESTARTING\n");
                    printf("Starting command %d: child %d pid of parent %d\n", new_index, getpid(), getppid());
                    fflush(stdout);

                    sprintf(filenameerr, "%d.err", getpid());
                    fd_err = open(filenameerr, O_RDWR | O_CREAT | O_APPEND, 0777);
                    dup2(fd_err, 2);

                    // execute the command using execvp()
                    if (execvp(cmds_array[0], cmds_array) <= 0) {
                    fprintf(stderr, "Error: Failed to execute command: %s\n", cmds_array[0]);
                    }
                }
                else if (pid > 0) {             //parent process

                    CommandNode* node = (CommandNode*)malloc(sizeof(CommandNode));
                    CreateCommandNode(node, cmds_array, new_index, pid, NULL);
                    if (head == NULL) {         //case where this no head yet in LinkedLis
                        head = node;
                        current = node;
                    } else {                    //case when head exists and we add to after the current node
                        InsertCommandAfter(current, node);
                    }
                    CommandNode* entry_new = FindCommand(head, pid);    //search for node by pid in linked list
                    entry_new->starttime = start;                       //assign value of node starttime

                    }
            }
        }
    }

    return 0;
}
