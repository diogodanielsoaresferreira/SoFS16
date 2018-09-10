/**
 *  \file filecluster.h
 *  \brief Set of operations to manage file clusters
 *
 *  \author Artur Pereira - 2008-2009, 2016
 *  \author Miguel Oliveira e Silva - 2009
 *  \author António Rui Borges - 2010-2015
 *
 *  \remarks In case an error occurs, every function throws a SOException
 */

#ifndef __SOFS16_FILECLUSTER__
#define __SOFS16_FILECLUSTER__

#include <stdint.h>

/* *************************************************** */
/** \defgroup filecluster filecluster
 * @{
 */
/* *************************************************** */

/**
 * \brief Get the cluster number of a given file cluster
 *
 *  \param ih inode handler
 *  \param fcn file cluster number
 *  \param cnp pointer to the variable where the cluster number must be put
 */
void soGetFileCluster(int ih, uint32_t fcn, uint32_t * cnp);

/* *************************************************** */

/**
 * \brief Associate a cluster to a given file cluster position
 *
 *  \param ih inode handler
 *  \param fcn file cluster number
 *  \param cnp pointer to the variable where the cluster number must be put
 */
void soAllocFileCluster(int ih, uint32_t fcn, uint32_t * cnp);

/* *************************************************** */

/**
 * \brief Free all file clusters from the given position on 
 *
 *  \param ih inode handler
 *  \param ffcn first file cluster number
 */
void soFreeFileClusters(int ih, uint32_t ffcn);

/* *************************************************** */

/**
 *  \brief Read a file cluster.
 *
 *  Data is read from a specific data cluster which is supposed to belong 
 *  to an inode associated to a file (a regular
 *  file, a directory or a symbolic link). 
 *
 *  If the referred file cluster has not been allocated yet, 
 *  the returned data will consist of a byte stream filled with the
 *  character null (ascii code 0).
 *
 *  \param ih inode handler
 *  \param fcn file cluster number
 *  \param buf pointer to the buffer where data must be read into
 */
void soReadFileCluster(int ih, uint32_t fcn, void *buf);

/* *************************************************** */

/**
 *  \brief Write a data cluster.
 *
 *  Data is written into a specific data cluster which is supposed
 *  to belong to an inode associated to a file (a regular
 *  file, a directory or a symbolic link). 
 *
 *  If the referred cluster has not been allocated yet,
 *  it will be allocated now so that the data can be stored as its
 *  contents.
 *
 *  \param ih inode handler
 *  \param fcn file cluster number
 *  \param buf pointer to the buffer containing data to be written
 */
void soWriteFileCluster(int ih, uint32_t fcn, void *buf);

/* *************************************************** */
/** @} */
/* *************************************************** */

#endif                          /* __SOFS16_FILECLUSTER__ */
