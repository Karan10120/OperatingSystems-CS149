#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

int main (void) {
    fork();
    fork();
    wait(NULL);
    wait(NULL);
    printf("hello world from PID %03d!\n", getpid() % 1000);
    return 0;
}
