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
int log_index=0;
int *logip = &log_index;


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

#define HASHSIZE 26
static struct NAME_NODE *table[HASHSIZE];

// WHITE ELEPHANT
int hash(char input)
{
    return input%26;
}

// find duplicates
struct NAME_NODE *find(char* name)
{
    struct NAME_NODE *names;
    for(names = table[hash(name[0])]; names != NULL; names = names->next)
    {
        if(strcmp(name, names->name_count.name)==0)
            return names;
    }
    return NULL;
}

// insert node in hash table or update count
struct NAME_NODE *count(char* name)
{
    struct NAME_NODE *names;
    if((names = find(name)) == NULL)
    {
        names = (struct NAME_NODE *)malloc(sizeof(*names));
        if(names == NULL)
            return NULL;
        strcpy(names->name_count.name, name);
        names->name_count.count = 1;
        int value = hash(name[0]);

        names->next = table[value];
        table[value] = names;
    }
    else
    {
        names->name_count.count = names->name_count.count + 1;
    }
    return names;
}

// empties hash so no memory leak occurs
void clear()
{
    struct NAME_NODE *names;
    for(int i = 0; i < HASHSIZE; i++)
    {
        while(table[i]!=NULL)
        {
            names = table[i];
            table[i] = names->next;
            free(names);
        }
    }
}

// local time and date to print out
void timeAndDay(char* time_day)
{
    time_t current;
    time(&current);
    int sec, min, hour, day, month, year;
    struct tm *local = localtime(&current);

    sec = local->tm_sec;
    min = local->tm_min;
    hour = local->tm_hour;
    day = local->tm_mday;
    month = local->tm_mon + 1;
    year = local->tm_year + 1900;

    // prints time
    // first 12 hours of the day
    if(hour < 12)
        sprintf(time_day, "%02d/%02d/%d %02d:%02d:%02d AM", day, month, year, hour, min, sec);
    // second half of the day
    else
        sprintf(time_day, "%02d/%02d/%d %02d:%02d:%02d PM", day, month, year, hour - 12, min, sec);
}

static char* date = NULL;

/*********************************************************
// function main 
*********************************************************/
int main(int argc, char *argv[])
{
    //TODO similar interface as A2: give as command-line arguments three filenames of numbers (the numbers in the files are newline-separated).
    if(argc != 3)
    {
        printf("Error: incorrect number of files (only 2 allowed)");
        return 0;
    }
    date = (char*)malloc(30);

    printf("\n========================LOG MESSAGES========================\n");

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
    printf("\n========================NAME COUNTS========================\n");

    struct NAME_NODE *names;
    for(int i = 0; i < HASHSIZE; i++)
    {
        names = table[i];
        while(names != NULL)
        {
            printf("%s: %d\n", names->name_count.name, names->name_count.count);
            names = names->next;
        }
    }

    clear();
    free(date);
    exit(0);

}// end main


/**********************************************************************
// function thread_runner runs inside each thread 
**********************************************************************/
void* thread_runner(void* x)
{
    FILE *fp; //for file reading
    char* file_name = (char*) x;
    
    pthread_t me;
    me = pthread_self();
    printf("This is thread %p (p=%p)\n",me,p);
    timeAndDay(date);

    pthread_mutex_lock(&tlock1);
    log_index++;
    printf("Logindex %d, thread %p, PID %d, %s: This is thread %p (p=%p)\n", log_index, me, getpid(), date , me, p);
    pthread_mutex_unlock(&tlock1);


    pthread_mutex_lock(&tlock2); // critical section starts
    if (p==NULL) 
    {
        p = (THREADDATA*) malloc(sizeof(THREADDATA));
        p->creator=me;
    }
    pthread_mutex_unlock(&tlock2);  // critical section ends

    pthread_mutex_lock(&tlock1);
    timeAndDay(date);
    if (p!=NULL && p->creator==me) 
    {
        log_index++;
        printf("Logindex %d, thread %p, PID %d, %s: This is thread %p and I created THREADDATA %p\n", log_index, me, getpid(), date, me, p);
    } 
    else 
    {
        log_index++;
        printf("Logindex %d, thread %p, PID %d, %s: This is thread %p and I can access the THREADDATA %p\n", log_index, me, getpid(), date, me, p);
    }
    pthread_mutex_unlock(&tlock1);


    // read file 
    fp = fopen(file_name, "r");
    
    if(fp == NULL)
    {
        fprintf(stderr, "range: cannot open file %s\n", file_name);
    } 
    else {
        timeAndDay(date);
        pthread_mutex_lock(&tlock1);
        log_index++;
        printf("Logindex %d, thread %p, PID %d, %s: opened file %s\n", log_index, me, getpid(), date, file_name);
        pthread_mutex_unlock(&tlock1);

        char str[30]; //initialize str to at most 30 characters
        int line_index = 0; 

        //while loop to read from file 
        while(fgets(str, 30, fp) != NULL) 
        {
            //increment line index
            line_index++;

            if(str[0] == 0 || str[0] == '\n')
            {
                fprintf(stderr,"Warning - file %s line %d is empty.\n", file_name, line_index);
                continue;
            }

            if(str[0] == ' ')
            {
                continue;
            }

            //if end of str is a newline
            if(str[strlen(str) - 1] == '\n')
            {
                //delete newline
                str[strlen(str) - 1] = 0;
            }

            struct NAME_NODE *names = find(str);
            
            pthread_mutex_lock(&tlock3);
            
            if(names == NULL)
            {
                count(str);
            } 

            //increment name count
            else 
            {
                names->name_count.count = names->name_count.count + 1;
            }
            
            pthread_mutex_unlock(&tlock3);  
        }
    }

    pthread_mutex_lock(&tlock1);
    
    log_index++;
    
    pthread_mutex_unlock(&tlock1);  
    pthread_mutex_lock(&tlock2);
    timeAndDay(date);

    if (p!=NULL && p->creator==me) {
        printf("Logindex %d, thread %p, PID %d, %s: This is thread %p and I delete THREADDATA\n", log_index, me, getpid(), date , me);
        //free p
        free(p);  
        p = NULL;

    }
    pthread_mutex_unlock(&tlock2);  
    pthread_exit(NULL);


    return NULL;

}// end thread_runner 
