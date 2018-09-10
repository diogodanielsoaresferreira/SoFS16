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
#include "dealers.h"
#include "direntries.h"
#include "filecluster.h"
#include "probing.h"
#include "exception.h"

/*
 *  \brief Write data into an open regular file.
 *
 *  It tries to emulate <em>write</em> system call.
 *
 *  \param path path to the file
 *  \param buff pointer to the buffer where data to be written is stored
 *  \param count number of bytes to be written
 *  \param pos starting [byte] position in the file data continuum where data is to be written into
 *
 *  \return 0 on success; 
 *      -errno in case of error, being errno the system error that better represents the cause of failure
 */
int soWrite(const char *path, void *buff, uint32_t count, int32_t pos)
{
    soProbe(230, "soWrite(\"%s\", %p, %u, %u)\n", path, buff, count, pos);

    try
    {
        uint32_t bpc = soGetBPC();
        uint32_t cn, idx, ih, fcn;
        char *xpath = strdupa(path);
        uint32_t inp, cnp;

        char buf [bpc];

        if (strlen(path)>PATH_MAX){
            throw SOException(ENAMETOOLONG, __FUNCTION__);
        }


        if(pos<0)
            throw SOException(EINVAL, __FUNCTION__);

        /* Supress warnings */
        inp = cnp = fcn =NULL_REFERENCE;

        /* Get inode number */
        soTraversePath(strdupa(xpath), &inp);

        cn = pos/bpc;
        idx = pos%bpc;
        
        ih = iOpen(inp);

        SOInode* inode = iGetPointer(ih);

        if((inode->mode & S_IFREG) != S_IFREG){
            iSave(ih);
            iClose(ih);
            throw SOException(ENOENT, __FUNCTION__);
        }
        
        if(iCheckAccess(ih, W_OK)==false){
            iSave(ih);
            iClose(ih);
            throw SOException(EACCES, __FUNCTION__);
        }

         if (pos+count>=soGetMaxFileSize()){
                iSave(ih);
                iClose(ih);
                throw SOException(EFBIG, __FUNCTION__);
         }

        uint32_t bpos = 0;

        for(uint32_t i=cn; i<=(pos+count)/bpc; i++){
            soGetFileCluster(ih, i, &fcn);

            /* Allocate new cluster */
            if (fcn==NULL_REFERENCE)
                soAllocFileCluster(ih, i, &fcn);

            soReadFileCluster(ih, i, buf);

            /* First Cluster */
            if(i==cn){
                /* Data only on first cluster */
                if (count > bpc-idx){
                    memcpy(buf+idx, (uint8_t*)buff+bpos, bpc-idx);
                    bpos+=bpc-idx;
                }
                else{
                    memcpy(buf+idx, (uint8_t*)buff+bpos, count);
                    bpos += count;
                }
            }

            /* Last Cluster */
            else if(i==(pos+count)/bpc){
                memcpy(buf, (uint8_t*)buff+bpos, count-bpos);
                bpos+=bpc;
            }

            else{
                memcpy(buf, (uint8_t*)buff + bpos, bpc);
                bpos+=bpc;
            }
            soWriteCluster(fcn, buf);
        }

        inode->size+=count;

        iSave(ih);
        iClose(ih);
        return count;
    }
    catch(SOException & err)
    {
        return -err.en;
    }
}
