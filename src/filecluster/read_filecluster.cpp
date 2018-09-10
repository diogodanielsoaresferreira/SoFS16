/**
 *  \author João Maia
 *  \tester João Maia
 */

#include "filecluster.h"

#include "probing.h"
#include "exception.h"

#include <errno.h>
#include <dealers.h>

void soReadFileCluster(int ih, uint32_t fcn, void *buf)
{	
    soProbe(600, "soReadFileCluster(%d, %u, %p)\n", ih, fcn, buf);

	uint32_t cn;

	soGetFileCluster(ih,fcn,&cn);

	if(cn == NULL_REFERENCE){
		memset(buf,'\0', soGetBPC());
	}else{
		soReadCluster(cn,buf);
	}
}
