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

#ifndef _STRLIB_H_
#define _STRLIB_H_


#include <stdio.h>
#include <stdlib.h>

namespace INI {
/*************************************************
  Function:			strlwc
  Description:		Convert a string to lowercase.
  Calls:			
  Called By:		
  Table Accessed:
  Table Updated:
  Input:			String to convert.
  Output:
  Return:			ptr to statically allocated string.
  Others:			This function returns a pointer to a statically allocated string
					containing a lowercased version of the input string. Do not free
					or modify the returned string! Since the returned string is statically
					allocated, it will be modified at each function call (not re-entrant).
*************************************************/
char * strlwc(char * s);

/*************************************************
  Function:			strupc
  Description:		Convert a string to uppercase.
  Calls:			
  Called By:		
  Table Accessed:
  Table Updated:
  Input:			String to convert.
  Output:
  Return:			ptr to statically allocated string.
  Others:			This function returns a pointer to a statically allocated string
					containing an uppercased version of the input string. Do not free
					or modify the returned string! Since the returned string is statically
					allocated, it will be modified at each function call (not re-entrant).
*************************************************/
char * strupc(char * s);

/*************************************************
  Function:			strskp
  Description:		Skip blanks until the first non-blank character.
  Calls:			
  Called By:		
  Table Accessed:
  Table Updated:
  Input:			String to parse.
  Output:
  Return:			Pointer to char inside given string.
  Others:			This function returns a pointer to the first non-blank character in the
					given string.
*************************************************/
char * strskp(char * s);

/*************************************************
  Function:			strcrop
  Description:		Remove blanks at the end of a string.
  Calls:			
  Called By:		
  Table Accessed:
  Table Updated:
  Input:			String to parse.
  Output:
  Return:			ptr to statically allocated string.
  Others:			This function returns a pointer to a statically allocated string,
					which is identical to the input string, except that all blank
					characters at the end of the string have been removed.
					Do not free or modify the returned string! Since the returned string
					is statically allocated, it will be modified at each function call
					(not re-entrant).
*************************************************/
char * strcrop(char * s);

/*************************************************
  Function:			strstrip
  Description:		Remove blanks at the beginning and the end of a string.
  Calls:			
  Called By:		
  Table Accessed:
  Table Updated:
  Input:			String to parse.
  Output:
  Return:			ptr to statically allocated string.
  Others:			This function returns a pointer to a statically allocated string,
					which is identical to the input string, except that all blank
					characters at the end and the beg. of the string have been removed.
					Do not free or modify the returned string! Since the returned string
					is statically allocated, it will be modified at each function call
					(not re-entrant).
*************************************************/
char * strstrip(char * s) ;

}; // !namespace INI {
#endif
