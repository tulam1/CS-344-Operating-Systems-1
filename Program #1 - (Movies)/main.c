/**********************************************************************************
 * Program: main.c
 * Author: Tu Lam
 * Date: January 17th, 2021
 * Description: This file contains code in Cs, and it is the main program where the
 * 				 main function will run to open the text file and process to see if 
 * 				 the file open correctly before running the actual program.
 *********************************************************************************/

//Defining all the header directives
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "movie_info.c"

/*********************************************************
 *
 * Function: "int main(int argc, char *argv[])"
 * Detail(s): This is the main function where it looks for
 * 			  an argument of the file name and try to open
 * 			  it before running the program.
 *
 ********************************************************/
int main(int argc, char *argv[]) {

	// Create a variable that hold the number of total movies in the file
	int num_count = 0;	

	// If the user provide the argument line less than 2, they must provide a file name
	// If not, program will exit
	if (argc < 2) {
		printf("\nYou must provide the name of the file to process.\n");
		printf("Example: './movies movies_sample_1.csv'\n");
		exit(1);
	}

	// If file is accept, start to parse file
	struct movie *movieList = parseFile(argv[1], &num_count);

	// This minus 1 is for the column header of the file being in the count instead of the movie
	num_count = num_count - 1;									
	printf("\nProcessed file '%s' and parse data for %i movie(s).\n", argv[1], num_count);

	// Calling the main menu of the program
	main_menu(movieList);
	
	// If user quit, free up the allocation
	free(movieList);

	return 0;
}
