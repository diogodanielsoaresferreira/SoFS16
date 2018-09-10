/*
 * \author João Pedro Almeida Maia
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
 *  \brief Read data from an open regular file.
 *
 *  It tries to emulate <em>read</em> system call.
 *
 *  \param path path to the file
 *  \param buff pointer to the buffer where data to be read is to be stored
 *  \param count number of bytes to be read
 *  \param pos starting [byte] position in the file data continuum where data is to be read from
 *
 *  \return 0 on success; 
 *      -errno in case of error, being errno the system error that better represents the cause of failure
 */
int soRead(const char *path, void *buff, uint32_t count, int32_t pos)
{
    soProbe(229, "soRead(\"%s\", %p, %u, %u)\n", path, buff, count, pos);

    try
    {
		char* xpath = strdupa(path);
		uint32_t inp, ihp, numcluster, i, nc, idx, bpos, last_c, last_idx;
		uint32_t bpc = soGetBPC();
		char buffer [bpc];
		SOInode* inode;
		
		//check if starting byte is within the limits
		if(pos < 0)
			throw SOException(EINVAL,__FUNCTION__);

		//check if path's length is within limits
		if(strlen(path) > PATH_MAX)
			throw SOException(ENAMETOOLONG,__FUNCTION__);

		//get inode to the assigned path
		soTraversePath(xpath,&inp);

		//get the pointer to the inode
		ihp = iOpen(inp);
		inode = iGetPointer(ihp);

		//check file's access
		if(iCheckAccess(ihp, R_OK) == false){
			iSave(ihp);
			iClose(ihp);
			throw SOException(EACCES,__FUNCTION__);
		}
		//check if the inode is a file
		if((inode->mode & S_IFREG) != S_IFREG){
			iSave(ihp);
			iClose(ihp);
			throw SOException(EPERM,__FUNCTION__);
		}

		if(inode->size==0){
			iSave(ihp);
			iClose(ihp);
			return 0;
		}

		//get the cluster number
		if (inode->size < count){
			memset((uint8_t*)buff, '\0', count-1);
			count = inode->size+1;
		}

		numcluster = pos/bpc;
		last_c = (pos+count-1)/bpc;

		idx = pos%bpc;
		last_idx = (pos+count-1)%bpc;
		bpos = 0;

		for(i = numcluster; i <= last_c; i++){

			//get the cluster of the file
			soGetFileCluster(ihp,i,&nc);

			if(nc==NULL_REFERENCE){
				iSave(ihp);
				iClose(ihp);
				return bpos;
			}

			//read the file allocated on the cluster
			soReadFileCluster(ihp,i,buffer);

			/* First Cluster */
			if(i==numcluster){
				if (count <= bpc-idx){
					memcpy(buff, buffer+idx, last_idx-idx+1);
					bpos+=last_idx-idx+1;
				}
				else{
					memcpy(buff, buffer+idx, bpc-idx);
					bpos+=bpc-idx;
				}
			}
			/* Last cluster */
			else if(i==last_c){
				memcpy((uint8_t*)buff+bpos, buffer, last_idx+1);
				bpos+=last_idx+1;
			}
			else{
				memcpy((uint8_t*)buff+bpos, buffer, bpc);
				bpos+=bpc;
			}
		}
		
		iSave(ihp);
		iClose(ihp);

        return count;
    }
    catch(SOException & err)
    {
        return -err.en;
    }
}
