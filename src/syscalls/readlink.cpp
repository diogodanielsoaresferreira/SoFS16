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
#include "filecluster.h"
#include "dealers.h"

#include "probing.h"
#include "exception.h"

/*
 *  \brief Read the value of a symbolic link.
 *
 *  It tries to emulate <em>readlink</em> system call.
 *
 *  \param path path to the symbolic link
 *  \param buff pointer to the buffer where data to be read is to be stored
 *  \param size buffer size in bytes
 *
 *  \return 0 on success; 
 *      -errno in case of error, being errno the system error that better represents the cause of failure
 */
int soReadlink(const char *path, char *buff, size_t size)
{
    soProbe(236, "soReadlink(\"%s\", %p, %u)\n", path, buff, size);

    try
    {
        char* path2 = strdup(path);

        if (size < 1)
            throw SOException(EINVAL,__FUNCTION__);

        if (strlen(path)>PATH_MAX)
            throw SOException(ENAMETOOLONG,__FUNCTION__);

        uint32_t inp,pih, bpc=soGetBPC();
        SOInode* ip;

        soTraversePath(path2,&inp);

        if(inp == NULL_REFERENCE){
            throw SOException(ENOENT,__FUNCTION__);
        }

        pih = iOpen(inp);
        ip = iGetPointer(pih);

        char tmp_buf[bpc];

        if((ip->mode & S_IFLNK) != S_IFLNK){
            iSave(pih);
            iClose(pih);
            throw SOException(EINVAL,__FUNCTION__);
        }

        if(iCheckAccess(pih, R_OK) == false){
            iSave(pih);
            iClose(pih);
            throw SOException(EACCES,__FUNCTION__);
        }

        soReadFileCluster(pih,0,&tmp_buf);

        if (sizeof(tmp_buf)/sizeof(char)>size){
            iSave(pih);
            iClose(pih);
            throw SOException(EFAULT,__FUNCTION__);
        }

        memcpy(buff, tmp_buf, size);

        iSave(pih);
        iClose(pih);

        return 0;
    }
    catch(SOException & err)
    {
        return -err.en;
    }
}
