/* imdb_functions.c

   Name: Sarah Depew
   Resources used (websites / peers / etc):
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "imdb_functions.h"
#include "types.h"

#define STRING_SIZE  200
#define LEN "199"

// copy the given string into a newly-malloc'd buffer
char* malloc_string(char* str)
{
  char* new = malloc(sizeof(char) * (strlen(str)+1));
  strcpy(new, str);
  return new;
}

// is the given string composed entirely of dashes?
bool all_dashes(char* str)
{
  while(*str)
  {
    if(*str++ != '-')
    {
      return false;
    }
  }
  return true;
}

// Reads in a cast member from the given file
// preconditions: the file is open and at a line where a cast member begins
//                member points to a valid cast_member struct
//                all_movies is a map of all known movies
// postconditions: if a cast_member can successfully be read from the file,
//                 *member is filled in. Any new movies found while reading
//                 the cast member are added to all_movies, and SUCCESS is returned
//                 Otherwise, if the end of the list of members is reached,
//                 END_OF_LIST is returned. Otherwise, FAILURE is returned.
read_result read_cast_member(FILE* file, cast_member* member, map all_movies)
{
  char buf[STRING_SIZE];
  if(fscanf(file, "%" LEN "[^\t\n]", buf) != 1) return FAILURE;

  if(all_dashes(buf)) return END_OF_LIST;

  // WRITE CODE HERE
  // At this point, `buf` contains the name of the cast member, and you can
  // fill in the two data fields of *member.

  member->name = buf; //fills the name part of the cast member struct
  member->members = llist_new(); //creates an empty llist to fill with movies 
  
  //start reading in all the movies
  while(fscanf(file, "%*[\t]%" LEN "[^\n]", buf) == 1)
  {
    getc(file); // eat the newline
    
    // cut it off at the first instance of 2 spaces
    char* spaces = strstr(buf, "  ");
    if(spaces)
    {
      *spaces = '\0'; // cut off the string here
    }

    // WRITE CODE HERE
    // At this point, `buf` contains the name of the movie. You should:
    //   1. Look up the movie in all_movies to see if it already exists.
    //   2. If not, create (malloc) a new movie object and add it to the map.
    //   3. Update the movie to include the cast member.
    //   4. Update the cast member to include the movie.
    
    movie *current_movie = map_get(all_movies, buf);
    
    //the movie is not yet contained in the map
    if(current_movie == NULL){
      	//create the movie and fill the fields
    	movie *new_movie = malloc(sizeof(movie));
    	new_movie->name = buf; 
    	new_movie->cast = array_new(); 
    	 
    	//add it to the map
    	map_put(all_movies, buf, new_movie);
    	
    	//update current_movie
    	current_movie = new_movie; 
    }
    
    //Update the movie to contain the cast member
    array_add(current_movie->cast, member); 
    
    //Update the cast member to include the movie
    llist_add(member->movies, current_movie);
  }
  return SUCCESS;
}

// Merges two sorted arrays. Runs in time linear to the length of the two input arrays.
// preconditions: src1 and src2 are two valid arrays.
//                src1 and src2 are both in sorted order
// postconditions: returns a fresh array containing all the elements of src1 and src2
//                 in sorted order
// Note: You are responsible for freeing the returned array.
array merge_arrays(array src1, array src2)
{
  //array in which I will store the merged values
  array merged = array_new();
  int index_1=0, index_2=0, index_merged=0; 

  while(index_1<array_size(src1) && index_2<array_size(src2)){
    //first element is smaller than second element, first element into merged array
    if(strcmp((array_get(src1,index_1)->name),(array_get(src2,index_2)->name))<0){ 
      array_add_at(merged,index_merged,array_get(src1,index_1));
      index_1++;
      index_merged++; 
    }
    
    //elements are either equal or second is greater than first, put second in merged array
    else{  
      array_add_at(merged,index_merged,array_get(src2,index_2));
      index_2++;
      index_merged++; 
    }
  }

  //there are still (sorted) elements in the first array that should all be transfered to merged
  if(index_1<array_size(src1)){
    for(;index_1<array_size(src1);index_1++,index_merged++){
      array_add_at(merged,index_merged,array_get(src1,index_1));
    }
  }

  //there are still (sorted) elements in the second array that should all be transfered to merged
  if(index_2<array_size(src2)){
    for(;index_2<array_size(src2);index_2++,index_merged++){
      array_add_at(merged,index_merged,array_get(src2,index_2));
    }
  }
  
  else{} //do nothing, since merged is filled and input arrays were both the same size
  
  return merged; 
}

// Finds a cast member in a sorted array of cast members
// preconditions: cast is a valid, sorted array of cast members
//                name is a valid string
// postconditions: If the cast member can be found, returns a pointer to the member
//                 Otherwise, returns NULL.
cast_member* find_cast_member(array cast, char* name)
{
  int low=0, high=array_size(cast)-1, midpoint=(low+high)/2;

  while(low<high){
    midpoint=(low+high)/2; 
     if(strcmp((array_get(cast,midpoint)->name),name)==0){
      return array_get(cast,midpoint); 
    }

    else if(strcmp((array_get(cast,midpoint)->name), name)<0){
      low=midpoint+1; 
    }
    else{
      high=midpoint-1; 
    } 
  }
  
  return NULL; 
}
