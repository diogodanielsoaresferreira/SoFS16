/**
 *  \file superblock.h
 *  \brief Definition of the superblock data type.
 *
 *  \author Artur Carneiro Pereira - 2008-2009, 2016
 *  \author Miguel Oliveira e Silva - 2009
 *  \author António Rui Borges - 2010-2015
 */

#ifndef __SOFS16_SUPERBLOCK__
#define __SOFS16_SUPERBLOCK__

#include <stdint.h>

/** \brief sofs15 magic number */
#define MAGIC_NUMBER 0x50F5

/** \brief sofs15 version number */
#define VERSION_NUMBER 0x2016

/** \brief maximum length of volume name */
#define PARTITION_NAME_SIZE 29

/** \brief constant signaling the file system was properly unmounted the last time it was mounted */
#define PRU 0

/** \brief constant signaling the file system was not properly unmounted the last time it was mounted */
#define NPRU 1

/** \brief reference to a null data block */
#define NULL_BLOCK ((uint32_t)(~0UL))

/** \brief size of cache */
#define FCT_CACHE_SIZE  50

/**
 *  \brief Definition of the reference cache data type.
 *
 *  It describes an easy access temporary storage area within the superblock for references to free data clusters.
 */

struct FCTRecord
{
    /** \brief storage area whose elements are the logical numbers of free data clusters */
    struct
    {
        uint32_t ref[FCT_CACHE_SIZE];
        uint32_t in;
        uint32_t out;
    } cache;
    /** \brief number of chead/ctail cluster */
    uint32_t cluster_number;
    /** \brief index of first filled/free position in previous cluster */
    uint32_t cluster_idx;
};

/**
 *  \brief Definition of the superblock data type.
 */
struct SOSuperBlock
{
    /* Header */

    /** \brief magic number - file system identification number (should be MAGIC_NUMBER macro value) */
    uint16_t magic;

    /** \brief version number (should be VERSION_NUMBER macro value) */
    uint16_t version;

    /** \brief volume name */
    char name[PARTITION_NAME_SIZE + 1];

    /** \brief flag signaling if the file system was properly unmounted the last time it was mounted
     *     \li PRU - if properly unmounted
     *     \li NPRU - if not properly unmounted
     */
    uint8_t mstat;

    /** \brief number of blocks per cluster */
    uint8_t csize;

    /** \brief total number of blocks in the device */
    uint32_t ntotal;

    /* Inode table metadata */

    /** \brief physical number of the block where the table of inodes starts */
    uint32_t itstart;
    /** \brief number of blocks that the table of inodes comprises */
    uint32_t itsize;

    /** \brief total number of inodes */
    uint32_t itotal;
    /** \brief number of free inodes */
    uint32_t ifree;

    /** \brief head of linked list of free inodes */
    uint32_t ihead;
    /** \brief tail of linked list of free inodes */
    uint32_t itail;

    /* Cluster zone metadata */

    /** \brief physical number of the block where the cluster zone starts */
    uint32_t czstart;

    /** \brief total number of data clusters */
    uint32_t ctotal;

    /** \brief number of free clusters */
    uint32_t cfree;
    /** \brief number of clusters used by the list of free clusters */
    uint32_t crefs;

    /** \brief head cache of references to free clusters */
    FCTRecord chead;
    /** \brief tail cache of references to free clusters */
    FCTRecord ctail;
};

#endif /*__SOFS16_SUPERBLOCK__ */
