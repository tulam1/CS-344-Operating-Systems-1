/**********************************************************************************
 * Program: enc_client.c
 * Author: Tu Lam
 * Date: March 1st, 2021
 * Description: This file contains code in Cs, and it is the program where it has a
 * 				 main function will run by accepting a command line to convert to str
 *         		 and use that to encrypt messages through the client side.
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
#include <netdb.h>
#include <stdbool.h>

// Declaration of all other functions needed for the main() program
void invalid_char(char *filename, int file_size);

// A lot of the codes is refer to the sample code given in class, below this comment is the reference URL.
// Reference: https://repl.it/@cs344/83clientc?lite=true#client.c

/***********************************************************
 *
 * Function: "int main(int argc, char *argv[])"
 * Detail(s): This function will deal with the client side
 * 			  aspect by encypting message send by the client
 * 			  and see if it can send to the server side.
 * 			  
 **********************************************************/
int main(int argc, char *argv[]) {

	// Check to see at least 4 or more arguments are enter on the cmd line
	if (argc < 4) {
		printf("\nERROR! Please provide at least 4 arguments in cmd line\n");
		fprintf(stderr, "USAGE: './%s plaintext keygen port'\n", argv[0]);
		exit(1);
	}

	// Create the variables and struct needed for the client side
	int sock_FD = 0, port_num = 0, connect_FD = 0;
	int char_WR = 0, char_RD = 0;
	char key[80000], text[80000];
	struct sockaddr_in serverAddr;
	struct hostent* serverHost;
	char buffer[80000];

	// Set up the socket and see if it create successfully
	sock_FD = socket(AF_INET, SOCK_STREAM, 0);					// Create the socket
	if (sock_FD == -1) {													// If it fail, exit
		fprintf(stderr, "Hull Breach: When creating socket()\n");
		exit(1);
	}
 
  // Turn on the reusable PORT
	int reuse = 1;
	setsockopt(sock_FD, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int));

	// Set up the struct for server address
	port_num = atoi(argv[3]);											// Assign the port number after converting it into int
	memset((char *)&serverAddr, '\0', sizeof(serverAddr));	// Clear out the struct and reset it
	serverAddr.sin_family = AF_INET;									// Create network-capable socket
	serverAddr.sin_port = htons(port_num);							// Store the port number
	serverHost = gethostbyname("localhost");						// Convert str into special form of address
	if (serverHost == NULL) {											// Check to see if the host exist, if not, exit
		fprintf(stderr, "ERROR: No such host\n");
		exit(1);
	}
	// Copy in the address
	memcpy((char *)&serverAddr.sin_addr.s_addr, (char *)serverHost->h_addr, serverHost->h_length);

	// Connect to the server
	connect_FD = connect(sock_FD, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	if (connect_FD < 0) {												// If the connection isn't successfully connected to server, exit
		fprintf(stderr, "CLIENT: There was an error when connecting to the server\n");
		exit(1);
	}

	//	Find the length of the plaintext & keys
	int key_length = 0, text_length = 0;							// Create an int for the length of key & text
	memset(text, '\0', sizeof(text));								// Reset the key and text str to NULL
	memset(key, '\0', sizeof(key));

	// Read the plaintext file
	FILE *F_txt = fopen(argv[1], "r");								// Open to read plaintext file
	if (F_txt == NULL) {													// If file open is NULL, exit
		fprintf(stderr, "ERROR: When opening '%s' file\n", argv[1]);
		exit(1);
	}
	fgets(text, 80000, F_txt);											// Read all the file texts
	fclose(F_txt);															// Close file once done reading
	text[strcspn(text, "\n")] = '\0';								// Look at the string and change every \n to \0
	
	// Read the keygen file
	FILE *F_key = fopen(argv[2], "r");								// Open to read keygen file
	if (F_key == NULL) {													// If file open is NULL, exit
		fprintf(stderr, "ERROR: When opening '%s' file\n", argv[2]);
		exit(1);
	}
	fgets(key, 80000, F_key);											// Read all the file texts
	fclose(F_key);															// Close file once done reading
	key[strcspn(key, "\n")] = '\0';									// Look at the string and change every \n to \0

	key_length = strlen(key);											// Get the size of the file from key & text
	text_length = strlen(text);
	if (key_length < text_length) {									// If key is shorter than text, print error
		fprintf(stderr, "ERROR: key '%s' is too short!\n", argv[2]);
		exit(1);
	}

	// Checking for invalid for both key & text file, if so report an error
	invalid_char(text, text_length);
	invalid_char(key, key_length);

	// Splice the message together
	memset(buffer, '\0', sizeof(buffer));							// Reset the buffer to NULL
	char check_enc[] = "enc_client";									// Create a string to check if the server connection is correct
	sprintf(buffer, "%s$%s$%s", text, key, check_enc);			// Combine the message together
	
	// Send the data in buffer to the server
	bool data_ck = false;												// Create a bool for checking if data has 

	do {																		// Do a do-while loop for sending data
		int tmp_WR = 0;													// Create a tmp_WR
		tmp_WR = send(sock_FD, buffer, strlen(buffer), 0);		// Send data and assign an int
		if (tmp_WR < 0) {													// If data was not written, then exit
			fprintf(stderr, "CLIENT: ERROR when writing to socket\n");
			exit(1);
		}

		char_WR += tmp_WR;												// Update the char_WR with the tmp_WR

		if (char_WR < strlen(buffer)) {								// If the data have not finish sending the whole thing, give warning
			printf("WARNING: Not all data have been writing to the server\n");
			data_ck = false;												// Assign it as false as not the entire data haas send
		}

		else if (char_WR >= strlen(buffer)) {						// If all data has send, break out of the loop
			data_ck = true;
		}
	} while (data_ck == false);

	// Receive the message back from server
	memset(buffer, '\0', sizeof(buffer));							// Clear out the buffer again
	char tmp_str[80000];													// Create a tmp string to copy after data receive

	do {
		memset(tmp_str, '\0', sizeof(tmp_str));					// Reset and clear string
		char_RD = recv(sock_FD, tmp_str, sizeof(tmp_str), 0);	// Read data from socket
		if (char_RD < 0) {												// If nothing received, then exit
			fprintf(stderr, "CLIENT: ERROR reading from the socket\n");
			exit(1);
		}

		strcat(buffer, tmp_str);										// Catenate the string together
	} while (buffer[strlen(buffer) - 1] != '\n');

	// Check to see if connecting to the right server
	if (buffer[0] == '\n') {											
		fprintf(stderr, "ERROR: Connecting to the wrong server at port [%d]\n", port_num);
		exit(2);
	}

	else {																	// If it connect correctly, stdout the receiving message
		printf("%s", buffer);											// Print out the content in buffer after receiving it
	}

	close(sock_FD);														// Close the socket
	return 0;																// Program is done running
}


/****************************************************************
 *
 * Function: "void invalid_char(char *filename, int file_size)"
 * Detail(s): This function will deal with the checking if the
 * 			  file that was given have any bad characters. If so,
 * 			  program will prompt an error
 * 			  
 ***************************************************************/
void invalid_char(char *filename, int file_size) {

	// Create a string to hold the alphabet to check for invalid char
	char alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";

	for (int i = 0; i < file_size; i++) {							// Create a loop to loop through the length size of file
		int count = 0;														// Create a counter for the count of invalid char
		for (int j = 0; j < 27; j++) {								// Create a loop for the alphabet check
			if (filename[i] != alphabet[j]) {						// If the alphabet does not match, increment count
				count += 1;
			}

			else if (filename[i] == alphabet [j]) {				// Else if it matches, break out and move to the next char
				break;
			}
		}

		if (count > 26) {													// If the count is higher than 26 meaning none char is match, exit
			fprintf(stderr, "ERROR: There was an invalid character in the file\n");
			exit(1);
		}
	}
}
