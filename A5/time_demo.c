/* Program to demonstrate time taken by function fun() */
#include <stdio.h> 
#include <time.h> 
  
// A function that terminates when enter key is pressed 
void fun() 
{ 
    printf("fun() starts \n"); 
    printf("Press enter to stop fun \n"); 
    while(1) 
    { 
        if (getchar()) 
            break; 
    } 
    printf("fun() ends \n"); 
} 
  
// The main program calls fun() and measures time taken by fun() 
int main() 
{ 
    // Calculate the time taken by fun() 
    struct timespec start, finish;
    double elapsed;

    //clock_t t = clock();
    clock_gettime(CLOCK_MONOTONIC, &start);
    printf("start %ld\n", start.tv_sec);
    fun(); 

    clock_gettime(CLOCK_MONOTONIC, &finish);
    printf("finish %ld\n", finish.tv_sec);
    //printf("CLOCKSPERSEC %ld\n", CLOCKS_PER_SEC);
    //double time_taken = ((double)t)*1000.0/CLOCKS_PER_SEC; // in seconds 
    elapsed = (finish.tv_sec - start.tv_sec);
    //Alternative with more precision:
    //elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
 
    printf("fun() took %f seconds to execute \n", elapsed); 
    return 0; 
} 
