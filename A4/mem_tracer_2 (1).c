#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdarg.h>


//linked list of pointers to function identifiers
//TRACE_TOP: head of the list
struct TRACE_NODE_STRUCT {
    // ptr to function identifier
    char* functionid;               
    // ptr to next node 
    struct TRACE_NODE_STRUCT* next; 
};
typedef struct TRACE_NODE_STRUCT TRACE_NODE;
// ptr to the top of the stack
static TRACE_NODE* TRACE_TOP = NULL;    


/* --------------------------------*/
/* function PUSH_TRACE */
/* The purpose of this stack is to trace the sequence of function calls,
just like the stack in your computer would do. The "global" string denotes the start of the 
function call trace */

// Push p to the stack
void PUSH_TRACE(char* p)          
{
    TRACE_NODE* tnode;
    static char glob[]="global";

    if(TRACE_TOP==NULL) {

        // initialize the stack with "global" identifier
        TRACE_TOP=(TRACE_NODE*) malloc(sizeof(TRACE_NODE));

        // no recovery needed if allocation failed, this is only
        // used in debugging, not in production
        if(TRACE_TOP==NULL) {
          printf("PUSH_TRACE: memory allocation error\n");
          exit(1);
        }

      TRACE_TOP->functionid = glob;
      TRACE_TOP->next=NULL;
    }

    // create the node for p    
    tnode = (TRACE_NODE*) malloc(sizeof(TRACE_NODE));

    // no recovery needed if allocation failed, this is only
    // used in debugging, not in production
    if (tnode==NULL) {
        printf("PUSH_TRACE: memory allocation error\n");
        exit(1);
    }

    tnode->functionid=p;
    // insert fnode as the first in the list
    tnode->next = TRACE_TOP;  
    // point TRACE_TOP to the first node
    TRACE_TOP=tnode;          

}
/*end PUSH_TRACE*/

/* --------------------------------*/
/* function POP_TRACE */
/* Pop a function call from the stack */

// remove the op of the stack
void POP_TRACE()    
{
    TRACE_NODE* tnode;
    tnode = TRACE_TOP;
    TRACE_TOP = tnode->next;
    free(tnode);

}/*end POP_TRACE*/



/* ---------------------------------------------- */
/* function PRINT_TRACE prints out the sequence of function calls that are on the stack at this instance */
/* For example, it returns a string that looks like: global:funcA:funcB:funcC. */
/* Printing the function call sequence the other way around is also ok: funcC:funcB:funcA:global */
char* PRINT_TRACE()
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
    for(i=1, tnode=TRACE_TOP->next;
        tnode!=NULL && i < depth;
        i++,tnode=tnode->next) {
    j = strlen(tnode->functionid);             // length of what we want to add
    if (length+j+1 < 100) {              // total length is ok
        sprintf(buf+length,":%s",tnode->functionid);
        length += j+1;
    }else                                // it would be too long
        break;
    }
    return buf;
} /*end PRINT_TRACE*/

// -----------------------------------------
// function REALLOC calls realloc
// TODO REALLOC should also print info about memory usage.
// For instance, example of print out:
// "File tracemem.c, line X, function F reallocated the memory segment at address A to a new size S"
// Information about the function F should be printed by printing the stack (use PRINT_TRACE)
void* REALLOC(void* p,int t,char* file,int line)
{
    p = realloc(p,t);
    // print info about memory usage
    printf("File %s, line %d, function %s reallocated the memory segment at address %p to a new size %d\n", file, line, PRINT_TRACE(), p, t);
    return p;
}

// -------------------------------------------
// function MALLOC calls malloc
// TODO MALLOC should also print info about memory usage.
// For instance, example of print out:
// "File tracemem.c, line X, function F allocated new memory segment at address A to size S"
// Information about the function F should be printed by printing the stack (use PRINT_TRACE)
void* MALLOC(int t,char* file,int line)
{
    void* p;
    p = malloc(t);
    printf("File %s, line %d, function %s allocated new memory segment at address %p to size %d\n", file, line, PRINT_TRACE(), p, t);
    return p;
}

// ----------------------------------------------
// function FREE calls free
// TODO FREE should also print info about memory usage.
// For instance, example of print out:
// "File tracemem.c, line X, function F deallocated the memory segment at address A"
// Information about the function F should be printed by printing the stack (use PRINT_TRACE)
void FREE(void* p,char* file,int line)
{
    free(p);
    printf("File %s, line %d, function %s deallocated the memory segment at address %p\n", file, line, PRINT_TRACE(), p);
}

#define realloc(a,b) REALLOC(a,b,__FILE__,__LINE__)
#define malloc(a) MALLOC(a,__FILE__,__LINE__)
#define free(a) FREE(a,__FILE__,__LINE__)


// -----------------------------------------
// function add_column will add an extra column to a 2d array of ints.
// This function is intended to demonstrate how memory usage tracing of realloc is done
// Returns the number of new columns (updated)
int add_column(int** array,int rows,int columns)
{
    PUSH_TRACE("add_column");
    int i;

    for(i=0; i<rows; i++) {
        array[i]=(int*) realloc(array[i],sizeof(int)*(columns+1));
        array[i][columns]=10*i+columns;
    }
    POP_TRACE();
    return (columns+1);
}// end add_column


// ------------------------------------------
// function make_extend_array
// Example of how the memory trace is done
// This function is intended to demonstrate how memory usage tracing of malloc and free is done
void make_extend_array()
{
    PUSH_TRACE("make_extend_array");
    int i, j;
    int **array;
    int ROW = 4;
    int COL = 3;

    //make array
    array = (int**) malloc(sizeof(int*)*4);  // 4 rows
    for(i=0; i<ROW; i++) {
    array[i]=(int*) malloc(sizeof(int)*3);  // 3 columns
    for(j=0; j<COL; j++)
        array[i][j]=10*i+j;
    }

    //display array
    for(i=0; i<ROW; i++) {
        for(j=0; j<COL; j++){
            printf("array[%d][%d]=%d\n",i,j,array[i][j]);
        }
        // and a new column
        int NEWCOL = add_column(array,ROW,COL);

        // now display the array again
        for(i=0; i<ROW; i++){
            for(j=0; j<NEWCOL; j++){
                printf("array[%d][%d]=%d\n",i,j,array[i][j]);
            }

            //now deallocate it
            for(i=0; i<ROW; i++){
                free((void*)array[i]);
            }
        }
        free((void*)array);

        POP_TRACE();
        return;
    }
}//end make_extend_array


// creates an array of type char **
// initializes array memory with malloc to initial size of 10 char * pointers
void createArray (char ***array, int row){
    PUSH_TRACE("createArray");

    // Row is initialized to 10 and col null
    // col is max char per command and row is number of commands 
    char **arr = (char **)malloc(sizeof(char *) * row);
    for(int i = 0; i < row; i++){
        arr[i] = NULL;
    }
    *array = arr;
    POP_TRACE();
}

// extension of make_extend_array
// prints the array
void printArray(char **arr, int row) 
{
  PUSH_TRACE("printArray"); 

  //for each row, print contents                             
  for (int i = 0; i < row; i++)                          
  {
    printf("array[%d] = %s\n", i, arr[i]); 
  }
  POP_TRACE();                                     
}

// set up a LinkedList
// node struct, each node has an index and a string
struct Node {
    int index;
    char* cmd; // the line
    struct Node* next; // the node that this node points to
};

// initializes head node as null
static struct Node* head = NULL;

// adds node to the end of the linkedlist
void addNode(char* line, int index){
    PUSH_TRACE("addNode");
    struct Node* newNode = (struct Node*)malloc(sizeof(struct Node)); // allocate for node to add
    newNode->cmd = (char*)malloc(strlen(line) + 1); // allocate for line and set cmd as the allocated space
    strncpy(newNode->cmd, line, strlen(line) + 1); // copy line to node newNode cmd
    newNode->index = index;
    newNode->next = NULL; // set next as NULL (tail)

    if(head == NULL){ // if the linkedList is empty, set node newNode as the head
        head = newNode;
    }

    // if linkedlist is not empty 
    else{ 
        struct Node* temp = head;
        while(temp->next != NULL){ // iterate to the end of the linkedList
            temp = temp->next;
        }
        // set the next of temp to the new node
        temp->next = newNode;
    }
    POP_TRACE();
}

// recursively prints to std_out the content of all nodes in the linked list
void PrintNodes(struct Node* theNode){
    PUSH_TRACE("PrintNodes");

    if(theNode != NULL){ // theNode will be null when reached the end of the linked list
        printf("Node printed: index = %d, command = %s\n", theNode->index, theNode->cmd);
        PrintNodes(theNode->next);
    }

    POP_TRACE();
}

// free linked list after usage
void freeLinkedList(){
    PUSH_TRACE("Free LinkedList");
    //node we iterate through the linkedList with
    struct Node* temp; // temp node holds node to free
    
    while(head != NULL){ // iterates through linked list until all nodes are freed
    	temp = head;
        head = temp->next;
        free(temp->cmd); // frees cmd of current node
        free(temp); // frees current node
    }

    POP_TRACE();
}

// ----------------------------------------------
// function main
int main(int argc, char *argv[])
{
    PUSH_TRACE("main");

    // redirect stdout (fd 1) to memtrace.out file using dup2
    int fd = open("memtrace.out", O_RDWR | O_CREAT | O_TRUNC, 0777); // create memtrace.out file
    dup2(fd, fileno(stdout));

    // create an initial array with 10 rows
    char** array = NULL; // initialize array
    int row = 10, col = 0; // initial array with ten rows; col will be used to hold the char length of commands
    createArray(&array, row);

    // read from stdin and put each line into the array
    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    // reading from stdin
    int counter = 0; // keeps track of # of rows from stdin
    while((read = getline(&line, &len, stdin)) != -1){
        counter++; // when a line is read, increment counter

        // replaces last line of the cmd (newline) with null terminator
        if(line[strlen(line) - 1] == '\n'){
            line[strlen(line) - 1] = '\0';
        }

        col = strlen(line) + 1; // col is the number of chars in a command

        if(counter > row){ // if lines read is greater than number of rows, need to reallocate more rows
            array = (char **) realloc(array, sizeof(char *)*counter); // reallocate for row
            array[counter - 1] = (char *) malloc(sizeof(char)*col); // allocate for column
        }
        else{
            array[counter - 1]=(char*)malloc(sizeof(char)*col); // allocate for column
        }

        // copy string value into allocated memory
        strncpy(array[counter - 1], line, col);
    }

    // insert array into linked list
    for(int i = 0; i < counter; i++){
        addNode(array[i], i);
    }

    // print linked list recursively
    PrintNodes(head);

    // deallocate the linked list
    freeLinkedList();


    // deallocate line
    if(line){
        free(line);
    }

    // deallocate the array
    for(int i = 0; i < counter; i++){
        free(array[i]);
    }
    free(array);

    // close memtrace.out
    close(fd);
    POP_TRACE();

    return(0);
}// end main