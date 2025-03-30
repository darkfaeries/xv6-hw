#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "spinlock.h"
#include "param.h"
#include "proc.h"

uint64 sys_ps_listinfo(void)
{
	uint64 buf;
	int lim;

	argaddr(0, &buf);
	argint(1, &lim);

	if (lim <= 0) {
		return -3;
	}

	// buf == null
	if (buf == 0) {
		return ps_count_used();
	}

	// get process pagetable for copyout
	struct proc* p = myproc();


	int ret = ps_routine(p->pagetable, buf, lim);

	if (ret == -1) {
		return -1;
	}

	if (ret == -2) {
		return -2;
	}

	return ret;
}