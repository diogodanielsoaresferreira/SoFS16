#include "mksofs.h"
#include "superblock.h"
#include "exception.h"
#include "inode.h"
#include "rawdisk.h"
#include "cluster.h"
#include <errno.h>



/* filling in the superblock fields:
   *   magic number should be set presently to 0xFFFF, 
   *   this enables that if something goes wrong during formating, the
   *   device can never be mounted later on
   */
void fillInSuperBlock(SOSuperBlock * sbp, const char *name,
                      uint32_t ntotal, uint32_t itotal, uint32_t bpc)
{

    /* Later change to MAGIC_NUMBER */
    sbp->magic = MAGIC_NUMBER;
    sbp->version = VERSION_NUMBER;

    /* Volume name */
    unsigned int i = 0;

    memset(sbp->name, '\0',PARTITION_NAME_SIZE+1);
    
    for(; name[i]!='\0' && i<PARTITION_NAME_SIZE; i++)
    	sbp->name[i] = name[i];

    sbp->name[i] = '\0';

    /* Flag of System Properly Unmounted */
    sbp->mstat = PRU;
    
    /* Blocks per Cluster */
    sbp->csize = bpc;
    /* Blocks in the device */
    sbp->ntotal = ntotal;

    /* Inode table metadata */

    /* Number of block where table of inodes start */
    sbp->itstart = 1;


    if ((itotal % IPB) == 0){
        sbp->itsize = itotal/IPB;
        sbp->itotal = itotal;
    }

    else{
        sbp->itsize = itotal/IPB + 1;
        sbp->itotal = sbp->itsize*IPB;
    }
    
    /* Adjusting total inodes and inode table size */
    while((ntotal-1-sbp->itsize)%bpc){
        sbp->itotal+=IPB;
        sbp->itsize++;
    }
    sbp->ctotal = (ntotal-1-sbp->itsize)/bpc;
    

    /* Number of free inodes */
    sbp->ifree = sbp->itotal-1;
    /* Head of linked list of free inodes */
    sbp->ihead = 1;
    /* Tail of linked list of free inodes */
    sbp->itail = sbp->itotal-1;


    /* Physical number of the block where the cluster zone starts */
    sbp->czstart = sbp->itsize+1;


    sbp->cfree=sbp->ctotal-1;

    if ((sbp->cfree)%((RPB*bpc))!=0){
        sbp->crefs = (sbp->cfree)/((RPB*bpc)) + 1;
    }
    else{
        sbp->crefs = (sbp->cfree)/((RPB*bpc));
    }

    sbp->cfree-=sbp->crefs;


    /* Checks if it needs last reference cluster */
    if ((sbp->crefs-1)*((RPB*bpc)-1)>sbp->cfree && sbp->crefs>0){
        sbp->crefs-=1;
        sbp->ctail.cluster_idx = ((RPB*bpc)-1);
    }
    else if((sbp->crefs-1)*((RPB*bpc)-1)==sbp->cfree && sbp->crefs>0){
        sbp->ctail.cluster_idx = 0;
    }
    else if ((sbp->cfree)%((RPB*bpc)-1)==0){
        sbp->ctail.cluster_idx = ((RPB*bpc)-1);
    }
    else{
        sbp->ctail.cluster_idx = (sbp->cfree)%((RPB*bpc)-1);
    }


    for(i=0;i<FCT_CACHE_SIZE;i++){
        sbp->chead.cache.ref[i] = NULL_BLOCK;
        sbp->ctail.cache.ref[i] = NULL_BLOCK;
    }

    sbp->chead.cache.in = 0;
    sbp->chead.cache.out = 0;
    sbp->ctail.cache.in = 0;
    sbp->ctail.cache.out = 0;

    sbp->chead.cluster_number = 1;
    sbp->chead.cluster_idx = 0;

    sbp->ctail.cluster_number = sbp->crefs;

}
