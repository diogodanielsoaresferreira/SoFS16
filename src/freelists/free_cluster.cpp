/**
 *  \author Ana Patrícia Gomes da Cruz
 *  \tester Ana Patrícia Gomes da Cruz
 */

#include "freelists.h"

#include "probing.h"
#include "exception.h"
#include "dealers.h"
#include <errno.h>
#include <inttypes.h>


/*
 * Dictates to be obeyed by the implementation:
 * - parameter cn must be validated, 
 *      throwing a proper error if necessary
 */
void soFreeCluster(uint32_t cn)
{
    soProbe(732, "soFreeCluster (%u)\n", cn);

	SOSuperBlock *psb;

	/*Get a pointer to the superblock */
	psb = sbGetPointer();


	if(cn <0 || cn >= psb->ctotal)
		throw SOException(EINVAL, __FUNCTION__);

	/*Verify if cache is full*/
	if(psb->ctail.cache.ref[psb->ctail.cache.in] != NULL_REFERENCE){
		soDeplete();
	}
	
	psb->ctail.cache.ref[psb->ctail.cache.in] = cn;
	psb->ctail.cache.in = (psb->ctail.cache.in + 1) % FCT_CACHE_SIZE;
	
	//if(psb->cfree < psb->ctotal)
	psb->cfree++;

	/*Save superblock*/
	sbSave();
}