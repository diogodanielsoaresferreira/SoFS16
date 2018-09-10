/**
 *  \author Diogo Daniel Soares Ferreira
 *  \tester Diogo Daniel Soares Ferreira
 */

#include "filecluster.h"

#include "freelists.h"
#include "dealers.h"
#include "probing.h"
#include "exception.h"


#include <errno.h>


void soWriteFileCluster(int ih, uint32_t fcn, void *buf)
{
    soProbe(600, "soWriteFileCluster(%d, %u, %p)\n", ih, fcn, buf);


    uint32_t cn;

    soGetFileCluster(ih, fcn, &cn);

    // If is not allocated, allocate
    if(cn==NULL_REFERENCE)
    	soAllocFileCluster(ih, fcn, &cn);

	soWriteCluster(cn, buf);

}
