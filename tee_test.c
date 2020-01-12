#include "tlpi_hdr.h"
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{
    int fi, fo;
    ssize_t len;

    if (argc == 1)
        usageErr("%s [-a] input_file output_file\n", argv[0]);
    else if (strcmp(argv[1], "--help") == 0)
        usageErr("%s [-a] input_file output_file\n", argv[0]);
    else if (argc != 3 && argc != 4)
        usageErr("%s [-a] input_file output_file\n", argv[0]);

    if (argc == 3)
    {
        fi = open(argv[1], O_RDONLY);
        if (fi == -1)
            errExit("open");
        
        fo = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fo == -1)
            errExit("open");
    }
    else if (argc == 4) // append -a
    {
        if (strcmp(argv[1], "-a") != 0)
            usageErr("%s [-a] input_file output_file\n", argv[0]);

        fi = open(argv[2], O_RDONLY);
        if (fi == -1)
            errExit("open");
        
        fo = open(argv[3], O_WRONLY | O_CREAT | O_APPEND, 0644);    
        if (fo == -1)
            errExit("open");
    }

    len = lseek(fi, 0L, SEEK_END);

    char buffer[len];
    pread(fi, buffer, len, 0L);

    write(fo, buffer, len);
    printf("%s\n", buffer);

    exit(0);
}
