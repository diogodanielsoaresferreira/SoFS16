/*
 * \author Nuno Capela
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
#include "direntries.h"
#include "filecluster.h"
#include "dealers.h"
#include "freelists.h"
#include "probing.h"
#include "exception.h"

/*
 *  \brief Create a directory.
 *
 *  It tries to emulate <em>mkdir</em> system call.
 *
 *  \param path path to the file
 *  \param mode permissions to be set:
 *          a bitwise combination of S_IRUSR, S_IWUSR, S_IXUSR, S_IRGRP, S_IWGRP, S_IXGRP, S_IROTH, S_IWOTH, S_IXOTH
 *
 *  \return 0 on success; 
 *      -errno in case of error, being errno the system error that better represents the cause of failute
 */
int soMkdir(const char *path, mode_t mode)
{
    soProbe(232, "soMkdir(\"%s\", %u)\n", path, mode);
    
    try{
        uint32_t inp,dir,cinp;
        uint32_t pih,cih;

        char* xpath = strdupa(path);
        char* bn = strdupa(basename(xpath));
        char* dn = dirname(xpath);

        if (strlen(xpath)>PATH_MAX)
            throw SOException(ENAMETOOLONG,__FUNCTION__);

        if (strlen(bn)>SOFS16_MAX_NAME)
            throw SOException(ENAMETOOLONG,__FUNCTION__);


        soTraversePath(dn, &inp);

        if(inp == NULL_REFERENCE){
            throw SOException(ENOENT,__FUNCTION__);
        }

        pih = iOpen(inp);
        SOInode* ip = iGetPointer(pih);

    /*Verificar se parent é diretorio*/
        if((ip->mode & S_IFDIR) != S_IFDIR){
            iSave(pih);
            iClose(pih);
            throw SOException(ENOTDIR,__FUNCTION__);
        }


    /*Acesso de escrita*/
        if(iCheckAccess(pih, W_OK)==false){
            iSave(pih);
            iClose(pih);
            throw SOException(EACCES,__FUNCTION__);
        }


        soGetDirEntry(pih, bn, &dir);
        if(dir!=NULL_REFERENCE){
            iSave(pih);
            iClose(pih);
            throw SOException(EEXIST, __FUNCTION__);
        }


        soAllocInode(S_IFDIR,&cinp);
        cih = iOpen(cinp);

        if (cih==NULL_REFERENCE){
            iSave(pih);
            iClose(pih);
            iSave(cih);
            iClose(cih);
            throw SOException(ENOMEM,__FUNCTION__);
        }

        iSetAccess(cih, mode);

    /*Criar direntries . e ..*/
        soAddDirEntry(cih,".",cinp);
        iIncRefcount(cih);

        soAddDirEntry(cih,"..", inp);
        iIncRefcount(cih);

    /*Criar direntry do diretorio*/
        soAddDirEntry(pih, bn, cinp);
        iIncRefcount(pih);

        iSave(cih);
        iClose(cih);
        iSave(pih);
        iClose(pih);

        return 0;
    }catch(SOException & err){
        return -err.en;
    }
}
