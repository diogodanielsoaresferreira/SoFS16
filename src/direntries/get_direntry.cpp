/**
 *  \author Eduardo Reis Silva
 *  \tester Eduardo Reis Silva
 */

#include "direntries.h"
#include "dealers.h"
#include "probing.h"
#include "exception.h"
#include "filecluster.h"

#include <errno.h>

void soGetDirEntry(int pih, const char *name, uint32_t * cinp)
{
    soProbe(500, "soGetDirEntry(%d, %s, %p)\n", pih, name, cinp);
    

    SOInode *inode = iGetPointer(pih);

    uint32_t bpc = soGetBPC();
    uint32_t dpc = soGetDPC();
    SODirEntry dir[dpc];

    uint32_t i,j;

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


    uint32_t dir_clus = inode->size / bpc;

    *cinp = NULL_REFERENCE;

    if(inode->size % bpc != 0){
    	dir_clus = dir_clus+1;
    }

	for(i=0;i<dir_clus;i++){
		soReadFileCluster(pih,i,&dir);
		for(j=0;j<inode->size/sizeof(SODirEntry);j++){
			if(strcmp(dir[j].name,name)==0){
				*cinp = dir[j].in;
                return;
			}
		}   
	}

}
