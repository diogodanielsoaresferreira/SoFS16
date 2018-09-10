/*
 * \author Eduardo Reis Silva
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
#include "freelists.h"
#include "filecluster.h"

#include "probing.h"
#include "exception.h"

/*
 *  \brief Creates a symbolic link which contains the given path.
 *
 *  It tries to emulate <em>symlink</em> system call.
 *
 *  \remark The permissions set for the symbolic link should have read (r), write (w) and execution (x) permissions for
 *          both <em>user</em>, <em>group</em> and <em>other</em>.
 *
 *  \param effPath path to be stored in the symbolic link file
 *  \param path path to the symbolic link to be created
 *
 *  \return 0 on success; 
 *      -errno in case of error, being errno the system error that better represents the cause of failure
 */
int soSymlink(const char *effPath, const char *path)
{
    soProbe(235, "soSymlink(\"%s\", \"%s\")\n", effPath, path);

    try
    {
        char* xpath = strdupa(path);
        char* bn = strdupa(basename(xpath));
        char* dn = dirname(xpath);

        if (strlen(effPath)>PATH_MAX || strlen(path)>PATH_MAX)
            throw SOException(ENAMETOOLONG,__FUNCTION__);

        if (strlen(bn)>PATH_MAX)
            throw SOException(ENAMETOOLONG,__FUNCTION__);

        uint32_t inp, pih, cinp, ain, aih, bpc = soGetBPC();
        SOInode* ip, *ip2;
        char cl[bpc];
        
        memset(cl, '\0', bpc);
        memcpy(cl, effPath, strlen(effPath));

        soTraversePath(dn,&inp);

        if(inp == NULL_REFERENCE){
            throw SOException(ENOENT,__FUNCTION__);
        }

        pih = iOpen(inp);
        ip = iGetPointer(pih);

        if((ip->mode & S_IFDIR) != S_IFDIR){
            iSave(pih);
            iClose(pih);
            throw SOException(ENOTDIR,__FUNCTION__);
        }


        if(iCheckAccess(pih, W_OK)==false){
            iSave(pih);
            iClose(pih);
            throw SOException(EACCES,__FUNCTION__);
        }

        soGetDirEntry(pih,bn,&cinp);

        if(cinp != NULL_REFERENCE){
            iSave(pih);
            iClose(pih);
            throw SOException(EEXIST,__FUNCTION__);
        }

        soAllocInode(S_IFLNK,&ain);

        if (ain==NULL_REFERENCE){
            iSave(pih);
            iClose(pih);
            throw SOException(ENOMEM,__FUNCTION__);
        }

        aih = iOpen(ain);

        iSetAccess(aih, 511);
        iIncRefcount(aih);
        
        ip2 = iGetPointer(aih);

        ip2->size=strlen(effPath);

        soWriteFileCluster(aih,0,cl);

        soAddDirEntry(pih,bn,ain);

        iSave(aih);
        iClose(aih);
        iSave(pih);
        iClose(pih);

        return 0;

    }
    catch(SOException & err)
    {
        return -err.en;
    }
}
