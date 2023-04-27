//LinkedList Implementation

typedef struct command_struct {
    char command[20][20];
    int index;
    int PID;
    int starttime;
    struct command_struct* nextCommandPtr;
} CommandNode;

void CreateCommandNode(CommandNode* thisNode, char cmd[20][20], int ind, CommandNode* nextCmd) {
    for (int i = 0; i < 20; i++) {
        for (int j = 0; j < 20; j++) {
            thisNode->command[i][j] = cmd [i][j];
        }
    }
    thisNode->index = ind;
    thisNode->PID = getpid();   //added
    thisNode->nextCommandPtr = nextCmd;
    return;
}

void InsertCommandAfter(CommandNode* thisNode, CommandNode* newNode) {
    CommandNode* tmpNext = NULL;

    tmpNext = thisNode->nextCommandPtr;
    thisNode->nextCommandPtr = newNode;
    newNode->nextCommandPtr = tmeNext;
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

