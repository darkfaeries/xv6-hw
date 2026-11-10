#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"

int
main(int argc, char* argv[]) {
	static const char hex[] = "0123456789abcdef";
	uchar buf[512];
	char line[48];  // 16 pairs of digits with spaces or a final newline.
	int left = 0, used = 0;
	if (argc != 3 || !argv[2][0]) {
		fprintf(2, "usage: hex_dump file count\n");
		exit(1);
	}
	for (char *p = argv[2]; *p; p++) {
		int digit = *p - '0';
		if (digit < 0 || digit > 9 || left > (0x7fffffff - digit) / 10) {
			fprintf(2, "hex_dump: invalid count\n");
			exit(1);
		}
		left = left * 10 + digit;
	}
	int fd = open(argv[1], O_RDONLY);
	if (fd < 0) {
		fprintf(2, "hex_dump: cannot open %s\n", argv[1]);
		exit(1);
	}
	int status = 0;
	while (left > 0) {
		int want = left < (int)sizeof(buf) ? left : (int)sizeof(buf);
		int n = read(fd, buf, want);
		if (n < 0) {
			fprintf(2, "hex_dump: read error\n");
			status = 1;
			break;
		}
		if (n == 0) {
			break;
		}
		left -= n;
		for (int i = 0; i < n; i++) {
			line[used++] = hex[buf[i] >> 4];
			line[used++] = hex[buf[i] & 15];
			line[used++] = ' ';
			if (used == sizeof(line)) {
				line[used - 1] = '\n';
				if (write(1, line, used) != used) {
					exit(1);
				}
				used = 0;
			}
		}
	}
	if (used) {
		line[used - 1] = '\n';
		if (write(1, line, used) != used) {
			exit(1);
		}
	}
	close(fd);
	exit(status);
}
