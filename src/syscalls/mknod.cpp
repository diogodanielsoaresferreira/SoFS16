/*
 * \author Ana Patrícia Gomes da Cruz
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
#include "dealers.h"
#include "direntries.h"
#include "freelists.h"
#include "probing.h"
#include "exception.h"

/*
 *  \brief Create a regular file with size 0.
 *
 *  It tries to emulate <em>mknod</em> system call.
 *
 *  \param path path to the file
 *  \param mode type and permissions to be set:
 *                    a bitwise combination of S_IFREG, S_IRUSR, S_IWUSR, S_IXUSR, S_IRGRP, S_IWGRP, S_IXGRP, S_IROTH,
 *                    S_IWOTH, S_IXOTH
 *
 *  \return 0 on success; 
 *      -errno in case of error, being errno the system error that better represents the cause of failure
 */
int soMknod(const char *path, mode_t mode)
{
    soProbe(228, "soMknod(\"%s\", %u)\n", path, mode);

    try
    {
        uint32_t inp, node, ih, aih, nd;

        char* xpath = strdupa(path);
        char* bn = strdupa(basename(xpath));
        char* dn = dirname(xpath);

        if(path == NULL || mode == 0)
            throw SOException(EINVAL, __FUNCTION__);

        if(strlen(path) > PATH_MAX)
            throw SOException(ENAMETOOLONG, __FUNCTION__);
        if(strlen(bn) > SOFS16_MAX_NAME)
            throw SOException(ENAMETOOLONG, __FUNCTION__);

        soTraversePath(strdupa(dn), &inp);
        
        if(inp == NULL_REFERENCE)
            throw SOException(ENOENT, __FUNCTION__);

        ih = iOpen(inp);

        /*Acesso de escrita*/
        if(iCheckAccess(ih, W_OK)==false){
            iSave(ih);
            iClose(ih);
            throw SOException(EACCES, __FUNCTION__);
        }

        soGetDirEntry(ih, bn, &node);

        if(node != NULL_REFERENCE){
            iSave(ih);
            iClose(ih);
            throw SOException(EEXIST, __FUNCTION__);
        }

        soAllocInode(S_IFREG, &nd);
        aih = iOpen(nd);

        iSetAccess(aih, mode);

        soAddDirEntry(ih, bn, nd);

        iIncRefcount(aih);

        iSave(aih);
        iClose(aih);
        iSave(ih);
        iClose(ih);
        return 0;
    }
    catch(SOException & err)
    {
        return -err.en;
    }
}
