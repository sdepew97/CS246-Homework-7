/* imdb.c

   Main program for interactive IMDB viewer.

   Name: Sarah Depew
   Resources used (websites / peers / etc): https://www.cyberciti.biz/faq/linux-unix-vim-save-and-quit-command/, http://stackoverflow.com/questions/2481879/ignore-extra-spaces-when-using-fgets, http://www.cplusplus.com/reference/cstring/strcmp/, TA, Ruby, http://www.ascii-code.com
*/

#include <stdlib.h>
#include <stdio.h>

#include "array.h"
#include "map.h"
#include "llist.h"
#include "types.h"

#include "imdb_functions.h"

#define LEN 199

// the IMDB files contain at most 241 header lines
#define HEADER_LINES 241

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
 
  int i=0; 
  //!feof(file)
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
 		i++;
      break;
      
    case FAILURE:
      skip_line(file); // this makes sure we're always moving forward
      free(member);
      break;
      
    case END_OF_LIST:
      printf("The file is done loading.\n");  
      free(member);  
      return cast;
    }
  }

  return cast; // shouldn't get here unless file is truncated
}

void replace_new_line(char *input)
{
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
       // file reading failed, but read_cast_member_file alerted the user already however the memory still needs to be freed
      array_free(some_cast); 
      continue;
    }
    
    //merged array
    array new_all_cast = merge_arrays(all_cast, some_cast); 
    
    //free all cast and some cast, since these arrays and their memories are no longer needed
    array_free(all_cast);        
    array_free(some_cast);
    
    //set all cast to new all cast
    all_cast = new_all_cast;  
  }
  
  //print instructions for the user on how to use this program
  char *instruction_string = "With this program, you can search for either a movie or an actor/actress. To search for a movie, press 1 and ENTER. To search for an actor/actress, press 2 and ENTER. To end, press 'q' or ENTER.\n>>"; 
  printf("%s", instruction_string); 
  
  //user interactive main loop, which I went for flex points with :)
  for(;;)
  {
  	char user_choice = getchar(); 
    if(user_choice=='2')
    {
    	//removes the user's choice from stdin 
    	skip_line_stdin(); 
    	
    	printf("To use this program, please enter the name of an actor/actress in Last, First form. (For example: \"Jolie, Angelina\".)\nThe program will print out a list of all the movies in which the actor/actress has started.\n>>"); 
    	//get input from user 
    	//create the string that the memory is stored in 
   		char input[LEN+1] = {}; 
    
    	//fill input array with length LEN leaving room for a null terminator
    	fgets(input, LEN, stdin);
    
    	//function that takes in an array pointer removes the new line  
    	replace_new_line(input); 
    	
    	printf("Searching...\n"); 
    
  	  //look up cast member 
      cast_member *target_person = find_cast_member(all_cast, input);
    
      //if this person is in the list, then return movies that they were in
      if(target_person!=NULL)
      {
        //get the head of the llist of movies for the person found
        llist_node *head = llist_head(target_person->movies);
      
        printf("Congratulations, you found %s!\n%s was in this(these) movie(s):\n", target_person->name,target_person->name);
      
        //print all the movies they have been in
        for(int i=0; i<llist_size(target_person->movies)&&head!=NULL; i++, head=head->next){
    	  printf("%s\n", head->data->name);  
        }
      
        printf("Would you like to go again? If so, please type in 1 to search for another movie name or 2 to search for another actor/actress. Otherwise, type 'q' or press ENTER to exit.\n>>"); 
       }
       
    else
    {
      printf("I am sorry, %s was not found.\n",input);
      printf("If you like to try again, please type in 1 to search for another movie name or 2 to search for another actor/actress. Otherwise, type 'q' or press ENTER to exit.\n>>"); 
    }
  }
  
    //search for a movie
    else if(user_choice=='1')
    {
    	//removes the user's choice from stdin 
    	skip_line_stdin(); 
    	
    	printf("To use this program, please enter the name of an a movie for which you would like to search. (For example: \"The Hunger Games\".)\nThe program will print out a list of all the actors/actresses who started in this movie.\n>>");
        //get input from user 
    	//create the string that the memory is stored in 
   		char input[LEN+1] = {}; 
    
    	//fill input array with length LEN leaving room for a null terminator
    	fgets(input, LEN, stdin);
    
    	//function that takes in an array pointer removes the new line  
    	replace_new_line(input); 
    	
    	printf("Searching...\n"); 
    	
    	//see if the map contains the movie
		bool contains = map_contains(all_movies, input);
		
		if(contains)
		{
			printf("Congratulations! %s was in the map.\nThese people were in this movie:\n", input); 
			
			movie* movie_input = map_get(all_movies, input);
			int cast_array_size = array_size(movie_input->cast); 
			
			//print out the cast member's names
			for(int i=0; i<cast_array_size; i++)
			{
				printf("%s\n", array_get(movie_input->cast, i)->name); 
			}
			
        	printf("Would you like to go again? If so, please type in 1 to search for another movie name or 2 to search for another actor/actress. Otherwise, type 'q' or press ENTER to exit.\n>>"); 
		}
		
    	else
    	{
      		printf("I am sorry, %s was not found.\n",input);
      		printf("If you like to try again, please type in 1 to search for another movie name or 2 to search for another actor/actress. Otherwise, type 'q' or press ENTER to exit.\n>>"); 
    	}
    }
    
    //The user wants to exit the program they can enter anything else
    else if(user_choice=='q'||user_choice=='\n')
    {
      printf("Thank you. Goodbye.\n"); 
      break; 
    }
    //do nothing and loop, again
    else
    {
    	printf("I am sorry, the input was invalid.\n");
    	skip_line_stdin();
    	printf("If you like to try again, please type in 1 to search for another movie name or 2 to search for another actor/actress. Otherwise, type 'q' or press ENTER to exit.\n>>"); 
    }
  }
  
  // Free all used memory before exiting.  
  
  //Free the movies using the map
  int size_map = map_size(all_movies); 
  map_entry *movies_in_map = malloc(sizeof(map_entry)*size_map); 
  
  map_get_entries(all_movies, movies_in_map); 
  
  for(int i=0; i<size_map; i++)
  {
  	free(movies_in_map[i].value->name); 
  	array_free(movies_in_map[i].value->cast);
  	free(movies_in_map[i].value); 
  }
  
  //Free the map
  free(movies_in_map);
  map_free(all_movies); 
  
  //free the cast members
  //size of the array
  int size = array_size(all_cast); 
  
  for(int i=0; i<size; i++)
  {
  	cast_member *current = array_get(all_cast, i); 
  	free(current->name); 
  	
  	//free the llist of movies for a cast member
  	llist_free(current->movies);
  	
  	//free the cast member
  	free(current);  	
  }
  
  //free the array of cast members
  array_free(all_cast); 
  
  return 0;
}

