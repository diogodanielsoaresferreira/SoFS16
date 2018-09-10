/**
 *  \file direntries.h
 *  \brief Set of operations to manage directories and directory entries
 *
 *  \author Artur Pereira 2008-2009, 2016
 *  \author Miguel Oliveira e Silva 2009
 *  \author António Rui Borges - 2012-2015
 *
 *  \remarks In case an error occurs, every function throws a SOException
 */

#ifndef __SOFS16_DIRENTRIES__
#define __SOFS16_DIRENTRIES__

#include "direntry.h"

/* ************************************************** */
/** \defgroup direntries direntries 
 * @{ 
 */
/* ************************************************** */

/**
 *  \brief Get the inode associated to the given path
 *
 *  The directory hierarchy of the file system is traversed to find 
 *  an entry whose name is the rightmost component of
 *  <tt>path</tt>. 
 *  The path is supposed to be absolute and each component of <tt>path</tt>, 
 *  with the exception of the rightmost one, 
 *  should be a directory name or symbolic link name to a path.
 *
 *  The process that calls the operation must have execution 
 *  (x) permission on all the components of the path with
 *  exception of the rightmost one.
 *
 *  \param path the path to be traversed
 *  \param inp Pointer to the variable, where the inode number must be put
 */
void soTraversePath(char *path, uint32_t * inp);

/* ************************************************** */

/**
 *  \brief Get an entry given a name
 *
 *  The directory contents, seen as an array of directory entries, 
 *  is parsed to find an entry whose name is <tt>name</tt>. 
 *
 *  The <tt>name</tt> must also be a <em>base name</em> and not a <em>path</em>, 
 *  that is, it can not contain the character '/'.
 *
 *  \param pih inode handler of the parent directory
 *  \param name the name entry to be searched for
 *  \param cinp Pointer to the variable where inode number associated to the entry is to be stored
 */
void soGetDirEntry(int pih, const char *name, uint32_t * cinp);

/* ************************************************** */

/**
 *  \brief Add a new entry to the parent directory.
 *
 *  A direntry is added connecting the parent inode to the child inode.
 *  The refcount of the child inode is not incremented by this function.
 *
 *  \param pih inode handler of the parent inode
 *  \param name name of the entry
 *  \param cin number of the child inode
 */
void soAddDirEntry(int pih, const char *name, uint32_t cin);

/* ************************************************** */

/**
 *  \brief Remove an entry from a parent directory.
 *
 *  A direntry associated from the given directory is deleted.
 *  The refcount of the child inode is not decremented by this function.
 *
 *  \param pih inode handler of the parent inode
 *  \param name name of the entry
 *  \param cinp Pointer to the variable where the number of the child inode is to be stored
 */
void soDeleteDirEntry(int pih, const char *name, uint32_t * cinp);

/* ************************************************** */

/**
 *  \brief Rename an entry of a directory.
 *
 *  A direntry associated from the given directory is renamed.
 *
 *  \param pih inode handler of the parent inode
 *  \param name current name of the entry
 *  \param newName new name for the entry
 */
void soRenameDirEntry(int pih, const char *name, const char *newName);

/* ************************************************** */
/** @} */
/* ************************************************** */

#endif                          /* __SOFS16_DIRENTRIES__ */
