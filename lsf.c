#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>


int main(int argc, char **argv){
    
    struct stat statbuf;
    struct dirent *direntp;
    DIR *dirp;
    
    dirp = opendir(".");

    while(1){

        if((direntp = readdir(dirp)) == NULL){
            break;              
        }

        lstat(direntp->d_name,&statbuf);

        if(S_ISDIR(statbuf.st_mode))
            continue;

        if(S_ISREG(statbuf.st_mode))
            printf("R\t");
        if(S_ISCHR(statbuf.st_mode) || S_ISCHR(statbuf.st_mode) || S_ISBLK(statbuf.st_mode) || 
            S_ISFIFO(statbuf.st_mode) || S_ISLNK(statbuf.st_mode) || S_ISSOCK(statbuf.st_mode))    
            printf("S\t");


        printf((statbuf.st_mode & S_IRUSR) ? "r":"-");
        printf((statbuf.st_mode & S_IWUSR) ? "w":"-");
        printf((statbuf.st_mode & S_IXUSR) ? "x":"-");

        printf((statbuf.st_mode & S_IRGRP) ? "r":"-");
        printf((statbuf.st_mode & S_IWGRP) ? "w":"-");
        printf((statbuf.st_mode & S_IXGRP) ? "x":"-");
        
        printf((statbuf.st_mode & S_IROTH) ? "r":"-");
        printf((statbuf.st_mode & S_IWOTH) ? "w":"-");
        printf((statbuf.st_mode & S_IXOTH) ? "x":"-");
     

        printf("\t%lld\t%s\n", (long long)statbuf.st_size, direntp->d_name);
 

    }

    closedir(dirp);

    return 0;

}
