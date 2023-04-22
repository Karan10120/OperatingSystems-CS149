/**
 * Description: This program stores the commands in an array and traces the
 * memory usage. It then prints out all the memory usage and contents that 
 * was read in a file called memtrace.out.
 * Author names: Luc Tang, Karan Gandhi
 * Author emails: luc.tang@sjsu.edu, karan.gandhi@sjsu.edu
 * Last modified date: 04/10/2023
 * Creation date: 04/07/2023
 **/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

#define MAX_COMMAND_LENGTH 100

struct TRACE_NODE_STRUCT {
  char* functionid;                // ptr to function identifier (a function name)
  struct TRACE_NODE_STRUCT* next;  // ptr to next frama
};
typedef struct TRACE_NODE_STRUCT TRACE_NODE;
static TRACE_NODE* TRACE_TOP = NULL;       // ptr to the top of the stack

/* function PUSH_TRACE
 * The purpose of this stack is to trace the sequence of function calls,
 * just like the stack in your computer would do. 
 * The "global" string denotes the start of the function call trace.
 * The char *p parameter is the name of the new function that is added to the call trace.
 */
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
  }

  tnode->functionid=p;
  tnode->next = TRACE_TOP;  // insert fnode as the first in the list
  TRACE_TOP=tnode;          // point TRACE_TOP to the first node
}

/* function POP_TRACE */
/* Pop a function call from the stack */
void POP_TRACE()    // remove the top of the stack
{
  TRACE_NODE* tnode;
  tnode = TRACE_TOP;
  TRACE_TOP = tnode->next;
  free(tnode);
}

/* function PRINT_TRACE prints out the sequence of function calls that are on the stack at this instance
 * For example, it returns a string that looks like: global:funcA:funcB:funcC.
 * Printing the function call sequence the other way around is also ok: funcC:funcB:funcA:global
 */
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
   * go over 100 chars and do not go over the bottom of the
   * stack 
   */

  sprintf(buf,"%s",TRACE_TOP->functionid);
  length = strlen(buf);                  // length of the string so far
  // going through all the stack traces to print function calls
  for(i=1, tnode=TRACE_TOP->next; tnode!=NULL && i < depth; i++,tnode=tnode->next) {
    j = strlen(tnode->functionid);       // length of what we want to add
    if (length+j+1 < 100) {              // total length is ok
      sprintf(buf+length,":%s",tnode->functionid);
      length += j+1;
    }else                                // it would be too long
      break;
  }
  return buf;
}

// function REALLOC calls realloc and prints info about memory usage.
void* REALLOC(void* p,int t,char* file,int line)
{
	p = realloc(p,t);
    printf("File=%s, line=%d, function=%s, reallocated the memory segment at address %p to a new size %d\n", file, line, PRINT_TRACER(), p, t);
	return p;
}

// function MALLOC calls malloc and prints info about memory usage.
void* MALLOC(int t,char* file,int line)
{
	void* p;
	p = malloc(t);
    printf("File=%s, line=%d, function=%s, allocated new memory segment at address %p to size %d\n", file, line, PRINT_TRACER(), p, t);
	return p;
}

// function FREE calls free and prints info about memory usage.
void FREE(void* p,char* file,int line)
{
    printf("File=%s, line=%d, function=%s, deallocated the memory segment at address %p\n", file, line, PRINT_TRACER(), p);
    free(p);
}

#define realloc(a,b) REALLOC(a,b,__FILE__,__LINE__)
#define malloc(a) MALLOC(a,__FILE__,__LINE__)
#define free(a) FREE(a,__FILE__,__LINE__)

// line input nodes
struct node {
  char* line;               // ptr to input line
  int line_num;             // in order line was read
  struct node* next;        // ptr to next frama
};

static struct node* head = NULL;       // ptr to the top of the stack
static struct node* tail = NULL;       // ptr to the end of the stack

// assigns a line num and input pointer to a new node at the end of a linked list
void add_node(int line_num, char* line_ptr) {
    PUSH_TRACE("addNode");



    struct node* new_node = (struct node*) malloc(sizeof(struct node));
    new_node->line_num = line_num;
    new_node->line = line_ptr;
    new_node->next = NULL;

    // checking if list is empty
    if (head == NULL) {
        new_node->next = NULL;
        head = new_node;
    }
    // initializing tail if it doesn't exist
    if (tail != NULL) {
        tail->next = new_node;
    }
    tail = new_node;
    POP_TRACE();
}

// printing all input lines from stdin by iterating through linked list
void print_nodes(struct node* current_node) {
    PUSH_TRACE("print_nodes");
    printf("Node %d: %s\n", current_node->line_num, current_node->line);
    fflush(stdout);
    // recursive call if end of linked list is not reached
    if (current_node->next != NULL) {
        print_nodes(current_node->next);
    }
    POP_TRACE();
}

// frees up memory taken up by the linked list storing lines
void free_linked_list(struct node* head_node) {
    PUSH_TRACE("free_linked_list");
    struct node* current = head_node;
    // frees up node and continues until tail is reached
    while(current != tail) {
        struct node* temp = current->next;
        free(current);
        current = temp;
    }
    // frees up memory taken by tail
    free(tail);
    POP_TRACE();
}

// resize the lines array by doubling its size
char** resize_array(char** lines, int *array_capacity) {
    PUSH_TRACE("resize_array");
    *array_capacity *= 2;
    lines = realloc(lines, *array_capacity * sizeof(char*));
    POP_TRACE();
    return lines;
}

// free the lines array and its contents
void free_lines_array(char** lines, int line_count) {
    PUSH_TRACE("free_lines_array");
    for (int i = 0; i < line_count; i++) {
        free(lines[i]);
    }
    free(lines);
    POP_TRACE();
}
// i think it should be a void func
void addArray(char** lines, int line_count, char line[]) {
  PUSH_TRACE("addArray");

  // store string into a pointer
  char *line_ptr = (char*) malloc(strlen(line) + 1);
  strcpy(line_ptr, line);

//   // check to see if we need to reallocate memory for line pointer array
//   if (line_count > *array_capacity) {
//     lines = resize_array(lines, array_capacity);
//   }

  //adding to array
  lines[line_count-1] = line_ptr;
  printf("array[%d] = \"%s\"\n", line_count - 1, line_ptr);
  fflush(stdout);
  free(line_ptr);
  POP_TRACE();
}

/**
 * This function reads from stdin line by line and adds commands to an array
 * and linked list.
 * Assumption:
 * 1. Each command is at most 100 characters.
 * 2. Each line contains a new command.
 * 3. Each command is separated by a new line.
 * 4. Input file contains only valid characters.
 *
 * Input parameters: stdin
 * Returns: memtrace.out file 
**/
int main() {
    PUSH_TRACE("main");
    char **lines = malloc(10 * sizeof(char*));
    char input[100];
    int line_count = 0;
    int array_capacity = 10;
    FILE *fp;
    int fd_out;
    char *filename = "memtrace.out";

    fd_out = open(filename, O_RDWR | O_CREAT | O_APPEND, 0777);
    dup2(fd_out, 1);

    // reads through the file line by line or until EOF signal
    while (fgets(input, MAX_COMMAND_LENGTH, stdin) != NULL) {

        // increment command line count
        line_count++;
        // removes new line character from line
        input[strcspn(input, "\n")] = 0;

        // store string into a pointer
        char *line_ptr = (char*) malloc(strlen(input) + 1);
        strcpy(line_ptr, input);

        // check to see if we need to reallocate memory for line pointer array
        if (line_count > array_capacity) {
            lines = resize_lines_array(lines, &array_capacity);
        }
        addArray(lines, line_count, input);
//         lines[line_count-1] = line_ptr;
//         printf("array[%d] = \"%s\"\n", line_count - 1, line_ptr);
//         fflush(stdout);

        // creating node
        add_node(line_count, line_ptr);
        free(line_ptr);
    }

    print_nodes(head);

    // frees up memory used by array
    free_lines_array(lines, line_count);
    // frees up memory used by linked list
    free_linked_list(head);
    
    close(fd_out);
    // frees main and global trace nodes
    POP_TRACE();
    POP_TRACE();
    exit(0);
}
