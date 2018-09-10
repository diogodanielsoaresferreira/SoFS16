/**
 *  \file direntry.h
 *  \brief Definition of the directory entry data type.
 *
 *  \author Artur Carneiro Pereira - 2008-2009, 2016
 *  \author Miguel Oliveira e Silva - 2009
 *  \author António Rui Borges - 2010-2015
 */

#ifndef __SOFS16_DIRENTRY__
#define __SOFS16_DIRENTRY__

#include "rawdisk.h"

/** \brief number of direntries per block */
#define DPB     (BLOCK_SIZE / sizeof(SODirEntry))

/** \brief maximum length of a file name (in characters) */
#define SOFS16_MAX_NAME 59

/** \brief Definition of the directory entry data type. */
struct SODirEntry
{
    /** \brief the name of a file (NULL-terminated string) */
    char name[SOFS16_MAX_NAME + 1];
    /** \brief the associated inode number */
    uint32_t in;
};

#endif                          /* __SOFS16_DIRENTRY__ */
