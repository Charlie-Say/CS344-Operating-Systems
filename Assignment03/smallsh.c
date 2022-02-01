/*
Assignment 3: smallsh (Portfolio Assignment)
By: Charlie Say
sayc@oregonstate.edu
CS 344 Operating Systems
01/28/2022

Requirements:
    - Provide a prompt for running commands
    - Handle blank lines and comments, which are lines beginning with the # character
    - Provide expansion for the variable $$
    - Execute 3 commands exit, cd, and status via code built into the shell
    - Execute other commands by creating new processes using a function from the exec family of functions
    - Support input and output redirection
    - Support running commands in foreground and background processes
    - Implement custom handlers for 2 signals, SIGINT and SIGTSTP
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>


#define MAX_CMD 2048        // Max command line input
#define MAX_ARGS 512        // Max arguments
int globalBG = 1;           // Global background variable


// Parse user input into array from Command Prompt
void wordArray()
{
    char cmd_arr[MAX_ARGS][256];  // Array of words
    char user_input[MAX_CMD];     // User input
    char pid[10];             // pid string
    int i;                    // Loop variable for character in user input
    int j;                    // Loop variable for character in pid
    int the_word;           // Keep track of word when finding special characters
    int the_char;             // Keep track of the character in word when finding special characters

    printf(": ");
    fflush(stdin);
    fgets(user_input, MAX_CMD, stdin);     // Get line of user input
	sprintf(pid, "%d", getpid());       // Convert pid to string for $$ expansion

    for(i = 0; i <= (strlen(user_input)); i++)
    {
        if(user_input[i] == '$' && user_input[i+1] == '$')    // Search for '$$' from user command
        {
            // Loop word each character to change '$' to pid
            for(j = 0; j < strlen(pid); j++)
            {
                cmd_arr[the_word][the_char] = pid[j];     // Change character to pid
                the_char++;         // Add the rest of pid
            }
            i++;    // Increment character in user input
        }

        // Indicate end of word
        else if(user_input[i] == ' ' || user_input[i] == '\0')
        {
            cmd_arr[the_word][the_char] = '\0';       // End of word is NULL character
            the_char = 0;      // Reset character to 0 for next word
            the_word++;     // Increment word in array
        }

        // The '&' character will toggle background only if it is the last character
        else if(user_input[i] == '&' && the_char == 0 && user_input[i+1] == '\0')
        {
            globalBG = 1;         // Toggle background for command
        }
        else
        {
            cmd_arr[the_word][the_char] = user_input[i];     // Add character to word
            the_char++;         // Increment the character in user input
        }
    }
}


/*
    An input file redirected via stdin should be opened for reading only;
    if your shell cannot open the file for reading,
    it should print an error message and set the exit status to 1 (but don't exit the shell).
*/
void inputFile(char theFile[MAX_CMD])
{
    int fd;             // File Descriptor: integer value of file in process
    int dup_error;
    fd = open(theFile, O_RDONLY);       // Use open() for returning a file descriptor. fopen() returns FILE*
                                        // Help Source: https://www.quora.com/What-is-the-difference-between-open-and-fopen-in-C-How-are-they-different-with-each-other-in-terms-of-internal-working
    if(fd == -1)
    {
        printf("Cannot open %s for input", theFile);        // Handle error in opening file
        fflush(stdout);
        exit(1);
    }

    // Make copy of file descriptor dup2()
    dup_error = dup2(fd, 0);       // stdin file descriptor represented by 0
    if(dup_error == -1)
    {
        perror("ERROR");           // Handle error in making copy of file descriptor
        fflush(stdout);
        exit(1);
    }
}


/*
    Similarly, an output file redirected via stdout should be opened for writing only;
    it should be truncated if it already exists or created if it does not exist.
    If your shell cannot open the output file it should print an error message and set the exit status to 1 (but don't exit the shell).
*/
void outputFile(char theFile[MAX_CMD])
{
    int fd;             // File Descriptor: integer value of file in process
    int dup_error;
    fd = open(theFile, O_WRONLY | O_CREAT | O_TRUNC, 0644);         // Use open() for returning a file descriptor. fopen() returns FILE*
                                                                    // Help Source: https://www.quora.com/What-is-the-difference-between-open-and-fopen-in-C-How-are-they-different-with-each-other-in-terms-of-internal-working
    if(fd == -1)
    {
        printf("Cannot open %s for output", theFile);        // Handle error in opening file
        fflush(stdout);
        exit(1);
    }

    // Make copy of file descriptor dup2()
    dup_error = dup2(fd, 1);       // stdout file descriptor represented by 1
    if(dup_error == -1)
    {
        perror("ERROR");           // Handle error in making copy of file descriptor
        fflush(stdout);
        exit(1);
    }
}

int* executeCMD(int the_word, char cmd_arr[MAX_ARGS][256])
{
    char* cmd_arg[512];         // Argument points to character string
    int i;                      // Loop variable for words in array
    int str_count = 0;          // Keep track of character in building argument string

    // Put arguments in array
    for (i = 0; i < the_word; i++)
    {
        // Search for redirection. If found, stop building argument string
        if(strcmp(cmd_arr[i], ">") == 0 || strcmp(cmd_arr[i], "<") == 0)
        {
            break;
        }
        else
        {
            // Build argument string
            cmd_arg[str_count] = cmd_arr[i];
            str_count++;
        }
    }

    // Loop characters in array
    while(i < the_word)
    {
        if(strcmp(cmd_arr[i], ">") == 0)
        {
            outputFile(cmd_arr[i+1]);       // Call output function with argument after redirection character
            i+=2;       // Increment 2 places after redirection character and argument to find more redirections
        }
        else if(strcmp(cmd_arr[i], "<") == 0)
        {
            inputFile(cmd_arr[i+1]);        // Call input function with argument after redirection character
            i+=2;       // Increment 2 places after redirection character and argument to find more redirections
        }
    }

    // Execute command
    if (execvp(cmd_arg[0], cmd_arg))
    {
        printf("%s: No such file or directory\n", cmd_arg[0]);      // Error handling in command argument
        fflush(stdout);
        exit(1);
    }
}


void signalStop()
{
    // Help Source: https://stackoverflow.com/questions/12902627/the-difference-between-stdout-and-stdout-fileno
    // Help Source: https://www.geeksforgeeks.org/input-output-system-calls-c-create-open-close-read-write/

    char* enter = "\nEntering foreground-only mode (& is now ignored)\n";
	char* exit = "\nExiting foreground-only mode\n";

    // Enter foreground
	if(globalBG == 1)
	{
		write(1, enter, 51);
		fflush(stdin);
		globalBG = 0;       // Turn background off
	}
	else
	{
        // Exit foreground
		write(1, exit, 31);
		fflush(stdin);
		globalBG = 1;       // Turn background on
	}
}


void signalHandler()
{
    // Help Source: https://stackoverflow.com/questions/31882797/how-to-use-sigrtmax-and-sigrtmin

    // SIGNINT signal to parents and all childern at the same time
    struct sigaction signal1;
    signal1.sa_handler = SIG_IGN;
    sigfillset(&signal1.sa_mask);
    sigaction(SIGINT, &signal1, NULL);

    // SIGTSTP signal to parent shell process and all children
    struct sigaction signal2 = {{0}};
    signal2.sa_handler = signalStop;
    sigfillset(&signal2.sa_mask);
    sigaction(SIGTSTP, &signal2, NULL);

    /*
        - A child, if any, running as a foreground process must ignore SIGTSTP.
        - Any children running as background process must ignore SIGTSTP.
    */
    signal1.sa_handler = SIG_DFL;       // Set SIGINT as default to ignore SIGTSTP
    sigaction(SIGINT, &signal1, NULL);
    // fflush(stdout);     // fflush everything
}


void userInput()
{
    char cmd_arr[MAX_ARGS][256];        // Array of words
    char user_input[MAX_CMD];           // User input
    int background = 0;         // Keep track of background for processes
    char directory[256];        // Name of directory
    int prev_exit = 0;          // Keep track of previous exit pid
    int i;           // Loop iterator for word in array
    int j;           // Loop iterator for characters in word
    char pid[10];
	sprintf(pid, "%d", getpid());   // Convert pid to string for $$ expansion

    pid_t child_fork;   // Data type for process identification used to represent process ids
    int child_signal = 1;

    int pid_count = 0;
    int child_status;



    while(1)
    {
        int the_word = 0;       // Keep track of words when parsing
		int the_char = 0;       // Keep track of the character in word when parsing

        printf(": ");
        fflush(stdout);
        fgets(user_input, MAX_CMD, stdin);      // Get user input

        user_input[strlen(user_input)-1] = '\0';  // Make end of user_input NULL instead of '\n' (infinite loop from ENTER key)

        // Create array of words
        for(i = 0; i <= (strlen(user_input)); i++)
		{
			if(user_input[i] == '$' && user_input[i+1] == '$')    // Search for '$$' from user command
			{
                // Loop word for each character to change '$' to pid
				for(j = 0; j < strlen(pid); j++)
				{
					cmd_arr[the_word][the_char] = pid[j];    // Change character in word to pid
					the_char++;      // Increment count of character in the word when adding pid
				}
				i++;    // Increment character in user input
			}
			else if(user_input[i] == ' ' || user_input[i] == '\0')
			{
				cmd_arr[the_word][the_char] = '\0';    // Indicate end of word
				the_char = 0;      // Reset character counter to 0 for next word
				the_word++;        // Increment to next word in array
			}

			// Turn on background if '&' character is found
			else if(user_input[i] == '&' && the_char == 0 && user_input[i+1] == '\0')       // Search for '&' only if it is surrounded by spaces and at the end of string
			{
				i = strlen(user_input);     // End the loop
				background = 1;             // Turn background on
			}
			else
			{
				cmd_arr[the_word][the_char] = user_input[i];      // Add character to word
				the_char++;         // Increment to next character in word
			}
		}

        // If first string of array is '#' or string length of zero, then to do nothing
        if(user_input[0] == '#' || strlen(user_input) == 0)
        {
            continue;       // Do nothing
        }
        else if(strcmp(cmd_arr[0], "status") == 0)
        {
            /*
                The [status] command prints out either the exit status or the
                terminating signal of the last foreground process ran by your shell.
            */
            if(child_signal == 0)         // If command is run before any foreground command, return exit status 0
            {
                child_signal = 0;
                printf("exit value %d\n", child_signal);
                fflush(stdout);
            }
            else
            {
                printf("exit value %d\n", prev_exit);
                fflush(stdout);
            }
        }
        else if(strcmp(cmd_arr[0], "exit") == 0)     // Compare first string in array with 'exit'
		{
            /*
                The [exit] command exits your shell.
                It takes no arguments. When this command is run, your shell must kill
                any other processes or jobs that your shell has started before it terminates itself.
            */

            break;
		}
        else if(strcmp(cmd_arr[0], "cd") == 0)
		{
            /*
                The [cd] command changes the working directory of smallsh.

                - By itself - with no arguments - it changes to the directory specified in the HOME environment variable
                    - This is typically not the location where smallsh was executed from,
                      unless your shell executable is located in the HOME directory,
                      in which case these are the same.
                - This command can also take one argument: the path of a directory to change to.
                  Your [cd] command should support both absolute and relative paths.
            */

			if(the_word == 1)     // Array contains 1 word
			{
				sprintf(directory, "%s", getenv("HOME"));   // Copy HOME environment path to string
                chdir(directory);       // Change directory to Home environment path
			}
			else if(the_word == 2)    // Array contains 2 words
			{
				strcpy(directory, cmd_arr[1]);      // Copy string of [cd] argument (path)
                chdir(directory);       // Change directory to argument after [cd] command
			}
		}

        else
        {
            /*
                Executing Other Commands

                Your shell will execute any commands other than the 3 built-in
                command by using fork(), exec() and waitpid()
            */

            signalHandler();        // Initialize with signal handlers

            child_fork = fork();    // Create child process with fork()
            if (child_fork == 0)    // If child process is Zero: process created
            {
                executeCMD(the_word, cmd_arr);      // Call function to execute other commands
			}
			else
			{
                /*
                    Checks to see if background was toggled by '&' and compares with the global background variable

                    Help Source: https://www.tutorialspoint.com/unix_system_calls/waitpid.htm
                */
				if(background && globalBG)
				{
                    // Return immediately if no child has exited, Parent process has WNOHANG
					waitpid(child_fork, &child_status, WNOHANG);        // pid_t waitpid(pid_t pid, int *status, int options);
					printf("Background pid is %d\n", child_fork);
				    fflush(stdout);
					pid_count++;        // Increment pid counter
				}
				else
				{
                    /*
                        System call suspends execution of the current
                        process until one of its children terminates

                        Help Source: https://stackoverflow.com/questions/47441871/why-should-we-check-wifexited-after-wait-in-order-to-kill-child-processes-in-lin

                        Help Source: https://www.geeksforgeeks.org/exit-status-child-process-linux/
                    */

					waitpid(child_fork, &child_status, 0);             // pid_t waitpid(pid_t pid, int *status, int options);

					if (WIFEXITED(child_status))       // Checks to see if child process has terminated (and calling waitpid() failed)
					{
                        // Exit status failed: child_status has value of 0
						child_signal = 0;
						prev_exit = WEXITSTATUS(child_status);      // WEXITSTATUS is macro evaluates the status argument that child process passed exit()
					}
					else
					{
                        /*
                            WIFSIGNALED macro indicates that the child process exited because it raised a signal,
                            the WTERMSIG macro returns the numeric value of the signal that was raised by the child process

                            Help Source: https://www.ibm.com/docs/en/ztpf/2019?topic=zca-wtermsig-determine-which-signal-caused-child-process-exit
                        */

						child_signal = WTERMSIG(child_status);
						printf("terminated by signal %d\n", child_signal);
				        fflush(stdout);
					}
				}
			}

            // Check the status of all processes BEFORE returning access to command line to user
			while((child_fork = waitpid(-1, &child_status, WNOHANG)) > 0)
			{
				if(pid_count == 0)
                {
                    continue;
                }
				else if(WIFEXITED(child_status))        // Checks to see if child process has terminated (and calling waitpid() failed)
				{
					child_signal = 0;
					prev_exit = WEXITSTATUS(child_status);      // WEXITSTATUS is macro evaluates the status argument that child process passed exit()
					printf("Background pid %d is done: exit value %d\n", child_fork, prev_exit);
				    fflush(stdout);
				}
				else
				{
					child_signal = WTERMSIG(child_status);
					printf("Background pid %d is done: terminated by signal %d\n", child_fork, child_signal);
				    fflush(stdout);
				}
				pid_count--;        // Exit loop when pid_count == 0
			}
		}
	background = 0;         // Turn background off
	}
}


int main()
{
    userInput();
	return 0;
}