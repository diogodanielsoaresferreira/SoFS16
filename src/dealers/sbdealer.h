/**
 *  \file sbdealer.h
 *  \brief superblock dealer: mediates access to the disk superblock
 *
 *  This module guarantees that only a single copy of the superblock is in memory,
 *  thus improving its consistensy.
 *
 *  \remarks In case an error occurs, every function throws a SOException
 *
 *  \author Artur Pereira - 2016
 */

#ifndef __SOFS16_SBDEALER__
#define __SOFS16_SBDEALER__

#include "dealers.h"

#include "superblock.h"

#include <stdint.h>



/* ***************************************** */

/**  
 * \brief Open the superblock dealer
 *
 * Prepare the internal data structure of the superblock dealer
 */
void soOpenSuperblockDealer();

/* ***************************************** */

/**  
 * \brief Close the superblock dealer
 *
 * Save superblock to disk and close dealer
 */
void soCloseSuperblockDealer();

/* ***************************************** */

/**
 * \brief Get a pointer to the superblock
 *
 * \return Pointer to superblock
 */
SOSuperBlock *sbGetPointer();

/* ***************************************** */

/**
 * \brief Save superblock to disk
 */
void sbSave();

/* ***************************************** */

/**
 * \brief Check superblock consistency 
 *
 * \remark To be implemented
 */
void sbCheckConsistency();

/* ***************************************** */

#endif /*__SOFS16_SBDEALER___ */
