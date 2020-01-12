#ifndef SEEK_DATA
// #warning "SEEK_DATA is undeclared and manually defined."
#define SEEK_DATA	3	/* seek to the next data */
#endif
#ifndef SEEK_HOLE
// #warning "SEEK_HOLE is undeclared and manually defined."
#define SEEK_HOLE	4	/* seek to the next hole */
#endif
#include "tlpi_hdr.h"
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    int srcfd, tgtfd;
    int ret = 0, nextoffset = 0, offset = 0;

    if (argc == 1)
        usageErr("%s file destination\n", argv[0]);
    else if (strcmp(argv[1], "--help") == 0)
        usageErr("%s file destination\n", argv[0]);
    else if (argc != 2 && argc != 3)
        usageErr("%s file destination\n", argv[0]);

    /* Create file with holes */
    // int len = 0;
    // srcfd = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    // len += 1000; ftruncate(srcfd, 1000);
    // len += 70; write(srcfd, "aabbccddeeffgghhiijjkkllmmnnooppqqrrssttvvwwxxyyzz00112233445566778899", 70);
    // len += 70; write(srcfd, "aabbccddeeffgghhiijjkkllmmnnooppqqrrssttvvwwxxyyzz00112233445566778899", 70);
    // len += 3000; ftruncate(srcfd, 4140);
    // len += 70; write(srcfd, "aabbccddeeffgghhiijjkkllmmnnooppqqrrssttvvwwxxyyzz00112233445566778899", 70);
    // len += 70; write(srcfd, "aabbccddeeffgghhiijjkkllmmnnooppqqrrssttvvwwxxyyzz00112233445566778899", 70);
    // exit(0);

    // Open source file
printf("Open source file\n");
    srcfd = open(argv[1], O_RDONLY);
    if (srcfd == -1)
        errExit("Open source file");


    // Open or create file to copy to
printf("Open target file\n");
    tgtfd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (tgtfd == -1)
        errExit("Open target file");

    // // Find file length
    // len = lseek(scrfd, 0L, SEEK_END);

    // Find current (starting) position
printf("Find current position of file\n");
    offset = lseek(srcfd, 0L, SEEK_CUR);
printf("offset: %i\tnextoffset: %i\n", offset, nextoffset);
    if (offset == (off_t) -1)
        errExit("Find current position of file");

    // Find data beginning at start of file
printf("Find data or hole at beginning of file\n");
    nextoffset = lseek(srcfd, offset, SEEK_DATA);
printf("offset: %i\tnextoffset: %i\n", offset, nextoffset);
    if (nextoffset == (off_t) -1)
    {
printf("before goto end_hole;\n");
        if (errno == ENXIO)
            goto end_hole;
        
        errExit("Find data or hole at beginning of file");
    }

    // If start of data != start of file => file starts with hole
printf("before goto hole;\n");
printf("offset: %i\tnextoffset: %i\n", offset, nextoffset);
    if (offset != nextoffset)
        goto hole;

    // Loop for copy-hole copy-data
printf("before loop\n");
    for (;;)
    {
        char *buffer = "";

        // Find start of hole
printf("Find end of data\n");
        nextoffset = lseek(srcfd, offset, SEEK_HOLE);
printf("offset: %i\tnextoffset: %i\n", offset, nextoffset);
        if (nextoffset == (off_t) -1)
        {
            if (errno != ENXIO)
                errExit("Find end of data");

printf("before goto end_data;\n");
            goto end_data;
        }

        // Rewind file offset to start of data
printf("Rewinding back to data\n");
        offset = lseek(srcfd, offset, SEEK_SET);
printf("offset: %i\tnextoffset: %i\n", offset, nextoffset);
        if (offset == (off_t) -1)
            errExit("Rewinding back to data");

        // Copy data
printf("Read data\n");
        ret = read(srcfd, buffer, nextoffset - offset);
        if (ret < 0)
            errExit("Read data");
printf("Write data\n");
        ret = write(tgtfd, buffer, nextoffset - offset);
        if (ret < 0)
            errExit("Write data");

        // Set offset to start of hole
printf("Set offset to start of hole\n");
        offset = nextoffset;
printf("offset: %i\tnextoffset: %i\n", offset, nextoffset);

        // Find end of hole
printf("Find end of hole\n");
        nextoffset = lseek(srcfd, offset, SEEK_DATA);
printf("offset: %i\tnextoffset: %i\n", offset, nextoffset);
        if (nextoffset == (off_t) -1)
        {
printf("before goto end_hole;\n");
            if (errno == ENXIO)
                goto end_hole;
            
            errExit("Find end of hole");
        }

// Found hole in file
hole:
printf("label hole:\n");
        // Truncate file with file_len + hole_len to add hole at end of file
printf("Truncate file to add hole\n");
        ret = ftruncate(tgtfd, nextoffset);
        if (ret < 0)
            errExit("Truncate file to add hole");

        // Move file offset to end of hole
printf("Move to after newly added hole\n");
        nextoffset = lseek(tgtfd, nextoffset, SEEK_SET);
printf("offset: %i\tnextoffset: %i\n", offset, nextoffset);
        if (nextoffset == (off_t) -1)
            errExit("Move to after newly added hole");

        // Set offset to end of hole
printf("Set offset to end of hole\n");
        offset = nextoffset;
printf("offset: %i\tnextoffset: %i\n", offset, nextoffset);
    }

// File ends with hole
end_hole:
printf("label end_hole:\n");
    // Find end of file
printf("Seek to end of file\n");
    nextoffset = lseek(srcfd, 0L, SEEK_END);
printf("offset: %i\tnextoffset: %i\n", offset, nextoffset);
    if (nextoffset == (off_t) -1)
        errExit("Seek to end of file");

    // If file offset is not at end of file => at hole from file offset to end of file
printf("Truncate to add hole at end of file\n");
printf("offset: %i\tnextoffset: %i\n", offset, nextoffset);
    if (offset != nextoffset)
    {
        ret = ftruncate(tgtfd, nextoffset);
        if (ret < 0)
            errExit("Truncate to add hole at end of file");
    }

// File ends with data
end_data:
printf("label end_data:\n");
printf("offset: %i\tnextoffset: %i\n", offset, nextoffset);
    // Copy data
printf("Read EOF data\n");
    char data[nextoffset - offset];
    ret = read(srcfd, data, nextoffset - offset);
    if (ret < 0)
        errExit("Read EOF data");
printf("Write EOF data\n");
    ret = write(tgtfd, data, nextoffset - offset);
    if (ret < 0)
        errExit("Write EOF data");

   exit(0);
}
