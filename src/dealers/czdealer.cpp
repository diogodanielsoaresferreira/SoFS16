/*
	\author Diogo Daniel Soares Ferreira
	\tester Diogo Daniel Soares Ferreira
*/

#include "dealers.h"

#include "cluster.h"
#include "direntry.h"
#include "exception.h"
#include "errno.h"


static int open = 0;
SOSuperBlock * sbp;
void soOpenClusterZoneDealer(){
	soProbe(820, "soOpenClusterZoneDealer()\n");

	open = 1;
	sbp = sbGetPointer();
}


void soCloseClusterZoneDealer(){
	soProbe(821, "soCloseClusterZoneDealer()\n");
	if(open == 0)
		throw SOException(ENODEV, __FUNCTION__);
	open = 0;
}


void soReadCluster(uint32_t n, void *buf){
	soProbe(822, "soReadCluster(%d, %d)\n", n, buf);
	if(open == 0)
		throw SOException(ENODEV, __FUNCTION__);
	soReadRawCluster(n*sbp->csize+sbp->czstart, buf, sbp->csize);
}


void soWriteCluster(uint32_t n, void *buf){
	soProbe(823, "soWriteCluster(%d, %d)\n", n, buf);

	if(open == 0)
		throw SOException(ENODEV, __FUNCTION__);
	soWriteRawCluster(n*sbp->csize+sbp->czstart, buf, sbp->csize);
}

uint32_t soGetBPC(){
	soProbe(824, "soGetBPC()\n");
	if(open == 0)
		throw SOException(ENODEV, __FUNCTION__);
	return sbp->csize * BLOCK_SIZE;
}


uint32_t soGetRPC(){
	soProbe(825, "soGetRPC()\n");
	if(open == 0)
		throw SOException(ENODEV, __FUNCTION__);
	return sbp->csize * RPB;
}


uint32_t soGetDPC(){
	soProbe(826, "soGetDPC()\n");
	if(open == 0)
		throw SOException(ENODEV, __FUNCTION__);
	return DPB * sbp->csize;
}

uint32_t soGetMaxFileSize(){
	soProbe(827, "soGetMaxFileSize()\n");
	if(open == 0)
		throw SOException(ENODEV, __FUNCTION__);
	return sbp->csize * BLOCK_SIZE * (N_DIRECT+N_INDIRECT*soGetRPC()+soGetRPC()*soGetRPC());
}
