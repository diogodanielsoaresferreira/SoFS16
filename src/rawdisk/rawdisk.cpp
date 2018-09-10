 /*
  *  \author Artur Carneiro Pereira - 2007-2009, 216
  *  \author Miguel Oliveira e Silva - 2009
  *  \author António Rui Borges - 2010-2015
  */

#include "rawdisk.h"

#include "probing.h"
#include "exception.h"

#include <sys/stat.h>
#include <unistd.h>
#include <inttypes.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>

/* ********************************************* */

/* Internal data structure */

/* File descriptor of the Linux file that simulates the disk */
static int fd = -1;

/* Total number of blocks of the storage device */
static uint32_t ntotal = 0;

/* ********************************************* */

void soOpenRawDisk(const char *devname, uint32_t * np)
{
    soProbe(901, "soOpenRawDisk(\"%s\", %p)\n", devname, np);

    /* check devname */
    if ((devname == NULL))
        throw SOException(EINVAL, __FUNCTION__);

    /* fail if device already open */
    if (fd != -1)
        throw SOException(EBUSY, __FUNCTION__);

    /* opening supporting file in async mode for read and write */
    if ((fd = open(devname, O_RDWR)) == -1)
        throw SOException(errno, __FUNCTION__); /* checking for opening error */

    /* checking device for conformity */
    struct stat st;
    if (stat(devname, &st) == -1)
        throw SOException(errno, __FUNCTION__);
    if ((st.st_size % BLOCK_SIZE) != 0)
        throw SOException(EMEDIUMTYPE, __FUNCTION__);

    /* get number of blocks of the device */
    ntotal = st.st_size / BLOCK_SIZE;

    /* return number of blocks, if requested */
    if (np != NULL)
        *np = ntotal;
}

/* ********************************************* */

void soCloseRawDisk(void)
{
    soProbe(991, "soCloseRawDisk()\n");

    /* checking for device close state */
    if (fd == -1)
        throw SOException(EBADF, __FUNCTION__);

    /* close the device */
    close(fd);
    ntotal = 0;
    fd = -1;
}

/* ********************************************* */

void soReadRawBlock(uint32_t n, void *buf)
{
    soProbe(951, "soReadRawBlock(%u, %p)\n", n, buf);

    /* checking arguments */
    if (buf == NULL)
        throw SOException(EINVAL, __FUNCTION__);

    if (n >= ntotal)
        throw SOException(EINVAL, __FUNCTION__);

    if (fd == -1)
        throw SOException(EBADF, __FUNCTION__);

    /* transfer block data */
    if (lseek(fd, BLOCK_SIZE * n, SEEK_SET) == -1)
        throw SOException(errno, __FUNCTION__);

    if (read(fd, buf, BLOCK_SIZE) != BLOCK_SIZE)
        throw SOException(EIO, __FUNCTION__);
}

/* ********************************************* */

void soWriteRawBlock(uint32_t n, void *buf)
{
    soProbe(961, "soWriteRawBlock(%u, %p)\n", n, buf);

    /* checking arguments */
    if (buf == NULL)
        throw SOException(EINVAL, __FUNCTION__);

    if (n >= ntotal)
        throw SOException(EINVAL, __FUNCTION__);

    if (fd == -1)
        throw SOException(EBADF, __FUNCTION__);

    /* transfer block data */
    if (lseek(fd, BLOCK_SIZE * n, SEEK_SET) == -1)
        throw SOException(errno, __FUNCTION__);
    if (write(fd, buf, BLOCK_SIZE) != BLOCK_SIZE)
        throw SOException(EIO, __FUNCTION__);
}

/* ********************************************* */

void soReadRawCluster(uint32_t n, void *buf, uint32_t csize)
{
    soProbe(855, "soReadRawCluster(%u, %p, %u)\n", n, buf, csize);

    /* checking arguments */
    if (buf == NULL)
        throw SOException(EINVAL, __FUNCTION__);

    if (n >= ntotal)
        throw SOException(EINVAL, __FUNCTION__);

    if (fd == -1)
        throw SOException(EBADF, __FUNCTION__);

    /* transfer cluster data */
    if (lseek(fd, BLOCK_SIZE * n, SEEK_SET) == -1)
        throw SOException(errno, __FUNCTION__);
    if (read(fd, buf, csize * BLOCK_SIZE) != (ssize_t)(csize * BLOCK_SIZE))
        throw SOException(EIO, __FUNCTION__);
}

/* ********************************************* */

void soWriteRawCluster(uint32_t n, void *buf, uint32_t csize)
{
    soProbe(856, "soWriteRawCluster(%u, %p, %u)\n", n, buf, csize);

    /* checking arguments */
    if (buf == NULL)
        throw SOException(EINVAL, __FUNCTION__);

    if (n >= ntotal)
        throw SOException(EINVAL, __FUNCTION__);

    if (fd == -1)
        throw SOException(EBADF, __FUNCTION__);

    /* transfer cluster data */
    if (lseek(fd, BLOCK_SIZE * n, SEEK_SET) == -1)
        throw SOException(errno, __FUNCTION__);
    if (write(fd, buf, csize * BLOCK_SIZE) != (ssize_t)(csize * BLOCK_SIZE))
        throw SOException(EIO, __FUNCTION__);
}

/* ********************************************* */
