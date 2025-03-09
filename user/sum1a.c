#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "stdbool.h"

#define BUF_SIZE 64

int
main (int argc, char* argv[])
{
  const char* read_fail = "failed to read\n";
  const char* buffer_fail = "failed to parse: buffer overflow\n";
  const char* parse_fail = "failed to parse\n";
  const char* atoi_fail = "failed to convert\n";
  
  char buffer[BUF_SIZE];

  int pos = 0;
  int read_ret = 1;


  while (true) {
    read_ret = read(0, buffer + pos, 1);

    if (read_ret < 0) {
      write(1, read_fail, strlen(read_fail));
      exit(1);
    }

    if (*(buffer + pos) == '\n') break;  // end of line
    pos++;

    if (pos >= BUF_SIZE) {
      write(1, buffer_fail, strlen(buffer_fail));
      exit(2);
    }
  }


  *(buffer + pos) = '\n'; // is it really needed?

  char* second = strchr(buffer, ' ');
  if (!second) {
    write(1, parse_fail, strlen(parse_fail));
    exit(3);
  }

  *second = '\n';
  int a = atoi(buffer);
  if (a == 0) {
    write(1, atoi_fail, strlen(atoi_fail));
    exit(4);
  }

  int b = atoi(second + 1);
  if (b == 0) {
    write(1, atoi_fail, strlen(atoi_fail));
    exit(4);
  }

  int res = a + b;
  printf("sum is %d\n", res);

  exit(0);
}