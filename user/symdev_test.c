#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define DATA_SIZE 64
#define STRING_SIZE 8

int
main(int argc, char* argv[]) {
	printf("testing utility started\n");

	printf("testing device NULL\n");
	printf("===============================\n");
	printf("testing read from null:\n");
	char* buf = malloc(DATA_SIZE);
	int null = open("null", 2);

	int res = read(null, buf, DATA_SIZE);
	printf("read result (should be zero): %d\n", res);
	printf("-------------------------------\n");
	printf("testing write to null:\n");

	res = write(null, buf, 4321948);
	printf("write result (should be 4321948): %d\n", res);
	printf("===============================\n");



	printf("testing device ZERO\n");
	printf("===============================\n");
	printf("testing read from zero:\n");
	int zero = open("zero", 2);

	res = read(zero, buf, DATA_SIZE);
	printf("read result (should be DATA_SIZE): %d\n", res);
	printf("bytes from device zero:\n");

	for (int i = 0; i < STRING_SIZE; i++) {
		for (int j = 0; j < DATA_SIZE / STRING_SIZE; ++j)
		{
			printf("%x ", buf[i * STRING_SIZE + j]);
		}
		printf("\n");
	}

	printf("-------------------------------\n");
	printf("testing write to zero:\n");

	res = write(zero, buf, 4321948);
	printf("write result (should be -1): %d\n", res);
	printf("===============================\n");


	printf("testing device URANDOM\n");
	printf("===============================\n");
	printf("testing read from urandom (default seed):\n");
	int urandom = open("urandom", 2);

	res = read(urandom, buf, DATA_SIZE);
	printf("read result (should be DATA_SIZE): %d\n", res);
	printf("bytes from device urandom:\n");

	for (int i = 0; i < STRING_SIZE; i++) {
		for (int j = 0; j < DATA_SIZE / STRING_SIZE; ++j)
		{
			printf("%x ", buf[i * STRING_SIZE + j]);
		}
		printf("\n");
	}

	printf("-------------------------------\n");
	printf("testing write to urandom with bad size:\n");

	res = write(urandom, buf, 5);
	printf("write result (should be -1): %d\n", res);

	printf("-------------------------------\n");
	printf("testing write to urandom with good size:\n");
	unsigned char seed = 42;
	res = write(urandom, &seed, 1);
	printf("write result (should be 1, seed updated): %d\n", res);
	printf("===============================\n");


	printf("testing device NULLSTAT\n");
	printf("===============================\n");
	printf("testing read from nullstat before writing:\n");
	int nullstat = open("nullstat", 2);

	uint64 nullstat_count = 0;
	res = read(nullstat, &nullstat_count, sizeof(uint64));
	printf("read result (should be sizeof(uint64)): %d\n", res);
	printf("nullstat_count (should be 0): %d\n", (int)nullstat_count);

	printf("-------------------------------\n");
	printf("testing write to nullstat:\n");

	res = write(nullstat, buf, DATA_SIZE);
	printf("write result (should be DATA_SIZE): %d\n", res);

	printf("-------------------------------\n");
	printf("testing read from nullstat after writing:\n");

	res = read(nullstat, &nullstat_count, sizeof(uint64));
	printf("read result (should be sizeof(uint64)): %d\n", res);
	printf("nullstat_count (should be DATA_SIZE): %d\n", (int)nullstat_count);

	printf("-------------------------------\n");
	printf("testing read from nullstat with bad size:\n");

	res = read(nullstat, buf, DATA_SIZE);
	printf("read result (should be -1): %d\n", res);
	printf("===============================\n");


	printf("testing finished\n");
	return 0;
}