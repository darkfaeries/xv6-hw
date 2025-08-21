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

// mode == 1 when writing bytes, 0 when reading
// (all in 1 function to isolate the static variable)

uint64
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

unsigned char
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

// write call device handler
int
symdevwrite(int user_src, uint64 src, int n, short minor)
{
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
  	if (n == 1) { //  matches seed size; 
  		if (either_copyin(&c, user_src, src, 1) == -1) {
  			return -1;
  		}
  		rng((unsigned char)c, 1); // update seed to read value
  		return 1;
  	}
  	else return -1; // different size of read, fail
  }

  // nullstat; discard the output and update the static variable
  else if (minor == _NULLSTAT) {
  	nullstat(n, 1);
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
	// null; output is EOF, return value is 0 (nothing was read)
	if (minor == _NULL) {
		char eof = 0;
		if (either_copyout(user_dst, dst, &eof, 1) == -1) {
  			return -1;
  		}

  		return 0;
	}

	// zero; output consists of n zero bytes
	else if (minor == _ZERO) {
		char buf[BUF_SIZE];
		memset(buf, 0, BUF_SIZE);

		int times = n / BUF_SIZE;
		for (int i = 0; i < times; i++) {
			if (either_copyout(user_dst + i * BUF_SIZE, dst + i * BUF_SIZE, buf, BUF_SIZE) == -1) {
  				return -1;
  			}
		}

		if (n != times * BUF_SIZE) {
			if (either_copyout(user_dst + times * BUF_SIZE, dst + times * BUF_SIZE, buf, n - times * BUF_SIZE) == -1) {
  				return -1;
  			}
		}
		
  		return n;
	}

	// urandom; return n random bytes
	else if (minor == _URANDOM) {
		unsigned char buf[BUF_SIZE];

		for (int i = 0; i < n; ++i)
		{
			buf[i] = rng(0, 0);
		}

		int times = n / BUF_SIZE;
		for (int i = 0; i < times; i++) {
			if (either_copyout(user_dst + i * BUF_SIZE, dst + i * BUF_SIZE, buf, BUF_SIZE) == -1) {
  				return -1;
  			}
		}

		if (n != times * BUF_SIZE) {
			if (either_copyout(user_dst + times * BUF_SIZE, dst + times * BUF_SIZE, buf, n - times * BUF_SIZE) == -1) {
  				return -1;
  			}
		}
		
  		return n;
	} 

	// nullstat; if the right n is given, return the current written value
	else if (minor == _NULLSTAT) {
		if (n != sizeof(uint64)) return -1;
		uint64 val = nullstat(0, 0);
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

	// connect read and write syscalls to symdevread and symdevwrite
	devsw[SYMBOLIC].read = symdevread;
	devsw[SYMBOLIC].write = symdevwrite;
}