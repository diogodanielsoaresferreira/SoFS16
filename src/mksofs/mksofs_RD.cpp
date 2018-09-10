#include "mksofs.h"
#include "superblock.h"
#include "exception.h"
#include "direntry.h"

#include <errno.h>

/*
 * filling in the contents of the root directory:
     the first 2 entries are filled in with "." and ".." references
     the other entries are empty
 */
void fillInRootDir(SOSuperBlock * p_sb)
{

    /* Array com todos os diretórios inicializados para um cluster */
    uint32_t dpc = DPB * p_sb->csize;

    SODirEntry array_dir[dpc];

    /* Diretórios iniciais já atualizados */
    std::string root = ".\0";
    std::string root2 = "..\0";

    unsigned int i;

    memset(&array_dir[0].name[0], '\0',SOFS16_MAX_NAME+1);
    
    for (i=0; i<SOFS16_MAX_NAME && root[i]!='\0'; i++){
    	array_dir[0].name[i] = root[i];
    }

    array_dir[0].in = 0;

    memset(&array_dir[1].name[0], '\0',SOFS16_MAX_NAME+1);

    for (i=0; i<SOFS16_MAX_NAME && root2[i]!='\0'; i++){
    	array_dir[1].name[i] = root2[i];
    }

    array_dir[1].in = 0;

    for (i=2; i<dpc; i++){
    	memset(&array_dir[i].name[0], '\0',SOFS16_MAX_NAME+1);
    	array_dir[i].in = NULL_BLOCK;
    }

    soWriteRawCluster(p_sb->czstart, &array_dir, p_sb->csize);
}
