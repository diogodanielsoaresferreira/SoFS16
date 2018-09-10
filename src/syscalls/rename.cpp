/*
 * \author Diogo Daniel Soares Ferreira
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

#include "syscalls.h"
#include "direntries.h"
#include "dealers.h"
#include "probing.h"
#include "exception.h"

/*
 *  \brief Change the name or the location of a file in the directory hierarchy of the file system.
 *
 *  It tries to emulate <em>rename</em> system call.
 *
 *  \param path path to an existing file
 *  \param newPath new path to the same file in replacement of the old one
 *
 *  \return 0 on success; 
 *      -errno in case of error, being errno the system error that better represents the cause of failure
 */

int soRename(const char *path, const char *newPath)
{
    soProbe(227, "soRename(\"%s\", \"%s\")\n", path, newPath);

    try
    {
        char *xpath = strdupa(path);
        char *bn = basename(strdupa(xpath));
        char *dn = dirname(strdupa(xpath));
        
        char *xpath2 = strdupa(newPath);
        char *bn2 = basename(strdupa(xpath2));
        char *dn2 = dirname(strdupa(xpath2));

        if (strlen(path)>PATH_MAX || strlen(newPath)>PATH_MAX){
        	throw SOException(ENAMETOOLONG, __FUNCTION__);
        }

        if (strlen(bn)>SOFS16_MAX_NAME || strlen(bn2)>SOFS16_MAX_NAME)
            throw SOException(ENAMETOOLONG, __FUNCTION__);
        
        uint32_t inp, inp2, inDeleted, iRenamed;

        /* Supress warnings */
        inp = inp2 = iRenamed = inDeleted = NULL_REFERENCE;

        /* If path and newPath are equal, return Sucess. */
        if(strcmp(strdupa(path), strdupa(newPath))==0)
            return 0;

        /* Get inode number */
        soTraversePath(strdupa(dn), &inp);
        uint32_t ih = iOpen(inp);

        /* Get inode number2 */
        soTraversePath(strdupa(dn2), &inp2);
        uint32_t nih = iOpen(inp2);

        soGetDirEntry(nih, bn2, &inDeleted);

        /* If entry already exists with that name, deletes it */
        if (inDeleted!=NULL_REFERENCE)
            soDeleteDirEntry(nih, bn2, &inDeleted);

        if((iCheckAccess(ih, W_OK) == false) || (iCheckAccess(nih, W_OK) == false)){
            iSave(ih);
            iSave(nih);
            iClose(ih);
            iClose(nih);
            throw SOException(EACCES, __FUNCTION__);
        }

        soDeleteDirEntry(ih, strdupa(bn), &iRenamed);
        soAddDirEntry(nih, bn2, iRenamed);

        uint32_t ih2 = iOpen(iRenamed);

        SOInode* iptr = iGetPointer(ih2);
        
        /* If is directory, change entries . and .. */
        if ((iptr->mode & S_IFDIR)==S_IFDIR){
            if(iCheckAccess(ih2, W_OK) == false){
                iSave(ih);
                iSave(nih);
                iSave(ih2);
                iClose(ih);
                iClose(nih);
                iClose(ih2);
                throw SOException(EACCES, __FUNCTION__);
            }
            soDeleteDirEntry(ih2, "..", &inDeleted);
            soAddDirEntry(ih2, "..", inp2);
            iIncRefcount(nih);
            iDecRefcount(ih);
        }

        iSave(ih);
        iSave(nih);
        iSave(ih2);
        iClose(ih);
        iClose(nih);
        iClose(ih2);
        
        return 0;
    }
    catch(SOException & err)
    {
        return -err.en;
    }
}
