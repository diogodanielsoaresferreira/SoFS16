/**
 *  \brief Display the contents of a block as:
 *
 *      \li as hexadecimal data
 *      \li as ascii data
 *      \li as superblock data
 *      \li as inode data
 *      \li as cluster references
 *      \li as directory entry.
 *
 *  \author Artur Pereira - 2007-2009, 2016
 *  \author Miguel Oliveira e Silva - 2009
 *  \author António Rui Borges - 2010-2015
 */

#ifndef __SOFS16_BLOCKVIEWS__
#define __SOFS16_BLOCKVIEWS__

#include <stdint.h>

#include "inode.h"

/**
 *  \brief Display block as hexadecimal data.
 *
 *  \param buf pointer to a buffer with block contents
 *  \param off offset for the labels
 */
void printBlockAsHex(void *buf, uint32_t off = 0x0);

/**
 *  \brief Display cluster as ASCII data.
 *
 *  \param buf pointer to a buffer with block contents
 *  \param off offset for the labels
 */
void printBlockAsAscii(void *buf, uint32_t off = 0x0);

/**
 *  \brief Display the block contents as superblock data.
 *
 *  \param buf pointer to a buffer with block contents
 */
void printSuperBlock(void *buf);

/**
 *  \brief Display the inode data.
 *
 *  \param p_inode pointer to a buffer with inode contents
 *  \param nInode inode number
 */
void printInode(void *buf, uint32_t nInode);

/**
 *  \brief Display the block contents as inode data.
 *
 *  \param buf pointer to a buffer with block contents
 *  \param off offset for the labels
 */
void printBlockOfInodes(void *buf, uint32_t off = 0x0);

/**
 *  \brief Display the block contents as direntry data.
 *
 *  \param buf pointer to a buffer with block contents
 *  \param off offset for the labels
 */
void printBlockOfDirents(void *buf, uint32_t off = 0x0);

/**
 *  \brief Display the block contents as reference data.
 *
 *  \param buf pointer to a buffer with block contents
 *  \param off offset for the labels
 */
void printBlockOfRefs(void *buf, uint32_t off = 0x0);

#endif                          /* __SOFS16_BLOCKVIEWS__ */
