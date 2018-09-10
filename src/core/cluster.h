/**
 *  \file cluster.h
 *  \brief Cluster related stuff
 *
 *
 *
 *  \author António Rui Borges - 2012-2015
 *  \author Artur Pereira - 2016
 */

#ifndef __SOFS16_CLUSTER__
#define __SOFS16_CLUSTER__

#include <stdint.h>

#include "rawdisk.h"

/** \brief number of references per block */
#define RPB (BLOCK_SIZE / sizeof (uint32_t))


/** \brief reference to a null cluster */
#define NULL_CLUSTER ((uint32_t)(~0UL))


#endif                          /* __SOFS16_CLUSTER__ */
