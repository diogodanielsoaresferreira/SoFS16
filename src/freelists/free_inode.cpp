/**
 *  \author João Maia
 *  \tester João Maia
 */

#include "freelists.h"

#include "dealers.h"

#include "probing.h"
#include "exception.h"

#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <inttypes.h>


/*
 * Dictates to be obeyed by the implementation:
 * - parameter in must be validated, 
 *      throwing a proper error if necessary
 */

void soFreeInode(uint32_t in)
{
	soProbe(712, "soFreeInode (%u)\n", in);

    
	uint32_t i, handler_tail, handler_inode;
    SOSuperBlock *sbp = sbGetPointer();

    if (in<1 || in>sbp->itotal){
        throw SOException(EINVAL, __FUNCTION__);
    }

    handler_inode = iOpen(in);
    SOInode *inode = iGetPointer(handler_inode);

    if((inode->mode & INODE_FREE)==INODE_FREE)
        return;

    if(sbp->itail==NULL_REFERENCE){
        sbp->itail=in;
        sbp->ihead=in;
    }

    handler_tail = iOpen(sbp->itail);
    
    SOInode *tail_inode = iGetPointer(handler_tail);
    tail_inode->next = in;
    iSave(handler_tail);

    
    //increment number of free inodes
    sbp->ifree ++;

    
    //fill the inode as free
    inode->mode |= INODE_FREE; //put the flag signaling the inode in is free
    inode->refcount = 0;
    inode->owner = getuid();
    inode->group = getuid();
    inode->size = 0;
    inode->csize = 0;
    inode->next = NULL_REFERENCE; //put the reference to next pointing to NULL
    inode->ctime = time(NULL);
    inode->mtime = time(NULL);
    for(i = 0; i<N_DIRECT; i++)
    	inode->d[i] = NULL_REFERENCE; //put all direct references to the data clusters pointing null
    for(i = 0; i<N_INDIRECT; i++)
    	inode->i1[i] = NULL_REFERENCE; //put all indirect references to the data clusters pointing NULL
    inode->i2 = NULL_REFERENCE; //put the reference to the cluster that extends i1 array pointing NULL
    
    sbp->itail = in;
    iSave(handler_inode);
    if (in==NULL_REFERENCE)
        sbp->ihead = NULL_REFERENCE;
    sbSave();
    iClose(handler_tail);
    iClose(handler_inode);
}
