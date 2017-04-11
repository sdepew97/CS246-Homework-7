/* imdb.c

   Main program for interactive IMDB viewer.

   Name: Sarah Depew
   Resources used (websites / peers / etc):
*/

#include <stdlib.h>
#include <stdio.h>

#include "array.h"
#include "map.h"
#include "llist.h"
#include "types.h"

#include "imdb_functions.h"

#define LEN 199

// the IMDB files contain 239 header lines
#define HEADER_LINES 241

// Discards all characters until the end of a line in the given file
void skip_line(FILE* file)
{
  while(!feof(file) && getc(file) != '\n')
    ;
}

// Reads in a file containing a list of cast members
// preconditions: all_movies exists and maps all movies encountered so far
// postconditions: If the file can be read successfully, returns an array
//                 of all cast members in the file, and updates all_movies
//                 to map all movies encountered. If there is an error reading
//                 the file, returns NULL.
// Note: You are responsible for freeing the returned array (if it's not NULL).
array read_cast_member_file(char* filename, map all_movies)
{
  FILE* file = fopen(filename, "r");
  if(!file)
  {
    printf("Cannot open file.");
    return NULL;
  }

  // skip header
  for(int i = 0; i < HEADER_LINES; i++)
  {
    skip_line(file);
  }

  array cast = array_new();

  while(!feof(file))
  {
    cast_member* member = malloc(sizeof(cast_member));
    read_result result = read_cast_member(file, member, all_movies);
    
    switch(result)
    {
    case SUCCESS:
      array_add(cast, member);

      // This is helpful for seeing progress as you're loading a file.
      if(array_size(cast) % 1000 == 0){
        printf("Added cast member %s\n", member->name);
      }
      
      break;
    case FAILURE: 
      skip_line(file); // this makes sure we're always moving forward
      break;
      
    case END_OF_LIST:
      return cast;
    }
  }

  return cast; // shouldn't get here unless file is truncated
}

void replace_new_line(char *input){
	int i=0; 
	
	while(input[i]!='\n'){
		i++; 
	}
	input[i] = '\0'; 	
}

// Our main program loop.
// See King, p. 302, about argc and argv.
int main(int argc, char** argv)
{
  if(argc < 2)
  {
    printf("Usage: %s [cast_member lists]\n", argv[0]);
    return 1;
  }
	   
  map all_movies = map_new();
  array all_cast = array_new();

  // start i at one to skip program name
  for(int i = 1; i < argc; i++)
  {
    array some_cast = read_cast_member_file(argv[i], all_movies);

    if(!some_cast)
    {
       // file reading failed, but read_cast_member_file alerted the user already
      continue;
    }

    // WRITE CODE HERE
    // At this point, some_cast has the cast members most recently read in,
    // and all_cast has all previous cast members.
    // You need to merge (with a call to merge_arrays) these two arrays, producing
    // a new all_cast that contains both.
    
    all_cast = merge_arrays(all_cast, some_cast); 
  }

  for(;;)
  {
    // WRITE CODE HERE
    // This is the main interactive loop, which you must write, as described
    // in the assignment.
    // When the user is done, this should `break`.
    
    //get input from user 
    //create the string that the memory is stored in 
    char input[LEN+1] = {}; 
    
    //fill input array with length LEN leaving room for a null terminator
    fgets(input, LEN, stdin);
    
    //function that takes in an array pointer removes the new line  
    replace_new_line(input); 
    
    //The user did not enter anything (just a newline), so they are done with the program. 
    if(input[0]=='\0'){
    	break;
    } 
    
    //look up cast member 
    cast_member *target_person = find_cast_member(all_cast, input);
    
    //if this person is in the list, then return movies that they were in
    if(target_person!=NULL){
      
      //print all the movies they have been in
      for(int i=0; i<llist_size(target_person->movies); i++){
      	printf(
      }
      
      //printf("Congratulations, you found a person!\n"); 
    }
    else{
      printf("I am sorry, %s was not found.\n",input); 
      //break; 
    }
  }

  // WRITE CODE HERE
  // Free all used memory before exiting.
  
  return 0;
}

