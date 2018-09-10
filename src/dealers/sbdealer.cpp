/*
	\author Eduardo Reis Silva
	\tester Eduardo Reis Silva
*/
#include "dealers.h"
#include "superblock.h"
#include "exception.h"
#include "errno.h"

SOSuperBlock psb;
static int open = 0;

void soOpenSuperblockDealer(){

	soProbe(815, "soOpenSuperblockDealer()\n");

	if(open!=1){
		open = 1;
		soReadRawBlock(0,&psb);
	}
}


void soCloseSuperblockDealer(){
	soProbe(816, "soCloseSuperblockDealer()\n");

	if(open == 0)
		throw SOException(ENODEV, __FUNCTION__);
	open = 0;
}


SOSuperBlock *sbGetPointer(){
	soProbe(817, "sbGetPointer()\n");

	if(open == 0)
		throw SOException(ENODEV, __FUNCTION__);
	return &psb;
}


void sbSave(){
	soProbe(818, "sbSave()\n");

	if(open == 0)
		throw SOException(ENODEV, __FUNCTION__);
	soWriteRawBlock(0,&psb);
}

void sbCheckConsistency(){
	soProbe(819, "soCheckConsistency()\n");
}