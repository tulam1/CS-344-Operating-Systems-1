/**********************************************************************************
 * Program: enc_server.c
 * Author: Tu Lam
 * Date: March 1st, 2021
 * Description: This file contains code in Cs, and it is the program where it has a
 * 				 main function will run by accepting a command line to convert to str
 *         		 and use that to encrypt messages through the server side.
 *********************************************************************************/

// Define all the library header needed for the program
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <sys/wait.h>

// A lot of the codes is refer to the sample code given in class, below this comment is the reference URL.
// Reference: https://repl.it/@cs344/83serverc?lite=true#server.c

/***********************************************************
 *
 * Function: "int main(int argc, char *argv[])"
 * Detail(s): This function will deal with the server side
 * 			  aspect by encypting message send by the client
 * 			  and allowing up to 5 connections.
 *
 **********************************************************/
int main(int argc, char *argv[]) {

	// Check to see at least 2 or more arguments are enter on the cmd line
	if (argc < 2) {
		printf("\nERROR! Please provide at least 2 arguments in cmd line\n");
		printf("USAGE: './%s port#'\n", argv[0]);
		printf("\n");
		exit(1);
	}

	// Create a file descriptor for listen & connection sockets, create a struct for server address & clientAddr,
	// an int for port number, a bind checker, and a char for string buffer, and clientInfo socklen_t.
	int listen_sock = 0, connect_sock = 0, bind_check = 0;
	int port_num = 0;
	int char_RD = 0, char_WR = 0;
	char buffer[80000], tmp_str[80000];
	socklen_t clientInfo;
	struct sockaddr_in serverAddr, clientAddr;
	char alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";
	bool data_ck = false;

	// Create a pid for forking process, and an int for status
	pid_t pid;
	int status = 0;

	// Set up the server address
	memset((char *)&serverAddr, '\0', sizeof(serverAddr));		// Clear and set 0 of the struct serverAddr
	port_num = atoi(argv[1]);												// Assign port number into the int port_num
	serverAddr.sin_family = AF_INET;										// Create a network-capable socket
	serverAddr.sin_port = htons(port_num);								// Store the port number
	serverAddr.sin_addr.s_addr = INADDR_ANY;							// Any address can be used for connection

	// Set up the socket
	listen_sock = socket(AF_INET, SOCK_STREAM, 0);					// Create the socket
	if (listen_sock < 0) {													// Check to see if socket was created successfully
		fprintf(stderr, "Hull breach: ERROR when using socket()\n");		// Exit if it fail
		exit(1);
	}
 
  // Turn on the reusable PORT
	int reuse = 1;
	setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int));

	// Turn on the function for socket to listen
	bind_check = bind(listen_sock, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
	if (bind_check < 0) {													// Check to see there is any error
		fprintf(stderr, "Hull Breach: ERROR when trying to use bind()\n");// Exit if it fail
		exit(1);
	}

	// Let the socket listen up to 5 connections
	listen(listen_sock, 5);													// Turn it on and now can received up to 5 connections
	int process_run = 0;														// Create a counter to count the number of process runnig

	// Enter into the loop to cipher the text given
	while(1) {
		// Accept the connection and block if one if not available until one connect
		clientInfo = sizeof(clientAddr);									// Get the size of the client Address
		connect_sock = accept(listen_sock, (struct sockaddr *)&clientAddr, &clientInfo);
		if (connect_sock == -1) {
			fprintf(stderr, "ERROR: On accepting connection\n");	// Print error if accept connection is bad
			exit(1);
		}
		
		if (process_run <= 5) {												// If the number of process running is less than 5
			// Time to fork the process
			pid = fork();
			
			if (pid == -1) {													// Exit if there is an error when forking
				fprintf(stderr, "ERROR: Occur when forking()\n");
				exit(1);
			}

			else if (pid == 0) {												// If the child process created, run the cipher
				memset(buffer, '\0', sizeof(buffer));					// Clear out buffer
				char text[80000], key[80000], keyword[80000], cipher[80000];		
				memset(text, '\0', sizeof(text));						// Clear out all the str created
				memset(key, '\0', sizeof(key));
				memset(keyword, '\0', sizeof(keyword));

				do {																// Received all the messages
					memset(tmp_str, '\0', sizeof(tmp_str));			// Clear out the tmp string
					char_RD = recv(connect_sock, tmp_str, sizeof(tmp_str), 0);
					if (char_RD < 0) {
						fprintf(stderr, "ERROR: Trying to get data from client\n");
						exit(1);
					}
					strcat(buffer, tmp_str);
				} while (buffer[strlen(buffer) - 1] != 't');			// Reach until reaching the last letter in the str 't'

				// Divide the string up into it own component
				sscanf(buffer, "%[^'$']$%[^'$']$%s", text, key, keyword);

				if (strcmp(keyword, "enc_client") != 0) {				// If keyword doesn't match, send it back to client
					send(connect_sock, "\n", 2, 0);						// Send that connecting to wrong server
					exit(1);
				}

				// Time to cipher the text
				memset(cipher, '\0', sizeof(cipher));					// Clear out the cipher string
				for (int i = 0; i < strlen(text); i++) {				// Loop through the plaintext
					int n1 = 0, n2 = 0, sum = 0;							// Create the int to cipher
					for (int j = 0; j < 27; j++) {						// Loop through the alphabet
						if (text[i] == alphabet[j]) {						// If alphabet letter matches one of the text
							n1 = j;												// Assign the index at j to n1
						}

						if (key[i] == alphabet[j]) {						// If a key matches one of the alphabet							
							n2 = j;												// Assign n2 to index at j
						}
					}
					sum = (n1 + n2) % 27;									// Add those two number together and divide by 27
					cipher[i] = alphabet[sum];								// Assign the cipher code char at that index from the alphabet
				}

				cipher[strlen(cipher)] = '\n';							// Assign a newline at the end of the cipher text

				// Send data back to enc_client
				do	{																// Do a do-while loop for sending data
					int tmp_WR = 0;														// Create a tmp_WR
					tmp_WR = send(connect_sock, cipher, strlen(cipher), 0);	// Send data and assign an int
					if (tmp_WR < 0) {														// If data was not written, then exit
						fprintf(stderr, "CLIENT: ERROR when writing to back\n");
						exit(1);
					}

					char_WR += tmp_WR;													// Update the char_WR with the tmp_WR

					if (char_WR < strlen(cipher)) {									// If the data have not finish, give warning
						printf("WARNING: Not all data have been writing to the server\n");
						data_ck = false;													// Assign it as false as not the entire data haas send
					}

					else if (char_WR >= strlen(cipher)) {							// If all data has send, break out of the loop
						data_ck = true;
					}
				} while (data_ck == false);
				
				close(connect_sock);											// Close the connection;
				exit(0);															// Exit the child
				break;
			}

			else {																// If the parents' is running, then increment process and break
				process_run += 1;												// Increment by one for the process
				break;															// Break out of the loop
			}
		}

		else if (process_run > 5) {										// If there more than 5 process, then give error
			fprintf(stderr, "ERROR: Number of active is process is 5. Waiting for one to complete\n");
		}

		// Loop to kill any child process left, do not let it become a zombie state
		for (int i = 0; i < 5; i++) {
			if (waitpid(-1, &status, WNOHANG) < 0) {
				fprintf(stderr, "ERROR: On the waitpid()\n");
			}

			if (WIFEXITED(status)) {										// If child exit normally, minus the process run
				process_run -= 1;
			}
		}
	}
	
	close(listen_sock);														// Close the socket
	return 0;																	// Program is done and ready to exit
}
