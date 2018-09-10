/**
 * \file dealers.h
 * \brief start/stop disk dealers
 *
 * \author Artur Pereira - 2016
 */

#ifndef __SOFS16_DEALERS__
#define __SOFS16_DEALERS__

#define NULL_REFERENCE 0xFFFFFFFF
 
/* ***************************************** */

#include "probing.h"
#include "sbdealer.h"
#include "itdealer.h"
#include "czdealer.h"

#include <stdint.h>
#include <stdlib.h>

/**
 * \brief call raw level disk openning function and init dealers
 *
 * \param devname path to the file containing the disk
 * \param ntp if not NULL, pointer to the variable where the total number of
 *      blocks of the disk is to be stored
 */
void soOpenDealersDisk(const char *devname, uint32_t * ntp = NULL);

/* ***************************************** */

/**
 * \brief close dealers and call raw level disk closing function
 */
void soCloseDealersDisk();

/* ***************************************** */

#endif                          /* __SOFS16_DEALERS__ */
