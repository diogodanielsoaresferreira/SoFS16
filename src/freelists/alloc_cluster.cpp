/**
 *  \author Eduardo Reis Silva
 *  \tester Eduardo Reis Silva
 */

#include "freelists.h"

#include "dealers.h"
#include "probing.h"
#include "exception.h"

#include <errno.h>


/*
 * Dictates to be obeyed by the implementation:
 * - error ENOSPC should be thrown if there is no free clusters
 * - after the reference is removed, 
 *      its location should be filled with NULL_REFERENCE
 */
void soAllocCluster(uint32_t * cnp)
{
    soProbe(713, "soAllocCluster(%u)\n", cnp);

    SOSuperBlock *sbp;				
    sbp = sbGetPointer();

    if(sbp->cfree == 0){
    	throw SOException(ENOSPC, __FUNCTION__);
    }

   	if(sbp->chead.cache.out==sbp->chead.cache.in && sbp->chead.cache.ref[sbp->chead.cache.out]==NULL_REFERENCE){
   		soReplenish();
   	}

   	*cnp = sbp->chead.cache.ref[sbp->chead.cache.out];

   	sbp->chead.cache.ref[sbp->chead.cache.out] = NULL_REFERENCE;

    if(++sbp->chead.cache.out == FCT_CACHE_SIZE){
      sbp->chead.cache.out = 0;
    }

    sbp->cfree--;
   	
   	sbSave();

    

}
