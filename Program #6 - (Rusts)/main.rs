/*************************************************************************
* Program: main.rs
* Author: Tu Lam
* Date: Match 10th, 2021
* Description: This program is an introduction to Rust and learning 
					the basic. Using the basic, build a partition program
					using thread to create a summation base on the argument
					given.
*************************************************************************/

use std::env; 			// To get arugments passed to the program
use std::thread; 		// Use the library for thread


/****************************************************************
* Print the number of partitions and the size of each partition
* @param vs A vector of vectors
****************************************************************/
fn print_partition_info(vs: &Vec<Vec<usize>>){
    println!("Number of partitions = {}", vs.len());
    for i in 0..vs.len(){
        println!("\tsize of partition {} = {}", i, vs[i].len());
    }
}


/***************************************************************
* Create a vector with integers from 0 to num_elements -1
* @param num_elements How many integers to generate
* @return A vector with integers from 0 to (num_elements - 1)
***************************************************************/
fn generate_data(num_elements: usize) -> Vec<usize>{
    let mut v : Vec<usize> = Vec::new();
    for i in 0..num_elements {
        v.push(i);
    }
    return v;
}


/***************************************************************
* Partition the data in the vector v into 2 vectors
* @param v Vector of integers
* @return A vector that contains 2 vectors of integers
***************************************************************/
fn partition_data_in_two(v: &Vec<usize>) -> Vec<Vec<usize>>{
    let partition_size = v.len() / 2;
    // Create a vector that will contain vectors of integers
    let mut xs: Vec<Vec<usize>> = Vec::new();

    // Create the first vector of integers
    let mut x1 : Vec<usize> = Vec::new();
    // Add the first half of the integers in the input vector to x1
    for i in 0..partition_size{
        x1.push(v[i]);
    }
    // Add x1 to the vector that will be returned by this function
    xs.push(x1);

    // Create the second vector of integers
    let mut x2 : Vec<usize> = Vec::new();
    // Add the second half of the integers in the input vector to x2
    for i in partition_size..v.len(){
        x2.push(v[i]);
    }
    // Add x2 to the vector that will be returned by this function
    xs.push(x2);
    // Return the result vector
    xs
}


/*********************************************************************
* Sum up the all the integers in the given vector
* @param v Vector of integers
* @return Sum of integers in v
* Note: this function has the same code as the reduce_data function.
*       But don't change the code of map_data or reduce_data.
*********************************************************************/
fn map_data(v: &Vec<usize>) -> usize{
    let mut sum = 0;
    for i in v{
        sum += i;
    }
    sum
}


/****************************************************
* Sum up the all the integers in the given vector
* @param v Vector of integers
* @return Sum of integers in v
****************************************************/
fn reduce_data(v: &Vec<usize>) -> usize{
    let mut sum = 0;
    for i in v{
        sum += i;
    }
    sum
}

/**********************************************
* A single threaded map-reduce program
**********************************************/
fn main() {

    // Use std::env to get arguments passed to the program
    let args: Vec<String> = env::args().collect();
    if args.len() != 3 {
        println!("ERROR: Usage {} num_partitions num_elements", args[0]);
        return;
    }
    let num_partitions : usize = args[1].parse().unwrap();
    let num_elements : usize = args[2].parse().unwrap();
    if num_partitions < 1{
      println!("ERROR: num_partitions must be at least 1");
        return;
    }
    if num_elements < num_partitions{
        println!("ERROR: num_elements cannot be smaller than num_partitions");
        return;
    }

    // Generate data.
    let v = generate_data(num_elements);

    // PARTITION STEP: partition the data into 2 partitions
    let xs = partition_data_in_two(&v);

    // Print info about the partitions
    print_partition_info(&xs);

    let mut intermediate_sums : Vec<usize> = Vec::new();

    // MAP STEP: Process each partition

    // CHANGE CODE START: Don't change any code above this line

    // Change the following code to create 2 threads that run concurrently and each of which uses map_data() function to process one of the two partitions
	 let temp_a = xs[0].clone();						// Copy data at index 0 & 1 to use when creating two threads
	 let temp_b = xs[1].clone();

    // Spawn two threads to run concurrently with each using map_data() function each partitions
	 let thread1 = thread::spawn(move || {map_data(&temp_a)});
	 let thread2 = thread::spawn(move || {map_data(&temp_b)});
	 let _run1 = thread1.join().unwrap();			// Run the two threads by joining it
	 let _run2 = thread2.join().unwrap();

	 // Using the snake case
    intermediate_sums.push(map_data(&xs[0]));
    intermediate_sums.push(map_data(&xs[1]));


    // CHANGE CODE END: Don't change any code below this line until the next CHANGE CODE comment

    // Print the vector with the intermediate sums
    println!("Intermediate sums = {:?}", intermediate_sums);

    // REDUCE STEP: Process the intermediate result to produce the final result
    let sum = reduce_data(&intermediate_sums);
    println!("Sum = {}", sum);

    // CHANGE CODE: Add code that does the following:
    // 1. Calls partition_data to partition the data into equal partitions
	 let xs = partition_data(num_partitions, &v);

    // 2. Calls print_partition_info to print info on the partitions that have been create
	 print_partition_info(&xs);


    // 3. Creates one thread per partition and uses each thread to concurrently process one partition
	 let mut temp_thread: Vec<thread::JoinHandle<_>> = Vec::new();		// Create a vector w/ thread joining
	 for _i in 0..num_partitions {												// Create a loop through each partition
	 	let data = xs[_i].clone();													// Copy the data from partition vector at that partition
		temp_thread.push(thread::spawn(move || map_data(&data)));		// Attach the content onto the temp_thread
	 }


    // 4. Collects the intermediate sums from all the threads
	 let mut inter_sum: Vec<usize> = Vec::new();			// Create a 1D vector to hold the intermediate sums
	 for _j in 0..num_partitions {							// Create a loop through each partition
	 	let temp_th = temp_thread.remove(0);				// Get data off of thread
		let par_sum = temp_th.join().unwrap();				// Get the data and store it in par_sum
		inter_sum.push(par_sum);								// Add content to the vector
	 }


    // 5. Prints information about the intermediate sums
    println!("Intermediate sums = {:?}", inter_sum);


    // 5. Calls reduce_data to process the intermediate sums
	 let total_sum = reduce_data(&inter_sum);	

    // 6. Prints the final sum computed by reduce_data
	 println!("Sum = {}", total_sum);

}

/****************************************************************
* CHANGE CODE: code this function
* Note: Don't change the signature of this function
*
* Partitions the data into a number of partitions such that
* - the returned partitions contain all elements that are in the input vector
* - if num_elements is a multiple of num_partitions, then all partitions must have equal number of elements
* - if num_elements is not a multiple of num_partitions, some partitions can have one more element than other partitions
*
* @param num_partitions The number of partitions to create
* @param v The data to be partitioned
* @return A vector that contains vectors of integers
* 
****************************************************************/
fn partition_data(num_partitions: usize, v: &Vec<usize>) -> Vec<Vec<usize>>{

	let mut new_v: Vec<Vec<usize>> = Vec::new();		// Create a vector to return later and store data
	let p_size = v.len() / num_partitions;				// Create a partition size that take the length of vector / num_par
	let mut remainder = v.len() % num_partitions;	// Figure out if partition division is odd or even

	// Create and assign variable for the loops
	let mut start = 0;										// Start var with value of 0
	let mut ending;											// An empty var

	// Create the "for" loop for each partitions
	for _i in 0..num_partitions {						
		if remainder == 0 {									// If remainder hold 0, the partition is even
			ending = start + p_size;						// Assign the ending var with partition size w/ start var
		} else {													// Else it odd partition and add an extra space at the end
			ending = start + p_size + 1;					// Add an extra space to store data
		}

		let mut temp_v: Vec<usize> = Vec::new();		// Create a temp 1D vector to store data in the inner loop
		
		// Create another loop to push the value from original vector onto the temp one
		for _j in start..ending {
			temp_v.push(v[_j]);								// Push content from original vector at the index onto the temp vector
		}
		new_v.push(temp_v);									// Attach content from temp_v to the 2D vectors

		if remainder > 0 {									// Check to see if remainder still greater than 0
			remainder = remainder - 1;						// Decrement the remainder
		}

		if _i != num_partitions - 1 {						// If the loop is not equal to number of partition - 1
			start = ending;									// Assign the ending to the start value
		}
	}

	return new_v;												// Return the brand new vector
}
