/**********************************************************************************
 * Program: keygen.c
 * Author: Tu Lam
 * Date: February 25th, 2021
 * Description: This file contains code in Cs, and it is the program where it has a
 * 				 main function will run by accepting a command line to convert a str
 *         		 of that number size to randomize letter in that spot.
 *********************************************************************************/

// Define all the library header needed for the program
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/***********************************************************
 *
 * Function: "int main(int argc, char *argv[])"
 * Detail(s): This function will generate a random key w/
 * 			  the letter of the alphabet based on the size
 * 			  of the key that the user enter from cmd line.
 *
 **********************************************************/
int main(int argc, char *argv[]) {

	// Declare an int for the genkey length, a num for index, and a constant char to hold the letter in the alphabet
	int length = 0;
	int num = 0;	
	char const_char[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";

	// Check to see if the user provided enough arguments in the cmd line
	if (argc < 2) {
		printf("\n");
		printf("ERROR: You provided not enough command argument, please try again!\n");
		printf("Example: '%s 10' or '%s 10 > mykey'\n", argv[0], argv[0]);
		printf("\n");
		exit(1);
	}

	srand(time(NULL));						// Declare a randomizer to run
	length = atoi(argv[1]);					// Convert the argument at index 1 to an integer and store it int length

	if (length <= 0) {						// If the length is 0 or below it, exit
		printf("ERROR: The number you give was too small! Please try again\n");
		exit(1);
	}

	char key[length];							// Declare a key char with size of length
	memset(key, '\0', sizeof(key));		// Reset the string to hold \0 in it not just junk

	for (int i = 0; i < length; i++) {	// Create a for loop to loop through the length of the key
		num = rand() % 27;					// Randomize a number from 0-26 (index) to find a letter to store in the key string
		key[i] = const_char[num];			// Go to that index and store it until the length is done
	}

	key[length] = '\0';						// Set the last length with '\0' to know that is the end
	printf("%s\n", key);						// Print it out for the stdin/stdout to a file

	return 0;									// Finish & exit the program
}
