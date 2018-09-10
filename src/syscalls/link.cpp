/*
 * \author João Pedro Almeida Maia
 */

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
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
#include "probing.h"
#include "exception.h"

/*
 *  \brief Make a new link to a file.
 *
 *  It tries to emulate <em>link</em> system call.
 *
 *  \param path path to an existing file
 *  \param newPath new path to the same file
 *
 *  \return 0 on success; 
 *      -errno in case of error, being errno the system error that better represents the cause of failure
 */

int soLink(const char *path, const char *newPath)
{
    soProbe(225, "soLink(\"%s\", \"%s\")\n", path, newPath);

    try
    {
    	char *xpath = strdupa(path);
    	char *dn = dirname(strdupa(xpath));
        
        char *xpath2 = strdupa(newPath);
        char *bn2 = basename(strdupa(xpath2));

        if (strlen(path)>PATH_MAX || strlen(newPath)>PATH_MAX){
        	throw SOException(ENAMETOOLONG, __FUNCTION__);
        }

        if(strlen(bn2)>SOFS16_MAX_NAME)
        	throw SOException(ENAMETOOLONG, __FUNCTION__);

		uint32_t inp, ih, cinp, ihl, ih2;
		SOInode* ip;

		//get the inode of path of existing file
		soTraversePath(xpath,&inp);

		//get the inode of directory for new path
		soTraversePath(dn,&cinp);
		
		//get the pointer to the inode
		ih = iOpen(cinp);

		/* Check access */
		if(iCheckAccess(ih, W_OK)==false){
			iClose(ih);
            throw SOException(EACCES, __FUNCTION__);
		}

		soTraversePath(xpath,&ihl);
		ih2 = iOpen(ihl);
		ip = iGetPointer(ih2);

		//check if the inode is a file
		if((ip->mode & S_IFREG) != S_IFREG && (ip->mode & S_IFLNK) != S_IFLNK){
			iClose(ih);
			iClose(ih2);
			throw SOException(EPERM, __FUNCTION__);
		}

		soAddDirEntry(ih,bn2,inp);
		iIncRefcount(ih2);

		iSave(ih);
		iClose(ih);
		iSave(ih2);
		iClose(ih2);

        return 0;
    }
    catch(SOException & err)
    {
        return -err.en;
    }

}
