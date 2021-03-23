/**********************************************************************************
 * Program: movie_info.c
 * Author: Tu Lam
 * Date: January 17th, 2021
 * Description: This file contains code in Cs, and it is creating a link list
 * 				 where to dislay data from a text file and display the information
 * 				 in there base on the user's choice.
 *********************************************************************************/

//Defining all the header directives
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

/*********************************************************
 *
 * Function: "struct movie {};"
 * Detail(s): Defining the struct for the movie info.
 *
 ********************************************************/
struct movie {
	char *title;
	int year;	
	char *language;
	double rating;
	struct movie *next;
};


/**********************************************************
 *
 * Function: "struct movie *createMovie(char *current_Line)"
 * Detail(s): This struct function is to create a new 
 * 			  entry of each movie by allocating memory.
 * 			  Also use token to get the data and store in
 * 			  the movie struct.
 *
 *********************************************************/
struct movie *createMovie(char *current_Line) {

	// Allocate dynamic memory for a new movie, creating the linked list
	struct movie *curr_movie = malloc(sizeof(struct movie));

	// Create a temporary pointer, number holder, and a floating holder
	char *ptr;
	int temp_num;
	double num_double;
	char *str;

	// Getting the first data and store in the title
	char *token = strtok_r(current_Line, ",", &ptr);
	curr_movie->title = calloc(strlen(token) + 1, sizeof(char));		
	strcpy(curr_movie->title, token);				// Copy the data from token into the title

	// Getting the second data and store in the year
	token = strtok_r(NULL, ",", &ptr);
	temp_num = atoi(token);								// Convert the token into an int
	curr_movie->year = temp_num;						// Assign the year into the struct

	// Getting the third data and store in the language
	token = strtok_r(NULL, ",", &ptr);
	curr_movie->language = calloc(strlen(token) + 1, sizeof(char));
	strcpy(curr_movie->language, token);
	
	// Getting the fourth data and store in the rating
	token = strtok_r(NULL, "\n", &ptr);
	num_double = strtod(token, &str);				// Convert string value to a double
	curr_movie->rating = num_double;					// Assign the rating into the struct

	// Set the next node to NULL in creating a new movie entry
	curr_movie->next = NULL;

	return curr_movie;
}


/**********************************************************
 *
 * Function: "struct movie *parseFile(char *files, int *count)"
 * Detail(s): This struct function is to read a file from 
 * 			  an argument and parse the info and return the
 * 			  Linked List of the movie.
 *
 *********************************************************/
struct movie *parseFile(char *fileP, int *count) {

	// Opening a specific file from the argument "files"
	FILE *movieFile = fopen(fileP, "r");

	// Create the head & tail of linked list and assign as NULL
	struct movie *head = NULL;
	struct movie *tail = NULL;

	// Create a temp char, token, length, and read variable
	char *temp = NULL;
	char *token;
	size_t length = 0;									// Create an unsigned type
	ssize_t read;											// Create an signed type

	// This is where it is reading the file line by line
	while ((read = getline(&temp, &length, movieFile)) != -1) {

		// This is a counter to count how many movies are in the file
		*count = *count + 1;

		// Create a new node of the line of movie information
		struct movie *new_node = createMovie(temp);

		// If the head is NULL, assign the new_node to both head & tail
		if (head == NULL) {
			head = new_node;
			tail = new_node;
		}

		// If the head is not NULL, update the tail to point to new content
		// and the tail->next to point to the the new content
		else {
			tail->next = new_node;
			tail = new_node;
		}
	}
	
	// Free up the memory that was allocated
	free(temp);
	fclose(movieFile);									// Close the file after finish reading

	return head;	
}


/**********************************************************
 *
 * Function: "void movie_by_year(struct movie * data)"
 * Detail(s): This function is to print out movie(s) based
 * 			  on the user input in the year.
 *
 *********************************************************/
void movie_by_year(struct movie *data) {

	// Create a year input and a boolean
	int year_input = 0;
	bool stop_print = false;

	printf("\nEnter the year for all the movie(s) to display in that year.\n");
	printf("Your entry: ");
	scanf("%d", &year_input);

	// Check to see if the Linked List is NULL or not
	while (data != NULL) {

		// If the year_input matches with the year in data, print the title of movie
		if (year_input == data->year) {
			printf("%s\n", data->title);
			stop_print = true;
		}

		// Assign to point to the next node
		data = data->next;
	}

	// If the year_input doesn't match and boolean still false, print no result
	if (stop_print == false) {
		printf("No data of any movie(s) was found in the year '%d'.\n", year_input);
	}
} 


/**********************************************************
 *
 * Function: "void movie_by_rating(struct movie *data)"
 * Detail(s): This function is to print out movie(s) based
 * 			  on the highest rating in each year.
 *
 *********************************************************/
void movie_by_rating(struct movie *data) {

	// Create a variable for the "for" loop
	int i = 0;

	// "for" loop going through the year 1900 to 2021 as the frame of when movies released
	for (i = 1900; i <= 2021; i++) {
		
		// Create two structs one hold the copy and on hold the highRate
		struct movie *cpy = NULL;
		struct movie *highRate = NULL;

		// Since we loop through the loop, we need to copy our data so it can start over again
		// as the year increment
		struct movie *data2 = data;

		// Loop through that copy in data2
		while (data2 != NULL) {
			// checking if the year match with "for" loop, if so copy the content of that year into the cpy struct
			if (data2->year == i) {
				cpy = data2;

				// Iterate through this cpy data and see if the year matches with the year on data2
				while (cpy != NULL) {
					if (cpy->year == data2->year) {
						// If the rating of cpy is higher than data2, then copy the 'cpy' into the highRate
						if (cpy->rating >= data2->rating) {
							highRate = cpy;
						}

						// Else if it is lower, then the data in 'data2' have the highest and copy from that
						else if (cpy->rating < data2->rating) {
							highRate = data2;
						}
					}

					// If the year doesn't match from both cpy and data2, move onto the next node
					cpy = cpy->next;
				}

				// Here is the printing process
				printf("%d  %.1f  %s\n", highRate->year, highRate->rating, highRate->title);
				break;
			}

			// If the year doesn't match, move onto the next node
			else {
				data2 = data2->next;
			}
		}
	}
}		

/**********************************************************
 *
 * Function: "void movie_by_language(struct movie *data)"
 * Detail(s): This function is to print out movie(s) based
 * 			  on the user input in what the language is.
 *
 *********************************************************/
void movie_by_language(struct movie *data) {

	// Create a string input size of 20, a boolean, and copy string
	char lang[20];
	bool stop_lang = false;
	
	// Ask user to input in a language
	printf("\nEnter a language for which movie(s) you want to see.\n");
	printf("Your entry: ");
	scanf("%s", lang);

	// If the data is not NULL, print out the entry match on the string
	while (data != NULL) {

		// Use the strstr() to compare substring
		if (strstr(data->language, lang)) {
			printf("%d  %s\n", data->year, data->title);
			stop_lang = true;
		}

		// Move to the next node
		data = data->next;
	}

	if (stop_lang == false) {
		printf("No data about movies released in this the language '%s'.\n", lang);
	}
}

/**********************************************************
 *
 * Function: "void main_menu(struct movie *mov_data)"
 * Detail(s): This function is to display the option to the 
 * 			  user and asking them for input to display
 * 			  the result based on their option.
 *
 *********************************************************/
void main_menu(struct movie *mov_data) {

	// Declare an int variable for the input and a boolean
	int num_input = 0;
	bool stop_prog = false;

	do {

		// Display the menu options of the program & get user's input
		printf("\nPlease choose from one of the option below.\n");
		printf("\n1. Show movies released in the specified year.\n");
		printf("2. Show the highest rated movie for each year.\n");
		printf("3. Show the title & year of release of all movies in a specific language.\n");
		printf("4. Exit from the program.\n");
		printf("\nYour entry: ");
		scanf("%d", &num_input);	

		if (num_input == 1) {
			movie_by_year(mov_data);
			stop_prog = false;
		}

		else if (num_input == 2) {
			movie_by_rating(mov_data);
			stop_prog = false;
		}

		else if (num_input == 3) {
			movie_by_language(mov_data);
			stop_prog = false;
		}

		else if (num_input == 4) {
			printf("\nThank you for using the program! the program will exit now...\n");
			printf("\n");
			stop_prog = true;
		}
	
		else {
			printf("\nThe entry you have enter is either out-of-range of our option / doesn't match with our option.\n");
			printf("Please try again.\n");
		}

	} while (stop_prog == false);
}
