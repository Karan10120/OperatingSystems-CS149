#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

/*****************************************
//CS149 SP23
//Template for assignment 6
//San Jose State University
//originally prepared by Bill Andreopoulos
*****************************************/

//thread mutex lock for access to the log index
//TODO you need to use this mutexlock for mutual exclusion
//when you print log messages from each thread
pthread_mutex_t tlock1 = PTHREAD_MUTEX_INITIALIZER;


//thread mutex lock for critical sections of allocating THREADDATA
//TODO you need to use this mutexlock for mutual exclusion
pthread_mutex_t tlock2 = PTHREAD_MUTEX_INITIALIZER;


//thread mutex lock for access to the name counts data structure
//TODO you need to use this mutexlock for mutual exclusion
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


//The name counts.
// You can use any data structure you like, here are 2 proposals: a linked list OR an array (up to 100 names).
//The linked list will be faster since you only need to lock one node, while for the array you need to lock the whole array.
//You can use a linked list template from A5. You should also consider using a hash table, like in A5 (even faster).
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

int hash(char input) {
  return input%HASHSIZE;
}

struct NAME_NODE *search(char* name) {
  struct NAME_NODE *current;
  for(current = table[hash(name[0])]; current != NULL; current = current->next) {
    if (strcmp(name, current->name_count.name)==0) {
      return current;
    }
  }
  return NULL;
}

struct NAME_NODE *update(char* name) {

  struct NAME_NODE *current;
  if((current = search(name)) == NULL) {
    current = (struct NAME_NODE *)malloc(sizeof(*current));
    if(current == NULL) {
      return NULL;
    }
    strcpy(current->name_count.name, name);
    current->name_count.count = 1;
    int value = hash(name[0]);

    current->next = table[value];
    table[value] = current;
  } else {
    current->name_count.count = current->name_count.count + 1;
  }
  return current;
}

void freeHashTable() {
  struct NAME_NODE *current;
  for(int i = 0; i < HASHSIZE; i++) {
    while(table[i]!=NULL){
      current = table[i];
      table[i] = current->next;
      free(current);
    }
  }
}

//want to charge to return char*
void getDateTime(char* time_day) {
  time_t current;
  time(&current);
  int hour, min, sec, day, month, year;
  struct tm *localTime = localtime(&current);

  hour = localTime->tm_hour;
  min = localTime->tm_min;
  sec = localTime->tm_sec;
  day = localTime->tm_mday;
  month = localTime->tm_mon + 1;
  year = localTime->tm_year + 1900;

  if (hour < 12) sprintf(time_day, "%02d/%02d/%04d %02d:%02d:%02d AM", month, day, year, hour, min, sec);
  else sprintf(time_day, "%02d/%02d/%04d %02d:%02d:%02d PM", month, day, year, hour - 12, min, sec);
}

/*********************************************************
// function main
*********************************************************/
static char* date = NULL;

int main(int argc, char *argv[])
{
  //TODO similar interface as A2: give as command-line arguments three filenames of numbers (the numbers in the files are newline-separated).

  if (argc < 2) {
    fprintf(stderr, "No file inputted.\n");
    return(0);
  } else if (argc > 3) {
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
  for (int i = 0; i < HASHSIZE; i++) {
    current_name = table[i];
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
  getDateTime(date);        //maybe comment out line

  pthread_mutex_lock(&tlock2); // critical section starts
  if (p==NULL) {
    p = (THREADDATA*) malloc(sizeof(THREADDATA));
    p->creator=me;
  }
  pthread_mutex_unlock(&tlock2);  // critical section ends

  pthread_mutex_lock(&tlock1);  // mutual exclusion logindex and printing starts
  getDateTime(date);
  if (p!=NULL && p->creator==me) {
    logindex++;
    printf("Logindex %d, thread %ld, PID %d, %s: This is thread %ld and I created THREADDATA %p\n",logindex, me, getpid(), date, me, p);
  } else {
    printf("Logindex %d, thread %ld, PID %d, %s: This is thread %ld and I can access the THREADDATA %p\n",logindex, me, getpid(), date, me, p);
  }
  pthread_mutex_unlock(&tlock1);  //  mutual exclusion logindex and printing ends


  /**
   * //TODO implement any thread name counting functionality you need.
   * Assign one file per thread. Hint: you can either pass each argv filename as a thread_runner argument from main.
   * Or use the logindex to index argv, since every thread will increment the logindex anyway
   * when it opens a file to print a log message (e.g. logindex could also index argv)....
   * //Make sure to use any mutex locks appropriately
   */

  fp = fopen(file_name, "r");

  if (fp == NULL) fprintf(stderr, "Can not open file: %s\n", file_name);
  else {
    getDateTime(date);
    pthread_mutex_lock(&tlock1);
    logindex++;
    printf("Logindex %d, thread %ld, PID %d, %s: opened file %s\n", logindex, me, getpid(), date, file_name);
    pthread_mutex_unlock(&tlock1);

    char line[31];
    int line_index = 0;

    while(fgets(line, sizeof(line), fp) != NULL) {
      //line_index++;
      if(line[0] == 0 || line[0] == '\n') {
        fprintf(stderr, "Warning - file %s line %d is empty.\n", file_name, line_index);
      } else {
        //Removes trailing new line character from string
        line[strcspn(line, "\n")] = 0;

        pthread_mutex_lock(&tlock3);
        //struct NAME_NODE *nameNode = update(line);
        update(line);
        pthread_mutex_unlock(&tlock3);
      }
    }
  }



  pthread_mutex_lock(&tlock1);
  logindex++;
  pthread_mutex_unlock(&tlock1);

  // TODO use mutex to make this a start of a critical section
  pthread_mutex_lock(&tlock2);  // critical section starts
  if (p!=NULL && p->creator==me) {
    printf("Logindex %d, thread %ld, PID %d, %s: This is thread %ld and I delete THREADDATA\n",logindex, me, getpid(), date, me);
    free(p);
    p = NULL;
  /**
   * TODO Free the THREADATA object.
   * Freeing should be done by the same thread that created it.
   * See how the THREADDATA was created for an example of how this is done.
   */
  } else {
    printf("Logindex %d, thread %ld, PID %d, %s: This is thread %ld and I can access the THREADDATA\n", logindex, me, getpid(), date, me);
  }
  pthread_mutex_unlock(&tlock2);// critical section ends

  pthread_exit(NULL);
  return NULL;

}//end thread_runner
