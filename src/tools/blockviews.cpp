/*
 *  \author Artur Pereira - 2007-2009, 2016
 *  \author Miguel Oliveira e Silva - 2009
 *  \author António Rui Borges - 2010-2012
 */

#define  __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <sys/stat.h>
#include <string.h>

#include "rawdisk.h"
#include "core.h"
#include "superblock.h"
#include "inode.h"
#include "cluster.h"
#include "direntry.h"

/* ********************************************************* */

void printBlockAsHex(void *buf, uint32_t off)
{
    /* cast buf to appropriated type */
    unsigned char *byte = (unsigned char *)buf;

    /* print cluster */
    for (unsigned int i = 0; i < BLOCK_SIZE; i++)
    {
        if ((i & 0x1f) == 0)
            printf("%4.4x:", i + off);
        /* print byte */
        printf(" %.2x", byte[i]);
        /* terminate present line, if required */
        if ((i & 0x1f) == 0x1f)
            printf("\n");
    }
}

/* ********************************************************* */

void printBlockAsAscii(void *buf, uint32_t off)
{
    /* cast buf to appropriated type */
    unsigned char *c = (unsigned char *)buf;

    /* print cluster */
    char line[256];             /* line to be printed */
    char *p_line = line;        /* pointer to a character in the line */
    for (unsigned int i = 0; i < BLOCK_SIZE; i++)
    {
        if ((i & 0x1f) == 0)
        {
            printf("%4.4d:", i + off);
            p_line = line;
        }
        /* add character to the line */
        switch (c[i])
        {
            case '\a':
                p_line += sprintf(p_line, " \\a");
                break;
            case '\b':
                p_line += sprintf(p_line, " \\b");
                break;
            case '\f':
                p_line += sprintf(p_line, " \\f");
                break;
            case '\n':
                p_line += sprintf(p_line, " \\n");
                break;
            case '\r':
                p_line += sprintf(p_line, " \\r");
                break;
            case '\t':
                p_line += sprintf(p_line, " \\t");
                break;
            case '\v':
                p_line += sprintf(p_line, " \\v");
                break;
            default:
                if ((c[i] >= ' ') && (c[i] != 0x7F) && (c[i] != 0x8F))
                    p_line += sprintf(p_line, " %c ", c[i]);
                else
                    p_line += sprintf(p_line, " %.2x", c[i]);
        }
        /* terminate and print present line, if required */
        if ((i & 0x1f) == 0x1f)
        {
            *p_line = '\0';
            printf("%s\n", line);
        }
    }
}

/* ********************************************************* */

void printSuperBlock(void *buf)
{
    /* cast buf to appropriated type */
    SOSuperBlock *sbp = (SOSuperBlock *) buf;

    /* header */
    printf("Header:\n");
    printf("   Magic number: 0x%0" PRIX16 "\n", sbp->magic);
    printf("   Version number: 0x%0" PRIX16 "\n", sbp->version);
    printf("   Volume name: %-s\n", sbp->name);
    printf("   Total number of blocks in the device: %u\n", sbp->ntotal);
    printf("   Properly unmounted: %s\n", (sbp->mstat == PRU) ? "yes" : "no");

    /* inode table metadata */
    printf("Inode table metadata:\n");
    printf("   Total number of inodes: %u\n", sbp->itotal);
    printf("   First block of the inode table: %u\n", sbp->itstart);
    printf("   Number of blocks of the inode table: %u\n", sbp->itsize);
    printf("   Number of free inodes: %u\n", sbp->ifree);
    printf("   Head of list of free inodes: ");
    if (sbp->ihead == NULL_REFERENCE)
        printf("(nil)\n");
    else
        printf("%" PRIu32 "\n", sbp->ihead);
    printf("   Tail of list of free inodes: ");
    if (sbp->itail == NULL_REFERENCE)
        printf("(nil)\n");
    else
        printf("%" PRIu32 "\n", sbp->itail);

    /* cluster zone */
    printf("Data zone:\n");
    printf("   Number of blocks per cluster: %u\n", sbp->csize);
    printf("   First block of the cluster zone: %u\n", sbp->czstart);
    printf("   Total number of clusters: %u\n", sbp->ctotal);
    printf("   Number of free clusters: %u\n", sbp->cfree);
    printf("   Number of clusters used by list of free clusters: %u\n", sbp->crefs);
    printf("   FCT head cache of references to free data clusters:\n");
    printf("      Index of the first filled cache element: %u\n", sbp->chead.cache.out);
    printf("      Index of the first free cache element: %u\n", sbp->chead.cache.in);
    printf("      Cache contents:\n");
    printf("        ");
    for (uint32_t i = 0; i < FCT_CACHE_SIZE; i++)
    {
        if (sbp->chead.cache.ref[i] == NULL_REFERENCE)
            printf(" (nil)");
        else
            printf(" %u", sbp->chead.cache.ref[i]);

        /* change line at every 10 elements */
        if (i % 10 == 9)
        {
            printf("\n");
            printf("        ");
        }
    }
    printf("\n");
    printf("   FCT tail cache of references to free data clusters:\n");
    printf("      Index of the first free cache element: %u\n", sbp->ctail.cache.in);
    printf("      Index of the first filled cache element: %u\n", sbp->ctail.cache.out);
    printf("      Cache contents:\n");
    printf("        ");
    for (uint32_t i = 0; i < FCT_CACHE_SIZE; i++)
    {
        if (sbp->ctail.cache.ref[i] == NULL_REFERENCE)
            printf(" (nil)");
        else
            printf(" %u", sbp->ctail.cache.ref[i]);
        /* change line at every 10 elements */
        if ((i % 10) == 9)
        {
            printf("\n");
            printf("        ");
        }
    }
    printf("\n");

    printf("   FCT head cluster of references to free clusters:\n");
    printf("     Cluster number: ");
    if (sbp->chead.cluster_number == NULL_REFERENCE)
        printf("(nil)\n");
    else
        printf("%u\n", sbp->chead.cluster_number);
    printf("     Index of first filled entry: %u\n", sbp->chead.cluster_idx);

    printf("   FCT tail cluster of references to free clusters:\n");
    printf("     Cluster number: ");
    if (sbp->ctail.cluster_number == NULL_REFERENCE)
        printf("(nil)\n");
    else
        printf("%u\n", sbp->ctail.cluster_number);
    printf("     Index of first empty entry: %u\n", sbp->ctail.cluster_idx);
}

/* ********************************************************* */

/** \brief Bit pattern description of the mode field in the inode data type */
static const char *inodetypes[] = {
    "clean",
    "INVALID_001",
    "INVALID-002",
    "INVALID_003",
    "directory",
    "INVALID-005",
    "INVALID-006",
    "INVALID_007",
    "regular file",
    "INVALID_011",
    "symlink",
    "INVALID_013",
    "INVALID-014",
    "INVALID-015",
    "INVALID-016" "INVALID-017"
};

void printInode(void *buf, uint32_t in)
{
    SOInode *ip = (SOInode *) buf;

    /* print inode number */
    printf("Inode #");
    if (in == NULL_REFERENCE)
        printf("(nil)\n");
    else
        printf("%u\n", in);

    /* decouple and print mode field */
    bool not_in_use = (ip->mode & INODE_FREE) == INODE_FREE;
    uint32_t typebits = (ip->mode & S_IFMT) >> 12;
    printf("type = %s%s, ", not_in_use ? "free " : " ", inodetypes[typebits]);

    uint32_t permbits = ip->mode & (S_IRWXU | S_IRWXG | S_IRWXO);
    char perm[10] = "rwxrwxrwx";    /* access permissions array */
    for (int i = 0; i < 9; i++)
    {
        if ((permbits % 2) == 0)    // LSB is zero ?
        {
            perm[8 - i] = '-';
        }
        permbits >>= 1;
    }
    printf("permissions = %s, ", perm);

    /* print reference count */
    printf("refcount = %" PRIu16 ", ", ip->refcount);

    /* print owner and group IDs */
    printf("owner = %" PRIu32 ", group = %" PRIu32 "\n", ip->owner, ip->group);

    /* print file size in bytes and in clusters */
    printf("size in bytes = %" PRIu32 ", size in clusters = %" PRIu32 "\n", ip->size, ip->csize);

    /* decouple and print information about dates of file manipulation, 
     * if inode is in use, or about the inode references
     * within the double-linked list that holds the free inodes */
    char timebuf[30];           /* date and time string */
    if (not_in_use)
    {
        printf("next = ");
        if (ip->next == NULL_REFERENCE)
            printf("(nil)\n");
        else
            printf("%" PRIu32 "\n", ip->next);
    } else
    {                           /* inode is in use */
        time_t t = ip->atime;
        ctime_r(&t, timebuf);
        timebuf[strlen(timebuf) - 1] = '\0';
        printf("atime = %s, \n", timebuf);
    }
    time_t t = ip->mtime;
    ctime_r(&t, timebuf);
    timebuf[strlen(timebuf) - 1] = '\0';
    printf("mtime = %s, ", timebuf);
    t = ip->ctime;
    ctime_r(&t, timebuf);
    timebuf[strlen(timebuf) - 1] = '\0';
    printf("ctime = %s, \n", timebuf);

    /* print direct references */
    printf("d[] = {");
    for (int i = 0; i < N_DIRECT; i++)
    {
        if (i > 0)
            printf(" ");
        if (ip->d[i] == NULL_REFERENCE)
            printf("(nil)");
        else
            printf("%" PRIu32 "", ip->d[i]);
    }
    printf("}, ");

    /* single indirect references */
    printf("i1[] = {");
    for (int i = 0; i < N_INDIRECT; i++)
    {
        if (i > 0)
            printf(" ");
        if (ip->i1[i] == NULL_REFERENCE)
            printf("(nil)");
        else
            printf("%" PRIu32 "", ip->i1[i]);
    }
    printf("}, ");

    /* print double indirect reference */
    printf("i2 = ");
    if (ip->i2 == NULL_REFERENCE)
        printf("(nil)\n");
    else
        printf("%" PRIu32 "\n", ip->i2);

    printf("----------------\n");
}

/* ********************************************************* */

void printBlockOfInodes(void *buf, uint32_t off)
{
    /* cast buf to appropriated type */
    SOInode *inode = (SOInode *) buf;

    /* treat each inode stored in the block separately */
    for (uint32_t i = 0; i < IPB; i++)
        printInode(&inode[i], i + off);
}

/* ********************************************************* */

void printBlockOfDirents(void *buf, uint32_t off)
{
    /* get dirents per cluster */
    uint32_t dpb = BLOCK_SIZE / sizeof(SODirEntry);

    /* cast buf to appropriated type */
    SODirEntry *dir = (SODirEntry *) buf;

    /* print */
    for (uint32_t i = 0; i < dpb; i++)
    {
        printf("%-*.*s ", SOFS16_MAX_NAME, SOFS16_MAX_NAME, dir[i].name);
        if (dir[i].in == NULL_REFERENCE)
            printf("(nil)\n");
        else
            printf("%.10" PRIu32 "\n", dir[i].in);
    }
}

/* ********************************************************* */

void printBlockOfRefs(void *buf, uint32_t off)
{
    /* get refs per block */
    uint32_t rpb = BLOCK_SIZE / sizeof(uint32_t);

    /* cast buf to appropriated type */
    uint32_t *ref = (uint32_t *) buf;

    for (uint32_t i = 0; i < rpb; i++)
    {
        if ((i & 0x07) == 0)
            printf("%4.4d:", i + off);
        /* print reference to a cluster */
        if (ref[i] == NULL_REFERENCE)
            printf("   (nil)   ");
        else
            printf(" %.10" PRIu32, ref[i]);
        /* terminate present line, if required */
        if (((i & 0x07) == 0x07) || (i == (rpb - 1)))
            printf("\n");
    }
}

/* ********************************************************* */
