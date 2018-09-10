#include "mksofs.h"
#include "superblock.h"
#include "exception.h"
#include "cluster.h"

#include <errno.h>

  /*
   * reset free clusters
   */
void resetFreeCluster(SOSuperBlock * p_sb)
{

    uint32_t cluster_size = BLOCK_SIZE*p_sb->csize;

    char clusters[cluster_size];
    uint32_t i;
    uint32_t fcstart = p_sb->czstart +(1+p_sb->crefs)*p_sb->csize;

    memset(clusters, '\0', cluster_size);

    for (i=fcstart; i<p_sb->ntotal; i+=p_sb->csize){
   	  
      soWriteRawCluster(i, clusters, p_sb->csize);
    }

}
