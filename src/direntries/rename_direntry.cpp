/**
 *  \author Ana Cruz
 *  \tester Ana Cruz
 */

#include "direntries.h"
#include "dealers.h"
#include "filecluster.h"
#include "probing.h"
#include "exception.h"

#include <unistd.h>
#include <errno.h>

void soRenameDirEntry(int pih, const char *name, const char *newName)
{
		soProbe(500, "soRenameDirEntry(%d, %s, %s)\n", pih, name, newName);
	 
		SOInode *inode = iGetPointer(pih);

		uint32_t dpc = soGetDPC();
		uint32_t bpc = soGetBPC();
		SODirEntry dir[dpc];

		uint32_t i, j, cn=NULL_REFERENCE, in=NULL_REFERENCE;

		uint32_t dir_cluster = inode->size / dpc;


		if(iCheckAccess(pih, W_OK | X_OK)==false)
			throw SOException(EACCES, __FUNCTION__);

		/* Check if name has slash */
		for(i=0;name[i]!='\0';i++){
				if(name[i]=='/'){
					throw SOException(EINVAL, __FUNCTION__);
				}
		}

		/* Check if filename if bigger that maximum name file */
	    if(i-1>SOFS16_MAX_NAME){
	    	throw SOException(ENAMETOOLONG, __FUNCTION__);
	    }

		
		/* Check inode is directory or inode is empty */
		if((inode->mode & S_IFDIR) != S_IFDIR || (inode->mode & INODE_FREE) == INODE_FREE)
				throw SOException(ENOENT, __FUNCTION__);
		
		if(strcmp(name, newName)==0)
			return;
		
		if(inode->size%bpc != 0)
			dir_cluster = dir_cluster + 1;

		for(i=0; i<dir_cluster; i++){

			soReadFileCluster(pih, i, &dir);

			for(j=0; j<dpc; j++){
				if(strcmp(dir[j].name,name)==0){

					/* Check if already exists file with same name */
					soGetDirEntry(pih, newName, &in);

					if(in!=NULL_REFERENCE)
						throw SOException(EEXIST, __FUNCTION__);
					strcpy(dir[j].name, newName);
					soGetFileCluster(pih, i, &cn);
					soWriteCluster(cn, &dir);
					return;
				}
			}
		}
	throw SOException(ENOENT, __FUNCTION__);

}