#include "kernel/types.h"
#include "user/user.h"

int
main(int argc, char* argv[])
{
	int lim = 100;
	int code;

	struct procinfo* buf = malloc(lim * sizeof(struct procinfo));

	if (buf == 0) {
		printf("malloc fail\n");
		exit(1);
	}

	printf("test with buf == NULL: should return number of processes\n");

	code = ps_listinfo(0, lim);
	printf("%d\n", code);


	printf("==========================\n");
	printf("test with lim < 0: should return -3\n");

	code = ps_listinfo(buf, -5);
	printf("%d\n", code);


	printf("==========================\n");
	printf("test with good address and lim = 1: should return -2\n");

	code = ps_listinfo(buf, 1);
	printf("%d\n", code);


	printf("==========================\n");
	printf("test with random address: should return -1\n");

	code = ps_listinfo((struct procinfo*)1, 1);
	printf("%d\n", code);


	printf("==========================\n");
	printf("test with good address and lim = 100: should return a positive number\n");

	code = ps_listinfo(buf, lim);
	printf("%d\n", code);

	return 0;
}