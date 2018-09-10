/**
 *  \author Pedro Marques Ferreira da Silva
 *  \tester Pedro Marques Ferreira da Silva
  */


#include "filecluster.h"

#include "probing.h"
#include "exception.h"
#include "inode.h"
#include "cluster.h"
#include "dealers.h"

#include <errno.h>
#include <stdint.h>

static void soGetIndirectFileCluster(SOInode * ip, uint32_t fcn, uint32_t * cnp);
static void soGetDoubleIndirectFileCluster(SOInode * ip, uint32_t fcn, uint32_t * cnp);


/* ********************************************************* */

void soGetFileCluster(int ih, uint32_t fcn, uint32_t * cnp)
{
	soProbe(600, "soGetFileCluster(%d, %u, %p)\n", ih, fcn, cnp);

  uint32_t rpc = soGetRPC();

  SOInode *inode = iGetPointer(ih);

  // checks if file cluster number is valid or out of range
  if((fcn < 0) || (fcn > (N_DIRECT + N_INDIRECT*rpc + rpc*rpc))){
    throw SOException(EINVAL, __FUNCTION__);
  }

  // depending on the fcn there are: direct, single indirect and double indirect references
  // necessary internal function will be called to assist
  
  if(fcn < N_DIRECT){
   	//if it is direct reference;
  	*cnp = inode->d[fcn];
  }
  else if(fcn < (N_DIRECT + N_INDIRECT*rpc)){
  	soGetIndirectFileCluster(inode, fcn, cnp);
  }
  else{
  	soGetDoubleIndirectFileCluster(inode, fcn, cnp);
  }

}

static void soGetIndirectFileCluster(SOInode * ip, uint32_t afcn, uint32_t * cnp)
{
  soProbe(600, "soGetIndirectFileCluster(%p, %u, %p)\n", ip, afcn, cnp);

  uint32_t rpc = soGetRPC();
  uint32_t ref[rpc];
   	
  // cluster index in the cluster of direct references(afcn - N_DIRECT)
  uint32_t number_ind = afcn - N_DIRECT;
   	
  /* Indirect cluster index */
  uint32_t indirect_cluster = number_ind/rpc;

  /* Index of cluster on indirect cluster */
  uint32_t index_cluster = number_ind%rpc;

  if(ip->i1[indirect_cluster]==NULL_CLUSTER){
    *cnp = NULL_CLUSTER;
  }
  else{
    soReadCluster(ip->i1[indirect_cluster], &ref);
    *cnp = ref[index_cluster];
  }

}

static void soGetDoubleIndirectFileCluster(SOInode * ip, uint32_t afcn, uint32_t * cnp)
{
  soProbe(600, "soGetDoubleIndirectFileCluster(%p, %u, %p)\n", ip, afcn, cnp);

  uint32_t rpc = soGetRPC();
  uint32_t ref[rpc];

  afcn -= (N_DIRECT+N_INDIRECT*rpc);

  uint32_t index_pos = afcn%rpc;

  uint32_t index_cluster = afcn/rpc;

  //if the table of double indirect references does not exist -> if i2 is NULL then the rest is NULL;
  if(ip->i2==NULL_CLUSTER){
    *cnp = NULL_CLUSTER;
  }
  else{
    soReadCluster(ip->i2, &ref);
    if(ref[index_cluster]==NULL_CLUSTER){
      *cnp = NULL_CLUSTER;
    }
    else{
      soReadCluster(ref[index_cluster], &ref);
      *cnp = ref[index_pos];
    }
  }

}
