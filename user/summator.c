#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main()
{
    const char* parse_fail = "Failed to parse!\n";
    const char* atoi_fail = "Failed to convert to int!\n";

    const int limit = 100;
    char str[limit];

    if (!gets(str, limit)) {
        write(1, parse_fail, strlen(parse_fail));
        exit(1);
    }

    char* second = strchr(str, ' ');
    if (!second) {
        write(1, parse_fail, strlen(parse_fail));
        exit(1);
    }

    *second = '\n';
    int a = atoi(str);
    if (a == 0) {
        write(1, atoi_fail, strlen(atoi_fail));
        exit(2);
    }

    int b = atoi(second + 1);
    if (b == 0) {
        write(1, atoi_fail, strlen(atoi_fail));
        exit(2);
    }

    int res = a + b;
    printf("sum is %d\n", res);

    exit(0);
}