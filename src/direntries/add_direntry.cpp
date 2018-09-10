/**
 *  \author Nuno Filipe Sousa Capela
 *  \tester Diogo Daniel Soares Ferreira
 */

#include "direntries.h"

#include "probing.h"
#include "exception.h"
#include "dealers.h"
#include "filecluster.h"

#include <errno.h>
#include <unistd.h>

void soAddDirEntry(int pih, const char *name, uint32_t cin)
{
    soProbe(500, "soAddDirEntry(%d, %s, %u)\n", pih, name, cin);

    SOInode* parent = iGetPointer(pih);

    uint32_t dpc = soGetDPC();
    uint32_t rpc = soGetRPC();
    uint32_t cn, in = NULL_REFERENCE;
    uint32_t last_index, last_cluster;

    uint32_t i,j;
    SODirEntry dir[dpc];

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

   	/* Check parent is directory or parent is empty */
    if((parent->mode & S_IFDIR) != S_IFDIR || (parent->mode & INODE_FREE) == INODE_FREE)
        throw SOException(ENOENT, __FUNCTION__);
	
   	/* To create a direntry, I need write permission of inode */
    if(iCheckAccess(pih, W_OK | X_OK)==false)
        throw SOException(EACCES, __FUNCTION__);

    /* Check if already exists file with same name */
    soGetDirEntry(pih, name, &in);
    
    if(in!=NULL_REFERENCE)
        throw SOException(EEXIST, __FUNCTION__);

    if(parent->size>=sizeof(SODirEntry)*(N_DIRECT+N_INDIRECT*rpc+rpc*rpc))
        /* Not enough space */
        throw SOException(ENOMEM, __FUNCTION__);

    last_cluster = ((parent->size/sizeof(SODirEntry)))/dpc;
    last_index = ((parent->size/sizeof(SODirEntry)))%dpc;

    soGetFileCluster(pih, last_cluster, &cn);

    // Check if cluster exists
    if(cn==NULL_REFERENCE){
        //If not alloc new cluster and set all direntries to NULL
        soAllocFileCluster(pih,last_cluster,&cn);  

        for (j = 1; j < dpc; j++)
        {
            dir[j].in=NULL_REFERENCE;
            memset(&dir[j].name, '\0',SOFS16_MAX_NAME+1);
        }
        strcpy(dir[0].name, name);
        dir[0].in = cin;
        soWriteCluster(cn,&dir);
        parent->size+=sizeof(SODirEntry);
        return;
    }
    // If exists, add new name
    else{
        soReadFileCluster(pih, last_cluster, &dir);
        strcpy(dir[last_index].name, name);
        dir[last_index].in = cin;
        soWriteCluster(cn,&dir);
        parent->size+=sizeof(SODirEntry);
        return;
    }

    
}
