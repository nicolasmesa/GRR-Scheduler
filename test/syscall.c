#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/syscall.h>


int sched_set_CPUgroup(int numCPU, int group)
{
	return syscall(378, numCPU, group);
}

int main(int argc, char **argv)
{
	int numCPU, group;

	numCPU = atoi(argv[1]);
	group = atoi(argv[2]);

	if (sched_set_CPUgroup(numCPU, group))
		printf("Error: %s\n", strerror(errno));

	return 0;
}
