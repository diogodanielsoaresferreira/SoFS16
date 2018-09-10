/**
 *  \author Eduardo Silva e Nuno Capela
 *  \tester Diogo Daniel Soares Ferreira
 */

#include "filecluster.h"
#include "freelists.h"

#include "probing.h"
#include "exception.h"
#include "inode.h"
#include "dealers.h"

#include <errno.h>
#include <stdint.h>

static void soAllocIndirectFileCluster(SOInode * ip, uint32_t afcn, uint32_t * cnp);
static void soAllocDoubleIndirectFileCluster(SOInode * ip, uint32_t afcn, uint32_t * cnp);


/* ********************************************************* */

void soAllocFileCluster(int ih, uint32_t fcn, uint32_t * cnp)
{
    soProbe(600, "soAllocFileCluster(%d, %u, %p)\n", ih, fcn, cnp);
    SOInode* inode = iGetPointer(ih);
    uint32_t rpc = soGetRPC();

    // checks if file cluster number is valid or out of range
    if((fcn < 0) || (fcn > (N_DIRECT + N_INDIRECT*rpc + rpc*rpc))){
        throw SOException(EINVAL, __FUNCTION__);
    }

    if(fcn < N_DIRECT){
        soAllocCluster(cnp);
        if (inode->d[fcn]==NULL_REFERENCE){
        	inode->csize++;
        }
        inode->d[fcn] = *cnp;

    }

    else if(fcn < (N_DIRECT + N_INDIRECT*rpc)){
        soAllocIndirectFileCluster(inode, fcn, cnp);
    }

    else{
        soAllocDoubleIndirectFileCluster(inode, fcn, cnp);
    }
}   


static void soAllocIndirectFileCluster(SOInode * ip, uint32_t afcn, uint32_t * cnp)
{
    soProbe(600, "soAllocIndirectFileCluster(%p, %u, %p)\n", ip, afcn, cnp);

    uint32_t cn1, cn2, fcn;
    uint32_t rpc = soGetRPC();
    uint32_t ref[rpc];

    fcn = afcn - N_DIRECT;

    if(ip->i1[fcn / rpc] == NULL_REFERENCE){

    	uint32_t i;
    	for(i=0;i<rpc;i++){
    		ref[i] = NULL_REFERENCE;
    	}

    	soAllocCluster(&cn1);
    	ip->i1[fcn / rpc] = cn1;
    	
        soAllocCluster(&cn2);
        ref[fcn % rpc] = cn2;
    	*cnp = cn2;

        soWriteCluster(cn1, &ref);

        ip->csize+=2;

    }

    else{
        soReadCluster(ip->i1[fcn / rpc], &ref);

    	soAllocCluster(&cn1);

    	*cnp = cn1;

    	if (ref[fcn % rpc]==NULL_REFERENCE){
    		ip->csize++;
    	}

    	ref[fcn % rpc] = cn1;
        
        soWriteCluster(ip->i1[fcn / rpc], &ref);
    }



}

/* ********************************************************* */

static void soAllocDoubleIndirectFileCluster(SOInode * ip, uint32_t afcn, uint32_t * cnp)
{
    soProbe(600, "soAllocDoubleIndirectFileCluster(%p, %u, %p)\n", ip, afcn, cnp);

    uint32_t cn1, cn2, cn3, fcn, i;
    uint32_t rpc = soGetRPC();
    uint32_t ref[rpc], ref2[rpc];

    fcn = afcn-(N_DIRECT+N_INDIRECT*rpc);

    uint32_t index_pos = fcn%rpc;

    uint32_t index_cluster = fcn/rpc;

    for(i=0; i<rpc; i++)
        ref[i] = NULL_REFERENCE;

    if(ip->i2==NULL_REFERENCE){

        /* Allocate first cluster i2 */
        soAllocCluster(&cn1);
        ip->i2 = cn1;

        /* Allocate second cluster */
        soAllocCluster(&cn2);
        ref[index_cluster] = cn2;
        /* Save second cluster on first and put reference to NULL to be used again */
        soWriteCluster(ip->i2, &ref);
        ref[index_cluster] = NULL_REFERENCE;

        /* Allocate third cluster and save its index on second cluster */
        soAllocCluster(&cn3);
        ref[index_pos] = cn3;
        soWriteCluster(cn2, &ref);



        ip->csize+=3;
    }
    else{
        soReadCluster(ip->i2, &ref2);

        if (ref2[index_cluster]==NULL_REFERENCE){
            /* Allocate second cluster */
            soAllocCluster(&cn2);
            ref2[index_cluster] = cn2;
            /* Save second cluster on first */
            soWriteCluster(ip->i2, &ref2);

            /* Allocate third cluster and save its index on second cluster */
            soAllocCluster(&cn3);
            ref[index_pos] = cn3;
            soWriteCluster(cn2, &ref);


            ip->csize+=2;

        }
        else{

            soReadCluster(ref2[index_cluster], &ref);
            /* Allocate third cluster and save its index on second cluster */
            soAllocCluster(&cn3);

            if (ref[index_pos]==NULL_REFERENCE){
            	ip->csize++;
            }

            ref[index_pos] = cn3;
            soWriteCluster(ref2[index_cluster], &ref);

        }
    }
    *cnp = cn3;

}

