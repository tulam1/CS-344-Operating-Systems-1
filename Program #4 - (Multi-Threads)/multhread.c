/**********************************************************************************
 * Program: multhread.c
 * Author: Tu Lam
 * Date: February 21st, 2021
 * Description: This file contains code in Cs, and it is the main program where the
 * 				 main function will run a multi-thread process by dipslaying the input
 *         		 and output redirect.
 *********************************************************************************/

// Defining all the header directives needed for this program
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>

// Most of the code will be a reference from the example code provided by the class
// Reference: https://repl.it/@cs344/65prodconspipelinec

// Define const variables
#define MAX_LINE 50		// Input line will have no more than 49 lines before stop-processing line plus the 
#define MAX_CHAR 1000	// Input will have no more than 1000 characters (including the line seperator)
#define OUT_LINE 80		// Output line no more than 80 characters per line

// Global variable for the first buffer
char buffer1[MAX_CHAR]; // A shared resource for the input thread and line seperator
int count_1 = 0;			// A counter to count the number of item in the buffer
pthread_mutex_t mutex_1 = PTHREAD_MUTEX_INITIALIZER;		// Initialize the mutex for buffer1
pthread_cond_t data_1 = PTHREAD_COND_INITIALIZER;			// Initialize the condition variable for buffer1

// Global variable for the second buffer
char buffer2[MAX_CHAR]; // A shared resource for the line seperator and plus sign
int count_2 = 0;			// A counter to count the number of item in the buffer
pthread_mutex_t mutex_2 = PTHREAD_MUTEX_INITIALIZER;		// Initialize the mutex for buffer2
pthread_cond_t data_2 = PTHREAD_COND_INITIALIZER;			// Initialize the condition variable for buffer2

// Global variable for the third buffer
char buffer3[MAX_CHAR]; // A shared resource for the plus sign and output thread
int count_3 = 0;			// A counter to count the number of item in the buffer
pthread_mutex_t mutex_3 = PTHREAD_MUTEX_INITIALIZER;		// Initialize the mutex for buffer3
pthread_cond_t data_3 = PTHREAD_COND_INITIALIZER;			// Initialize the condition variable for buffer3


/*********************************************************
 *
 * Function: "char *get_buffer1()"
 * Detail(s): This function is get the buffer1 info.
 *
 ********************************************************/
char *get_buffer1() {
	
	pthread_mutex_lock(&mutex_1);					// Lock the mutex before checking if the buffer has data

	while (count_1 == 0) {							// If the counter still hold 0, mean buffer is empty
		pthread_cond_wait(&data_1, &mutex_1);	// Buffer is empty, waiting for producer to signal that buffer has data
	}
	
	char p_data[MAX_CHAR];							// Declare an empty string holding MAX_CHAR
	strcpy(p_data, buffer1);						// Copy the data from bufer and store it into the string that was just created
	count_1 = count_1 - 1;							// Decrement the counter after storing the data
	pthread_mutex_unlock(&mutex_1);				// Unlock the mutex

	return p_data;										// Return the string with value
}


/*********************************************************
 *
 * Function: "void put_buffer1(char *value)"
 * Detail(s): This function act as producer to put thing
 * 			  into the buffer.
 *
 ********************************************************/
void put_buffer1(char *value) {

	pthread_mutex_lock(&mutex_1);					// Lock the mutex before putting an item into the buffer
	int size = strlen(value);
	for (int i = 0; i < size; i++) { 			// Make a for loop with size of the string pass through to put item into buffer
		buffer1[i] = value[i];						// Copy value from string and put it into the buffer1
	}

	count_1 = count_1 + 1;							// Increment the counter
	pthread_cond_signal(&data_1);					// Signal consumer that the buffer is not empty anymore
	pthread_mutex_unlock(&mutex_1);				// Unlock the mutex
}


/*********************************************************
 *
 * Function: "char *get_buffer2()"
 * Detail(s): This function is get the buffer2 info.
 *
 ********************************************************/
char *get_buffer2() {

	pthread_mutex_lock(&mutex_2);					// Lock the mutex before checking if the buffer has data

	while (count_2 == 0) {							// If the counter still hold 0, mean buffer is empty
		pthread_cond_wait(&data_2, &mutex_2);	// Buffer is empty, waiting for producer to signal that buffer has data
	}
	
	char p_data[MAX_CHAR];							// Declare an empty string holding MAX_CHAR
	strcpy(p_data, buffer2);						// Copy the data from bufer and store it into the string that was just created
	count_2 = count_2 - 1;							// Decrement the counter after storing the data
	pthread_mutex_unlock(&mutex_2);				// Unlock the mutex

	return p_data;										// Return the string with value
}


/*********************************************************
 *
 * Function: "void put_buffer2(char *value)"
 * Detail(s): This function act as producer to put thing
 * 			  into the buffer.
 *
 ********************************************************/
void put_buffer2(char *value) {

	pthread_mutex_lock(&mutex_2);					// Lock the mutex before putting it in a buffer
	strcpy(buffer2, value);							// Copy entire string into the buffer2
	count_2 = count_2 + 1;							// Increment the counter after adding in an item
	pthread_cond_signal(&data_2);					// Signal consumer that the buffer is not empty anymore
	pthread_mutex_unlock(&mutex_2);				// Unlock the mutex
}


/*********************************************************
 *
 * Function: "char *get_buffer3()"
 * Detail(s): This function is get the buffer3 info.
 *
 ********************************************************/
char *get_buffer3() {

	pthread_mutex_lock(&mutex_3);					// Lock the mutex before checking has data

	while (count_3 == 0) {							// If counter is 0, then the buffer is empty
		pthread_cond_wait(&data_3, &mutex_3);	// Is empty, waiting for producer to signal that buffer has data
	}

	char p_data[MAX_CHAR];							// Declare an empty string 
	strcpy(p_data, buffer3);						// Copy the data from buffer3 into p_data
	count_3 -= 1;										// Decrement the counter
	pthread_mutex_unlock(&mutex_3);				// Unlock the mutex

	return p_data;										// Return the copy string p_data
}


/*********************************************************
 *
 * Function: "void put_buffer3(char *value)"
 * Detail(s): This function act as producer to put thing
 * 			  into the buffer.
 *
 ********************************************************/
void put_buffer3(char *value) {

	pthread_mutex_lock(&mutex_3);					// Lock the mutes before putting it in a buffer
	strcpy(buffer3, value);							// Copy the value into the buffer3
	count_3 += 1;										// Increment the counter each time copy a value
	pthread_cond_signal(&data_3);					// Signal consumer that the buffer is not empty anymore
	pthread_mutex_unlock(&mutex_3);				// Unlock the mutex
}


/*********************************************************
 *
 * Function: "char *get_user_input()"
 * Detail(s): This function just let user enter in the str.
 *
 ********************************************************/
char *get_user_input() {

	char user_input[MAX_CHAR];						// Create a string holding the max characters input
	printf(": ");										// Print out the command line to enter
	fflush(stdout);									// Flush out buffer
	fgets(user_input, MAX_CHAR, stdin);			// Read the user input from stdin

	return user_input;								// Return the string once the user has finish entering it
}


/*********************************************************
 *
 * Function: "void *input_thread(void *args)"
 * Detail(s): This function runs the first thread.
 *
 ********************************************************/
void *input_thread(void *args) {

	for (int i = 0; i < MAX_CHAR; i++) { 		// If i has not reach the MAX_CHAR, get user input
		char *value = get_user_input();			// Go to the get_user_input() function and let user enter in a command
		if (strcmp(value, "STOP\n") == 0) {		// If the string compare value and find that if it end with "STOP\n"
			put_buffer1(value);						// Store the last value into buffer before break the for loop
			break;										// This is the end of the input and break out of the for loop
		}

		else {											// Else continue to put item value into the buffer1
			put_buffer1(value);						// Continue to store the value into buffer
		}
	}

	return NULL;										// This function return nothing
}


/*********************************************************
 *
 * Function: "char *line_divide(char *value)"
 * Detail(s): This function look at \n and add space.
 *
 ********************************************************/
char *line_divide(char *value) {

	for (int j = 0; j <= strlen(value); j++) {
		if (value[j] == '\n') {						// If the string have a \n char, than change it to space
			value[j] = ' ';							// Assign space to that spot
		}
	}

	return value;										// Return the string with replacing the line with space
}


/*********************************************************
 *
 * Function: "void *line_separator(void *args)"
 * Detail(s): This function runs the second thread.
 *
 ********************************************************/
void *line_separator(void *args) {

	char *value;										// Create a string for storing the info from buffer1
	for (int i = 0; i < MAX_LINE; i++) {		// Loop through the max 50 lines for input
		value = get_buffer1();						// Copy the entire data from buffer into the temp string
		if (strcmp(value, "STOP\n") == 0) {		// If the string counter the word STOP
			put_buffer2(value);						// Put the last value into the buffer2 and break out of loop
			break;
		}

		else {
			line_divide(value);						// Check for line divide to change it into space
			put_buffer2(value);						// If it has not reach the word "STOP", keep putting in buffer
		}
	}

	return NULL;										// Return nothing
}


/*********************************************************
 *
 * Function: "char *replace_plus(char *value)"
 * Detail(s): This fucntion replace the ++ with ^.
 *
 ********************************************************/
char *replace_plus(char *value) {
	char tmp[strlen(value) - 1];					// Create a temp string to hold the size of the passing-in string

	while (strstr(tmp, "++") != NULL) {			// While the string hit a ++ sign
		memset(tmp, '\0', sizeof(tmp));			// Reset set tmp string to 0

		for (int i = 0; i < strlen(value); i++) {
			if (value[i] == '+' && value[i + 1] == '+') {	// If the ++ sign appear in the string
				strncpy(tmp, value, i);								// Copy the value string into the tmp string at that index
				strcat(tmp, "^");										// Combine the ^ into the + sign
				int i1 = i + 2;										// Create a tmp index ptr to point 2 index at i
				int i2 = strlen(tmp);								// Create another ptr to hold the sizeof the string

				while (i1 < (strlen(value) - 1)) { 				// While the first ptr is still less than sizeof value str
					tmp[i2++] = value[i1];							// Assign value at that new index while increment new space
					i1 += 1;												// Increment index ptr by 1 until it is no longer is smaller than str value
				}

				i2 -= 1;													// We need to decrement the sizeof tmp str

				for (int k = 0; k <= i2; k++) {					// Loop through the sizeof tmp string and replace every \n w/ space
					if (tmp[k] == '\n') {
						tmp[k] = ' ';
					}
				}

				memset(value, '\0', sizeof(value));				// Clear the value item with 0
				strcat(value, tmp);									// Catenate the tmp str value into the reset str
				break;													// break out of the entire loop
			}
		}
	}

	return value;										// Return the value after replace the plus sign
}


/*********************************************************
 *
 * Function: "void *plus_sign(void *args)"
 * Detail(s): This function runs the third thread.
 *
 ********************************************************/
void *plus_sign(void *args) {
	
	char *value;										// Create a temporary string to put in from buffer2
	for (int i = 0; i < MAX_LINE; i++) {
		value = get_buffer2();						// Get the buffer2 data and store it into the value
		if (strcmp(value, "STOP\n") == 0) {
			put_buffer3(value);						// Put the last value into the buffer3 and break out
			break;
		}

		else {											// Else, put more value into the buffer
			replace_plus(value);						// Replace the + sign with ^
			put_buffer3(value);
		}
	}

	return NULL;										// Nothing need to be return here
}


/*********************************************************
 *
 * Function: "char *print_output(char *value)"
 * Detail(s): This function runs displaying the output of
 * 			  the input in the beginning of the program.
 *
 ********************************************************/
char *print_output(char *value) {

	char tmp[MAX_CHAR];								// Create a temp string
	memset(tmp, '\0', sizeof(tmp));				// Set string to hold NULL
	int count = strlen(value);						// Assign the length to hold of the value

	while (count >= OUT_LINE) {					// While the length of string is greater than 80
		printf("Display: ");
		fflush(stdout);
		write(1, value, 80);							// Write out the value at sizeof 80 at index 1
		write(1, "\n", 2);							// Write out the \n line at sizeof 2 at index 1
		int ptr = 79;									// Create an int holding the rest of the line

		for (int i = 0; i < strlen(value); i++) {
			if (value[ptr++] != '\0') {			// If the value at that index hit 80 and does not encounter the \0
				tmp[i] = value[ptr];					// Copy that value into the tmp string to hold it and divide it up
			}

			else {										// Else, if it does, break out of it
				break;				
			}
		}

		memset(value, '\0', sizeof(value));		// Reset the value to 0
		strcpy(value, buffer3);						// Copy the rest value from buffer3 into the value after reset
		ptr = ptr - 80;								// Subtract ptr of 80
	}

	return value;										// Set to return the string
}


/*********************************************************
 *
 * Function: "void *output_thread(void *args)"
 * Detail(s): This function runs the fourth thread.
 *
 ********************************************************/
void *output_thread(void *args) {

	char *value;										// Create a string named it value
	char temp[MAX_CHAR];								// Create a tmp string holding max char
	memset(temp, '\0', sizeof(value));			// Set the string to 0
	
	for (int i = 0; i < MAX_LINE; i++) {
		value = get_buffer3();						// Get each value from the buffer3 
		strcat(temp, value);							// Copy the value and catenate with the temp string
		if (strstr(temp, "STOP\n") != NULL) {	// If the sub-str have the word STOP 
			print_output(temp);
			break;										// Break out of the loop as we at the end of the sentence
		}

		else {											// Else, go print out the output
			print_output(temp);						// Go in to print output			
		}
	}

	return NULL;										// Return nothing
}


/*********************************************************
 *
 * Function: "int main()"
 * Detail(s): This function runs the multi-thread program.
 *
 ********************************************************/
int main() {

	// Declare var for the pthread
	pthread_t input_t, line_t, plus_t, output_t;
 
	// Reset buffer to make sure they are 0
	memset(buffer1, '\0', sizeof(buffer1));
	memset(buffer2, '\0', sizeof(buffer2));
	memset(buffer3, '\0', sizeof(buffer3));

	// Create the thread
	pthread_create(&input_t, NULL, input_thread, NULL);
	pthread_create(&line_t, NULL, line_separator, NULL);
	pthread_create(&plus_t, NULL, plus_sign, NULL);
	pthread_create(&output_t, NULL, output_thread, NULL);

	// Wait until thread finished executing
	pthread_join(input_t, NULL);
	pthread_join(line_t, NULL);
	pthread_join(plus_t, NULL);
	pthread_join(output_t, NULL);

	return 0;
}
