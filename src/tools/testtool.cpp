/**
 *  \brief A test tool
 *  
 *  It provides a simple method to test separately the file system internal operations.
 *
 *  \author Artur Pereira - 2005-2009, 2016
 *  \author Miguel Oliveira e Silva - 2009
 *  \author António Rui Borges - 2010-2015
 */

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <libgen.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <stdarg.h>

#include "probing.h"
#include "exception.h"
#include "rawdisk.h"
#include "core.h"
#include "superblock.h"
#include "inode.h"
#include "cluster.h"
#include "freelists.h"
#include "filecluster.h"
#include "direntries.h"
#include "blockviews.h"
#include "dealers.h"

/* quiet level 
 * 0 - print all messages
 * 1 - print only result messages
 * 2 - print nothing 
 */
static int quiet = 0;

/* Log stream id */
static FILE *flog = stdout;     /* log stream default */
static FILE *fin = stdin;       /* input stream */
static const char *devname = NULL;

static char *progName = NULL;   /* this program's basename */
static char *progDir = NULL;    /* this program's directory */

/* ******************************************** */
/* print help message */
static void printUsage(char *cmd_name)
{
    if (quiet > 0)
        return;

    printf("Sinopsis: %s [OPTIONS] supp-file\n"
           "  OPTIONS:\n"
           "  -q level --- set quiet mode (default: 0)\n"
           "  -l depth --- set log depth (default: 0,0)\n"
           "  -h       --- print this help\n", cmd_name);
}

/* ******************************************** */
/* print a prompt message */
static void promptMsg(const char *fmt, ...)
{
    if (quiet > 0)
        return;

    /* print the message */
    //fprintf(stdout, "\e[01;34m");
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stdout, fmt, ap);
    va_end(ap);
    //fprintf(stdout, "\e[0m");
    fflush(stdout);
}

/* ******************************************** */
/* print a result message */
static void resultMsg(const char *fmt, ...)
{
    if (quiet > 1)
        return;

    /* print the message */
    //fprintf(stdout, "\e[01;34m");
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stdout, fmt, ap);
    va_end(ap);
    //fprintf(stdout, "\e[0m");
    fflush(stdout);
}

/* ******************************************** */
/* print an ERROR message */
static void errorMsg(const char *fmt, ...)
{
    if (quiet > 1)
        return;

    /* print the message */
    fprintf(stderr, "\e[01;31m");
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    fprintf(stderr, "\e[0m\n");
}

/* ******************************************** */
/* print an ERRNO message */
static void errnoMsg(int en, const char *fmt, ...)
{
    if (quiet > 1)
        return;

    /* print the message */
    fprintf(stderr, "\e[01;31m");
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    fprintf(stderr, ": %s (%d).\e[0m\n", strerror(en), en);
}

/* ******************************************** */
/* purge remainder of line */
static void fPurge(FILE * fin)
{
    //return;
    fscanf(fin, "%*[^\n]");
    fscanf(fin, "%*c");
}

/* ******************************************** */
/* not used */
static void notUsed(void)
{
    fprintf(stderr, "\e[02;41m==>\e[0m ");
    fprintf(stderr, "Option not used. Try another\n");
}

/* ******************************************** */
/* show block */
static void showBlock(void)
{
    /* ask for format */
    promptMsg("Display format (s, i, r, d, x, a): ");
    char t;
    fscanf(fin, "%c", &t);
    fPurge(fin);
    if (strchr("sirdxa", t) == NULL)
    {
        errorMsg("Wrong format\n");
        return;
    }

    /* ask for range */
    char range[100];
    promptMsg("block range: ");
    fscanf(fin, "%s", range);
    fPurge(fin);

    /* call showblock */
    char cmd[1000];
    sprintf(cmd, "%s/showblock -%c %s %s", progDir, t, range, devname);
    system(cmd);
}

/* ******************************************** */
/* alloc inode */
static uint32_t iType[] = { S_IFREG, S_IFDIR, S_IFLNK };

static void allocInode(void)
{
    /* ask for type */
    promptMsg("Inode type (1 - file, 2 - dir, 3 - symlink): ");
    int t;
    fscanf(fin, "%d", &t);
    fPurge(fin);
    if (t < 1 || t > 3)
    {
        errorMsg("Wrong type: %d", t);
        return;
    }

    /* call function */
    uint32_t in;
    soAllocInode(iType[t - 1], &in);

    /* print result */
    resultMsg("Inode number %u allocated\n", in);
}

/* ******************************************** */
/* free inode */
static void freeInode(void)
{
    /* ask for inode number */
    promptMsg("Inode number: ");
    uint32_t in;
    fscanf(fin, "%u", &in);
    fPurge(fin);

    /* call function */
    soFreeInode(in);

    /* print result */
    resultMsg("Inode number %u freed\n", in);
}

/* ******************************************** */
/* alloc data cluster */
static void allocCluster(void)
{
    /* call function */
    uint32_t cn;
    soAllocCluster(&cn);

    /* print result */
    resultMsg("Cluster number %u allocated\n", cn);
}

/* ******************************************** */
/* replenish */
static void replenish(void)
{
    /* call function */
    soReplenish();

    /* print result */
    resultMsg("replenish done\n");
}

/* ******************************************** */
/* free file cluster */
static void freeCluster(void)
{
    /* ask for cluster number */
    promptMsg("Cluster number: ");
    uint32_t cn;
    fscanf(fin, "%u", &cn);
    fPurge(fin);

    /* call function */
    soFreeCluster(cn);

    /* print result */
    resultMsg("Cluster number %u freed\n", cn);
}

/* ******************************************** */
/* deplete */
static void deplete(void)
{
    /* call function */
    soDeplete();

    /* print result */
    resultMsg("deplete done\n");
}

/* ******************************************** */
/* get file cluster */
static void getFileCluster(void)
{
    /* ask for inode number */
    promptMsg("Inode number: ");
    uint32_t in;
    fscanf(fin, "%u", &in);
    fPurge(fin);

    /* ask for file cluster index */
    promptMsg("File cluster index: ");
    uint32_t fcn;
    fscanf(fin, "%u", &fcn);
    fPurge(fin);

    /* call function */
    uint32_t cn;
    /* open inode */
    uint32_t ih = iOpen(in);

    /* get file cluster */
    soGetFileCluster(ih, fcn, &cn);

    /* close inode */
    iClose(ih);

    /* print result */
    if (cn != NULL_REFERENCE)
        resultMsg("Cluster number %u retrieved\n", cn);
    else
        resultMsg("Cluster number (nil) retrieved\n");
}

/* ******************************************** */
/* alloc file cluster */
static void allocFileCluster(void)
{
    /* ask for inode number */
    promptMsg("Inode number: ");
    uint32_t in;
    fscanf(fin, "%u", &in);
    fPurge(fin);

    /* ask for file cluster index */
    promptMsg("File cluster index: ");
    uint32_t fcn;
    fscanf(fin, "%u", &fcn);
    fPurge(fin);

    /* call function */
    uint32_t cn;
    /* open inode */
    uint32_t ih = iOpen(in);

    /* alloc file cluster */
    soAllocFileCluster(ih, fcn, &cn);

    /* save and close inode */
    iSave(ih);
    iClose(ih);

    /* print result */
    resultMsg("Cluster number %u allocated\n", cn);
}

/* ******************************************** */
/* free file clusters */
static void freeFileClusters(void)
{
    /* ask for inode number */
    promptMsg("Inode number: ");
    uint32_t in;
    fscanf(fin, "%u", &in);
    fPurge(fin);

    /* ask for file cluster index */
    promptMsg("First file cluster index: ");
    uint32_t ffcn;
    fscanf(fin, "%u", &ffcn);
    fPurge(fin);

    /* call function */
    /* open inode */
    uint32_t ih = iOpen(in);

    /* free clusters */
    soFreeFileClusters(ih, ffcn);

    /* save and close inode */
    iSave(ih);
    iClose(ih);

    resultMsg("Operation done.\n");
}

/* ******************************************** */
/* read file cluster */
static void readFileCluster(void)
{
    /* ask for inode number */
    promptMsg("Inode number: ");
    uint32_t in;
    fscanf(fin, "%u", &in);
    fPurge(fin);

    /* ask for file cluster index */
    promptMsg("File cluster index: ");
    uint32_t fcn;
    fscanf(fin, "%u", &fcn);
    fPurge(fin);

    /* open inode */
    uint32_t ih = iOpen(in);

    /* read cluster */
    uint32_t bpc = soGetBPC();
    char buf[bpc];
    memset(buf, 0, bpc);
    soReadFileCluster(ih, fcn, buf);

    /* close inode */
    iClose(ih);

    /* show results */
    uint32_t csize = bpc / BLOCK_SIZE;
    for (uint32_t i = 0; i < csize; i++)
        printBlockAsHex(buf + BLOCK_SIZE * i, BLOCK_SIZE * i);
}

/* ******************************************** */
/* write file cluster */
static void writeFileCluster(void)
{
    /* ask for inode number */
    promptMsg("Inode number: ");
    uint32_t in;
    fscanf(fin, "%u", &in);
    fPurge(fin);

    /* ask for file cluster index */
    promptMsg("File cluster index: ");
    uint32_t fcn;
    fscanf(fin, "%u", &fcn);
    fPurge(fin);

    /* ask for byte pattern */
    promptMsg("Byte pattern in hexadecimal: ");
    uint8_t bpx;
    fscanf(fin, "%hhx", &bpx);
    fPurge(fin);

    /* open inode */
    uint32_t ih = iOpen(in);

    /* read cluster */
    uint32_t bpc = soGetBPC();
    char buf[bpc];
    memset(buf, bpx, bpc);
    soWriteFileCluster(ih, fcn, buf);

    /* close inode */
    iSave(ih);
    iClose(ih);
}


/* ******************************************** */
/* write file cluster no pattern */
static void writeFileClusterNotPattern(void)
{
    /* ask for inode number */
    promptMsg("Inode number: ");
    uint32_t in;
    fscanf(fin, "%u", &in);
    fPurge(fin);
    /* ask for file cluster index */
    promptMsg("File cluster index: ");
    uint32_t fcn;
    fscanf(fin, "%u", &fcn);
    fPurge(fin);

    /* read cluster */
    uint32_t bpc = soGetBPC();
    char buf[bpc];
    memset(buf, '\0', bpc);
    /* ask for content*/
    promptMsg("Content: ");

    fscanf(fin, "%s", buf);
    fPurge(fin);

    /* open inode */
    uint32_t ih = iOpen(in);

    soWriteFileCluster(ih, fcn, buf);

    /* close inode */
    iSave(ih);
    iClose(ih);
}

/* ******************************************** */
/* get direntry */
static void getDirEntry()
{
    /* ask for inode number */
    promptMsg("Parent inode number: ");
    uint32_t pin;
    fscanf(fin, "%u", &pin);
    fPurge(fin);

    /* ask for direntry name */
    promptMsg("Direntry name: ");
    char name[100];
    fscanf(fin, "%[^\n]", name);
    fPurge(fin);

    /* open parent inode */
    int pih = iOpen(pin);

    /* call function */
    uint32_t cin;
    soGetDirEntry(pih, name, &cin);

    /* close inode */
    iClose(pih);

    /* print result */
    resultMsg("Child inode number = %u.\n", cin);
}

/* ******************************************** */
/* add direntry */
static void addDirEntry()
{
    /* ask for parent inode number */
    promptMsg("Parent inode number: ");
    uint32_t pin;
    fscanf(fin, "%u", &pin);
    fPurge(fin);

    /* ask for direntry name */
    promptMsg("Direntry name: ");
    char name[100];
    fscanf(fin, "%[^\n]", name);
    fPurge(fin);

    /* ask for child inode number */
    promptMsg("Child inode number: ");
    uint32_t cin;
    fscanf(fin, "%u", &cin);
    fPurge(fin);

    /* open parent and child inodes */
    int pih = iOpen(pin);

    /* call function */
    soAddDirEntry(pih, name, cin);

    /* close inodes */
    iSave(pih);
    iClose(pih);

    /* print result */
    resultMsg("Direntry added.\n");
}

/* ******************************************** */
/* rename direntry */
static void renameDirEntry()
{
    /* ask for parent inode number */
    promptMsg("Parent inode number: ");
    uint32_t pin;
    fscanf(fin, "%u", &pin);
    fPurge(fin);

    /* ask for direntry name */
    promptMsg("Direntry name: ");
    char name[100];
    fscanf(fin, "%[^\n]", name);
    fPurge(fin);

    /* ask for new direntry name */
    promptMsg("New direntry name: ");
    char newname[100];
    fscanf(fin, "%[^\n]", newname);
    fPurge(fin);

    /* open parent inode */
    int pih = iOpen(pin);

    /* call function */
    soRenameDirEntry(pih, name, newname);

    /* close inode */
    iSave(pih);
    iClose(pih);

    /* print result */
    resultMsg("Direntry renamed.\n");
}

/* ******************************************** */
/* delete direntry */
static void deleteDirEntry()
{
    /* ask for inode number */
    promptMsg("Parent inode number: ");
    uint32_t pin;
    fscanf(fin, "%u", &pin);
    fPurge(fin);

    /* ask for direntry name */
    promptMsg("Direntry name: ");
    char name[100];
    fscanf(fin, "%[^\n]", name);
    fPurge(fin);

    /* open parent inode */
    int pih = iOpen(pin);

    /* call function */
    uint32_t cin;
    soDeleteDirEntry(pih, name, &cin);

    /* close inode */
    iSave(pih);
    iClose(pih);

    /* print result */
    resultMsg("Child inode number = %u.\n", cin);
}

/* ******************************************** */
/* traverse path */
void traversePath()
{
    /* ask for PATH */
    promptMsg("path: ");
    char path[500];
    fscanf(fin, "%[^\n]", path);
    fPurge(fin);

    /* call function */
    uint32_t in;
    soTraversePath(path, &in);

    /* print result */
    resultMsg("inode number = %u\n", in);
}

/* ******************************************** */
/* get inode permissions */
void setInodeAccess()
{
    /* ask for inode number */
    promptMsg("inode number: ");
    uint32_t in;
    fscanf(fin, "%u", &in);
    fPurge(fin);

    /* ask for permissions */
    promptMsg("permissions in octal: ");
    uint32_t perm;
    fscanf(fin, "%o", &perm);
    fPurge(fin);

    /* call function */
    int ih = iOpen(in);
    iSetAccess(ih, perm);

    /* print result */
    SOInode *ip = iGetPointer(ih);
    printInode(ip, in);

    /* save and close inode */
    iSave(ih);
    iClose(ih);
}

/* ******************************************** */
/* check inode permissions */
void checkInodeAccess()
{
    /* ask for inode number */
    promptMsg("inode number: ");
    uint32_t in;
    fscanf(fin, "%u", &in);
    fPurge(fin);

    /* ask for access mode */
    promptMsg("requested access in octal: ");
    uint16_t access;
    fscanf(fin, "%ho", &access);
    fPurge(fin);

    /* call function */
    int ih = iOpen(in);
    bool granted = iCheckAccess(ih, access);
    iClose(ih);

    /* print result */
    resultMsg("access %0s\n", granted ? "granted" : "denied");
}

/* ******************************************** */
/* dec inode refcount */
void decInodeRefcount()
{
    /* ask for inode number */
    promptMsg("inode number: ");
    uint32_t in;
    fscanf(fin, "%u", &in);
    fPurge(fin);

    /* call function */
    int ih = iOpen(in);
    uint16_t rc = iDecRefcount(ih);
    iSave(ih);
    iClose(ih);

    /* print result */
    resultMsg("refcount = %hu\n", rc);
}

/* ******************************************** */
/* inc inode refcount */
void incInodeRefcount()
{
    /* ask for inode number */
    promptMsg("inode number: ");
    uint32_t in;
    fscanf(fin, "%u", &in);
    fPurge(fin);

    /* call function */
    int ih = iOpen(in);
    uint16_t rc = iIncRefcount(ih);
    iSave(ih);
    iClose(ih);

    /* print result */
    resultMsg("refcount = %hu\n", rc);
}

/* ******************************************** */
/* Definition of the handler functions */
typedef void (*handler) (void);

static handler hdl[] = {
    /*  0 */ NULL,
    /*  1 */ showBlock,
    /*  2 */ allocInode,
    /*  3 */ freeInode,
    /*  4 */ allocCluster,
    /*  5 */ freeCluster,
    /*  6 */ replenish,
    /*  7 */ deplete,
    /*  8 */ getFileCluster,
    /*  9 */ allocFileCluster,
    /* 10 */ freeFileClusters,
    /* 11 */ writeFileClusterNotPattern,
    /* 12 */ readFileCluster,
    /* 13 */ writeFileCluster,
    /* 14 */ getDirEntry,
    /* 15 */ addDirEntry,
    /* 16 */ renameDirEntry,
    /* 17 */ deleteDirEntry,
    /* 18 */ traversePath,
    /* 19 */ notUsed,
    /* 20 */ checkInodeAccess,
    /* 21 */ setInodeAccess,
    /* 22 */ incInodeRefcount,
    /* 23 */ decInodeRefcount
};

#define HDL_LEN (sizeof (hdl) / sizeof (handler))

/* ******************************************** */
/* print menu */
static void printMenu(void)
{
    if (quiet > 0)
        return;

    printf
        ("+==============================================================+\n"
         "|                      IFuncs testing tool                     |\n"
         "+==============================================================+\n"
         "|  0 - exit                     |  1 - show block              |\n"
         "+-------------------------------+------------------------------+\n"
         "|  2 - alloc inode              |  3 - free inode              |\n"
         "|  4 - alloc cluster            |  5 - free cluster            |\n"
         "|  6 - replenish                |  7 - deplete                 |\n"
         "+-------------------------------+------------------------------+\n"
         "|  8 - get file cluster         |  9 - alloc file cluster      |\n"
         "| 10 - free file clusters       | 11 - Write file cluster      |\n"
         "| 12 - read file cluster        | 13 - write file c. pattern   |\n"
         "+-------------------------------+------------------------------+\n"
         "| 14 - get direntry             | 15 - add direntry            |\n"
         "| 16 - rename direntry          | 17 - delete direntry         |\n"
         "| 18 - traverse path            | 19 - NOT USED                |\n"
         "+-------------------------------+------------------------------+\n"
         "+ 20 - check inode access       | 21 - set inode access        +\n"
         "+ 22 - inc inode refcount       | 23 - dec inode refcount      +\n"
         "+==============================================================+\n");
}

/* ******************************************** */
/* The main function */
int main(int argc, char *argv[])
{
    progName = basename(argv[0]);   // must be called before dirname!
    progDir = dirname(argv[0]);
    /* process command line options */
    int opt;
    while ((opt = getopt(argc, argv, "l:q:h")) != -1)
    {
        switch (opt)
        {
            case 'l':          /* log depth */
            {
                int lower, higher;
                if (sscanf(optarg, "%d,%d", &lower, &higher) != 2)
                {
                    errorMsg("%s: Bad argument to l option.\n", progName);
                    printUsage(progName);
                    return EXIT_FAILURE;
                }
                soSetProbeDepths(lower, higher);
                break;
            }
            case 'q':          /* quiet mode */
            {
                quiet = atoi(optarg);
                if (quiet < 0) quiet = 0;
                else if (quiet > 2) quiet = 2;
                break;
            }
            case 'h':          /* help mode */
            {
                printUsage(progName);
                return EXIT_SUCCESS;
            }
            default:
            {
                errorMsg("%s: Wrong option.\n", progName);
                printUsage(progName);
                return EXIT_FAILURE;
            }
        }
    }

    /* check existence of mandatory argument: storage device name */
    if ((argc - optind) != 1)
    {
        fprintf(stderr, "%s: Wrong number of mandatory arguments.\n", progName);
        printUsage(progName);
        return EXIT_FAILURE;
    }
    devname = argv[optind];

    /* set log stream */
    if (flog == NULL)
        flog = stdout;          /* if the switch -L was not used, set output to stdout */
    else
        stderr = flog;          /* if the switch -L was used, set stderr to log file */

    /* open disk */
    try
    {
        soOpenDealersDisk(devname);
    }
    catch(SOException & err)
    {
        errnoMsg(err.en, err.msg);
        return EXIT_FAILURE;
    }

    /* process the command */
    while (true)
    {
        printMenu();
        promptMsg("\nYour command: ");
        unsigned int cmdNumb;
        scanf("%u", &cmdNumb);
        fPurge(stdin);

        if (cmdNumb == 0)
        {
            break;
        } else if (cmdNumb < HDL_LEN)
        {
            try
            {
                hdl[cmdNumb] ();
            }
            catch(SOException & err)
            {
                errnoMsg(err.en, err.msg);
            }
        } else
        {
            notUsed();
        }
    }

    /* close the unbuffered communication channel with the storage device */
    try
    {
        soCloseDealersDisk();
    }
    catch(SOException & err)
    {
        errnoMsg(err.en, err.msg);
    }

    /* that's all */
    promptMsg("Bye!\n");
    return EXIT_SUCCESS;
}                               /* end of main */
