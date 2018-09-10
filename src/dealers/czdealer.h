/**
 *  \file czdealer.h
 *  \brief clusters dealer: mediates access to the diak cluster zone
 *
 *  This module provides functions to access the cluster zone,
 *  using the logical number of the cluster.
 *
 *  \remarks In case an error occurs, every function throws a SOException
 *
 *  \author Artur Pereira - 2016
 */

#ifndef __SOFS16_CZDEALER__
#define __SOFS16_CZDEALER__

#include <stdint.h>

/* ***************************************** */

/** \brief Open the cluster zone dealer
 *
 * Prepare the internal data structure for the cluster zone dealer
 */
void soOpenClusterZoneDealer();

/* ***************************************** */

/**
 * \brief Close the cluster zone dealer
 *
 * 
 */
void soCloseClusterZoneDealer();

/* ***************************************** */

/**
 *  \brief Read a cluster of data from the storage device.
 *
 *  \param n the logical number of the cluster to be read from
 *  \param buf pointer to the buffer where the data must be read into
 */
void soReadCluster(uint32_t n, void *buf);

/* ***************************************** */

/**
 *  \brief Write a cluster of data to the storage device.
 *
 *  \param n the logical number of the cluster to be written into
 *  \param buf pointer to the buffer containing the data to be written from
 */
void soWriteCluster(uint32_t n, void *buf);

/* ***************************************** */

/**
 * \brief retrieve the number of bytes per cluster
 */
uint32_t soGetBPC();

/* ***************************************** */

/**
 * \brief retrieve the number of references per cluster
 */
uint32_t soGetRPC();

/* ***************************************** */

/**
 * \brief retrieve the number of direntries per cluster
 */
uint32_t soGetDPC();

/* ***************************************** */

/**
 * \brief retrieve the maximum number of bytes a file can comprise
 */
uint32_t soGetMaxFileSize();

/* ***************************************** */

#endif                          /* __SOFS16_CZDEALER__ */
