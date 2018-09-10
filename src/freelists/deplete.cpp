/**
 *  \author Pedro Marques Ferreira da Silva
 *  \tester Pedro Marques Ferreira da Silva
 */

#include "freelists.h"

#include "dealers.h"
#include "probing.h"
#include "exception.h"

#include <errno.h>

/*
 * Even if some of them are not functionally necessary,
 * the following dictates must be obyed by the implementation:
 * - if crefs is equal to zero, 
 *      first transfer as much as possible to head cache 
 * 
 */

void soDeplete(void)
{
    soProbe(722, "soDeplete()\n");

    uint32_t rpc = soGetRPC();
    uint32_t ref[rpc];
    uint32_t alloc_c, index, i;

    SOSuperBlock *sbp = sbGetPointer();
    uint32_t cluster_idx = sbp->ctail.cluster_number;

    // If cache is empty, returns
    if(sbp->ctail.cache.ref[sbp->ctail.cache.out]==NULL_REFERENCE)
    	return;
    /* If there is no reference cluster, transfers for head cache */
    if(sbp->crefs==0){
    	for(index=sbp->ctail.cache.out; sbp->ctail.cache.ref[index]!=NULL_REFERENCE && sbp->chead.cache.ref[sbp->chead.cache.in]==NULL_REFERENCE; index=(index+1)%FCT_CACHE_SIZE){
    		sbp->chead.cache.ref[sbp->chead.cache.in++]=sbp->ctail.cache.ref[index];
    		sbp->ctail.cache.ref[index]=NULL_REFERENCE;

    		if(sbp->ctail.cache.out==FCT_CACHE_SIZE)
				sbp->ctail.cache.out=0;
    	}
    	sbp->ctail.cache.out=index;
    }
    else{
    	soReadCluster(cluster_idx, &ref);
    	for(index=sbp->ctail.cache.out; sbp->ctail.cache.ref[index]!=NULL_REFERENCE; index=(index+1)%FCT_CACHE_SIZE){
    		// Transfers for tail cluster
    		if(sbp->ctail.cluster_idx!=rpc-1){
    			ref[sbp->ctail.cluster_idx++]=sbp->ctail.cache.ref[index];
    			sbp->ctail.cache.ref[index]=NULL_REFERENCE;
    		}
            // Allocate another cluster and extend reference list
    		else{
    			soAllocCluster(&alloc_c);
    			ref[rpc-1] = alloc_c;
    			sbp->crefs++;
                soWriteCluster(sbp->ctail.cluster_number, &ref);
    			sbp->ctail.cluster_number = alloc_c;
                sbp->ctail.cluster_idx = 1;
                soReadCluster(sbp->ctail.cluster_number, &ref);
                for(i=1; i<rpc; i++)
                    ref[i] = NULL_REFERENCE;
                ref[0] = sbp->ctail.cache.ref[index];
                sbp->ctail.cache.ref[index]=NULL_REFERENCE;
    		}
    	}
        soWriteCluster(sbp->ctail.cluster_number, &ref);
    	sbp->ctail.cache.out = index;
    }


    sbSave();
}
