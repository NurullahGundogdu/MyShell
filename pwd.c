#include <stdio.h>
#include <unistd.h>


int main(int argc, char **argv){

	char path[1024];

	getcwd(path,sizeof(path));
    fprintf(stdout,"%s\n",path);

	return 0;
}
