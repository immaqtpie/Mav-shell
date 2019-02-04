/*

	Name: Subodh Dhakal
	ID: 1001444393
*/

#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

#define WHITESPACE " \t\n"      // We want to split our command line up into tokens
				// so we need to define what delimits our tokens.
				// In this case  white space
				// will separate the tokens on our command line

#define MAX_COMMAND_SIZE 255    // The maximum command-line size

#define MAX_NUM_ARGUMENTS 10     // Mav shell only supports five arguments
//Global Variables

pid_t child_pid;
pid_t parent_pid;
pid_t temp_pid;
pid_t temp1_pid;
int i=0;
int k=0;
int z=0;
int tmp;     //index of history array seperated from '!'
pid_t pid_list[15];

void signal_handler(int sig)
{
	if(sig==SIGINT && getpid()!=parent_pid)		//We are in the child process
		{
			kill(getppid(),SIGINT);		//Sends itself an SIGINT
		}

	if(sig==SIGTSTP && getpid()!=parent_pid)   //We are in the child process
		{
			kill(getppid(),SIGTSTP);   //Sends itself an SIGTSTP
		}
}

int main()
{

parent_pid=getpid();
int ch='!';				//We need later for strchr function
int j=0; 
static int checker;
char history[15][MAX_COMMAND_SIZE];
char * cmd_str = (char*) malloc( MAX_COMMAND_SIZE );
char *cmd=(char*)malloc(255);
char *path1="/usr/local/bin/";   // To search for various paths 
char *path2="/usr/bin/";	//in order  for execvp function
char *path3="/bin/";

while( 1 )
{
struct sigaction act;
memset(&act,'\0',sizeof(act));			//Set act empty
act.sa_handler=&signal_handler;
sigaction(SIGINT,&act,NULL);			//Add SIGINT to the handler
sigaction(SIGTSTP,&act,NULL);			//ADD SIGTSTP to the handler
sleep(0.5);								//Sleep for 0.5 seconds

// Print out the msh prompt

printf ("msh> ");
// Read the command from the commandline.  The
// maximum command that will be read is MAX_COMMAND_SIZE
// This while command will wait here until the user
// inputs something since fgets returns NULL when there
// is no input

/*
When checker==1, we know that user gave an "!anynumber" command
Then, we seperate that number from !anynumber using strchr and
store the history indexed no. to cmd_str
*/

if(checker==1)
{
strcpy(cmd_str,history[tmp]);
}

else
{		
while( !fgets (cmd_str, MAX_COMMAND_SIZE, stdin) );   //
}
/* Parse input */
checker=0;				//else checker will be zero to keep the loop
char *token[MAX_NUM_ARGUMENTS];
int status;				//Gives the status of the process
int   token_count = 0;                                                       
// Pointer to point to the token
// parsed by strsep
char *arg_ptr;                                         
                               
char *working_str  = strdup( cmd_str );                

// we are going to move the working_str pointer so
// keep track of its original value so we can deallocate
// the correct amount at the end
char *working_root = working_str;

// Tokenize the input stringswith whitespace used as the delimiter
while ( ( (arg_ptr = strsep(&working_str, WHITESPACE ) ) != NULL) && 
(token_count<MAX_NUM_ARGUMENTS))
	{
		token[token_count] = strndup( arg_ptr, MAX_COMMAND_SIZE );
		if( strlen( token[token_count] ) == 0 )
		{
			token[token_count] = NULL;
		}
		token_count++;
	}

	if(cmd_str!=NULL && token[0]!=NULL)
	{
		if(i>14)				//If command exceeds the size of history table
		{						
			for(j=0;j<i;j++)
			{
				strcpy(history[j],history[j+1]);	//We remove the top of the command at index 0
									//and add it with index 1 and so on.
			}
			i--;						//To store more commands in history 
		}							//variable i needs to be decreased
	strcpy(history[i],cmd_str);
		i++;							//Here i is increased in initial state
	}


	if(token[0]==NULL)				//Let the loop run even if it is null
	continue;              

	if(strcmp(token[0],"history")==0)
	{
		for(j=0;j<i;j++)
			{	
				printf("%d: %s\n",j,history[j]);

			}
	}

	if(k<15)			//list of pids should be less than 15. i.e 0-14
	{
		pid_list[k++]=getpid();	//gets pid value of running processes
	}

	if(strcmp(token[0],"listpids")==0 || strcmp(token[0],"showpids")==0)	
	{
		for(z=0;z<k;z++)
		{
			printf("%d: %d\n",z,pid_list[z]);		//prints the process pids
		}	
	}
// Now print the tokenized input as a debug check
// \TODO Remove this code and replace with your shell functionality
	if((strcmp(token[0],"exit")==0) ||( strcmp(token[0],"quit")==0))
	{
		printf("Sometimes have to type exit/quit twice or thrice. Bug i couldn't fix O_O\n");	
		break;	
	}

	if(strcmp(token[0],"bg")==0)			//when user types bg, we take the pid that was suspended which is temp_pid
	{						//then we send SIGCONT which continues the progrma to run in background
		kill(temp_pid,SIGCONT);			
	}

//strchr does string handling. Takes two parameters(string,ch) where ch is the character to be located

	if(strchr(*token,ch)!=NULL)
	{
		tmp=atoi(strtok(cmd_str,"!"));   // It checks for the occurrance of "!" which is stored in ch
		checker=1;			//checker is set to 1 so we can create a condition later
	}

	pid_t pid=fork();		//Forking starts. creates a copy of process. Parent and Child process is created

	if(pid<0)			//Fork error or resoruce not available
	{
		perror("Fork Failed\n");
	}

	else if(pid==0)
	{							//Here I am basically checking the user input in every directory
		if(execvp(token[0],token)<0)			//Checking the user input in the working directory.
		{											
			memset(cmd,0,sizeof(cmd));		//clearing any junk in cmd and making it NULL/Empty
			strcat(cmd,path1);			//Copying the path with priority in order
			strcat(cmd,token[0]);			//Now joining two commands together. One from user and next one is the path.
		}

		else
		{
			break;
		}

		if(execvp(cmd,&cmd)<0)				//Checking the above joined commands in path1.
		{						//If it's -1, we again empty the "cmd" variable
			memset(cmd,0,sizeof(cmd));		//And copy path2
			strcat(cmd,path2);			//Likewise path2 is copied to cmd
			strcat(cmd,token[0]);			//And joins the intial user command (cmd=path2+userinput)
		}
		else
		{
			break;
		}

		if(execvp(cmd,&cmd)<0)				//Execvp searches in path2. If -1, same thing continues.
		{
			memset(cmd,0,sizeof(cmd));		//Emptying cmd variable
			strcat(cmd,path3);			//storing path3
			strcat(cmd,token[0]);			//adding cmd=path3+intial_userinput
		}

		else
		{
			break;
		}

		if(execvp(cmd,&cmd)<0 && strcmp(token[0],"cd")!=0 && strcmp(token[0],"bg")!=0 && strcmp(token[0],"history")!=0 && strcmp(token[0],"listpids")!=0 && strcmp(token[0],"showpids")!=0 && strchr(*token,ch)==NULL)
		{
			printf("Command not found\n");		//user command not found in any path
		}

		else
		{
			break;
		}
	}
		else
		{
			waitpid(pid,&status,0);			//wait till child process ends
			if(strcmp(token[0],"cd")==0)		//check for cd commmand in parent process
			{					//as child process doesn't support cd command
			chdir(token[1]);
			}
		}

		free( working_root );
		}
		return 0;
}

