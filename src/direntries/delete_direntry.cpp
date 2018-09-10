/**
 *  \author Diogo Daniel Soares Ferreira
 *  \tester Diogo Daniel Soares Ferreira
 */

#include "direntries.h"

#include "probing.h"
#include "exception.h"
#include "dealers.h"
#include "filecluster.h"

#include <errno.h>
#include <unistd.h>
#include <stdbool.h>


void soDeleteDirEntry(int pih, const char *name, uint32_t * cinp)
{
    soProbe(500, "soDeleteDirEntry(%d, %s, %p)\n", pih, name, cinp);

    uint32_t total, fcn, cn, cn2, i, j, k;
    uint32_t dpc = soGetDPC();
    uint32_t rpc = soGetRPC();
    SODirEntry dir[dpc], dir2[dpc];

    SOInode* inode = iGetPointer(pih);

    cn = NULL_REFERENCE;
    total = inode->csize;
    fcn = 0;

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

    /* To delete a direntry, I need execution and write permission of inode */
    if(iCheckAccess(pih, W_OK | X_OK)==false)
        throw SOException(EACCES, __FUNCTION__);

    /* While there are clusters and while the fcn < number of maximum clusters */
    while(total!=0 && fcn<N_DIRECT+N_INDIRECT*rpc+rpc*rpc){
    	soGetFileCluster(pih, fcn, &cn);
    	if(cn==NULL_REFERENCE) break;
    	soReadFileCluster(pih, fcn, &dir);
    	for(i=0; i<dpc; i++){
    		if(strcmp(dir[i].name, name)==0){

    			/* Decrement size of inode */
    			inode->size-=sizeof(SODirEntry);

    			*cinp = dir[i].in;

    			/* Last directory existent and not on last position of cluster and cluster is not empty */
    			if(i>0 && i<dpc-1 && dir[i+1].name[0]=='\0'){
    				
    				/* Free directory */
    				dir[i].in=NULL_REFERENCE;
    				memset(&dir[i].name, '\0',SOFS16_MAX_NAME+1);
    				
    				/* Save on disk */
	    			soWriteCluster(cn, &dir);
	    			return;
    			}
                
    			/* Last directory existent and on first position of cluster */
    			if(i==0 && dir[i+1].name[0]=='\0'){
    				
    				/* Free directory */
    				dir[i].in=NULL_REFERENCE;
    				memset(&dir[i].name, '\0',SOFS16_MAX_NAME+1);

    				soFreeFileClusters(pih, fcn);
    				/* Save on disk */
	    			soWriteCluster(cn, &dir);
	    			return;
    			}

    			/* Loads content of next cluster */
    			if(fcn+1<N_DIRECT+N_INDIRECT*rpc+rpc*rpc){
    				soGetFileCluster(pih, fcn+1, &cn2);
    			}
    			else{
    				cn2 = NULL_REFERENCE;
    			}

    			/* Last directory existent and on last position of cluster */
    			if(i==dpc-1 && cn2==NULL_REFERENCE){
    				/* Free directory */
	    			dir[i].in=NULL_REFERENCE;
	    			memset(&dir[i].name, '\0',SOFS16_MAX_NAME+1);

	    			/* Save on disk */
		    		soWriteCluster(cn, &dir);
		    		return;
    			}
    			
    			/* Not last directory existent but last is on same cluster */
    			if(cn2==NULL_REFERENCE){
    				for(j=dpc-1;j>0 && dir[j].name[0]=='\0'; j--);
    				
    				strcpy(dir[i].name,dir[j].name);
       				dir[i].in = dir[j].in;

    				/* Free directory */
    				memset(&dir[j].name, '\0',SOFS16_MAX_NAME+1);
    				dir[j].in=NULL_REFERENCE;

    				/* Save on disk */
		    		soWriteCluster(cn, &dir);
		    		return;
    			}


    			/* Different Cluster */

    			/* Get cluster number */
    			for(j=fcn+1;fcn<N_DIRECT+N_INDIRECT*rpc+rpc*rpc; j++){
    				soGetFileCluster(pih, j, &cn2);
    				if(cn2==NULL_REFERENCE) break;
    			}
    			
    			soGetFileCluster(pih, j-1, &cn2);
    			soReadFileCluster(pih, j-1, &dir2);

    			/* Get index */
    			for(k=dpc-1;j>=0 && dir2[k].name[0]=='\0'; k--);

    			strcpy(dir[i].name,dir2[k].name);
    			dir[i].in = dir2[k].in;
    			
    			dir2[k].in=NULL_REFERENCE;
	    		memset(&dir2[k].name, '\0',SOFS16_MAX_NAME+1);
	    		soWriteCluster(cn2, &dir2);

	    		/* If index of eliminated cluster is 0, free cluster */
    			if(k==0){
    				soFreeFileClusters(pih, j-1);
    			}
    			
    			/* Save on disk */
		    	soWriteCluster(cn, &dir);
		    	return;
    			
    		}
    	
    	}
    	fcn++;
	}

	throw SOException(ENOENT, __FUNCTION__);
	
}

