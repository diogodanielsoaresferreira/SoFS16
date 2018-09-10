/*
 * \author Pedro Marques Ferreira da Silva
 * \tester Pedro Marques Ferreira da Silva
 */

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stdbool.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/statvfs.h>
#include <sys/stat.h>
#include <time.h>
#include <utime.h>
#include <libgen.h>
#include <string.h>
#include <linux/limits.h>

#include "errno.h"
#include "syscalls.h"
#include "freelists.h"
#include "dealers.h"
#include "direntries.h"
#include "filecluster.h"
#include "probing.h"
#include "exception.h"

/*
 *  \brief Delete a link to a file from a directory and possibly the file it refers to from the file system.
 *
 *  It tries to emulate <em>unlink</em> system call.
 *
 *  \param path path to the file to be deleted
 *
 *  \return 0 on success; 
 *      -errno in case of error, being errno the system error that better represents the cause of failure
 */
int soUnlink(const char *path)
{
    soProbe(226, "soUnlink(\"%s\")\n", path);

    try
    {
        char *xpath = strdupa(path);
        char *bn = strdupa(basename(xpath));
        char *dn = dirname(xpath);

        uint32_t inp, inode_handler, cinp, aih, bpc = soGetBPC();
        SOInode* inode_1;

        char cluster[bpc];

        /* validation of the arguments */
        if(inp == NULL_REFERENCE){
            throw SOException(ENOENT, __FUNCTION__);
        }

        if(strlen(path) > PATH_MAX){
            throw SOException(ENAMETOOLONG, __FUNCTION__);

        }

        if(strlen(bn)>SOFS16_MAX_NAME)
            throw SOException(ENAMETOOLONG, __FUNCTION__);

        
        memset(cluster, '\0', bpc);
        memcpy(cluster, path, strlen(path));

        soTraversePath(dn, &inp);


        inode_handler = iOpen(inp);
        inode_1 = iGetPointer(inode_handler);

        /* check if inode is not a directory */
        if((inode_1->mode & S_IFDIR) != S_IFDIR){
            iSave(inode_handler);
            iClose(inode_handler);
            throw SOException(ENOTDIR, __FUNCTION__);
        }

        if(iCheckAccess(inode_handler, W_OK) == false){
            iSave(inode_handler);
            iClose(inode_handler);
            throw SOException(EACCES, __FUNCTION__);
        }

        soGetDirEntry(inode_handler, bn, &cinp);

        if(cinp == NULL_REFERENCE){
            iSave(inode_handler);
            iClose(inode_handler);
            throw SOException(ENOENT, __FUNCTION__);
        }

        aih = iOpen(cinp);

        uint32_t ref = iDecRefcount(aih);
        if (ref==0){
            soDeleteDirEntry(inode_handler, bn, &inp);
            soFreeFileClusters(aih, 0);
            soFreeInode(cinp);
            iSave(aih);
            iClose(aih);
            iSave(inode_handler);
            iClose(inode_handler);
            return 0;
        }

        iSave(aih);
        iClose(aih);
        iSave(inode_handler);
        iClose(inode_handler); 

        return 0;       

    }
    catch(SOException & err)
    {
        return -err.en;
    }
    
}
