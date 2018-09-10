/**
 *  \file rawdisk.h
 *  \brief Access to raw disk blocks and clusters.
 *
 *  The storage device is presently a Linux file which simulates a magnetic disk.
 *  The following operations are defined:
 *    \li open a communication channel with the storage device
 *    \li close the communication channel previously established
 *    \li read a block of data from the storage device
 *    \li write a block of data to the storage device
 *    \li read a cluster of data from the storage device
 *    \li write a cluster of data to the storage device.
 *
 *  \author Artur Carneiro Pereira - 2007-2009, 2016
 *  \author Miguel Oliveira e Silva - 2009
 *  \author António Rui Borges - 2010-2015
 *
 *  \remarks In case an error occurs, every function throws an error code (an int)
 */

#ifndef __SOFS16_RAWDISK__
#define __SOFS16_RAWDISK__

#include <stdint.h>
#include <stdlib.h>

/* ***************************************** */

/** \brief block size (in bytes) */
#define BLOCK_SIZE 512U

/* ***************************************** */

/**
 *  \brief Open the storage device.
 *
 *  A communication channel is established with the storage device.
 *  It is supposed that no communication channel was previously established.
 *  The Linux file that simulates the storage device must exist and
 *  have a size multiple of the block size.
 *
 *  \param devname absolute path to the Linux file that simulates the storage device
 *  \param np if not null, 
 *      pointer to a location where the number of blocks of the device is to be stored
 *
 */
void soOpenRawDisk(const char *devname, uint32_t * np = NULL);

/* ***************************************** */

/**
 *  \brief Close the storage device.
 *
 *  The communication channel previously established with the storage device is closed.
 */
void soCloseRawDisk(void);

/* ***************************************** */

/**
 *  \brief Read a block of data from the storage device.
 *
 *  \param n physical number of the data block to be read from
 *  \param buf pointer to the buffer where the data must be read into
 */
void soReadRawBlock(uint32_t n, void *buf);

/* ***************************************** */

/**
 *  \brief Write a block of data from the storage device.
 *
 *  \param n physical number of the block to be written into
 *  \param buf pointer to the buffer containing the data to be written from
 */
void soWriteRawBlock(uint32_t n, void *buf);

/* ***************************************** */

/**
 *  \brief Read a cluster of data from the storage device.
 *
 *  \param n physical number of the first block of the data cluster to be read from
 *  \param buf pointer to the buffer where the data must be read into
 *  \param csize number of blocks of a cluster
 */
void soReadRawCluster(uint32_t n, void *buf, uint32_t csize);

/* ***************************************** */

/**
 *  \brief Write a cluster of data from the storage device.
 *
 *  \param n physical number of the first block of the data cluster to be written into
 *  \param buf pointer to the buffer containing the data to be written from
 *  \param csize number of blocks of a cluster
 */
void soWriteRawCluster(uint32_t n, void *buf, uint32_t csize);

/* ***************************************** */

#endif                          /* __SOFS16_RAWDISK__ */
