/**
 *  \author Pedro Marques Ferreira da Silva
 *  \tester Pedro Marques Ferreira da Silva
  */

#include "direntries.h"
#include "direntry.h"
#include "filecluster.h"

#include "probing.h"
#include "exception.h"
#include "dealers.h"
#include "inode.h"
#include "cluster.h" 
#include <linux/limits.h>

#include <unistd.h>
#include <errno.h>
#include <dealers.h>
#include <libgen.h>

static uint32_t links_available = _PC_SYMLINK_MAX;

static void relTraversePath(char *path, uint32_t * inp);

void soTraversePath(char *path, uint32_t * inp)
{
    soProbe(400, "soTraversePath(%s, %p)\n", path, inp);

    uint32_t inode_handler;
    //uint32_t last_inp;

    /*Validation of the arguments */
    if(strlen(path) == 0 || strlen(path)>PATH_MAX || path[0]!='/'){
    	throw SOException(EINVAL, __FUNCTION__);
    }

    if(strcmp(strdupa(path), "/")==0){
        *inp = 0;
        return;
    }

    /* If is root, inode is 0 */
    if(strcmp(dirname(strdupa(path)), "/")==0){
        inode_handler = iOpen(0);
        *inp = 0;
    }
    /* Else, get inode */
    else{
        soTraversePath(dirname(strdupa(path)), inp);
        inode_handler = iOpen(*inp);
    }

    /* Execution acess */
    if(iCheckAccess(inode_handler, (uint32_t)X_OK)==false){
        iClose(inode_handler);
        throw SOException(EACCES, __FUNCTION__);
    }

    SOInode* inode = iGetPointer(inode_handler);

    /* Check inode is not directory or inode is empty */
    if((inode->mode & S_IFDIR) != S_IFDIR || (inode->mode & INODE_FREE) == INODE_FREE){
        iClose(inode_handler);
        throw SOException(ENOENT, __FUNCTION__);
    }

    /* Save parent inode */
    //last_inp = *inp;

    /* Check if already exists file that name */
    soGetDirEntry(inode_handler, basename(strdupa(path)), inp);

    if(*inp==NULL_REFERENCE){
        iClose(inode_handler);
        throw SOException(ENOENT, __FUNCTION__);
    }

    /* Descomentar para testar manualmente atalhos */
    /* Não funciona no FuseMount! */

//    uint32_t i, inode_handler2;
//    uint32_t bpc = soGetBPC();
//    char buf[bpc];
//    char path_link[PATH_MAX];


    /* Open inode */
//    inode_handler2 = iOpen(*inp);
//    inode = iGetPointer(inode_handler2);

    /* Check if is symlink */
//    if(((inode->mode & S_IFLNK) == S_IFLNK) && (inode->mode & INODE_FREE) != INODE_FREE){

        /* If there are no more links available, throw exception. */
//        links_available--;
//        if(links_available==0)
//            throw SOException(ELOOP, __FUNCTION__);

//        memset(buf, '\0', bpc);
//        memset(path_link, '\0', bpc);

//        soReadFileCluster(inode_handler2, 0, &buf);
        /* Copy path inside symlink */
//        for(i=0; i<PATH_MAX && buf[i]!='\0'; i++){
//            path_link[i] = buf[i];
//        }
        /* Check if is absolute path or relative path */
//        if(path_link[0]=='/'){
//            soTraversePath(strdupa(path_link), inp);
//        }
//        else{
//            char new_path[strlen(path_link)+1];
//            strcpy(new_path, "/");
//            strcat(new_path, path_link);
//            relTraversePath(new_path, &last_inp);
//            *inp = last_inp;
//        }
//    }
    
    //iClose(inode_handler2);
    iClose(inode_handler);

}

static void relTraversePath(char *path, uint32_t * inp){

    uint32_t inode_handler;
    //uint32_t last_inp;

    /*Validation of the arguments */
    if(strlen(path) == 0 || strlen(path)>PATH_MAX || path[0]!='/'){
        throw SOException(EINVAL, __FUNCTION__);
    }
    
    if(strcmp(dirname(strdupa(path)), "/")!=0){
        relTraversePath(dirname(strdupa(path)), inp);
    }
    
    inode_handler = iOpen(*inp);
    

    /* Execution acess */
    if(iCheckAccess(inode_handler, (uint32_t)X_OK)==false){
    	iClose(inode_handler);
        throw SOException(EACCES, __FUNCTION__);
    }
        

    SOInode* inode = iGetPointer(inode_handler);

    /* Check inode is not directory or inode is empty */
    if((inode->mode & S_IFDIR) != S_IFDIR || (inode->mode & INODE_FREE) == INODE_FREE){
        iClose(inode_handler);
        throw SOException(ENOENT, __FUNCTION__);
    }

    /* Save parent inode */
    //last_inp = *inp;

    /* Check if already exists file that name */
    soGetDirEntry(inode_handler, basename(strdupa(path)), inp);
    
    if(*inp==NULL_REFERENCE){
        iClose(inode_handler);
        throw SOException(ENOENT, __FUNCTION__);
    }

    /* Descomentar para testar manualmente atalhos */
    /* Não funciona no FuseMount! */

//    uint32_t i, inode_handler2;
//    uint32_t bpc = soGetBPC();
//    char buf[bpc];
//    char path_link[PATH_MAX];

    /* Open inode */
    //inode_handler2 = iOpen(*inp);
    //inode = iGetPointer(inode_handler2);

    /* Check if is symlink */
//    if(((inode->mode & S_IFLNK) == S_IFLNK) && (inode->mode & INODE_FREE) != INODE_FREE){

        /* If there are no more links available, throw exception. */
//        links_available--;
//        if(links_available==0)
//            throw SOException(ELOOP, __FUNCTION__);

//        memset(buf, '\0', bpc);
//        soReadFileCluster(inode_handler2, 0, &buf);
        /* Copy path inside symlink */
//        for(i=0; i<PATH_MAX && buf[i]!='\0'; i++){
//            path_link[i] = buf[i];
//        }

        /* Check if is absolute path or relative path */
//        if(path_link[0]=='/'){
//            soTraversePath(strdupa(path_link), inp);
//        }
//        else{
//            char new_path[strlen(path_link)+1];
//            strcpy(new_path, "/");
//            strcat(new_path, path_link);
//            relTraversePath(new_path, &last_inp);
//            *inp = last_inp;
//       }
//    }
    
    //iClose(inode_handler2);
    iClose(inode_handler);
}