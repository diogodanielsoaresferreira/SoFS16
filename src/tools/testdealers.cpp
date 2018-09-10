#include "dealers.h"
#include <stdio.h>

int main(){


	soOpenDealersDisk("zzz");
	/* Cluster Zone */
	
	

	int32_t dir [soGetRPC()];

	soCloseClusterZoneDealer();
	soOpenClusterZoneDealer();
	soOpenClusterZoneDealer();
	//soCloseClusterZoneDealer();
	soCloseClusterZoneDealer();
	soOpenClusterZoneDealer();
	soCloseClusterZoneDealer();
	soOpenClusterZoneDealer();
	printf("%d\n", soGetBPC());
	printf("%d\n", soGetDPC());
	printf("%d\n", soGetRPC());

	soReadCluster(100, &dir);

	for(uint32_t i=0;i<soGetRPC(); i++)
		printf("%d\n",dir[i]);
	soWriteCluster(100, &dir);
	printf("MFS:%d\n", soGetMaxFileSize());

	/* Superblock Zone */
	soOpenSuperblockDealer();
	soCloseSuperblockDealer();
	soOpenSuperblockDealer();
	soOpenSuperblockDealer();
	soCloseSuperblockDealer();
	soOpenSuperblockDealer();
	SOSuperBlock* sbp = sbGetPointer();
	printf("CSIZE:%d\n",sbp->csize);
	sbp->csize++;
	sbSave();
	sbp = sbGetPointer();
	printf("CSIZE:%d\n", sbp->csize);


	/* Inode zone */
	
	soOpenInodeTableDealer();
	soOpenInodeTableDealer();
	soCloseInodeTableDealer();
	soOpenInodeTableDealer();
	
	uint32_t i = iOpen(0);
	uint32_t i2 = iOpen(1);


	SOInode *inode = iGetPointer(i);
	SOInode *inode2 = iGetPointer(i2);

	printf("REFACC:%d\n", inode->refcount);
	iIncRefcount(i);
	//iSave(i);
	printf("REFACC1:%d\n", inode->refcount);
	printf("REFACC2:%d\n", inode2->refcount);
	iDecRefcount(i);
	printf("REFACC:%d\n", inode2->refcount);
	iDecRefcount(i);
	//iClose(i);
	//iClose(i);
	printf("REFACC1:%d\n", inode->refcount);
	//iSave(i);
	printf("ACC:%d\n", iGetAccess(i));
	iSetAccess(i, 2);
	printf("ACC:%d\n", iGetAccess(i));
	printf("CA:%d\n", iCheckAccess(i, 1));
	printf("CA:%d\n", iCheckAccess(i2, 2));

	soCloseDealersDisk();
}