/*
	\author Diogo Daniel Soares Ferreira
	\tester Diogo Daniel Soares Ferreira
*/

#include "dealers.h"
#include "errno.h"
#include "exception.h"
#include <stdint.h>

static int open = 0;

void soOpenDealersDisk(const char *devname, uint32_t * ntp){
	
	soProbe(814, "soOpenDealersDisk(%s, %d)\n", devname, ntp);

	soOpenRawDisk(devname, ntp);
	soOpenSuperblockDealer();
	soOpenClusterZoneDealer();
	soOpenInodeTableDealer();
	open = 1;
}
void soCloseDealersDisk(){

	soProbe(815, "soCloseDealersDisk()\n");

	if(open == 0)
		throw SOException(ENODEV, __FUNCTION__);
	open = 0;
	soCloseSuperblockDealer();
	soCloseClusterZoneDealer();
	soCloseInodeTableDealer();
	soCloseRawDisk();
}