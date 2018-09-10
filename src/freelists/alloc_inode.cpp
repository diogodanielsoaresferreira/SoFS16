/**
 *  \author Nuno Capela
 *  \tester Nuno Capela
 */

#include "freelists.h"

#include "dealers.h"
#include "probing.h"
#include "exception.h"

#include <errno.h>
#include <unistd.h>
#include <inttypes.h>
#include <time.h>


/*
 * Dictates to be obeyed by the implementation:
 * - error ENOSPC should be thrown if there is no free inodes
 * - the allocated inode must be properly initialized
 */
void soAllocInode(uint32_t type, uint32_t * inp)
{
  soProbe(711, "soAllocInode(%u, %p)\n", type, inp);

  uint32_t i;

  SOSuperBlock *sbp = sbGetPointer();

  if(sbp->ifree == 0){
    throw SOException(ENOSPC, __FUNCTION__);
  }

  sbp->ifree--;

  uint32_t inode_handler = iOpen(sbp->ihead);

  SOInode* inode = iGetPointer(inode_handler);

  *inp = sbp->ihead;
  sbp->ihead = inode->next;

  inode->mode = type;
  inode->refcount = 0;
  inode->owner = getuid(); 
  inode->group = getgid();
  inode->size = 0;
  inode->csize = 0;
  inode->ctime = time(NULL);
  inode->mtime = time(NULL);
  inode->atime = time(NULL);

  for(i = 0; i < N_DIRECT; i++){
    inode->d[i] = NULL_REFERENCE; /*put all direct and indirect references to the data clusters pointing null*/
  }
  for(i = 0; i < N_INDIRECT; i++){
      inode->i1[i] = NULL_REFERENCE; /*put all direct and indirect references to the data clusters pointing null*/
  }
  inode->i2 = NULL_REFERENCE;

  iSave(inode_handler);
  sbSave();
  iClose(inode_handler);
}
