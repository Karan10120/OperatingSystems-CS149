/**
 * Description: This program counts the occurences of names in
 * multiple files parallely using multi-threading.
 * Author names: Luc Tang, Karan Gandhi
 * Author emails: luc.tang@sjsu.edu, karan.gandhi@sjsu.edu
 * Last modified date: 05/10/2023
 * Creation date: 05/07/2023
 **/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>

/*****************************************
//CS149 SP23
//Template for assignment 6
//San Jose State University
//originally prepared by Bill Andreopoulos
*****************************************/

//thread mutex lock for access to the log index
pthread_mutex_t tlock1 = PTHREAD_MUTEX_INITIALIZER;

//thread mutex lock for critical sections of allocating THREADDATA
pthread_mutex_t tlock2 = PTHREAD_MUTEX_INITIALIZER;

//thread mutex lock for access to the name counts data structure
pthread_mutex_t tlock3 = PTHREAD_MUTEX_INITIALIZER;

void* thread_runner(void*);
pthread_t tid1, tid2;

//struct points to the thread that created the object.
//This is useful for you to know which is thread1. Later thread1 will also deallocate.
struct THREADDATA_STRUCT
{
  pthread_t creator;
};
typedef struct THREADDATA_STRUCT THREADDATA;

THREADDATA* p=NULL;

//variable for indexing of messages by the logging function.
int logindex=0;
int *logip = &logindex;


//struct that contains name up to 30 chars and it's count
struct NAME_STRUCT
{
  char name[30];
  int count;
};
typedef struct NAME_STRUCT THREAD_NAME;

//array of 100 names
THREAD_NAME names_counts[100];

//node with name_info for a linked list
struct NAME_NODE
{
  THREAD_NAME name_count;
  struct NAME_NODE *next;
};

#define HASHSIZE 101
static struct NAME_NODE *table[HASHSIZE];

//hashing function by first letter
int hash(char input) {
  return input%HASHSIZE;
}

//searching for a node by name
struct NAME_NODE *search(char* name) {
  struct NAME_NODE *current;
  //look through hashtable bucket
  for(current = table[hash(name[0])]; current != NULL; current = current->next) {
    //return node if names match
    if (strcmp(name, current->name_count.name)==0) {
      return current;
    }
  }
  return NULL;
}

//increment count of name node
struct NAME_NODE *update(char* name) {

  struct NAME_NODE *current;
  //checking to see if name is not in hashtable
  if((current = search(name)) == NULL) {
    current = (struct NAME_NODE *)malloc(sizeof(*current));
    //checking if malloc failed
    if(current == NULL) {
      return NULL;
    }
    strcpy(current->name_count.name, name);
    current->name_count.count = 1;
    int value = hash(name[0]);
    //add node to front of linked list (bucket)
    current->next = table[value];
    table[value] = current;
  } else {  //increments count if already in hashtable
    current->name_count.count = current->name_count.count + 1;
  }
  return current;
}

//free memory for all hashtable nodes
void freeHashTable() {
  struct NAME_NODE *current;
  //go through each bucket
  for(int i = 0; i < HASHSIZE; i++) {
    //loops through all nodes in bucket and frees node
    while(table[i]!=NULL){
      current = table[i];
      table[i] = current->next;
      free(current);
    }
  }
}

//prints out current date and time
void getDateTime(char* time_day) {
  time_t current;
  time(&current);
  struct timeval tv;
  gettimeofday(&tv, NULL);
  int hour, min, sec, day, month, year;
  struct tm *localTime = localtime(&current);

  hour = localTime->tm_hour;
  min = localTime->tm_min;
  sec = localTime->tm_sec;
  day = localTime->tm_mday;
  month = localTime->tm_mon + 1;
  year = localTime->tm_year + 1900;

  //conditionals for A.M. and P.M. time
  if (hour < 12) sprintf(time_day, "%02d/%02d/%04d %02d:%02d:%02d AM", month, day, year, hour, min, sec);
  else sprintf(time_day, "%02d/%02d/%04d %02d:%02d:%02d PM", month, day, year, hour - 12, min, sec);
}

/*********************************************************
// function main
*********************************************************/
static char* date = NULL;
/**
 * This function counts and prints names for two files.
 * Assumption:
 * 1. The longest name is 30 characters long.
 * 2. The files have at most 100 unique names in total.
 * 3. Each line contains atmost a name.
 * 4. Each name is separated by a new line.
 * 5. Names contain simple characters of Ascii values from 0-128.
 * 6. Input file contains only valid characters.
 *
 * Input parameters: argc, argv[]
 * Returns: int
 * Prints: 
 * "Log Messages"
 *     . . .
 * "Name Counts"
**/
int main(int argc, char *argv[])
{
  //check if no files are inputted
  if (argc < 2) {
    fprintf(stderr, "No file inputted.\n");
    return(0);
  } else if (argc > 3) {    //checks if more than 2 files are inputted
    fprintf(stderr, "Too many files inputted, please only pass 2 input file.\n");
    return(0);
  }
  date = (char*)malloc(30);

  printf("\n======================= Log Messages ========================\n");

  printf("create first thread\n");
  pthread_create(&tid1,NULL,thread_runner,argv[1]);

  printf("create second thread\n");
  pthread_create(&tid2,NULL,thread_runner,argv[2]);

  printf("wait for first thread to exit\n");
  pthread_join(tid1,NULL);
  printf("first thread exited\n");

  printf("wait for second thread to exit\n");
  pthread_join(tid2,NULL);
  printf("second thread exited\n");

  //TODO print out the sum variable with the sum of all the numbers
  printf("\n======================== Name Counts =======================\n");

  struct NAME_NODE *current_name;
  //going through all buckets in hashtable
  for (int i = 0; i < HASHSIZE; i++) {
    current_name = table[i];
    //printing all names in bucket
    while(current_name != NULL) {
      printf("%s: %d\n", current_name->name_count.name, current_name->name_count.count);
      current_name = current_name->next;
    }
  }

  freeHashTable();
  free(date);

  exit(0);

}//end main


/**********************************************************************
// function thread_runner runs inside each thread
**********************************************************************/
void* thread_runner(void* x)
{
  pthread_t me;
  FILE *fp;
  char* file_name = (char*) x;

  me = pthread_self();
  printf("This is thread %ld (p=%p)\n",me,p);
  getDateTime(date);

  pthread_mutex_lock(&tlock2); // critical section starts
  //allocates THREADDATA memory if it has not been allocated
  if (p==NULL) {
    p = (THREADDATA*) malloc(sizeof(THREADDATA));
    p->creator=me;
  }
  pthread_mutex_unlock(&tlock2);  // critical section ends

  pthread_mutex_lock(&tlock1);  // mutual exclusion logindex and printing starts
  getDateTime(date);
  //checking if this thread created THREADDATA
  if (p!=NULL && p->creator==me) {
    // logindex++;
    printf("Logindex %d, thread %ld, PID %d, %s: This is thread %ld and I created THREADDATA %p\n",++logindex, me, getpid(), date, me, p);
  } else {      //this thread can access THREADDATA
    printf("Logindex %d, thread %ld, PID %d, %s: This is thread %ld and I can access the THREADDATA %p\n",++logindex, me, getpid(), date, me, p);
  }
  pthread_mutex_unlock(&tlock1);  //  mutual exclusion logindex and printing ends

  fp = fopen(file_name, "r");

  //checking if file cannot be opened
  if (fp == NULL) fprintf(stderr, "Can not open file: %s\n", file_name);
  else {    //file was opened and begin reading
    getDateTime(date);
    pthread_mutex_lock(&tlock1);
    logindex++;
    printf("Logindex %d, thread %ld, PID %d, %s: opened file %s\n", logindex, me, getpid(), date, file_name);
    pthread_mutex_unlock(&tlock1);

    char line[31];
    int line_index = 0;

    //reading file line by line
    while(fgets(line, sizeof(line), fp) != NULL) {
      line_index++;
      //checking if line is empty
      if(line[0] == 0 || line[0] == '\n') {
        fprintf(stderr, "Warning - file %s line %d is empty.\n", file_name, line_index);
      } else {    //updates name count in hashtable
        //Removes trailing new line character from string
        line[strcspn(line, "\n")] = 0;

        pthread_mutex_lock(&tlock3);
        update(line);
        pthread_mutex_unlock(&tlock3);
      }
    }
    fclose(fp);
    // fp=NULL;
  }

  pthread_mutex_lock(&tlock1);
  pthread_mutex_lock(&tlock2);  // critical section starts
  //checking if this thread allocated THREADDATA and deletes (frees) it
  if (p!=NULL && p->creator==me) {
    printf("Logindex %d, thread %ld, PID %d, %s: This is thread %ld and I delete THREADDATA\n",++logindex, me, getpid(), date, me);
    free(p);
    p = NULL;
  } else {    //this thread did not create THREADDATA and can access it
    printf("Logindex %d, thread %ld, PID %d, %s: This is thread %ld and I can access the THREADDATA\n", ++logindex, me, getpid(), date, me);
  }
  pthread_mutex_unlock(&tlock2);// critical section ends
  pthread_mutex_unlock(&tlock1);
  pthread_exit(NULL);
  return NULL;

}//end thread_runner
