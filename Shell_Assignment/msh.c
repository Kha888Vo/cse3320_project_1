// The MIT License (MIT)
// 
// Copyright (c) 2016, 2017 Trevor Bakker 
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.


#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>

#define WHITESPACE " \t\n"      // We want to split our command line up into tokens
                                // so we need to define what delimits our tokens.
                                // In this case  white space
                                // will separate the tokens on our command line

#define MAX_COMMAND_SIZE 255    // The maximum command-line size

#define MAX_NUM_ARGUMENTS 5     // Mav shell only supports five arguments

int pid_array[10];
void add_pid(int pid_num)
	{
		int i;
		for (i=0; i <10 && pid_num != pid_array[i]; i++)
		{
			if(pid_array[i] == 0)
			{
				pid_array[i] = pid_num;
				i+=10;
			}
	
			if(pid_array[9] != 0)
			{
				for(i= 0 ; i < 9 ; i++)
				{
					pid_array[i] = pid_array[i+1];
				}
				pid_array[9] = pid_num;
			}
		}
	}

void print_pid()
	{
		int i;
		for(i = 0 ; i <  10; i ++)
		{
			if(pid_array[i] !=0)
			{
				printf("%d. %d\n",i+1,pid_array[i]);
			}
		}
	}

int main()
{

  char * cmd_str = (char*) calloc(100,MAX_COMMAND_SIZE );
	
  while( 1 )
  {
    // Print out the msh prompt
	
    printf ("msh> ");

    // Read the command from the commandline.  The
    // maximum command that will be read is MAX_COMMAND_SIZE
    // This while command will wait here until the user
    // inputs something since fgets returns NULL when there
    // is no input
	
	int flag = 1;
    while(  flag )
	{
	
	fgets(cmd_str,MAX_COMMAND_SIZE,stdin);
	
	if(&cmd_str != NULL)
	{
	
	flag = 0;	
	}
	else 
	{
		printf("msh> ");	
	}
	}

    /* Parse input */
	
	
   char *token[MAX_NUM_ARGUMENTS];

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

    // Now print the tokenized input as a debug check
    // \TODO Remove this code and replace with your shell functionality
/*
    int token_index  = 0;
    for( token_index = 0; token_index < token_count; token_index ++ ) 
    {
      printf("token[%d] = %s\n", token_index, token[token_index] );  
    }
*/
	if(token[0] != 0)
	{
		int safe = 1;
	if(strcmp(token[0], "exit")==0|| strcmp(token[0],"quit")==0)
	{
		safe = 0;
		exit(0);	
		
	}
	if(strcmp(token[0], "cd") == 0 )
	{
		pid_t getpid(void);
		if(chdir(token[1]) >= 0){}
		else	
		{
		printf("Directory [%s] cannot be found. \n", token[1]);		
		}
	add_pid(getpid());
		safe = 0;
	}

	if(strcmp(token[0],"showpid") == 0 )
	{
		print_pid();
		safe = 0;	
	}
	if(safe)
	{
	pid_t pid = fork();
	add_pid( (int) pid);
	if (pid < 0 )
	{
		printf("Fail fork(): \n");
		exit(EXIT_FAILURE);
	}	
	else if (pid == 0 )
	{
	execvp(token[0],token);
	fflush(NULL);
	exit (EXIT_SUCCESS);	

	}

	else{
	int stat;
	waitpid(pid, &stat, 0);
	fflush(NULL);
	}

	}
	
}
    free( working_root );

  }
  return EXIT_SUCCESS;
}
