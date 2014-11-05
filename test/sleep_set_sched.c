#include <sched.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>

int main(int argc, char **argv)
{
	struct sched_param param;
	int ret;

	param.sched_priority = 0;

	ret = sched_setscheduler(0, 6, &param);

	if (ret == -1) {
		printf("Error: %s\n", strerror(errno));
		return 1;
	}

	printf("Returned: %d\n", ret);

	while (1) {
		printf("Printed %d\n", ret++);
		sleep(2);
	}

	return 0;
}
