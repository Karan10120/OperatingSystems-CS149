#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdarg.h>

#define MAX_COMMAND_LENGTH 100

typedef struct TRACE_NODE_STRUCT {
  char* functionid;                // ptr to function identifier (a function name)
  struct TRACE_NODE_STRUCT* next;  // ptr to next frama
} TRACE_NODE;

static TRACE_NODE* TRACE_TOP = NULL;       // ptr to the top of the stack


void PUSH_TRACE(char* p)          // push p on the stack
{
  TRACE_NODE* tnode;
  static char glob[]="global";

  if (TRACE_TOP==NULL) {

    // initialize the stack with "global" identifier
    TRACE_TOP=(TRACE_NODE*) malloc(sizeof(TRACE_NODE));

    // no recovery needed if allocation failed, this is only
    // used in debugging, not in production
    if (TRACE_TOP==NULL) {
      printf("PUSH_TRACE: memory allocation error\n");
      exit(1);
    }

    TRACE_TOP->functionid = glob;
    TRACE_TOP->next=NULL;
  }//if

  // create the node for p
  tnode = (TRACE_NODE*) malloc(sizeof(TRACE_NODE));

  // no recovery needed if allocation failed, this is only
  // used in debugging, not in production
  if (tnode==NULL) {
    printf("PUSH_TRACE: memory allocation error\n");
    exit(1);
  }//if

  tnode->functionid=p;
  tnode->next = TRACE_TOP;  // insert fnode as the first in the list
  TRACE_TOP=tnode;          // point TRACE_TOP to the first node

}/*end PUSH_TRACE*/


void POP_TRACE()    // remove the top of the stack
{
  TRACE_NODE* tnode;
  tnode = TRACE_TOP;
  TRACE_TOP = tnode->next;
  free(tnode);

}/*end POP_TRACE*/


char* PRINT_TRACER()
{
  int depth = 50; //A max of 50 levels in the stack will be combined in a string for printing out.
  int i, length, j;
  TRACE_NODE* tnode;
  static char buf[100];

  if (TRACE_TOP==NULL) {     // stack not initialized yet, so we are
    strcpy(buf,"global");   // still in the `global' area
    return buf;
  }

  /* peek at the depth(50) top entries on the stack, but do not
     go over 100 chars and do not go over the bottom of the
     stack */

  sprintf(buf,"%s",TRACE_TOP->functionid);
  length = strlen(buf);                  // length of the string so far
  for(i=1, tnode=TRACE_TOP->next; tnode!=NULL && i < depth; i++,tnode=tnode->next) {
    j = strlen(tnode->functionid);             // length of what we want to add
    if (length+j+1 < 100) {              // total length is ok
      sprintf(buf+length,":%s",tnode->functionid);
      length += j+1;
    }else                                // it would be too long
      break;
  }
  return buf;
} /*end PRINT_TRACE*/


//seperating the redefinitions
void* REALLOC(void* p,int t,char* file,int line)
{
	p = realloc(p,t);
    printf("File=%s, line=%d, function=%s, segment reallocated to address %p to a new size %d\n", file, line, PRINT_TRACER(), p, t);
	return p;
}


void* MALLOC(int t,char* file,int line)
{
	void* p;
	p = malloc(t);
    printf("File=%s, line=%d, function=%s, allocated new memory segment at address %p to size %d\n", file, line, PRINT_TRACER(), p, t);
	return p;
}


void FREE(void* p,char* file,int line)
{
	free(p);
    printf("File=%s, line=%d, function=%s, deallocated the memory segment at address %p\n", file, line, PRINT_TRACER(), p);
}

#define realloc(a,b) REALLOC(a,b,__FILE__,__LINE__)
#define malloc(a) MALLOC(a,__FILE__,__LINE__)
#define free(a) FREE(a,__FILE__,__LINE__)


typedef struct ListNode_Struct {

    char *command;
    int command_num;
    struct ListNode_Struct *next;

} ListNode;

static ListNode *head = NULL;
static ListNode *tail = NULL;

void addNodes(char *command, int command_num, int size) {
    PUSH_TRACE("addNodes");
    printf("adding node\n");
    ListNode *new_node = malloc(sizeof(ListNode));
    char *str = malloc(size + 1);
    strcpy(str, command);
    new_node->command = str;
    new_node->command_num = command_num;
    new_node->next = NULL;

    if (head == NULL) {
        head = new_node;
        tail = new_node;
    } else {
        tail->next = new_node;
        tail = new_node;
    }
    POP_TRACE();
}

void printNodes(ListNode *node){
    PUSH_TRACE("printNodes");
    printf("Line %d: %s\n", node->command_num, node->command);
    if (node->next != NULL) {
        printNodes(node->next);
    }
    POP_TRACE();
}

void freeListNodes(ListNode *node){
  if (node->next != NULL) {
    freeListNodes(node->next);
  }
  free(node->command);
  free(node);

}

// void freeListNodes() {
//     PUSH_TRACE("freeListNodes");
//     ListNode *curr;
//     while (curr->next != NULL) {
//       head = curr->next;
//       free(curr->command);
//       free(curr);
//       curr = head;
//     }
//     free(curr->command);
//     free(curr);
//     head = NULL;
//     POP_TRACE();
// }


int main() {
    char *line = NULL;
    char **array[10];
    size_t lineSize = 0;
    ssize_t read;

    PUSH_TRACE("main");

//     for(int i = 0; i < sizeof(array); i++) {
//         array[i] = NULL;
//     }

    int count = 0;
    int size;
    while((size = getline(&line, &lineSize, stdin)) != EOF) {

        line[strcspn(line, "\n")] = 0;
        addNodes(line, count, size);
        count++;

    }
    free(line);
    //printf("going into printNodes\n");
    printNodes(head);
    freeListNodes(head);
  POP_TRACE();
  POP_TRACE();
  return(0);

}




