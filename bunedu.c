#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int sizepathfun (char *path);
int postOrderApply (char *path, int pathfun (char *path1));

int isZ=0;

int main(int argc, char **argv) {
		

	if (argc != 1 && argc != 2 && argc !=3) {

		fprintf(stderr, "Usage: %s \"buNeDu [-z] directory_name\"\n", argv[0]);
		return 1;				/*check argumnet size*/
	}
	if(argc==1){
		

		char command[1024],command2[1024],command3[1024];
		int num=0;

		fgets(command, 1024, stdin);
		
		num=sscanf(command,"%s %s",command3,command2);


		if(num==1){
			fprintf(stdout,"Output of “buNeDu %s” gives total sizes:\n",command3);
			
			isZ=1;

			postOrderApply(command3,sizepathfun);

			fprintf(stdout,"%s\n",command3);

		}else if(num==2){

			if(strcmp(command3,"-z")!=0){
				fprintf(stderr, "Usage: %s \"buNeDu [-z] directory_name\"\n",command3);
				return 1;	
			}
			
			fprintf(stdout,"Output of “buNeDu %s” gives total sizes:\n",command2);

			postOrderApply(command2,sizepathfun);

			fprintf(stdout,"%s\n",command2);

		}else{
			fprintf(stderr, "Usage: %s \"buNeDu [-z] directory_name\"\n", argv[0]);
			return 1;				/*check argumnet size*/
		}
	}
	if(argc==2){
		if(strcmp(argv[1],"-z")==0){
			
			fprintf(stdout,"Output of “buNeDu %s” gives total sizes:\n",argv[1]);
			int i=0;
			char command[1024], temp;
			
			while((temp=getc(stdin))!=EOF){
				if(temp=='\n')
					break;
				command[i]=temp;
				i++;
			}	
			
			command[i]='\0';

			postOrderApply(command,sizepathfun);

			fprintf(stdout,"%s\n",command);
		}else{

			isZ=1;			/*check 4 parameters command*/

			fprintf(stdout,"Output of “buNeDu %s” gives total sizes:\n",argv[1]);
			
			postOrderApply(argv[1],sizepathfun);

			fprintf(stdout,"%s\n",argv[1]);
		
		}
	}
	if(argc==3){
		if(strcmp(argv[1],"-z")!=0){
			fprintf(stderr, "Usage: %s \"buNeDu [-z] directory_name\"\n", argv[0]);
			return 1;
		}
		
		fprintf(stdout,"Output of “buNeDu -z %s” don’t add subdirectory sizes:\n",argv[2]);
		
		postOrderApply(argv[2],sizepathfun);
 				/*check 4 parameters command*/
		fprintf(stdout,"%s\n",argv[2]);

	}
	
	fflush(0);
	return 0;
	
}

int sizepathfun (char *path){
	
	struct stat statbuf;

	if(lstat(path,&statbuf) != 0){
        perror("Failed to get file status");
        return -1;
    }
    if(S_ISDIR(statbuf.st_mode))
    	return -1;
    else{				/*return size of path*/
    	return (int)statbuf.st_size;
    } 
}


int postOrderApply (char *path, int pathfun (char *path1)){
	
	struct stat statbuf;
	struct dirent *direntp;
	char pathTemp[1024];
	DIR *dirp;

	int num=0,num2;

	if ((dirp = opendir(path)) == NULL) {
		perror ("Failed to open directory");
		return 1;
	}
	
	while (1){

		if((direntp = readdir(dirp)) == NULL){
			printf("%d\t",num);
			break;				/*print path size*/
		}

		if (strcmp(direntp->d_name,".")==0 || strcmp(direntp->d_name,"..")==0 ){
			continue;
		}
		sprintf(pathTemp, "%s/%s",path,direntp->d_name); 
		lstat(pathTemp,&statbuf);

        if(S_ISFIFO(statbuf.st_mode) || S_ISLNK(statbuf.st_mode)|| S_ISBLK(statbuf.st_mode) || S_ISCHR(statbuf.st_mode) || S_ISSOCK(statbuf.st_mode)){
        	fprintf(stdout,"Special file %s\n",direntp->d_name);
        	continue;
        }				/*check file type*/
		if(S_ISREG(statbuf.st_mode)){		
            num+=pathfun(pathTemp);
            continue;
        }
		
		if(stat(path, &statbuf)==0 || (S_ISDIR(statbuf.st_mode))){		
			if(isZ==0){
				num2=postOrderApply(pathTemp,pathfun);
				if(num2==-1)
					perror("Cannot read folder  ");
				else
					num+=num2;	
			}
			else
				postOrderApply(pathTemp,pathfun);
			fprintf(stdout,"%s\n",pathTemp);			/*print path name*/
		}
		
	}

	while ((closedir(dirp) == -1) && (errno == EINTR)){}		/*close file*/

	return num;
	
}

