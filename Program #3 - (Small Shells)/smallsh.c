/**********************************************************************************
 * Program: smallsh.c
 * Author: Tu Lam
 * Date: February 6th, 2021
 * Description: This file contains code in Cs, and it is the main program where the
 * 				 main function will run a small shell program that created as our
 *         		 portfolio.
 *********************************************************************************/

// This include all the directive headers needed for the program
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <signal.h>

// This include declaration of functions
void catchSIGTSTP(int signo);
void printStat(int exitStat, int *sig);

// Global variables declaration
bool foreground = false;
bool background = false;

/*********************************************************
 *
 * Function: "int main()"
 * Detail(s): This function runs the small shell program.
 *
 ********************************************************/
int main() {
  
	// Create & Intialize the SIGINT for the command ^C 
	struct sigaction SIGINT_action = {0};
	SIGINT_action.sa_handler = SIG_IGN;	
	sigfillset(&SIGINT_action.sa_mask);				// Block/delay all signal arriving while this mask is in place
	SIGINT_action.sa_flags = SA_RESTART;			// Set flags to RESTART
	sigaction(SIGINT, &SIGINT_action, NULL);

  // Create & Intialize the SIGTSTP for the command ^Z
	struct sigaction SIGTSTP_action = {0};
	SIGTSTP_action.sa_handler = catchSIGTSTP;
	sigfillset(&SIGTSTP_action.sa_mask);       	// Block/delay all signal arriving while this mask is in place
	SIGTSTP_action.sa_flags = SA_RESTART;        // Set flags to RESTART
	sigaction(SIGTSTP, &SIGTSTP_action, NULL);

	// Create a string to hold max of 2048 characters, an array to hold up to 512 arguments & 2D array with max of 512,
	// a bool to check if we need to continue the program or not, and two int to check for argument and character count
	bool check = false;
	char strC[2048];
	char arg[512][50] = {0};
	char *arg_s[512];
	char bg_child[40];
	pid_t process_c[100];

	// This is  for the int to track how many word are in the command line and an int to count the char in the word
	int trk_char = 0;
	int arg_cmd = 0;

	// These are string constant to check and see if any of the command enter matches with any of these words
	char null_end[] = "\0";
	char ch_dir[] = "cd";
	char exit_w[] = "exit";
	char stat[] = "status";

	// Have a string to hold the absolute PATH of HOME directory and a string to hold relative PATH
	char *dir = getenv("HOME");
	char dir_r[100];

	// Create a pid_t process variable, an exit variable for the child, a status int to determine the status, 
	// a way to determine the termination signal, and a child counter
	pid_t spawnPid = -5;
	int childExitMethod = -5;
  	int statusExit = 0;
	int terminate = 0;
	int child_counter = 0;

	// Have a string to get the Pid into a string of the parent's process ID
	char str_p[20];
	sprintf(str_p, "%d", getpid());

	do {
		printf(": ");											// Print out the command line for user to enter
		fflush(stdin);											// Flush out the buffer
		fgets(strC, 2048, stdin);							// Ask user for the input of the command

		// Reference: https://stackoverflow.com/questions/51310985/remove-newline-from-fgets-input
		strC[strlen(strC) - 1] = '\0';					// Remove \n and give it a NULL ending from fgets()
		trk_char = 0;											// Reset to 0 for both tracking the word and char in the arg for later use
		arg_cmd = 0;
		memset(arg, 0, 512 * 50 * sizeof(char[0][0]));				// Reset the arg to hold zero data everytime a new command is enter
		
		// Create a for loop to store each word enter in the command line into the array argument
		// Reference: https://www.includehelp.com/c-programs/c-program-to-split-string-by-space-into-words.aspx
		for (int i = 0; i <= (strlen(strC)); i++) {
			if (strC[i] == ' ' || strC[i] == '\0') {					// If the loop encounter a space or null char
				arg[arg_cmd][trk_char] = '\0';							// End the word with assigning \0 and move onto the next word
				arg_cmd = arg_cmd + 1;										// Move the row to the next position
				trk_char = 0;													// Reset the char back to 0 for the next word
			}

			else if (strC[i] == '&' && strC[i+1] == '\0') { 		// If the command found the '&' at the end of the string
				if (foreground == false) {									// If foreground hasn't been activated, assign true to accept '&'
					background = true;
				}
				
				else {															// Else, leave background false as it '&' does not matter
					background = false;
				}
			}

			else if (strC[i] == '$' && strC[i+1] == '$') {			// If the cmd have '$$' in it, concat the PID into the $
				for (int j = 0; j < strlen(str_p); j++) {
					arg[arg_cmd][trk_char] = str_p[j];					// Store the PID into the argument 
					trk_char = trk_char + 1;								// Increment the tracking character
				}
				i = i + 1;														// Increment to the next element in the array
			}

			else {																// Else, word has not ended and add each character into the arg
				arg[arg_cmd][trk_char] = strC[i];						// Assign the character to each spot
				trk_char = trk_char + 1;									// Increment to move to the next spot in the column
			}
		}

		// "# or ' '" COMMAND
		// If the first character is # or the line is empty
		if ((strC[0] == '#') || (strcmp(strC, null_end) == 0)) {	
			check = false;														// Check assign false, so shell re-prompt the command line
		}

		// "cd" COMMAND
		// If the command line matches with the word "cd" at the index 0
		else if (strcmp(arg[0], ch_dir) == 0) {								
			if (arg_cmd == 2) {												// If there are 2 argument in the command line 
				strcpy(dir_r, arg[1]);										// Relativey copy the cd argument at index 1 into dir_r
				int chk_dir = chdir(dir_r);								// Change the directory to match with the arg[1]
				if (chk_dir == -1) {											// If chdir() return a -1, print error
					printf("Error, couldn't find the '%s' directory\n", dir_r);
				}
			}	

			else if (arg_cmd == 1) {										// If user only type in "cd"
				int chk_dir = chdir(dir);									// Change the directory to HOME
				if (chk_dir == -1) {											// Check if it equal to -1, if so print error
					printf("Error, couldn't reach HOME directory\n");
				}
			}

			else {																// Else if there are more than 2 arguments, print error
				printf("Error there is an unexpected command for 'cd'\n");
				fflush(stdout);												// Flush out the stdout buffer
			}
			check = false;														// Assign bool false to continue with the program
		}	

		// "status" COMMAND
		// If the user enter the word "status"
		else if (strcmp(arg[0], stat) == 0) {
			printStat(statusExit, &terminate);							// Print out the status of the exit status of child
		} 

		// "exit" COMMAND
		// If the user enter the word "exit"
		else if (strcmp(arg[0], exit_w) == 0) {
			check = true;														// Assign true to exit
			for (int i = 0; i < child_counter; i++) {					// Kill any process that are leftover
				kill(process_c[child_counter], SIGTERM);
			}
		}

		// "Non-built" COMMAND
		// If non-built command received, then fork and make a child process to help with the non-built command
		else {
			spawnPid = fork();												//	Fork the parent's process and a make a cpy of process
			
			if (spawnPid == -1) {											// If spawnPid is -1, no child process created and then exit
				perror("Hull breach!\n");
				exit(1);
			}

			else if (spawnPid == 0) {										// Child process created successfully

				SIGINT_action.sa_handler = SIG_DFL;						// Child process will take ^C as default in the child process
				sigaction(SIGINT, &SIGINT_action, NULL);
			
				int i = 0;														// Create an int for the looping and an int for redirection
				int chk_d = 0;													
				for (i = 0; i < arg_cmd; i++) {
					if (strcmp(arg[i], "<") == 0 || strcmp(arg[i], ">") == 0) {		// If the cmd detect the '<' or the '>', break off
						break;
					}

					else {														// Else, copy the command and store it in a string
						arg_s[chk_d] = arg[i];
						chk_d = chk_d + 1;
					}
				}
	
				int f_des = 0;
				while (i < arg_cmd) {
					if (strcmp(arg[i], "<") == 0) {					// If one of the command is a <
						f_des = open(arg[i+1], O_RDONLY);			// Open file and make it a read-only file
						if (f_des == -1) {
							printf("Can't open file for input\n");
							exit(1);
						}

						int final_data = dup2(f_des, 0);				// Move the process to point to the cpy data
						if (final_data == -1) {
							printf("Error in pointing the child process\n");
							exit(1);
						}
						i = i + 2;
					}

					if (strcmp(arg[i], ">") == 0) {					// If one of the command is a >
						f_des = open(arg[i+1], O_WRONLY | O_CREAT | O_TRUNC, 0644);	// Open file and make it a write file
						if (f_des == -1) {
							printf("Can't open file for input\n");
							exit(1);
						}

						int final_data = dup2(f_des, 1);				// Move the process to point to the cpy data
						if (final_data == -1) {
							printf("Error in pointing the child process\n");
							exit(1);
						}
						i = i + 2;
					}

					if ((background == true) && (strcmp(arg[i], "<") != 0 || strcmp(arg[i], ">") != 0)) {
						f_des = open("/dev/null", O_RDONLY);       //Redirect to NULL if no file write or read
                  if(f_des == -1) {
                  	perror("open error\n");
                  	exit(1);
                  }
	
                  if(dup2(f_des, 0) == -1) {
                   	perror("dup2 error \n");
                     exit(1);
                  }
					}
				}

				arg_s[chk_d] = '\0';											// Reset the string array to be empty
				execvp(arg_s[0], arg_s);
				if (execvp(arg_s[0], arg_s) == -1) {				 	// If the command can not be found print out error
					printf("No such file(s) or directory.\n");
					fflush(stdout);
					exit(2);
				}
				fcntl(f_des, F_SETFD, FD_CLOEXEC); 						// Close the file pointer
			}

			else {																// Default parent's process
				if (foreground == true || background == false) {
					waitpid(spawnPid, &childExitMethod, 0);			// Telling parent's process to wait until child process exit
					if (WIFEXITED(childExitMethod)) {					// If child exit normally and succeed
						statusExit = WEXITSTATUS(childExitMethod);	// Assign the exit status to the statusExit
						terminate = 0;											// Reset terminate signal to 0 as no signal was interrupt
					}

					else {														// Child exit abruptly, print out the signal status
						int signal_stat = WTERMSIG(childExitMethod);
						printf("terminated by signal %d\n", signal_stat);
						fflush(stdout);
						terminate = signal_stat;							// Assign the singal to terminate var
					}
				}

				else if (background == true) {							// Else if background is active
					bg_child[child_counter] = spawnPid;					// Store child into an array for later deletion
					process_c[child_counter] = spawnPid;
					waitpid(spawnPid, &childExitMethod, WNOHANG);
					printf("Background pid is %d\n", spawnPid);		// Get the background PID
					fflush(stdout);											// Flush out buffer
					child_counter = child_counter + 1;					// Keep track of how many child have not end their process
					background = false;										// Reset the background process	
				}
			}
		}

		// This part keep track looping through and end any specific child process need to end, so we don't end up with
		// zombie child process
		for (int i = 0; i < child_counter; i++) {
			if (waitpid(bg_child[i], &childExitMethod, WNOHANG) > 0) {
				if (WIFEXITED(childExitMethod)) {							// If child exit normally, print PID and exit status
					printf("background pid %d is done: exit value %d\n", spawnPid, WEXITSTATUS(childExitMethod));
					fflush(stdout);												// Flush out buffer
					statusExit = WEXITSTATUS(childExitMethod);			// Copy the exit value into a var
				}

				if (WIFSIGNALED(childExitMethod)) {							// Else there was an abrupt interrupt
					printf("background pid %d is done: terminated by signal %d\n", spawnPid, WTERMSIG(childExitMethod));
					fflush(stdout);												// Flush out buffer
					terminate = WTERMSIG(childExitMethod);					// Copy the signal into a var
				}
			}
		}
	} while (check == false);													// Keep running the program if bool is false

	// Exiting the actual program
	return 0;
}


/*********************************************************
 *
 * Function: "void catchSIGTSTP(int signo)"
 * Detail(s): This function will act as an interrupt to
 *        	  turn on foreground or not.
 *
 ********************************************************/
void catchSIGTSTP(int signo) {

	if (foreground == false) {
		char* message1 = "\nEntering foreground-only mode (& is now ignored)\n";
		write(STDOUT_FILENO, message1, 50); 		// Write out the message
		fflush(stdout);									// Flush out buffer
		fflush(stdin);
		foreground = true;								// Assign the foreground to true
	}

	else if (foreground == true) {
		char* message2 = "\nExiting foreground-only mode\n";
		write(STDOUT_FILENO, message2, 30);			// Write out the message
    	fflush(stdout);									// Flush out buffer
		fflush(stdin);
		foreground = false;								// Assign foreground as false
	}
}


/*********************************************************
 *
 * Function: "void printStat(int stat)"
 * Detail(s): This function will print out the exit status
 * 			  or the signal that has been terminated.
 *
 ********************************************************/
void printStat(int exitStat, int *sig) {

	if (foreground == false) {							// If this command run before foreground
		printf("exit value 0\n");						// Print exit stat to 0
		fflush(stdout);									// Flush out buffer
	}

	else {													// Else not print the status
		if (sig != 0) {									// If the signal is not 0, then return the terminate signal
			printf("terminated by signal %d\n", *sig);
			fflush(stdout);								// Flush out buffer
			*sig = 0;										// Reset signal to 0
		}

		else {												// If the child exit normally return exit value
			printf("exit value %d\n", exitStat);
			fflush(stdout);
		}
	}
}

