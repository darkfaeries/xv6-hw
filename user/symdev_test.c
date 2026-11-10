#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"

#define DATA_SIZE 513

static void
check(char* test, int actual, int expected) {
	printf("%s (should be %d): %d\n", test, expected, actual);
	if (actual != expected) {
		fprintf(2, "FAILED: %s\n", test);
		exit(1);
	}
}

int
main(int argc, char* argv[]) {
	unsigned char buf[DATA_SIZE];
	memset(buf, 42, sizeof(buf));
	printf("testing utility started\n");

	printf("testing device NULL\n");
    printf("===============================\n");
	int null = open("null", O_RDWR);
	check("read from null (EOF)", read(null, buf, DATA_SIZE), 0);
	check("first byte after EOF", buf[0], 42);
	check("write to null", write(null, buf, DATA_SIZE), DATA_SIZE);
	close(null);

	printf("testing device ZERO\n");
    printf("===============================\n");
	int zero = open("zero", O_RDWR);
	printf("testing read from zero across the 128-byte buffer boundary\n");
	check("bytes read", read(zero, buf, DATA_SIZE), DATA_SIZE);
	int nonzero = 0;
	for (int i = 0; i < DATA_SIZE; i++) {
		if (buf[i] != 0) {
			nonzero++;
		}
	}
	check("nonzero bytes", nonzero, 0);
	check("write to zero", write(zero, buf, 1), -1);
	close(zero);

	printf("testing device URANDOM\n");
    printf("===============================\n");
	int urandom = open("urandom", O_RDWR);
	unsigned char seed = 42;
	check("write with bad seed size", write(urandom, buf, 2), -1);
	check("write seed 42", write(urandom, &seed, sizeof(seed)), sizeof(seed));
	check("bytes read from urandom", read(urandom, buf, DATA_SIZE), DATA_SIZE);
	int mismatches = 0;
	for (int i = 0; i < DATA_SIZE; i++) {
		seed = seed * 13 + 27;
		if (buf[i] != seed) {
			mismatches++;
		}
	}
	check("bytes differing from the sequence for seed 42", mismatches, 0);
	close(urandom);

	printf("testing device NULLSTAT\n");
    printf("===============================\n");
	int nullstat = open("nullstat", O_RDWR);
	uint64 before, after;
	check("read count before writing", read(nullstat, &before, sizeof(before)), sizeof(before));
	check("write to nullstat", write(nullstat, buf, DATA_SIZE), DATA_SIZE);
	check("read count after writing", read(nullstat, &after, sizeof(after)), sizeof(after));
	printf("counter increase (should be %d): %lu\n", DATA_SIZE, after - before);
	if (after - before != DATA_SIZE) {
		exit(1);
	}
	check("read count with bad size", read(nullstat, buf, 1), -1);
	close(nullstat);

	printf("testing finished: OK\n");
	return 0;
}
