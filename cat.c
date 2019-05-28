#include <stdio.h>
#include <stdlib.h>


int main(int argc, char **argv){

	FILE *temp;
	
	char screen[1024];


	if(argc==1){

		temp=stdin;
		
		
		while(fgets(screen,1024,temp)!=NULL){    		
			printf("%s",screen);
			fflush(stdout);
		}
	}else{

		temp = fopen(argv[1],"r");

		if ( temp== NULL ){
	        printf( "Could not open file %s\n",argv[1]) ;
			return -1;
		}
		while(fgets(screen,1024,temp)!=NULL){    		
			printf("%s",screen);
		}
	}
	
	fflush(0);

	return 0;
}
