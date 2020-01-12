#include "tlpi_hdr.h"
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>

extern char **environ;

int main(int argc, char *argv[])
{
    if (argc == 1)
    {
        printf("1\n");
        usageErr("%s name=value\nor %s name value\n", argv[0], argv[0]);
    }
    else if (strcmp(argv[1], "--help") == 0)
    {
        printf("2\n");
        usageErr("%s name=value\nor %s name value\n", argv[0], argv[0]);
    }
    else if (argc != 2 && argc != 3)
    {
        printf("3\n");
        printf("%i\n", (argc != 1 && argc != 2));
        printf("argc: %i\n", argc);
        usageErr("%s name=value\nor %s name value\n", argv[0], argv[0]);
    }

    if (argc == 2)
    {   // name=value
        if (putenv(argv[1]) != 0)
            errExit("putenv");
    }
    else if (argc == 3)
    {   // name value       
        if (setenv(argv[1], argv[2], 1) != 0)
            errExit("setenv");
    }

    char **ep;
    for (ep = environ; *ep != NULL; ep++)
        printf("%s\n", *ep);

    exit(0);
}
