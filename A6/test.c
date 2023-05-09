#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>

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

//linked list node for name_info
struct NAME_NODE
{
  THREAD_NAME name_count;
  struct NAME_NODE *next;
};
struct NAME_NODE *head = NULL;

/*********************************************************
// function main
*********************************************************/
int main(int argc, char *argv[])
{
  if(argc != 4){
    fprintf(stderr, "Usage: %s file1 file2 file3\n", argv[0]);
    return 1;
  }

  //TODO similar interface as A2: give as command-line arguments three filenames of numbers (the numbers in the files are newline-separated).
  //parse command-line arguments and assign them to threads
  char *file1 = argv[1];
  char *file2 = argv[2];

  printf("create first thread");
  pthread_create(&tid1,NULL,thread_runner,file1);

  printf("create second thread");
  pthread_create(&tid2,NULL,thread_runner,file2);

  printf("wait for first thread to exit");
  pthread_join(tid1,NULL);
  printf("first thread exited");

  printf("wait for second thread to exit");
  pthread_join(tid2,NULL);
  printf("second thread exited");

}

/**********************************************************************
// function thread_runner runs inside each thread
**********************************************************************/
void* thread_runner(void* x)
{
  pthread_t me;

  me = pthread_self();
  printf("This is thread %ld (p=%p)",me,p);

  pthread_mutex_lock(&tlock2); // critical section starts
  if (p==NULL) {
    p = (THREADDATA*) malloc(sizeof(THREADDATA));
    p->creator=me;
  }
  pthread_mutex_unlock(&tlock2);  // critical section ends

  if (p!=NULL && p->creator==me) {
    printf("This is thread %ld and I created THREADDATA %p",me,p);
  } else {
    printf("This is thread %ld and I can access the THREADDATA %p",me,p);
}

