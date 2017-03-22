/*
 Name: TJ Gudeman
Date: 9/30/16
Assignment: Build your own shell.
Purpose: This program is meant to work as a functioning linux shell.
		 After the user is prompted for input, they can enter linux commands.
*/

#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>

		int main(){

			char input[50];
			char progs[50]="$";
			char args[50]=".";
			char location[256];
			strcpy(location, "/bin/");
			int fd[2];
			int leaveApp = 0;
			char temp[5];

			while(readAndParseCmdLine( progs, args, fd) != 1){
				memset(location, 0, 256);
				strcpy(location, "/bin/");
				strcat(location, progs);
				
				int child_pid= fork();
			// Child process
				if(child_pid == 0){
					execl(location, progs, args,(char*)NULL);	
				}
				// Parent
				else{
					int status;
					waitpid(child_pid, &status, 0);
				}
				memset(progs, 0, 50);
				args[0]='.';
				for (int i=1; i< 50; i++)
					args[i]='\0';
		}
		printf("See ya!\n");
		return 0;
	}


	
/******************************************************************
This funciton will parse the user input and then decide which handler
we need to go to.
*****************************************************************/
int readAndParseCmdLine(char *progs, char *args, int *fd){
	
	/* Create array and fill */
	char input[256];
	char *tempChar="$";
	for(int i = 0; i <256 ; i ++){
		input[i] = *tempChar;
	}
	
	/* Initalize date */
	char location[256]= " ";
	int leaveApp=0;
	int doWePipe=0;
	char *exit="exit";
	char *checkSpace= " ";
	char temPprogs[200];
	char left[50];
	char right[50];
	int counter=0;

	 //Gets from keyboard input
	printf("Type things in here: "); 
	gets(input);					

		/* Our escape flag (Searches for the word "exit")
			and returns 1 which will exit while loop in main */
	if(strstr(input, exit)) { 
		leaveApp = 1;
		return 1;}

		// Execute this if user doesn't type exit
		else{

			for(int i = 0; i<=50; i++){
				char c = input[i];
				
					if ( c == '|'){ // i will be the pipe
						doWePipe=1;
						for( int j =0; j < i; j++){
							left[j] = input[j];
							counter=j;
						}
						int l = 0;
						for( int k = i + 1; k < 15; k++){
							right[l] = input[k];
							l++;
						}
					}
				}

				/* DoWePipe get flagged if there is pipe; sends us to pipe 
					and skips rest of function */
				if(doWePipe){
					handlePipe(left,right,progs,args, counter, fd);
				}


				/**********************************************************
				We did not pipe. Now we need to parse commands
				****************************************************/
				else{

					int min = 0;
					int max = 1;
				int tempMin = 0; // A way to search through without replaceing min value


				/**** Loads argumetns into tempProgs. This makes it easier
					to manuipulate without worrying about overrideing progs***/
				while(input[max] != '$'){
					max++;
				}
				tempMin=min;
				memset(temPprogs, 0, 50);
				char progs[max];
				for(int j = tempMin; tempMin <max; tempMin++){
					temPprogs[tempMin]=input[tempMin];
				}
			}
			memset(progs, 0, 50);

				/*  Check for Space: If space progs gets left side
					and args gets right side */
			for(int i =0; i < 50; i++)
			{
				char c = temPprogs[i];
				if ( c == ' '){
					for( int j =0; j < i; j++){
						progs[j] = input[j];
					}
					int l = 0;
					for( int k = i + 1; k < 30; k++){
						args[l] = input[k];
						l++;
					}
				}
			}
			 // Input does not have space; Set Progs to temPprogs
			if(progs[0] == NULL){
				for(int i =0; i < strlen(temPprogs); i++){
					progs[i]=temPprogs[i];
				}
			}
		}
		return 0;
	}

/******************************************************************
This funciton happens when we need to pipe. It will sort the arguments
into the correct progs and args spot. From there it will open the pipe
and execute it for the first time, returning the output to the pipe
which will be executed in main.
*****************************************************************/
void handlePipe(char *left, char *right, char *progs, char *args, int counter, int *fd){

	// Declare these so we can execute without worrying of overriding
	char tempProgs[20];
	char tempArgs[20];
	int wasThereASpace= 0;
	char location[50];

	//First we parse left side to look for space (meaning there are 2 commands)
	for(int i =0; i < counter-1; i++)
	{
		char c = left[i];
		if ( c == ' '){
			wasThereASpace=1;
					memset(tempProgs,0,10); // Prep array
					for( int j =0; j < i; j++){
						tempProgs[j] = left[j];
					}
					int l = 0;
					for( int k = i + 1; k < 21; k++){
						tempArgs[l] = left[k];
						l++;
					}
				}
			}
					// If we don't find a space, then the left side is jsut progs
			if(!wasThereASpace){
				for(int i = 0; i < 10; i++){
					tempProgs[i]=left[i];
				}
			}

				//Now we fill progs with right side
			int lengthofRight=strlen(right);
			for(int i =0; i<lengthofRight; i++){
				progs[i]= right[i];
			}

			// Create pipe and pipe command
			pid_t pid;
			pipe(fd);
			pid=fork();

			// This will pipe output of first command
			if(pid==0){
				dup2(fd[1], STDOUT_FILENO);
				strcpy(location, "/bin/");
				strcat(location, tempProgs);
				execl(location, tempProgs, tempArgs,  NULL); //Run command and puts it into pipe
				close(fd[1]);		
				}
				else{
					//Fork so we can run the second command of the pipe
					pid=fork();
					if(pid==0){
						dup2(fd[0], STDIN_FILENO);
						strcpy(location, "/bin/");
						strcat(location, progs);				
						execl(location, progs, (char *)NULL);
						close(fd[0]);
					}
					// This is our parent. Will wait for chilf
					else{
						int status;
						waitpid(pid, &status, 0);
					}
				}
			}

				


