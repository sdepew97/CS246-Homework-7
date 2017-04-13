/* imdb_functions.c

   Name: Sarah Depew
   Resources used (websites / peers / etc): https://en.wikipedia.org/wiki/Binary_search_algorithm, TA Rachel, Ruby, Professor 
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "imdb_functions.h"
#include "types.h"

#define STRING_SIZE  200
#define LEN "199"

// Discards all characters until the end of a line in the given file
void skip_line(FILE* file)
{
  while(!feof(file) && getc(file) != '\n')
    ;
}

//taken from your binary.c file and consumes all characters from stdin, so the program works correctly
void skip_line_stdin(){
	while(!feof(stdin) && getchar() != '\n')
	;
}

// copy the given string into a newly-malloc'd buffer
char* malloc_string(char* str)
{
  char* new = malloc(sizeof(char) * (strlen(str)+1));
  strcpy(new, str);
  return new;
}

// like strcmp, but ignores differences in case (why does this work?)
int stricmp(char* s1, char* s2)
{
  while(true)
  {
    char c1 = *s1++;
    char c2 = *s2++;
    int d = toupper(c1) - toupper(c2);

    if(d != 0 || !c1)
    {
      return d;
    }
  }
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

// are there any non-ascii characters in the string?
bool any_bad_chars(char* str)
{
  while(*str)
  {
    char c = *str++;
    if(c < 0)
    {
      return true;
    }
  }
  return false;
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
  if(fscanf(file, "%" LEN "[^\t\n]", buf) != 1) 
    return FAILURE;

  if(all_dashes(buf)) 
    return END_OF_LIST;
    
  // non-ascii chars are alphabetized differently, so we can't handle them here
  if(any_bad_chars(buf)){
    return FAILURE;
  }

  // non-ascii chars are alphabetized differently, so we can't handle them here
  if(any_bad_chars(buf)) return FAILURE;

  //fills the name and movie list part of the cast member struct
  member->name = malloc_string(buf); 
  member->movies = llist_new(); //creates an empty llist to fill with movies 
  
  //start reading in all the movies
  while(fscanf(file, "%*[\t]%" LEN "[^\n]", buf) == 1)
  {
    skip_line(file); // eat rest of line
    
    // cut it off at the first instance of 2 spaces
    char* spaces = strstr(buf, "  ");
    if(spaces)
    {
      *spaces = '\0'; // cut off the string here
    }

    //Look up movie to see if it already exists
    bool movie_in_map = map_contains(all_movies, buf);
    movie *current_movie; 
    
    //the movie is not yet contained in the map
    if(movie_in_map == false)
    {
      	//create the movie and fill the fields
    	movie *new_movie = malloc(sizeof(movie));
    	
    	new_movie->name = malloc_string(buf); 
    	new_movie->cast = array_new(); 
    	 
    	//add it to the map
    	map_put(all_movies, buf, new_movie);
    
    	//update current_movie
    	current_movie = new_movie;
    	
    }
    
    else
    {
        //the movie exists, so we need to get it from the map
    	current_movie = map_get(all_movies, buf);
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

  while(index_1<array_size(src1) && index_2<array_size(src2))
  {
    //first element is smaller than second element, first element into merged array
    if(stricmp((array_get(src1,index_1)->name),(array_get(src2,index_2)->name))<0)
    { 
      array_add_at(merged,index_merged,array_get(src1,index_1));
      index_1++;
      index_merged++; 
    }
    
    //elements are either equal or second is greater than first, put second in merged array
    else
    {  
      array_add_at(merged,index_merged,array_get(src2,index_2));
      index_2++;
      index_merged++; 
    }
  }

  //there are still (sorted) elements in the first array that should all be transfered to merged
  if(index_1<array_size(src1))
  {
    for(;index_1<array_size(src1);index_1++,index_merged++)
    {
      array_add_at(merged,index_merged,array_get(src1,index_1));
    }
  }

  //there are still (sorted) elements in the second array that should all be transfered to merged
  if(index_2<array_size(src2))
  {
    for(;index_2<array_size(src2);index_2++,index_merged++)
    {
      array_add_at(merged,index_merged,array_get(src2,index_2));
    }
  }
  
  else{} //do nothing, since merged is filled and input arrays were both the same size
  
  //return a copy of the merged array 
  return merged; 
}

// Finds a cast member in a sorted array of cast members
// preconditions: cast is a valid, sorted array of cast members
//                name is a valid string
// postconditions: If the cast member can be found, returns a pointer to the member
//                 Otherwise, returns NULL.
cast_member* find_cast_member(array cast, char* name)
{
  //the int markers for where we are in an array
  int low=0, high=array_size(cast)-1, midpoint;
  
  //while low<=high, since we want to check even if they are equal since the element could be at this location
  while(low<=high)
  {
    //reset the midpoint's value each iteration 
    midpoint=(low+high)/2;
     //check to see if the key is less than, greater than, or equal to that of the current string 
     
     //if equal, then the element has been found 
     if(stricmp((array_get(cast,midpoint)->name),name)==0)
     {
      return array_get(cast,midpoint); 
    }
    //if less than, then the element will be located above where we are currently searching 
    else if(stricmp((array_get(cast,midpoint)->name),name)<0)
    {
      low=midpoint+1; 
    }
    //otherwise, look below where we are currently searching
    else
    {
      high=midpoint-1; 
    } 
  }
  //the element was not found
  return NULL; 
}
