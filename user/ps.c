#include "kernel/types.h"
#include "user/user.h"

int
main(int argc, char* argv[])
{
	static char *states[] = {
  		[_SLEEPING]  "sleep ",
  		[_RUNNABLE]  "runble",
  		[_RUNNING]   "run   ",
  		[_ZOMBIE]    "zombie"
  	};
  	char* state;


	int lim = 10;
	int code = -1;

	struct procinfo* buf;

	while (1) {
		buf = malloc(lim * sizeof(struct procinfo));

		if (buf == 0) {
			printf("ps: malloc fail\n");
			exit(1);
		}

		code = ps_listinfo(buf, lim);

		if (code == -3) {
			printf("ps: lim int overflow\n");
			exit(2);
		}

		if (code == -1) {
			printf("ps: memory error\n");
			exit(3);
		}

		if (code >= 0) break;
		lim *= 2;
	}

	const uint padding_len = 5;
	char* name;
	char* parent = malloc(16); // max process name length
	int pflag;

	printf("id  name   state   ppid  pname\n");
	printf("------------------------------\n");
	for (int i = 0; i < code; ++i)
	{
		state = states[buf[i].status];

		// adding padding for process name (beautification)
		name = buf[i].name;
		uint name_len = strlen(name);
		for (uint i = name_len; i < padding_len; ++i)
		{
			name[i] = ' ';
		}
		name[padding_len] = '\0';

		// looking in the list for a process with pid = parent to get its name
		pflag = 0;
		for (int j = 0; j < code; ++j)
		{
			if (buf[j].pid == buf[i].parent) {
				strcpy(parent, buf[j].name);
				pflag = 1;
				break;
			}
		}
		if (pflag == 0) *parent = '\0';


		printf("%d   %s  %s  %d     %s\n", buf[i].pid, buf[i].name, state, buf[i].parent, parent);
	}
}