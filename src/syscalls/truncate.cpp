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
 *  \brief Truncate a regular file to a specified length.
 *
 *  It tries to emulate <em>truncate</em> system call.
 *
 *  \param path path to the file
 *  \param length new size for the regular size
 *
 *  \return 0 on success; 
 *      -errno in case of error, being errno the system error that better represents the cause of failure
 */
int soTruncate(const char *path, off_t length)
{
    soProbe(231, "soTruncate(\"%s\", %u)\n", path, length);
    
    try
    {
          char *xpath = strdupa(path);

          uint32_t inp, inode_handler, bpc = soGetBPC();
          SOInode* inode;

          char buf[bpc];

          if (strlen(path)>PATH_MAX){
            throw SOException(ENAMETOOLONG, __FUNCTION__);
          }

          memset(buf, '\0', bpc);
          
          soTraversePath(xpath, &inp);

          /*validaçao dos argumentos */
          if(inp == NULL_REFERENCE){
            throw SOException(ENOENT, __FUNCTION__);
          }

          if(length < 0){
             throw SOException(EINVAL, __FUNCTION__);
          }

          if(length > soGetMaxFileSize())
            throw SOException(EFBIG, __FUNCTION__);
         
          inode_handler = iOpen(inp);
          inode = iGetPointer(inode_handler);

          /*check inode is not regular file or if inode is empty */
          if((inode->mode & S_IFREG) != S_IFREG || (inode->mode & INODE_FREE) == INODE_FREE){
            iSave(inode_handler);
            iClose(inode_handler);
            throw SOException(ENOENT, __FUNCTION__);
          }

          /*check access permissions */
          if(iCheckAccess(inode_handler, W_OK) == false){ /*write permissions */
            iSave(inode_handler);
            iClose(inode_handler);
            throw SOException(EACCES, __FUNCTION__);
          }
    
          int diff = length-inode->size;

          if(diff<0){
            soFreeFileClusters(inode_handler, (length/bpc)+1);
            soReadFileCluster(inode_handler, length/bpc, buf);
            memset(buf+length%bpc, '\0', bpc-length%bpc);
            soWriteFileCluster(inode_handler, length/bpc, buf);
          }

          inode->size = length;
          iSave(inode_handler);
          iClose(inode_handler);

          return 0;

        
    }
    catch(SOException & err)
    {
        return -err.en;
    }

}
