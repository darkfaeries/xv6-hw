#include "types.h"
#include "param.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "fs.h"
#include "file.h"
#include "memlayout.h"
#include "riscv.h"
#include "defs.h"
#include "proc.h"
#include "symdev.h"

#define URANDOM_DEFAULT_SEED (unsigned char)113
#define URANDOM_A            (unsigned char)13
#define URANDOM_C            (unsigned char)27

#define BUF_SIZE 128

static struct spinlock stat_lock;
static struct spinlock rng_lock;

// mode == 1 when writing bytes, 0 when reading
// (all in 1 function to isolate the static variable)

static uint64
nullstat(uint64 bytes, int mode) {
	static uint64 written = 0;
	if (mode) {
		written += bytes;
		return 0;
	}
	else {
		return written;
	}
}

// mode == 1 when setting the seed, 0 when reading
// (all in 1 function to isolate the static variable)

static unsigned char
rng(unsigned char new_seed, int mode) {
	static unsigned char seed = URANDOM_DEFAULT_SEED;
	if (mode) {
		seed = new_seed;
		return 0;
	}
	else {
		seed = seed * URANDOM_A + URANDOM_C;
		return seed;
	}
}

// hold stat_lock when calling nullstat and rng_lock when calling rng

// write call device handler
int
symdevwrite(int user_src, uint64 src, int n, short minor)
{
  if (n < 0) {
    return -1;
  }

  // null; do nothing, return success
  if (minor == _NULL) {
    return n;
  }

  // zero; do nothing, return failure
  else if (minor == _ZERO) {
    return -1;
  }

  // urandom; try to update the seed
  else if (minor == _URANDOM) {
    char c;
    if (n == sizeof(c)) { // matches seed size
      if (either_copyin(&c, user_src, src, 1) == -1) {
        return -1;
      }
      acquire(&rng_lock);
      rng((unsigned char)c, 1); // update seed to read value
      release(&rng_lock);
      return 1;
    }
    else return -1; // different size of write, fail
  }

  // nullstat; discard the output and update the static variable
  else if (minor == _NULLSTAT) {
    acquire(&stat_lock);
    nullstat(n, 1);
    release(&stat_lock);
    return n;
  }

  else {
    printf("symdev: unknown minor device number\n");
    return -1;
  }

  return -1;
}


// read call device handler
int
symdevread(int user_dst, uint64 dst, int n, short minor)
{
	if (n < 0) {
		return -1;
	}

	// null; output is EOF, return value is 0 (nothing was read)
	if (minor == _NULL) {
		return 0;
	}

	// zero; output consists of n zero bytes
	// urandom; return n random bytes
	else if (minor == _ZERO || minor == _URANDOM) {
		unsigned char buf[BUF_SIZE];
		if (minor == _ZERO) {
			memset(buf, 0, BUF_SIZE);
		}

		int done = 0;
		while (done < n) {
			int chunk = n - done;
			if (chunk > BUF_SIZE) {
				chunk = BUF_SIZE;
			}

			if (minor == _URANDOM) {
				acquire(&rng_lock);
				for (int i = 0; i < chunk; ++i) {
					buf[i] = rng(0, 0);
				}
				release(&rng_lock);
			}

			if (either_copyout(user_dst, dst + done, buf, chunk) == -1) {
				return done == 0 ? -1 : done;
			}
			done += chunk;
		}

		return n;
	}

	// nullstat; if the right n is given, return the current written value
	else if (minor == _NULLSTAT) {
		if (n != sizeof(uint64)) return -1;
		acquire(&stat_lock);
		uint64 val = nullstat(0, 0);
		release(&stat_lock);
		if (either_copyout(user_dst, dst, (char*)&val, n) == -1) {
			return -1;
		}

		return n;
	}

	else {
		printf("symdev: unknown minor device number\n");
		return -1;
	}

	return -1;
}


void
symdevinit(void) {

	initlock(&stat_lock, "nullstat");
	initlock(&rng_lock, "urandom");

	// connect read and write syscalls to symdevread and symdevwrite
	devsw[SYMBOLIC].read = symdevread;
	devsw[SYMBOLIC].write = symdevwrite;
}
