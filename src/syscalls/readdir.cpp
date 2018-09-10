/*
 * \author Nuno Capela
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
#include "filecluster.h"
#include "probing.h"
#include "exception.h"

/*
 *  \brief Read a directory entry from a directory.
 *
 *  It tries to emulate <em>getdents</em> system call, but it reads a single directory entry at a time.
 *
 *  Only the field <em>name</em> is read.
 *
 *  \remark The returned value is the number of bytes read from the directory in order to get the next in use
 *          directory entry. 
 *          The point is that the system (through FUSE) uses the returned value to update file position.
 *
 *  \param path path to the file
 *  \param buff pointer to the buffer where data to be read is to be stored
 *  \param pos starting [byte] position in the file data continuum where data is to be read from
 *
 *  \return 0 on success; 
 *      -errno in case of error, being errno the system error that better represents the cause of failure
 */
int soReaddir(const char *path, void *buff, int32_t pos)
{
    soProbe(234, "soReaddir(\"%s\", %p, %u)\n", path, buff, pos);

    
    try{
        
        char* xpath = strdupa(path);

        uint32_t inp, pih,nc;
        uint32_t cluster_number, cluster_index;
        uint32_t dpc = soGetDPC();
        SODirEntry buffer[dpc];

        //check if starting byte is within the limits
        if(pos < 0)
            throw SOException(EINVAL,__FUNCTION__);

        //check if path's length is within limits
        if(strlen(path) > PATH_MAX)
            throw SOException(ENAMETOOLONG,__FUNCTION__);

        soTraversePath(xpath,&inp);
        pih = iOpen(inp);
        SOInode* inode = iGetPointer(pih);

        /*Verificar se parent é diretorio*/
        if((inode->mode & S_IFDIR) != S_IFDIR){
            iSave(pih);
            iClose(pih);
            throw SOException(ENOTDIR,__FUNCTION__);
        }


        if(iCheckAccess(pih, R_OK)==false){
            iSave(pih);
            iClose(pih);
            throw SOException(EACCES,__FUNCTION__);
        }

        //get the cluster number
        cluster_number = (pos/sizeof(SODirEntry))/dpc;
        cluster_index = (pos/sizeof(SODirEntry))%dpc;

        if ((uint32_t)pos>=inode->size){
            iSave(pih);
            iClose(pih);
            return 0;
        }

        //get the cluster of the file
        soGetFileCluster(pih,cluster_number,&nc);

        if(nc==NULL_REFERENCE){
            iSave(pih);
            iClose(pih);
            return 0;
        }
        else{

            //read the file allocated on the cluster
            soReadFileCluster(pih, cluster_number, buffer);
            memcpy(buff, buffer[cluster_index].name, 1+strlen(buffer[cluster_index].name));
            iSave(pih);
            iClose(pih);

            return sizeof(SODirEntry);
        }

        iSave(pih);
        iClose(pih);
        
        return 0;
    }
    catch(SOException & err)
    {
        return -err.en;
    }
}
