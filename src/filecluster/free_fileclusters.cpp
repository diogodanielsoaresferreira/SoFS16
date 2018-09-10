/**
 *  \author Ana Patrícia Gomes da Cruz
 *  \tester Ana Patrícia Gomes da Cruz
 */

#include "filecluster.h"
#include "freelists.h"
#include "probing.h"
#include "exception.h"
#include "dealers.h"
#include <errno.h>
#include <stdint.h>

static int soFreeIndirectFileClusters(SOInode * ip, uint32_t ffcn, uint32_t* ptotal_clusters);
static int soFreeDoubleIndirectFileClusters(SOInode * ip, uint32_t ffcn, uint32_t* ptotal_clusters);

/* ********************************************************* */

void soFreeFileClusters(int ih, uint32_t ffcn)
{
    soProbe(600, "soFreeFileClusters(%d, %u)\n", ih, ffcn);

    SOInode *inode;

    inode = iGetPointer(ih);
    uint32_t RPC = soGetRPC();
    uint32_t total = inode->csize;

    if((ffcn < 0) || (ffcn > (N_DIRECT + N_INDIRECT*RPC + RPC*RPC)))
    	return;

    for(uint32_t i=0; i<N_DIRECT; i++)
    {   
        /* If there is no clusters ahead, return */
        if(total==0)
            return;

        /* If cluster exists */
    	if(inode->d[i] != NULL_REFERENCE){
            total--;
            /* If cluster is to be freed */
            if(i>=ffcn){
                soFreeCluster(inode->d[i]);
                inode->d[i] = NULL_REFERENCE;
                inode->csize--;
            }
        }
    }
    
    /* If there is no clusters ahead, return */
    if(total==0)
        return;

    if(ffcn < (N_DIRECT + N_INDIRECT*RPC)){
        soFreeIndirectFileClusters(inode, ffcn, &total);
        
    }
    if(total==0)
        return;

    soFreeDoubleIndirectFileClusters(inode, ffcn, &total);
    
    
}

/* ********************************************************* */

static int soFreeIndirectFileClusters(SOInode * ip, uint32_t ffcn, uint32_t* ptotal_clusters)
{
    soProbe(600, "soFreeIndirectFileClusters(%p, %u)\n", ip, ffcn);

    uint32_t RPC = soGetRPC();
    uint32_t ref[RPC], erase;

    /* For all the Indirect arrays */
    for(uint32_t i=0; i<N_INDIRECT; i++){
        while(ip->i1[i]==NULL_REFERENCE && i<N_INDIRECT){
                i++;
        }
        if(i==N_INDIRECT) break;
        erase = 1;
        soReadCluster(ip->i1[i], &ref);
        
        /* For all the rpc */
        for(uint32_t j=0; j<RPC; j++){
            
            /* If there is only one cluster left, is itself. */
            if((*ptotal_clusters)<=1){
                if(erase){
                    soFreeCluster(ip->i1[i]);
                    ip->csize--;
                    ip->i1[i] = NULL_REFERENCE;
                    (*ptotal_clusters)--;
                }
                soWriteCluster(ip->i1[i], &ref);
                return -1;
            }

            /* If position j of ref is filled */
            if(ref[j]!=NULL_REFERENCE){
                (*ptotal_clusters)--;
                /* If cluster is to be freed */
                if(N_DIRECT>=ffcn || (i*RPC)+j>=ffcn-N_DIRECT){
                    soFreeCluster(ref[j]);
                    ip->csize--;
                    ref[j] = NULL_REFERENCE;
                }
                /* Cluster is not empty */
                else{
                    /* Do NOT erase cluster */
                    erase = 0;
                }
            }
        }
        
        soWriteCluster(ip->i1[i], &ref);

        if(erase){
            ip->csize--;
            soFreeCluster(ip->i1[i]);
            ip->i1[i] = NULL_REFERENCE;
            (*ptotal_clusters)--;
            return -1;
        }


    }


    return 0;
}

/* ********************************************************* */

static int soFreeDoubleIndirectFileClusters(SOInode * ip, uint32_t ffcn, uint32_t* ptotal_clusters)
{
    soProbe(600, "soFreeDoubleIndirectFileClusters(%p, %u)\n", ip, ffcn);

    uint32_t RPC = soGetRPC();
    uint32_t ref[RPC], ref2[RPC], j, i, erase, erase_2;

    if(ip->i2==NULL_REFERENCE)
        return 0;

    erase = 1;
    soReadCluster(ip->i2, &ref);
    for(i=0; i<RPC; i++){
        while(ref[i]==NULL_REFERENCE && i<RPC){
            i++;
        }
        if(i==RPC) break;
        soReadCluster(ref[i], &ref2);

        erase_2 = 1;
        
        for(j=0; j<RPC; j++){
            while(ref2[j]==NULL_REFERENCE && i<RPC){
                j++;
            }
            if(j==RPC) break;

            /* If there are only two clusters left, is itself and the parent. */
            if((*ptotal_clusters)<=2){
                if(erase_2){
                    soFreeCluster(ref[i]);
                    ip->csize--;
                    ref[i] = NULL_REFERENCE;
                    (*ptotal_clusters)--;
                    if(erase){
                        soFreeCluster(ip->i2);
                        ip->csize--;
                        ip->i2 = NULL_REFERENCE;
                        (*ptotal_clusters)--;
                    }
                }
                soWriteCluster(ref[i], &ref2);
                soWriteCluster(ip->i2, &ref);
                return -1;
            }

            /* If position j of ref2 is filled */
            if(ref2[j]!=NULL_REFERENCE){
                (*ptotal_clusters)--;
                /* If cluster is to be freed */
                if(N_DIRECT+N_INDIRECT*RPC>=ffcn || (i*RPC)+j>=ffcn-N_DIRECT-N_INDIRECT*RPC){
                    soFreeCluster(ref2[j]);
                    ip->csize--;
                    ref2[j] = NULL_REFERENCE;
                }
                /* Cluster is not empty */
                else{
                    /* Do NOT erase cluster */
                    erase_2=0;
                }
            }
        }
        // Write on c2
        soWriteCluster(ref[i], &ref2);
        if(erase_2){
            ip->csize--;
            soFreeCluster(ref[i]);
            ref[i] = NULL_REFERENCE;
            (*ptotal_clusters)--;
        }
        else{
            erase = 0;
        }
    }
    // Write on cluster
    soWriteCluster(ip->i2, &ref);
    if(erase){
        ip->csize--;
        soFreeCluster(ip->i2);
        ip->i2 = NULL_REFERENCE;
        (*ptotal_clusters)--;
        return -1;
    }


    return 0;
}

/* ********************************************************* */