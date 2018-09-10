/**
 *  \brief The SOFS15 formatting tool.
 *
 *  \author Artur Pereira - 2007-2009, 2016
 *  \author Miguel Oliveira e Silva - 2009
 *  \author António Rui Borges - 2010-2015
 */

#ifndef __SOFS16_MKSOFS__
#define __SOFS16_MKSOFS__

#include "superblock.h"

void fillInSuperBlock(SOSuperBlock * sbp, const char *name,
                      uint32_t ntotal, uint32_t itotal, uint32_t bpc);

void fillInInodeTable(SOSuperBlock * sbp);

void fillInRootDir(SOSuperBlock * sbp);

void fillInFreeClusterList(SOSuperBlock * sbp);

void resetFreeCluster(SOSuperBlock * sbp);

#endif                          /* __SOFS16_MKSOFS__ */
