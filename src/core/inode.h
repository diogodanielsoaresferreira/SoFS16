/**
 *  \file inode.h
 *  \brief Definition of the inode data type.
 *
 *  \author Artur Carneiro Pereira - 2008-2009, 2016
 *  \author Miguel Oliveira e Silva - 2009
 *  \author António Rui Borges - 2010-2015
 */

#ifndef __SOFS16_INODE__
#define __SOFS16_INODE__

#include "rawdisk.h"
#include <sys/stat.h>
#include <stdint.h>

/** \brief number of inodes per block */
#define IPB (BLOCK_SIZE / sizeof(SOInode))

/** \brief flag signaling inode is free (it uses the sticky bit) */
#define INODE_FREE (0001000)

/** \brief number of direct references in the inode */
#define N_DIRECT 5

/** \brief number of indirect references in the inode */
#define N_INDIRECT 2

/** \brief Definition of the inode data type. */
struct SOInode
{
    /** \brief inode mode: it stores the file type and permissions.
     *  (see man 2 stat).
     */
    uint16_t mode;
    /** \brief reference count: number of hard links (directory entries) associated to the inode */
    uint16_t refcount;
    /** \brief user ID of the file owner */
    uint32_t owner;
    /** \brief group ID of the file owner */
    uint32_t group;
    /** \brief file size in bytes: */
    uint32_t size;
    /** \brief cluster count: total number of clusters used by the file */
    uint32_t csize;

    /* \brief usage depends on state */
    union
    {
        /** \brief time of last access to file information (only used when inode is in use) */
        uint32_t atime;
        /** \brief next free inode (only used when inode is free) */
        uint32_t next;
    };
    /** \brief time of last change to inode information */
    uint32_t ctime;
    /** \brief time of last change to file information */
    uint32_t mtime;

   /** \brief direct references to the clusters that comprise the file information content */
    uint32_t d[N_DIRECT];
   /** \brief reference to clusters that extend the d array */
    uint32_t i1[N_INDIRECT];
   /** \brief reference to a cluster that extends the i1 array */
    uint32_t i2;
};

#endif                          /* __SOFS16_INODE__ */
