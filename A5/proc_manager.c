#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

//LinkedList Implementation

typedef struct command_struct {
    char **commands_array;
    int index;
    int PID;
    struct timespec starttime;
    struct command_struct* nextCommandPtr;
} CommandNode;

void CreateCommandNode(CommandNode* thisNode, char **cmd, int ind, CommandNode* nextCmd) {
    thisNode->commands_array = cmd;
    thisNode->index = ind;
    thisNode->PID = getpid();   //added
    thisNode->nextCommandPtr = nextCmd;
    return;
}

void InsertCommandAfter(CommandNode* thisNode, CommandNode* newNode) {
    CommandNode* tmpNext = NULL;

    tmpNext = thisNode->nextCommandPtr;
    thisNode->nextCommandPtr = newNode;
    newNode->nextCommandPtr = tmpNext;
    return;
}

CommandNode* GetNextCommand(CommandNode* thisNode) {
    return thisNode->nextCommandPtr;
}

CommandNode* FindCommand(CommandNode* head, int pid) {
    CommandNode* current = head;
    while (current != NULL) {
        if (current->PID == pid) { return current; }
        current = current->nextCommandPtr;
    }

    return NULL;
}
//end of LinkedList Implementation

char** tokenize_line (char* line) {
    char* token = strtok(line, " ");
    char** words = malloc(21 * sizeof(char*));
    int count = 0;

    while (token!= NULL && count < 20) {
        words[count] = strdup(token);
        token = strtok(NULL, " ");
        count++;
    }
    words[count] = NULL;     //set last element in array to null
    return words;
}

void print_words (char** words) {
    printf("Array: ");
    for (int i = 0; words[i] != NULL; i++) {
        printf("%s ", words[i]);
    }
}

void PrintLinkedList(CommandNode* head) {
    CommandNode* current = head;

    while (current != NULL) {
        printf("Command: ");
        for (int i = 0; i < 20; i++) {
            printf("%s ", current->commands_array[i]);
        }
        printf("\n");
        printf("Index: %d\n", current->index);
        printf("PID: %d\n", current->PID);
        printf("Start time: %ld\n", current->starttime.tv_sec);
        printf("\n");
        current = current->nextCommandPtr;
    }
}

void free_linked_list(CommandNode* head) {
    CommandNode* current = head;
    CommandNode* next = NULL;

    while (current != NULL) {
        for (int i = 0; i < 20; i++) {
            free(current->commands_array[i]); // free the memory for each string in the array
        }
        free(current->commands_array); // free the memory for the array itself
        next = current->nextCommandPtr;
        free(current);
        current = next;
    }
}


int main(void) {
    int index = 0;
    char line[101];
    CommandNode* head = NULL;
    CommandNode* current = NULL;

    while (fgets(line, 100, stdin) != NULL){
        index++;
        line[strcspn(line, "\n")] = '\0';
        printf("line before: %s", line);

        //char cmds_array[20][20];
        int count = 0;
        char** cmds_array = tokenize_line(line);
        print_words(cmds_array);

        printf(" Line after: %s\n", line);

        struct timespec start;
        clock_gettime(CLOCK_MONOTONIC, &start);

        pid_t pid = fork(); // create a child process

        if (pid == 0) { // child process
            // execute the command using execvp()
            if (execvp(cmds_array[0], cmds_array) < 0) {
            printf("Error: Failed to execute command.\n");
            return 1;
            }
        }
        else if (pid > 0) { // parent process
            wait(NULL); // wait for the child process to finish
            CommandNode* node = (CommandNode*)malloc(sizeof(CommandNode));
            CreateCommandNode(node, cmds_array, index, NULL);
            if (head == NULL) {
                head = node;
                current = node;
            } else {
                InsertCommandAfter(current, node);
            }
            CommandNode* entry_new = FindCommand(head, pid);
            entry_new->starttime = start;
        }
        else { // error occurred
            printf("Error Forking\n");
            exit(2);
        }
    }

    //PrintLinkedList(head);
    free(head);
    return 0;
}
