/*
 * \author Ana Patrícia Gomes da Cruz
 */

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stdbool.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/statvfs.h>
#include <sys/stat.h>
#include <time.h>
#include <utime.h>
#include <libgen.h>
#include <string.h>
#include <linux/limits.h>

#include "syscalls.h"
#include "dealers.h"
#include "direntries.h"
#include "filecluster.h"
#include "freelists.h"
#include "probing.h"
#include "exception.h"

/*
 *  \brief Delete a directory.
 *
 *  It tries to emulate <em>rmdir</em> system call.
 *  
 *  The directory should be empty, ie. only containing the '.' and '..' entries.
 *
 *  \param path path to the directory to be deleted
 *
 *  \return 0 on success; 
 *      -errno in case of error, being errno the system error that better represents the cause of failure
 */
int soRmdir(const char *path)
{
    soProbe(233, "soRmdir(\"%s\")\n", path);

    try
    {
        uint32_t inp, pih, dir, cih, cinp;
        SOInode* ip;

        char* xpath = strdupa(path);
        char* bn = strdupa(basename(xpath));
        char* dn = dirname(xpath);
        
        if(strlen(xpath) > PATH_MAX)
            throw SOException(ENAMETOOLONG, __FUNCTION__);

        if(strlen(bn) > SOFS16_MAX_NAME)
            throw SOException(ENAMETOOLONG, __FUNCTION__);
        
        soTraversePath(strdupa(dn), &inp);
        
        if(inp == NULL_REFERENCE)
            throw SOException(ENOENT, __FUNCTION__);
        

        pih = iOpen(inp);
        ip = iGetPointer(pih);

        /*Verificar se parent é diretório*/
        if((ip->mode & S_IFDIR) != S_IFDIR)
        {
            iSave(pih);
            iClose(pih);
            throw SOException(ENOTDIR, __FUNCTION__);
        }

        /* Verificar permissoes de execução */
        if(iCheckAccess(pih, X_OK)==false){
            iSave(pih);
            iClose(pih);
            throw SOException(EACCES,__FUNCTION__);
        }
        
        /* Get inode to be deleted */
        soGetDirEntry(pih, bn, &cinp);
  
        if(cinp==NULL_REFERENCE){
            iSave(pih);
            iClose(pih);
            throw SOException(ENOENT, __FUNCTION__);
        }

        cih = iOpen(cinp);
        SOInode* ih = iGetPointer(cih);

        if((ih->mode & S_IFDIR)!=S_IFDIR){
            iSave(pih);
            iClose(pih);
            iSave(cih);
            iClose(cih);
            throw SOException(ENOTDIR, __FUNCTION__);
        }

        if(iCheckAccess(cih, W_OK)==false){
            iSave(pih);
            iClose(pih);
            iSave(cih);
            iClose(cih);
            throw SOException(EACCES,__FUNCTION__);
        }

        /* Check if directory only has two entries */
        if(ih->size!=sizeof(SODirEntry)*2){
            iSave(pih);
            iClose(pih);
            iSave(cih);
            iClose(cih);
            throw SOException(ENOTEMPTY, __FUNCTION__);
        }

        /*remover direntries "." e ".." */
        soDeleteDirEntry(cih, "..", &dir);
        soDeleteDirEntry(cih, ".", &dir);

        /*remover direntry do diretorio*/
        soDeleteDirEntry(pih, bn, &dir);

        iDecRefcount(pih);

        /* Apagar todos os clusters desse inode */
        soFreeFileClusters(cih, 0);
        soFreeInode(cinp);

        iSave(pih);
        iClose(pih);
        iSave(cih);
        iClose(cih);

        return 0;
    }
    catch(SOException & err)
    {
        return -err.en;
    }
}
