/**
 *  \author Diogo Daniel Soares Ferreira
 *  \tester Diogo Daniel Soares Ferreira
 */

#include "freelists.h"

#include "probing.h"
#include "exception.h"
#include "dealers.h"

#include <errno.h>

/*
 * Even if some of them are not functionally necessary,
 * the following dictates must be obeyed by the implementation:
 * - do nothing if head cache is not empty;
 * - the head cache should be filled from the beginning, 
 *      so, at the end, index out should be put at zero;
 * - if crefs is equal to zero, transfer references from the tail cache;
 * - otherwise, transfer references only from the head cluster;
 * - if after the transfer the head cluster get empty, it should be freed;
 * - after every reference is transferred, 
 *      the previous location should be filled with NULL_REFERENCE
 */
void soReplenish(void)
{
    soProbe(733, "soReplenish()\n");
    

    SOSuperBlock *sbp = sbGetPointer();

	uint8_t index = 0;
	uint8_t cluster_idx = sbp->chead.cluster_number;

	/* References per cluster */
    uint32_t rpc = soGetRPC();
	
	/* Reference table */
	uint32_t ref[rpc];
	uint32_t t, save;

	// If cache is not empty, returns
	if(sbp->chead.cache.ref[sbp->chead.cache.out]!=NULL_REFERENCE){
		return;
	}

	/* 	the head cache should be filled from the beginning, 
	 *  so, at the end, index out should be put at zero*/
	sbp->chead.cache.out=0;

	if (sbp->crefs==0){
		// Transfer from tail cache
		for(index = 0; index<FCT_CACHE_SIZE && sbp->ctail.cache.ref[sbp->ctail.cache.out]!=NULL_REFERENCE; index++){
			sbp->chead.cache.ref[index] = sbp->ctail.cache.ref[sbp->ctail.cache.out];
			sbp->ctail.cache.ref[sbp->ctail.cache.out++] = NULL_REFERENCE;

			if(sbp->ctail.cache.out==FCT_CACHE_SIZE)
				sbp->ctail.cache.out=0;

		}
		if(index==FCT_CACHE_SIZE)
			index=0;
		sbp->chead.cache.in=index;

	}
	else{
		// Transfer from head cluster
		// if after the transfer the head cluster get empty, it should be freed

		soReadCluster(cluster_idx, &ref);

		for(index = 0; index<FCT_CACHE_SIZE; index++){

			// Se head cluster index chega ao tail cluster index, não há mais referências livres
			if( (sbp->chead.cluster_number>sbp->ctail.cluster_number) || (sbp->chead.cluster_number==sbp->ctail.cluster_number && sbp->chead.cluster_idx>=sbp->ctail.cluster_idx)){

				save = sbp->chead.cluster_number;
				sbp->crefs--;
				sbp->cfree++;

				
				sbp->ctail.cache.ref[sbp->ctail.cache.in++] = sbp->chead.cluster_number;
				if(sbp->ctail.cache.in==FCT_CACHE_SIZE)
					sbp->ctail.cache.in = 0;
				sbp->chead.cluster_number = NULL_REFERENCE;
				sbp->chead.cluster_idx = 0;
				sbp->ctail.cluster_number = NULL_REFERENCE;
				sbp->ctail.cluster_idx = 0;

				break;
			}

			sbp->chead.cache.ref[index] = ref[sbp->chead.cluster_idx];
			ref[sbp->chead.cluster_idx++] = NULL_REFERENCE;

			// Se Head cluster index chega ao final
			if(sbp->chead.cluster_idx==rpc-1){
				
				// Se tail cache estiver cheia, fazer Deplete.
				if (sbp->ctail.cache.ref[sbp->ctail.cache.in]==NULL_REFERENCE){
					soDeplete();
				}

				// Armazenar número de head cluster no tail como referência de cluster livre
				sbp->ctail.cache.ref[sbp->ctail.cache.in++] = sbp->chead.cluster_number;
				if(sbp->ctail.cache.in==FCT_CACHE_SIZE)
					sbp->ctail.cache.in = 0;


				sbp->crefs--;
				sbp->cfree++;

				// Delete last reference of head. Change head to last reference. Set index to 0.
				t = ref[rpc-1];
				ref[rpc-1] = NULL_REFERENCE;
				save = sbp->chead.cluster_number;
				sbp->chead.cluster_number = t;
				sbp->chead.cluster_idx = 0;
				index++;

				break;
			}
			save = sbp->chead.cluster_number;
		}
		soWriteCluster(save, &ref);
		if (index==FCT_CACHE_SIZE)
			index = 0;
		sbp->chead.cache.in = index;
	}


	sbSave();
}

