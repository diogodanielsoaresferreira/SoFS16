/**
 *  \file freelists.h
 *  \brief Set of operations to manage the list of free inodes 
 *      and the list of free clusters
 *
 *  The operations are:
 *      \li allocate a free inode
 *      \li free the referenced inode
 *      \li allocate a free cluster
 *      \li free the referenced cluster.
 *
 *  \author Artur Pereira 2008-2009, 2016
 *  \author Miguel Oliveira e Silva 2009
 *  \author António Rui Borges - 2010-2015
 *
 *  \remarks In case an error occurs, every function throws a SOException
 */

#ifndef __SOFS16_FREELISTS__
#define __SOFS16_FREELISTS__

#include <stdint.h>

/* *************************************************** */
/** \defgroup freelists freelists
 * @{
 */
/* *************************************************** */

/**
 *  \brief Allocate a free inode.
 *
 *  An inode is retrieved from the list of free inodes, marked in use, 
 *  associated to the legal file type passed as
 *  a parameter and is generally initialized. 
 *
 *  \param type the inode type (it must represent either a file, or a directory, or a symbolic link)
 *  \param inp pointer to the location where the number of the just allocated inode is to be stored
 */
void soAllocInode(uint32_t type, uint32_t * inp);

/* *************************************************** */

/**
 *  \brief Free the referenced inode.
 *
 *  The inode is inserted into the list of free inodes.
 *
 *  \param in number of the inode to be freed
 */
void soFreeInode(uint32_t in);

/* *************************************************** */

/**
 *  \brief Allocate a free cluster.
 *
 *  A cluster is retrieved from the list of free clusters. 
 *
 *  \param cnp pointer to the location where the number of the allocated cluster is to be stored
 */
void soAllocCluster(uint32_t * cnp);

/* *************************************************** */

/**
 *  \brief Free the referenced cluster.
 *
 *  The cluster is inserted into the list of free clusters.
 *
 *  \param cn number of the cluster to be freed
 */
void soFreeCluster(uint32_t cn);

/* *************************************************** */

/**
 * \brief replenish the head cache
 *
 * If the head cache is empty, it is filled with references retrieved
 *      from the references head cluster or from the tail cache.
 */
void soReplenish();

/* *************************************************** */

/**
 * \brief Deplete the tail cache
 *
 * The references in the tail cache are transferred to the head cache or to
 *      the references tail cluster.
 */
void soDeplete();

/* *************************************************** */
/** @} */
/* *************************************************** */

#endif                          /* __SOFS16_FREELISTS__ */
