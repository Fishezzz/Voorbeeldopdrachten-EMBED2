#include "tlpi_hdr.h"
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{
    int fd;

    if (argc != 3 || strcmp(argv[1], "--help") == 0)
        usageErr("%s file text\n", argv[0]);

    int len = strlen(argv[2]);
    fd = open(argv[1], O_APPEND | O_RDWR, 0644);
    if (fd == -1)
        errExit("open");

    lseek(fd, 0L, SEEK_SET);
    write(fd, argv[2], len);

    int flen = lseek(fd, 0L, SEEK_END);
    char buffer[flen];
    pread(fd, buffer, flen, 0L);
    printf("%s\n", buffer);

    exit(0);
}

/*
    De bytes kwamen op het einde van het bestand terecht.
    Door de O_APPEND flag zal de file offset naar het einde van het bestand gezet worden voor iedere write().
*/
