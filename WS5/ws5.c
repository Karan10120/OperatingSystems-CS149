#include <stdio.h>
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
}
