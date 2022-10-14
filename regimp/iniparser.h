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

#ifndef _INIPARSER_H_
#define _INIPARSER_H_

/*---------------------------------------------------------------------------
                  Includes
 ---------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dictionary.h"

namespace INI {

char * iniparser_getkeyname(dictionary * d, char * sect, int n);

int iniparser_getnkey(dictionary * d, char *sect);

/*************************************************
  Function:			iniparser_getnsec
  Description:		Get number of sections in a dictionary
  Calls:			
  Called By:		
  Table Accessed:
  Table Updated:
  Input:			Dictionary to examine
  Output:
  Return:			Number of sections found in dictionary
  Others:			This function returns the number of sections found in a dictionary.
					The test to recognize sections is done on the string stored in the
					dictionary: a section name is given as "section" whereas a key is
					stored as "section:key", thus the test looks for entries that do not
					contain a colon.

					This clearly fails in the case a section name contains a colon, but
					this should simply be avoided.

					This function returns -1 in case of error.
*************************************************/
int iniparser_getnsec(dictionary * d);

/*************************************************
  Function:			iniparser_getsecname
  Description:		Get name for section n in a dictionary.
  Calls:			
  Called By:		
  Table Accessed:
  Table Updated:
  Input:			Dictionary to examine,Section number (from 0 to nsec-1).
  Output:
  Return:			Pointer to char string
  Others:			This function locates the n-th section in a dictionary and returns
					its name as a pointer to a string statically allocated inside the
					dictionary. Do not free or modify the returned string!

					This function returns NULL in case of error.
*************************************************/
char * iniparser_getsecname(dictionary * d, int n);


/*************************************************
  Function:			iniparser_dump_ini
  Description:		Save a dictionary to a loadable ini file
  Calls:			
  Called By:		
  Table Accessed:
  Table Updated:
  Input:			Dictionary to dump,Opened file pointer to dump to.
  Output:
  Return:			
  Others:			This function dumps a given dictionary into a loadable ini file.
					It is Ok to specify @c stderr or @c stdout as output files.
*************************************************/
void iniparser_dump_ini(dictionary * d, FILE * f);

/*************************************************
  Function:			iniparser_dump
  Description:		Dump a dictionary to an opened file pointer.
  Calls:			
  Called By:		
  Table Accessed:
  Table Updated:
  Input:			Dictionary to examine,Opened file pointer to dump to.
  Output:
  Return:			
  Others:			This function prints out the contents of a dictionary, one element by
					line, onto the provided file pointer. It is OK to specify @c stderr
					or @c stdout as output files. This function is meant for debugging
					purposes mostly.
*************************************************/
void iniparser_dump(dictionary * d, FILE * f);

/*************************************************
  Function:			iniparser_getstr
  Description:		Get the string associated to a key, return NULL if not found
  Calls:			
  Called By:		
  Table Accessed:
  Table Updated:
  Input:			Dictionary to search,Key string to look for
  Output:
  Return:			pointer to statically allocated character string, or NULL.
  Others:			This function queries a dictionary for a key. A key as read from an
					ini file is given as "section:key". If the key cannot be found,
					NULL is returned.
					The returned char pointer is pointing to a string allocated in
					the dictionary, do not free or modify it.

					This function is only provided for backwards compatibility with 
					previous versions of iniparser. It is recommended to use
					iniparser_getstring() instead.
*************************************************/
char * iniparser_getstr(dictionary * d, char * key);


/*************************************************
  Function:			iniparser_getstring
  Description:		Get the string associated to a key, return NULL if not found
  Calls:			
  Called By:		
  Table Accessed:
  Table Updated:
  Input:			Dictionary to search,Key string to look for,Default value to return if key not found
  Output:
  Return:			pointer to statically allocated character string, or NULL.
  Others:			This function queries a dictionary for a key. A key as read from an
					ini file is given as "section:key". If the key cannot be found,
					the pointer passed as 'def' is returned.
					The returned char pointer is pointing to a string allocated in
					the dictionary, do not free or modify it.

*************************************************/
char * iniparser_getstring(dictionary * d, char * key, char * def);

/*************************************************
  Function:			iniparser_getint
  Description:		Get the string associated to a key, convert to an int
  Calls:			
  Called By:		
  Table Accessed:
  Table Updated:
  Input:			Dictionary to search,Key string to look for,
					notfound Value to return in case of error
  Output:
  Return:			integer
  Others:			This function queries a dictionary for a key. A key as read from an
					ini file is given as "section:key". If the key cannot be found,
					the notfound value is returned.

*************************************************/
int iniparser_getint(dictionary * d, char * key, int notfound);

/*************************************************
  Function:			iniparser_getdouble
  Description:		Get the string associated to a key, convert to a double
  Calls:			
  Called By:		
  Table Accessed:
  Table Updated:
  Input:			Dictionary to search,Key string to look for,
					notfound Value to return in case of error
  Output:
  Return:			double
  Others:			This function queries a dictionary for a key. A key as read from an
					ini file is given as "section:key". If the key cannot be found,
					the notfound value is returned.

*************************************************/
double iniparser_getdouble(dictionary * d, char * key, double notfound);

/*************************************************
  Function:			iniparser_getboolean
  Description:		Get the string associated to a key, convert to a boolean
  Calls:			
  Called By:		
  Table Accessed:
  Table Updated:
  Input:			Dictionary to search,Key string to look for,
					notfound Value to return in case of error
  Output:
  Return:			integer
  Others:			This function queries a dictionary for a key. A key as read from an
					ini file is given as "section:key". If the key cannot be found,
					the notfound value is returned.

					A true boolean is found if one of the following is matched:

					- A string starting with 'y'
					- A string starting with 'Y'
					- A string starting with 't'
					- A string starting with 'T'
					- A string starting with '1'

					A false boolean is found if one of the following is matched:

					- A string starting with 'n'
					- A string starting with 'N'
					- A string starting with 'f'
					- A string starting with 'F'
					- A string starting with '0'

					The notfound value returned if no boolean is identified, does not
					necessarily have to be 0 or 1.
*************************************************/
int iniparser_getboolean(dictionary * d, char * key, int notfound);


/*************************************************
  Function:			iniparser_setstr
  Description:		Finds out if a given entry exists in a dictionary
  Calls:			
  Called By:		
  Table Accessed:
  Table Updated:
  Input:			Dictionary to modify,Entry to modify (entry name),New value to associate to the entry.
  Output:
  Return:			int 0 if Ok, -1 otherwise.
  Others:			If the given entry can be found in the dictionary, it is modified to
					contain the provided value. If it cannot be found, -1 is returned.
					It is Ok to set val to NULL.
*************************************************/
int iniparser_setstr(dictionary * ini, char * entry, char * val);

/*************************************************
  Function:			iniparser_unset
  Description:		Delete an entry in a dictionary
  Calls:			
  Called By:		
  Table Accessed:
  Table Updated:
  Input:			Dictionary to modify,Entry to delete (entry name)
  Output:
  Return:			
  Others:			If the given entry can be found, it is deleted from the dictionary.
*************************************************/
void iniparser_unset(dictionary * ini, char * entry);

/*************************************************
  Function:			iniparser_find_entry
  Description:		Finds out if a given entry exists in a dictionary
  Calls:			
  Called By:		
  Table Accessed:
  Table Updated:
  Input:			Dictionary to search,Key string to look for
  Output:
  Return:			integer 1 if entry exists, 0 otherwise
  Others:			Finds out if a given entry exists in the dictionary. Since sections
					are stored as keys with NULL associated values, this is the only way
					of querying for the presence of sections in a dictionary.

*************************************************/
int iniparser_find_entry(dictionary * ini, char * entry) ;

/*************************************************
  Function:			iniparser_load
  Description:		Parse an ini file and return an allocated dictionary object
  Calls:			
  Called By:		
  Table Accessed:
  Table Updated:
  Input:			ininame Name of the ini file to read.
  Output:
  Return:			Pointer to newly allocated dictionary
  Others:			This is the parser for ini files. This function is called, providing
					the name of the file to be read. It returns a dictionary object that
					should not be accessed directly, but through accessor functions
					instead.

					The returned dictionary must be freed using iniparser_freedict().
*************************************************/
dictionary * iniparser_load(char * ininame);

/*************************************************
  Function:			iniparser_freedict
  Description:		Free all memory associated to an ini dictionary
  Calls:			
  Called By:		
  Table Accessed:
  Table Updated:
  Input:			Dictionary to free
  Output:
  Return:			
  Others:			Free all memory associated to an ini dictionary.
					It is mandatory to call this function before the dictionary object
					gets out of the current context.
*************************************************/
void iniparser_freedict(dictionary * d);
}; //! namespace INI {
#endif
