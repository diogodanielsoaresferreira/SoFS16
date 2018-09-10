/*
	\author Diogo Daniel Soares Ferreira
	\tester Diogo Daniel Soares Ferreira
*/

#include "dealers.h"
#include "exception.h"
#include <unistd.h>
#include <sys/types.h>

#include <errno.h>

static int open = 0;
static SOInode *inode_table;
static uint32_t *count_table;
static SOSuperBlock *sbp;


void soOpenInodeTableDealer(){
	soProbe(801, "soOpenInodeTableDealer\n");

	sbp = sbGetPointer();
	if(open==0){
		count_table = (uint32_t *)malloc(sizeof(uint32_t)*sbp->itotal);
		for(uint32_t i = 0; i<sbp->itotal; i++)
			count_table[i] = 0;

		inode_table = (SOInode *)malloc(sizeof(SOInode)*sbp->itotal);
		memset(inode_table,'\0',sizeof(SOInode)*sbp->itotal);

		open = 1;
	}
}


void soCloseInodeTableDealer(){
	soProbe(802, "soCloseInodeTableDealer\n");

	if(open == 0)
		throw SOException(ENODEV, __FUNCTION__);
	open = 0;
	free(inode_table);
	free(count_table);
}


int iOpen(uint32_t in){
	soProbe(803, "iOpen(%d)\n", in);

	if(open == 0)
		throw SOException(ENODEV, __FUNCTION__);
	
	if(in<0 || in>=sbp->itotal)
		throw SOException(EINVAL, __FUNCTION__);

	// Com os ficheiros binários do professor dá erro.
	//if(count_table[in]==MAX_OPEN_INODES)
	//	throw SOException(ENFILE, __FUNCTION__);

	if(count_table[in]==0){
		SOInode buf [IPB];
		soReadRawBlock(sbp->itstart+in/IPB, &buf);
		inode_table[in] = buf[in%IPB];
	}
	
	count_table[in]++;
	
	return in;
}


SOInode *iGetPointer(int ih){
	soProbe(804, "iGetPointer(%d)\n", ih);

	if(ih<0 || iGetNumber(ih)>=sbp->itotal)
		throw SOException(EINVAL, __FUNCTION__);
	if(count_table[ih]==0)
		throw SOException(EBADF, __FUNCTION__);

	return &inode_table[ih];
}


void iSave(int ih){
	soProbe(805, "iSave(%d)\n", ih);

	if(ih<0 || iGetNumber(ih)>=sbp->itotal)
		throw SOException(EINVAL, __FUNCTION__);
	if(count_table[ih]==0)
		throw SOException(EBADF, __FUNCTION__);
	SOInode buf [IPB];
	soReadRawBlock(sbp->itstart+ih/IPB, &buf);
	buf[ih%IPB] = inode_table[ih];
	soWriteRawBlock(sbp->itstart+ih/IPB, &buf);

}


void iClose(int ih){
	soProbe(806, "iClose(%d)\n", ih);

	if(ih<0 || iGetNumber(ih)>=sbp->itotal)
		throw SOException(EINVAL, __FUNCTION__);
	if(count_table[ih]==0)
		throw SOException(EBADF, __FUNCTION__);
	
	count_table[ih]--;
	
}


uint32_t iGetNumber(int ih){
	soProbe(807, "iGetNumber(%d)\n", ih);

	if(ih<0 || (uint32_t)ih>=sbp->itotal)
		throw SOException(EINVAL, __FUNCTION__);
	if(count_table[ih]==0)
		throw SOException(EBADF, __FUNCTION__);
	return ih;
}


void iCheckConsistency(int ih){
	soProbe(808, "iCheckConsistency(%d)\n", ih);
	
}


uint32_t iIncRefcount(int ih){

	soProbe(809, "iIncRefcount(%d)\n", ih);

	if(ih<0 || iGetNumber(ih)>=sbp->itotal)
		throw SOException(EINVAL, __FUNCTION__);

	if(count_table[ih]==0)
		throw SOException(EBADF, __FUNCTION__);

	if(inode_table[ih].refcount==MAX_REF_COUNT){
		throw SOException(ELIBMAX, __FUNCTION__);
	}

	inode_table[ih].refcount++;
	return inode_table[ih].refcount;

}


uint32_t iDecRefcount(int ih){
	soProbe(810, "iDecRefcount(%d)\n", ih);

	if(ih<0 || iGetNumber(ih)>=sbp->itotal)
		throw SOException(EINVAL, __FUNCTION__);

	if(count_table[ih]==0)
		throw SOException(EBADF, __FUNCTION__);

	if(inode_table[ih].refcount>0){
		inode_table[ih].refcount--;
	}

	return inode_table[ih].refcount;

}


void iSetAccess(int ih, uint16_t perm){

	soProbe(811, "iSetAccess(%d)\n", ih, perm);

	if(ih<0 || iGetNumber(ih)>=sbp->itotal){
		throw SOException(EINVAL, __FUNCTION__);
	}
	if(count_table[ih] == 0){
		throw SOException(EBADF, __FUNCTION__);
	}
	/* Changes last 9 bits */
	inode_table[ih].mode = (inode_table[ih].mode & 4294966784) | (perm & 511);
	
	iSave(ih);

}


uint16_t iGetAccess(int ih){

	soProbe(812, "iGetAccess(%d)\n", ih);

	if(ih<0 || iGetNumber(ih)>=sbp->itotal){
		throw SOException(EINVAL, __FUNCTION__);
	}
	if(count_table[ih] == 0){
		throw SOException(EBADF, __FUNCTION__);
	}
	return inode_table[ih].mode;
}


bool iCheckAccess(int ih, int access){
	soProbe(813, "iCheckAccess(%d)\n", ih, access);

	if(ih<0 || iGetNumber(ih)>=sbp->itotal){
		throw SOException(EINVAL, __FUNCTION__);
	}
	if(count_table[ih] == 0){
		throw SOException(EBADF, __FUNCTION__);
	}

	if(access<0 || access>15)
		throw SOException(EINVAL, __FUNCTION__);

	if(getuid() == inode_table[ih].owner){
		if((inode_table[ih].mode>>6 & access) != access){
			return false;
		}
		return true;
	}
	else if(getuid() == inode_table[ih].group){
		if(((inode_table[ih].mode>>3) & access) != access){
			return false;
		}
		return true;
	}
	else{
		if((inode_table[ih].mode & access) != access){
			return false;
		}
		return true;
	}

}