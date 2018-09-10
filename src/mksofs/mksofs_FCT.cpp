#include "mksofs.h"
#include "superblock.h"
#include "exception.h"
#include "cluster.h"

#include <errno.h>


/*
 * create the table of references to free data clusters 
 */
void fillInFreeClusterList(SOSuperBlock * p_sb)
{

    /* References per cluster */
    uint32_t rpc = p_sb->csize * RPB;

	/* Reference table -> it has crefs clusters and each cluster has rpc references */
	uint32_t ref[p_sb->crefs][rpc];

	uint32_t i,j, k = p_sb->ctotal-p_sb->cfree;
	for(i=0; i<p_sb->crefs; i++){

		for(j=0; j<rpc; j++){
			if (k>=p_sb->ctotal)
				ref[i][j] = NULL_BLOCK;
			else if (j!=rpc-1){
				ref[i][j] = k++;
			}
			else{
				ref[i][j] = i + 2;
			}
		}
		soWriteRawCluster(i*p_sb->csize+p_sb->czstart+p_sb->csize, &ref[i], p_sb->csize);
	}


}
