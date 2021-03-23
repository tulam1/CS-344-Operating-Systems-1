/**********************************************************************************
 * Program: file.c
 * Author: Tu Lam
 * Date: January 20th, 2021
 * Description: This file contains code in C and the file contain the sub-code from
 * 				 the main program and provide all the behind-the-scene work of the
 * 				 whole program. This contain functions such as main_menu, sub_menu,
 * 				 files and directories, etc...				  				 
 *********************************************************************************/

// This include all the directive headers
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

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
 * Function: "struct movie *parseFile(char *fileP)"
 * Detail(s): This struct function is to read a file from 
 * 			  an argument and parse the info and return the
 * 			  Linked List of the movie.
 *
 *********************************************************/
struct movie *parseFile(char *fileP) {

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


/*********************************************************
 *
 * Function: "char *concatenate(char *str1, char *str2)"
 * Detail(s): This function combine and concatenate two
 * 			  string together.
 *
 ********************************************************/
char *concatenate(char *str1, char *str2) {
   char *cpy_str = malloc(strlen(str1) + strlen(str2) + 1); // Adding 1 '\0'
   strcpy(cpy_str, str1);												// Copy 1st string
   strcat(cpy_str, str2);												// Catenate it together

   return cpy_str;
}
// Reference: https://stackoverflow.com/questions/8465006/how-do-i-concatenate-two-strings-in-c


/*********************************************************
 *
 * Function: "void create_dir_file(char *file_data)"
 * Detail(s): This function hold the creation of the
 * 			  directory and store the movie file into the 
 * 			  their own ".txt" by year.
 *
 ********************************************************/
void create_dir_file(char *file_data) {
	
	// Create a declaration of string to num being convert from int to str, an int to check if directory is good,
	// the name str, and create an int to store the random number
	char num[30];
	int directory_ch = 0;
	long int rand_num = 0;
	char name[] = "lamtu.movies.";
	
	// Summon the generating tool from 0-99999
	srandom(time(NULL));
	rand_num = random() % 100000;					// Randomize a number from 0-99999
	sprintf(num, "%d", rand_num);					// Convert the random number into a string to concat

	// Concat the string of number to the "lamtu.movies." w/ the number
	char *dir_path = concatenate(name, num);
	directory_ch = mkdir(dir_path, 0750);		// Create a directory with permission rwxr-x---

	if (directory_ch == 0) {						// If checking directory == 0, dir create correctly
		printf("Created directory with the name '%s'\n", dir_path);
	}

	else {												// If not, quite out of the program
		printf("\nSorry, there was something wrong while create the directory. Program will exit. ..\n");
		exit(1);
	}

	// Now parse the data into the struct movie and have a str to hold in the value of the year
	// Also create a str hold the slash and the .txt
	struct movie *mov_data = parseFile(file_data);
	char year_num[50];
	char slash[] = "/";
	char txt[] = ".txt";
	FILE *ptr;

	// Create a DIR to open directory
	DIR* dir_fd;
	dir_fd = opendir(dir_path);							// Open the directory
	int desc = 0;												// Create an int for the descriptor of the dir

	if (dir_fd > 0) {											// Making sure the dir can be open
		for (int yr = 1900; yr <= 2021; yr++) {		// Create a "for" loop to loop through the movie file
			struct movie *cpy = mov_data;					// Create a copy of the movie file
			while (cpy != NULL) {
				if (cpy->year == yr) {								// If the year matches start create the file to store content of that year
					sprintf(year_num, "%d", yr);					// Convert the year into a str
					char *f1 = concatenate(dir_path, slash); 	// Concat the directory path with a "/"
					char *f2 = concatenate(year_num, txt);		// Concat the YYYY.txt
					char *f_path = concatenate(f1, f2);			// Concat both the dir_path and txt file together	
					ptr = fopen(f_path, "a");						// Open file and create to append it
					chmod(f_path, 0640);								// Give permission to file as rw-r-----

					if (ptr == NULL) {								// If file can't be created
						printf("\nSomething went wrong when trying to create '%s'. Program will exit\n", f_path);
						exit(1);
					}

					fprintf(ptr, "%s\n", cpy->title);	// Write the data into the txt file
					fclose(ptr);								// Close the file

					// Free all the strings that was concatenate
					free(f1);						
					free(f2);
					free(f_path);
				}

				// Move to the next node
				cpy = cpy->next;
			}
		}
	}

	closedir(dir_fd);											// Close the directory	
	free(dir_path);
}


/*********************************************************
 *
 * Function: "struct dirent *user_srfile()"
 * Detail(s): Defining the struct for finding if the file
 * 			  that is in the directory or not, based on the 
 *            user's choice.
 *
 ********************************************************/
struct dirent *user_srfile() {

	// Declare a string for user to enter,bool for checking valid file, a string check int,  and a return value
	char user_input[256];
	struct dirent *file_match;
	bool check = false;
	int str_check = 0;
	
	// Ask user for the file name
	printf("\nPlease enter the complete file name: ");
	scanf("%s", user_input);

	// Set up the DIR to read the directory and search through it
	DIR* dir_loc;
	struct dirent *dir_ptr;												// Create a directory pointer
	dir_loc = opendir(".");												// Open the directory that it is run in

	if (dir_loc > 0) {													// Check to see if directory can be open
		while ((dir_ptr = readdir(dir_loc)) != NULL) {			// Read the directory line by line
			str_check = strcmp(dir_ptr->d_name, user_input);	// Assign the value after compare string
			if (str_check == 0) {										// If the string enter matches with any file
				file_match = dir_ptr;									// Assign content at that stage to file_match
				check = true;												// Change check to true
			}
		}

		if (check == false) {											// If check is still false, no file were found
			printf("\nThe file '%s' was not found. Please try again.\n", user_input);
			return file_match = NULL;
		}
		printf("Now processing the chose file named '%s'\n", user_input);
		closedir(dir_loc);												// Close the directory
		return file_match;
	}	
}


/*********************************************************
 *
 * Function: "struct dirent *large_file()"
 * Detail(s): Defining the struct for finding the largest
 * 			  file.
 *
 ********************************************************/
struct dirent *large_file() {

	// Declare all the struct, stat, and DIR variable to find the smallest file
	DIR* dir_loc;										// Hold the directory we are starting at
	struct dirent *dir_ptr;							// Hold the sub-directory of the starting directory
	struct stat info1;								// Hold the information of the file size1
	struct stat info2;								// Hold the information of the file size2

	// Declare struct to compare two size files, bool to check, and a return struct
	struct dirent *biggest_file;
	struct dirent *tmp;
	bool check = false;

	// Other declare content to declare the prefix
	char target_prefix[] = "movies_";			// Hold the prefix needed
	char ext_prefix[] = ".csv";					// Hold extra prefix
	char newdir_name[256];							// Hold the new directory name w/ the same prefix
	off_t size1;										// Hold the file size value of the 1st one
	off_t size2;										// Hold the file size value of the 2nd one
	
	// Opening the directory
	dir_loc = opendir(".");

	// Check to see if the directory can be open
	if (dir_loc > 0) {
		while ((dir_ptr = readdir(dir_loc)) != NULL) {				// Checking each entry in directory
			// Check if the diretory matches w/ the prefix
			if ((strstr(dir_ptr->d_name, target_prefix) != NULL) && (strstr(dir_ptr->d_name, ext_prefix) != NULL)) {	
				stat(dir_ptr->d_name, &info1);							// Get the file info and put in info1
				size1 = info1.st_size;										// Assign the 1st value of size
				tmp = dir_ptr;													// Assign a copy of each entry in directory to a tmp				
				
				while ((tmp = readdir(dir_loc)) != NULL) {			// Check each entry again for the other files
					// Check if the diretory matches w/ the prefix
					if ((strstr(tmp->d_name, target_prefix) != NULL) && (strstr(tmp->d_name, ext_prefix) != NULL)) {	
						stat(tmp->d_name, &info2);							// Get the file info and put in info2
						size2 = info2.st_size;								// Assign the 2nd value of size

						// Now compare which file is bigger
						if (size1 <= size2) {								// If size2 if bigger and equal to size1
							biggest_file = tmp;								// Send the tmp to big_file
							check = true;
						}
	
						else {													// Else if it is the other way
							biggest_file = dir_ptr;							// Original file is bigger and send to big_file
							check = true;
						}
					}
				}	
			}
		}

		if (check == false) {
			printf("\nSorry, there was no .csv file detected in your directory. Program will return to the main menu\n");
			return biggest_file = NULL;
		}

	printf("\nNow processing the chosen file named '%s'\n", biggest_file->d_name);
	closedir(dir_loc);														// Close the directory
	return biggest_file;
	}
}
// Reference: The code from above is reference to the Lecture 'File access in C'


/*********************************************************
 *
 * Function: "struct dirent *small_file()"
 * Detail(s): Defining the struct for finding the smallest
 * 			  file.
 *
 ********************************************************/
struct dirent *small_file() {

	// Declare all the struct, stat, and DIR variable to find the smallest file
	DIR* dir_loc;										// Hold the directory we are starting at
	struct dirent *dir_ptr;							// Hold the sub-directory of the starting directory
	struct stat info1;								// Hold the information of the file size1
	struct stat info2;								// Hold the information of the file size2

	// Declare struct to compare two size files, bool to check, and a return struct
	struct dirent *smallest_file;
	struct dirent *tmp;
	bool check = false;

	// Other declare content to declare the prefix
	char target_prefix[] = "movies_";			// Hold the prefix needed
	char ext_prefix[] = ".csv";					// Hold extra prefix
	char newdir_name[256];							// Hold the new directory name w/ the same prefix
	off_t size1;										// Hold the file size value of the 1st one
	off_t size2;										// Hold the file size value of the 2nd one
	
	// Opening the directory
	dir_loc = opendir(".");

	// Check to see if the directory can be open
	if (dir_loc > 0) {
		while ((dir_ptr = readdir(dir_loc)) != NULL) {				// Checking each entry in directory
			// Check if the diretory matches w/ the prefix
			if ((strstr(dir_ptr->d_name, target_prefix) != NULL) && (strstr(dir_ptr->d_name, ext_prefix) != NULL)) {	
				stat(dir_ptr->d_name, &info1);							// Get the file info and put in info1
				size1 = info1.st_size;										// Assign the 1st value of size
				tmp = dir_ptr;													// Assign a copy of each entry in directory to a tmp				
				
				while ((tmp = readdir(dir_loc)) != NULL) {			// Check each entry again for the other files
					// Check if the diretory matches w/ the prefix
					if ((strstr(tmp->d_name, target_prefix) != NULL) && (strstr(tmp->d_name, ext_prefix) != NULL)) {	
						stat(tmp->d_name, &info2);							// Get the file info and put in info2
						size2 = info2.st_size;								// Assign the 2nd value of size

						// Now compare which file is bigger
						if (size1 >= size2) {								// If size2 if smaller and equal to size1
							smallest_file = tmp;								// Send the tmp to small_file
							check = true;
						}
	
						else {													// Else if it is smaller
							smallest_file = dir_ptr;						// Original file is smaller and send to small file
							check = true;
						}
					}
				}	
			}
		}

		if (check == false) {
			printf("\nSorry, there was no .csv file detected in your directory. Program will return to the main menu\n");
			return smallest_file = NULL;
		}

	printf("\nNow processing the chosen file named '%s'\n", smallest_file->d_name);
	closedir(dir_loc);														// Close the directory
	return smallest_file;
	}
}
// Reference: The code from above is reference to the Lecture 'File access in C'


/**********************************************************
 *
 * Function: "void sub_menu()"
 * Detail(s): This function serve as the sub menu of the 
 * 			  program about the file.
 * 			 
 *********************************************************/
void sub_menu() {

	// This is an declaration of the int as an input for value
	int choice_pick = 0;
	bool correct_opt = false;

	// Create a struct to hold all the file options
	struct dirent *file1;
  	struct dirent *file2;
	struct dirent *file3;
	char *tmp;

	// This is message display all the options you can use
	do {
		printf("\nWhich file do you want to process?\n");
		printf("(Press 1) to pick the largest file\n");
		printf("(Press 2) to pick the smallest file\n");
		printf("(Press 3) to specify the name of the file\n");
		printf("Your Entry: ");
		scanf("%d", &choice_pick);	

		// This is all the possible cases can happen
		if (choice_pick == 1) {
			file1 = large_file();
			create_dir_file(file1->d_name);
			correct_opt = true;
		}

		else if (choice_pick == 2) {
			file2 = small_file();
			tmp = file2->d_name;
			printf("%s\n", tmp);
			create_dir_file(file2->d_name);
			correct_opt = true;
		}

		else if (choice_pick == 3) {
			file3 = user_srfile();

			if (file3 != NULL) {
				create_dir_file(file3->d_name);
				correct_opt = true;
			}

			else if (file3 == NULL) {
				correct_opt = false;
			}
		}

		else {
			printf("\nThe choice you pick might be out-of-range / invalid. Please try again\n");
			correct_opt = false;
		}
	} while (correct_opt == false);
}


/**********************************************************
 *
 * Function: "void main_menu()"
 * Detail(s): This function serve as the main menu options
 * 			  for the program.
 *
 *********************************************************/
void main_menu() {

	// Declare an input int and a boolean as false for checking if the program is done
	int opt_input = 0;
	bool done = false;

	// This part of the program ask user for input in the option and choose from one of
	// the statement presented
	do {
		printf("\nPlease choose from one of the option below.\n");
		printf("(Press 1) - Select file to process\n");
		printf("(Press 2) - Exit the program\n");
		printf("Your Input: ");
		scanf("%d", &opt_input);

		// This is all the cases for the input option
		if (opt_input == 1) {
			sub_menu();
			done = false;
		}

		else if (opt_input == 2) {
			printf("\nThe program will exit now. Thank you for using the program!\n");
			printf("\n");
			done = true;	
		}

		else {
			printf("\nSorry, one of your input choice didn't matches with any options listed above.\n");
			printf("Please try again.\n");
			done = false;
		}
	} while (done == false);
}
