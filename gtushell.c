#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <ctype.h>
#include <errno.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>

#define PROMPT_STRING "gtushell>>"
#define COMNUM 100
#define COMLENG 1024


char commands[COMNUM][COMLENG];
int commandsNumber=0;


void help();
void execute(char *parameters[], int parameterNumber, char *shellPath);
void addCommandToCommands(char *command);
void commandParse(char *command,char *parameters[],int *parameterNumber);
int isPipeOrRedirect(char *parameters[], int parameterNumber, char *input);
void ParsePipe(char *param[],char *parameters[],char *parameters2[],int parameterNumber);
void ParseRedirect(char *param[],char *parameters[],int parameterNumber, char *input);
int writeToFile(int pipeNumber, char *filename);
int readFile(char *filename);
void handle_sigint(int sig);



int main (void) {
	
	char command[COMLENG];
	char *parameters[COMLENG];
	char shellPath[COMLENG];
	getcwd(shellPath, sizeof(shellPath));


	int parameterNumber=0;
	
	signal(SIGTERM, handle_sigint); 


	while(1) {

		fflush(stdout);
		fflush(stdin);

		puts(PROMPT_STRING);

		if (fgets(command, COMLENG, stdin) == NULL)
			continue;

		if(strcmp(command,"\n")==0 || strcmp(command,"\0")==0 || strcmp(command," ")==0)
			continue;

		commandParse(command,parameters,&parameterNumber);

		if(parameters[0]==NULL)
			continue;

		addCommandToCommands(command);


		execute(parameters,parameterNumber,shellPath);

		memset(parameters,'\0',COMLENG*sizeof(parameters[0]));

	}

	return 0;
}

void addCommandToCommands(char *command){
	strcpy(commands[commandsNumber++],command);
}

void commandParse(char *command,char *parameters[],int *parameterNumber){

	int paramNumber=0;
	char *copyCommand;
	char *param;

	copyCommand = command + strspn(command," ");

	param = strtok(copyCommand," \n\t");

	while(param != NULL){
		parameters[paramNumber]=param;
		paramNumber++;
		param = strtok(NULL," \n\t");
	}

	*parameterNumber=paramNumber;

}

void handle_sigint(int sig){
  
  if(sig==SIGTERM){
   	printf("Caught signal %d\n", sig); 
   	exit(0);
  }

}


void ParsePipe(char *param[],char *parameters[],char *parameters2[],int parameterNumber){


	int i=0, j=0;

	while(1){
		if(strstr(param[i],"|")!=NULL)
			break;
		parameters[i]=param[i];
		i++;
	}
	
	i++;

	while(i<parameterNumber){

		parameters2[j]=param[i];
		j++;
		i++;
	}

}
void ParseRedirect(char *param[],char *parameters[],int parameterNumber, char *input){


	int i=0;

	while(1){
		if(strstr(param[i],input)!=NULL)
			break;
		parameters[i]=param[i];
		i++;
	}

}



void help(){

	printf("- lsf; which will list file type, access rights, file size and file name of all files in the present working directory\n");
	printf("- pwd; which will print the present working directory\n") ;
	printf("- cd; which will change the present working directory to the location provided as argument\n");
	printf("- help; which will print the list of supported commands\n");
	printf("- cat; which will print on standard output the contents of the file provided to it as argument\n");
	printf("- wc; which will print on standard output the number of lines in the file provided to it as argument\n");
	printf("- bunedu; This time, the file path argument of bunedu can also come from standard input.\n");
	printf("- exit; which will exit the shell\n");
}
int isPipeOrRedirect(char *parameters[], int parameterNumber, char *input){

	int i=0;
	for(i=0; i<parameterNumber; i++)
		if(strstr(parameters[i],input)!=NULL)
			return i;

	return 0;
}

int writeToFile(int pipeNumber, char *filename){
	
	char output[1024]; 

	FILE *pipeFile=fdopen(pipeNumber,"r");

	if(pipeFile== NULL){
		printf("Could not open file pipe file\n") ;
		return -1;
	}

	FILE *temp = fopen(filename,"w");

	if(temp== NULL){
		printf("Could not open file %s\n",filename ) ;
		return -1;
	}


	while(fgets(output,1024,pipeFile)!=NULL){
		fprintf(temp,"%s",output);
	}

	fclose(temp);
	fclose(pipeFile);


	return 0;

}

int readFile(char *filename){

	FILE *inputFile = fopen(filename,"r");
	
	if(inputFile== NULL){
		printf("Could not open file %s\n",filename) ;
		return -1;
	}

	char input[1024]; 

	while(fgets(input,1024,inputFile)!=NULL){
		printf("%s",input);
		fflush(stdout);
	}

	fclose(inputFile);
	
	return 0;
}


void execute(char *parameters[], int parameterNumber, char *shellPath) {

	char path[1024];
	pid_t childPid,childPid2;

	if((strcmp(parameters[0],"exit"))==0){
			
			exit(0);
		
	}else if((strcmp(parameters[0],"help"))==0){

		help();

	}else if( parameters[0][0]=='!' && isdigit(parameters[0][1])!=0 ){
		
		char *param = strtok(parameters[0],"!");
		int value=0;
		sscanf(param,"%d",&value);

		if(value<commandsNumber){
			  
			int paramNumber=0;

			char *temp[COMLENG];
			
			commandParse(commands[(commandsNumber-1)-value],temp,&paramNumber);
			strcpy(commands[commandsNumber-1],commands[(commandsNumber-1)-value]);
			
			execute(temp,paramNumber,shellPath);
			
		}else
			fprintf(stdout,"There is not the %d-th previous command.\n",value);

	}else if((strcmp(parameters[0],"cd"))==0){
		if(parameters[1]==NULL)
			chdir(shellPath);
		else
			if(chdir(parameters[1])==-1){
				fprintf(stdout,"there is not such a directory.\n");
			}

	}else if(isPipeOrRedirect(parameters,parameterNumber,"|")!=0){

			char *firstCommand[COMLENG];
			char *secondCommand[COMLENG];

			ParsePipe(parameters,firstCommand,secondCommand,parameterNumber);

			int pipeFd[2];
			pipe(pipeFd);

			childPid=fork();
				
			if(childPid==-1){

				fprintf(stderr, "Error fork %d !!!\n",errno );
				exit(1);

			}else if(childPid==0){   
				close(pipeFd[0]);
				dup2(pipeFd[1],STDOUT_FILENO);
				
				close(pipeFd[1]);
				close(pipeFd[0]);


				sprintf(path, "%s/%s", shellPath, firstCommand[0]); 
				execv(path,firstCommand);

				exit(1);
			}
			else {
					
				childPid2=fork();

				if(childPid2==-1){

					fprintf(stderr, "Error fork %d !!!\n",errno );
					exit(1);

				}else if(childPid2==0){   
					close(pipeFd[1]);
					dup2(pipeFd[0],STDIN_FILENO);
					
					close(pipeFd[1]);
					close(pipeFd[0]);


					sprintf(path, "%s/%s", shellPath, secondCommand[0]); 
					execv(path,secondCommand);

					exit(1);
				}
				else{
					close(pipeFd[1]);
					close(pipeFd[0]);
					wait(NULL);

				} 
				close(pipeFd[1]);
				close(pipeFd[0]);
				wait(NULL);
			}
		memset(firstCommand,'\0',COMLENG*sizeof(firstCommand[0]));
		memset(secondCommand,'\0',COMLENG*sizeof(secondCommand[0]));

	}else if(isPipeOrRedirect(parameters,parameterNumber,"<")!=0){

		int pipeFd[2];
		pipe(pipeFd);

		char *firstCommand[COMLENG];
		char tmp[1024],tmp2[1024];
		childPid=fork();
			
		if(childPid==-1){

			fprintf(stderr, "Error fork %d !!!\n",errno );
			exit(1);

		}else if(childPid==0){   
			
			close(pipeFd[0]);
			dup2(pipeFd[1],STDOUT_FILENO);

			close(pipeFd[1]);
			close(pipeFd[0]);

			int index=isPipeOrRedirect(parameters,parameterNumber,"<")+1;
			if(strstr(parameters[index],"/")!=NULL){
				getcwd(tmp, sizeof(tmp));
				sprintf(tmp2,"%s/%s", tmp, parameters[index]);
			}
			else
				sprintf(tmp2,"%s",parameters[index]);

			
			readFile(tmp2);

			exit(1);
		}
		else {
				
			childPid2=fork();

			if(childPid2==-1){

				fprintf(stderr, "Error fork %d !!!\n",errno );
				exit(1);

			}else if(childPid2==0){   

				close(pipeFd[1]);
				dup2(pipeFd[0],STDIN_FILENO);
				
				close(pipeFd[1]);
				close(pipeFd[0]);
				ParseRedirect(parameters,firstCommand,parameterNumber,"<");

				sprintf(path, "%s/%s", shellPath, firstCommand[0]); 
				execv(path,firstCommand);

				exit(1);

			}
			else{
				close(pipeFd[1]);
				close(pipeFd[0]);
				wait(NULL);

			} 
			close(pipeFd[1]);
			close(pipeFd[0]);
			wait(NULL);
		}



	}else if(isPipeOrRedirect(parameters,parameterNumber,">")!=0){
		int pipeFd[2];
		pipe(pipeFd);

		char *firstCommand[COMLENG];

		childPid=fork();
			
		if(childPid==-1){

			fprintf(stderr, "Error fork %d !!!\n",errno );
			exit(1);

		}else if(childPid==0){   
			close(pipeFd[0]);
			dup2(pipeFd[1],STDOUT_FILENO);
			
			close(pipeFd[1]);
			close(pipeFd[0]);
			ParseRedirect(parameters,firstCommand,parameterNumber,">");

			sprintf(path, "%s/%s", shellPath, firstCommand[0]); 
			execv(path,firstCommand);

			exit(1);
		}
		else {
				
			childPid2=fork();

			if(childPid2==-1){

				fprintf(stderr, "Error fork %d !!!\n",errno );
				exit(1);

			}else if(childPid2==0){   
				close(pipeFd[1]);
				//close(pipeFd[0]);

				int index=isPipeOrRedirect(parameters,parameterNumber,">")+1;
				
				writeToFile(pipeFd[0],parameters[index]);

				exit(1);
			}
			else{
				close(pipeFd[1]);
				close(pipeFd[0]);
				wait(NULL);

			} 
			close(pipeFd[1]);
			close(pipeFd[0]);
			wait(NULL);
		}



	}else{
		childPid=fork();
				
		if(childPid==-1){

			fprintf(stderr, "Error fork %d !!!\n",errno );
			exit(1);

		}else if(childPid==0){   
			sprintf(path, "%s/%s", shellPath, parameters[0]); 
			execv(path,parameters);

			exit(1);
		}
		else 
			wait(NULL);

	}

}
