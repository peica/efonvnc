/*************************************************
Copyright (c) 2000-2008 by Nicolas Devillard.
MIT License

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without
restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following
conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.
*************************************************/

#ifndef _DICTIONARY_H_
#define _DICTIONARY_H_

/*---------------------------------------------------------------------------
                  Includes
 ---------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*---------------------------------------------------------------------------
                  New types
 ---------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
/**
  @brief  Dictionary object

  This object contains a list of string/string associations. Each
  association is identified by a unique string key. Looking up values
  in the dictionary is speeded up by the use of a (hopefully collision-free)
  hash function.
 */
/*-------------------------------------------------------------------------*/
typedef struct _dictionary_ {
  int       n ;   /** Number of entries in dictionary */
  int       size ;  /** Storage size */
  char    **  val ; /** List of string values */
  char    **  key ; /** List of string keys */
  unsigned   *  hash ;  /** List of hash values for keys */
} dictionary ;

namespace INI {

/*************************************************
  Function:			dictionary_hash
  Description:		Compute the hash key for a string.
  Calls:			
  Called By:		
  Table Accessed:
  Table Updated:
  Input:			Character string to use for key.
  Output:
  Return:			1 unsigned int on at least 32 bits.
  Others:			This hash function has been taken from an Article in Dr Dobbs Journal.
					This is normally a collision-free function, distributing keys evenly.
					The key is stored anyway in the struct so that collision can be avoided
					by comparing the key itself in last resort.
*************************************************/
unsigned dictionary_hash(char * key);

/*************************************************
  Function:			dictionary_new
  Description:		Create a new dictionary object.
  Calls:			
  Called By:		
  Table Accessed:
  Table Updated:
  Input:			Optional initial size of the dictionary.
  Output:
  Return:			1 newly allocated dictionary objet.
  Others:			This function allocates a new dictionary object of given size and returns
					it. If you do not know in advance (roughly) the number of entries in the
					dictionary, give size=0.
*************************************************/
dictionary * dictionary_new(int size);

/*************************************************
  Function:			dictionary_del
  Description:		Delete a dictionary object.
  Calls:			
  Called By:		
  Table Accessed:
  Table Updated:
  Input:			dictionary object to deallocate.
  Output:
  Return:			
  Others:			Deallocate a dictionary object and all memory associated to it.
*************************************************/
void dictionary_del(dictionary * vd);

/*************************************************
  Function:			dictionary_get
  Description:		Get a value from a dictionary.
  Calls:			
  Called By:		
  Table Accessed:
  Table Updated:
  Input:			dictionary object to search.,Key to look for in the dictionary.,
					Default value to return if key not found.
  Output:
  Return:			1 pointer to internally allocated character string.
  Others:			This function locates a key in a dictionary and returns a pointer to its
					value, or the passed 'def' pointer if no such key can be found in
					dictionary. The returned character pointer points to data internal to the
					dictionary object, you should not try to free it or modify it.
*************************************************/
char * dictionary_get(dictionary * d, char * key, char * def);

/*************************************************
  Function:			dictionary_getchar
  Description:		Get a value from a dictionary, as a char.
  Calls:			
  Called By:		
  Table Accessed:
  Table Updated:
  Input:			dictionary object to search.,Key to look for in the dictionary.,
					Default value to return if key not found.
  Output:
  Return:			char
  Others:			This function locates a key in a dictionary using dictionary_get,
					and returns the first char of the found string.
*************************************************/
char dictionary_getchar(dictionary * d, char * key, char def) ;

/*************************************************
  Function:			dictionary_getint
  Description:		Get a value from a dictionary, as an int.
  Calls:			
  Called By:		
  Table Accessed:
  Table Updated:
  Input:			dictionary object to search.,Key to look for in the dictionary.,
					Default value to return if key not found.
  Output:
  Return:			int
  Others:			This function locates a key in a dictionary using dictionary_get,
					and applies atoi on it to return an int. If the value cannot be found
					in the dictionary, the default is returned.
*************************************************/
int dictionary_getint(dictionary * d, char * key, int def);

/*************************************************
  Function:			dictionary_getdouble
  Description:		Get a value from a dictionary, as a double.
  Calls:			
  Called By:		
  Table Accessed:
  Table Updated:
  Input:			dictionary object to search.,Key to look for in the dictionary.,
					Default value to return if key not found.
  Output:
  Return:			double
  Others:			This function locates a key in a dictionary using dictionary_get,
					and applies atof on it to return a double. If the value cannot be found
					in the dictionary, the default is returned.
*************************************************/
double dictionary_getdouble(dictionary * d, char * key, double def);

/*************************************************
  Function:			dictionary_set
  Description:		Set a value in a dictionary.
  Calls:			
  Called By:		
  Table Accessed:
  Table Updated:
  Input:			dictionary object to modify..,Key to modify or add.,
					Value to add.
  Output:
  Return:			
  Others:			If the given key is found in the dictionary, the associated value is
					replaced by the provided one. If the key cannot be found in the
					dictionary, it is added to it.

					It is Ok to provide a NULL value for val, but NULL values for the dictionary
					or the key are considered as errors: the function will return immediately
					in such a case.

					Notice that if you dictionary_set a variable to NULL, a call to
					dictionary_get will return a NULL value: the variable will be found, and
					its value (NULL) is returned. In other words, setting the variable
					content to NULL is equivalent to deleting the variable from the
					dictionary. It is not possible (in this implementation) to have a key in
					the dictionary without value.
*************************************************/
void dictionary_set(dictionary * vd, char * key, char * val);

/*************************************************
  Function:			dictionary_getdouble
  Description:		Delete a key in a dictionary
  Calls:			
  Called By:		
  Table Accessed:
  Table Updated:
  Input:			dictionary object to modify.,Key to remove.
  Output:
  Return:			
  Others:			This function deletes a key in a dictionary. Nothing is done if the
					key cannot be found.
*************************************************/
void dictionary_unset(dictionary * d, char * key);

/*************************************************
  Function:			dictionary_setint
  Description:		Set a key in a dictionary, providing an int.
  Calls:			
  Called By:		
  Table Accessed:
  Table Updated:
  Input:			Dictionary to update.,Key to modify or add,
					Integer value to store (will be stored as a string).
  Output:
  Return:			double
  Others:			This helper function calls dictionary_set() with the provided integer
					converted to a string using %d.
*************************************************/
void dictionary_setint(dictionary * d, char * key, int val);

/*************************************************
  Function:			dictionary_setdouble
  Description:		Set a key in a dictionary, providing a double.
  Calls:			
  Called By:		
  Table Accessed:
  Table Updated:
  Input:			Dictionary to update.,Key to modify or add,
					Double value to store (will be stored as a string).
  Output:
  Return:			
  Others:			This helper function calls dictionary_set() with the provided double
					converted to a string using %g.
*************************************************/
void dictionary_setdouble(dictionary * d, char * key, double val);

/*************************************************
  Function:			dictionary_dump
  Description:		Dump a dictionary to an opened file pointer.
  Calls:			
  Called By:		
  Table Accessed:
  Table Updated:
  Input:			Dictionary to dump.,Opened file pointer.
  Output:
  Return:			
  Others:			Dumps a dictionary onto an opened file pointer. Key pairs are printed out
					as @c [Key]=[Value], one per line. It is Ok to provide stdout or stderr as
					output file pointers.
*************************************************/
void dictionary_dump(dictionary * d, FILE * out);

}; // !namespace INI {
#endif
