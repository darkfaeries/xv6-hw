#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main()
{
    char* prompt = "Enter your name:\n";
    char* hi = "Hello, ";
    char* oops = "Failed!";

    write(1, prompt, strlen(prompt));

    char name[100];
    if (!gets(name, 100)) {
        write(1, oops, strlen(oops));
        exit(1);
    }

    write(1, hi, strlen(hi));
    write(1, name, strlen(name) - 1);
    write(1, "!\n", 2);

    exit(0);
}