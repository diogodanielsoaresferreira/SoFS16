/**
 *  \file itdealer.h
 *  \brief inodes dealer: mediates access to the disk inode table
 *      and provides functions to manipulate inodes
 *
 *  This module guarantees that only a single copy of every inode is in memory,
 *  thus improving consistency.
 *
 *  \remarks In case an error occurs, every function throws a SOException
 *
 *  \author Artur Pereira - 2016
 */

#ifndef __SOFS16_ITDEALER__
#define __SOFS16_ITDEALER__

#define MAX_OPEN_INODES 100
#define MAX_REF_COUNT 100

#include "dealers.h"

#include "inode.h"

#include <stdint.h>

/* ***************************************** */

/** \brief Open inode table dealer
 *
 * Prepare the internal data structure for the inode table dealer
 */
void soOpenInodeTableDealer();

/* ***************************************** */

/** 
 * \brief Close the inode table dealer
 *
 * Save to disk all openning inodes and close dealer
 */
void soCloseInodeTableDealer();

/* ***************************************** */

/**
 * \brief open inode
 *
 * If inode is already open, just increment usecount;
 * otherwise, transfer the inode from disk and 
 * put usecount at 1.
 *
 * \param in the number of the inode to open
 * \return inode handler
 */
int iOpen(uint32_t in);

/* ***************************************** */

/**
 * \brief get pointer to an open inode
 *
 * A pointer to the SOInode structured where the inode
 * is loaded is returned.
 *
 * \param ih inode handler
 * \return pointer to the inode
 */
SOInode *iGetPointer(int ih);

/* ***************************************** */

/**
 * \brief Save an open inode to disk
 *
 * The inode is not closed.
 *
 * \param ih inode handler
 */
void iSave(int ih);

/* ***************************************** */

/**
 * \brief Close an open inode
 *
 * Decrement usecount of given inode,
 * releasing slot if 0 is reached.
 *
 * \param ih inode handler
 */
void iClose(int ih);

/* ***************************************** */

/**
 * \brief Return the number of the inode associated to the given handler
 * \param ih inode handler
 * \return inode number
 */
uint32_t iGetNumber(int ih);

/* ***************************************** */

/**
 * \brief check inode for consistency
 * \param ih inode handler
 * \remark to be implemented
 */
void iCheckConsistency(int ih);

/* ***************************************** */

/**
 * \brief increment, if possible, reference count of an open inode.
 *
 * The increment fails if max reference count was already rached.
 *
 * \param ih inode handler
 * \return refcount after increment
 */
uint32_t iIncRefcount(int ih);

/* ***************************************** */

/**
 * \brief decrement, if possible, reference count of an open inode
 *
 * The decrement fails if reference count is zero
 *
 * \param ih inode handler
 * \return reference after decrement
 */
uint32_t iDecRefcount(int ih);

/* ***************************************** */

/**
 * \brief set access permissions of an open inode
 * \param ih inode handler
 * \param perm permissions to be set
 */
void iSetAccess(int ih, uint16_t perm);

/* ***************************************** */

/**
 * \brief retrieve access permissions of an open inode
 * \param ih inode handler
 * \return access permissions
 */
uint16_t iGetAccess(int ih);

/* ***************************************** */

/**
 * \brief check an open inode against a requested access
 * \param ih inode handler
 * \param access requested access
 * \return true, for access granted; false for access denied
 */
bool iCheckAccess(int ih, int access);

/* ***************************************** */

#endif                          /* __SOFS16_ITDEALER__ */
