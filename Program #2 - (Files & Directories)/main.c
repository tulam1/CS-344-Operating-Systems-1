/**********************************************************************************
 * Program: main.c
 * Author: Tu Lam
 * Date: January 20th, 2021
 * Description: This file contains code in Cs, and it is the main program where the
 * 				 main function will run the other functions from the other C file to make
 *         and create file/directory.
 *********************************************************************************/

// This include all the directive headers
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "file.c"

/*********************************************************
 *
 * Function: "int main()"
 * Detail(s): This function calls the other functions in
 *         	  the "file.c" to run the program 
 *
 ********************************************************/
int main() {

	main_menu();

	return 0;
}
