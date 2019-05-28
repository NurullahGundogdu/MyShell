#include <stdio.h>


int main(int argc, char **argv){


	int num=0;

	char command[1024];

	FILE *file;
	
	if(argc==1){
		file=stdin;
	}
	else{
		file = fopen(argv[1],"r");

		if ( file== NULL ){
	        printf( "Could not open file %s\n",argv[1] ) ;
			return -1;
		}
	
	}
	
	
	while (fgets(command, 1024, file)) {
		
     		num++;
  	}
	
	printf("%d\n",num );

	fclose(file);
	
	fflush(0);

	
	return num;

}
