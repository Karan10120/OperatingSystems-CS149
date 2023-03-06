#include <stdio.h>
<<<<<<< HEAD
#include <unistd.h>
#include <sys/wait.h>

int main()
{
	pid_t pid;
	pid = fork();
	pid = fork();

	if (pid < 0) {
		printf("Error: fork failed\n");
		return 1;
	} else if (pid == 0) {
		printf("hello world from PID %d!\n", getpid());
	} else {
		printf("hello world from PID %d!\n", getpid());
		wait(NULL);
	}
	return 0;
}
=======
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
>>>>>>> 1899582d104d386ddc24626934e53d6bbf78120c
