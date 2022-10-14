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

#include <string.h>
#include <ctype.h>

#include "strlib.h"

#define ASCIILINESZ 256

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
char * strlwc(char * s)
{
	static char l[ASCIILINESZ+1];

	int i ;

	if (s==NULL) 
	{
		return NULL ;
	}

	memset(l, 0, ASCIILINESZ+1);
	i=0 ;
	while (s[i] && i<ASCIILINESZ)
	{
		l[i] = (char)tolower((int)s[i]);
		i++ ;
	}
	l[ASCIILINESZ]=(char)0;

	return l ;
}

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
char * strupc(char * s)
{
	static char l[ASCIILINESZ+1];
	int i ;

	if (s==NULL) 
	{
		return NULL ;
	}

	memset(l, 0, ASCIILINESZ+1);
	i=0 ;
	while (s[i] && i<ASCIILINESZ)
	{
		l[i] = (char)toupper((int)s[i]);
		i++ ;
	}

	l[ASCIILINESZ]=(char)0;
	return l ;
}

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
char * strskp(char * s)
{
	char * skip = s;
	if (s==NULL)
	{
		return NULL ;
	}

	while (isspace((int)*skip) && *skip)
	{
		skip++;
	}
	return skip ;
} 

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
char * strcrop(char * s)
{
	static char l[ASCIILINESZ+1];
	char * last ;

	if (s==NULL)
	{
		return NULL ;
	}

	memset(l, 0, ASCIILINESZ+1);
	strcpy(l, s);
	last = l + strlen(l);
	while (last > l)
	{
		if (!isspace((int)*(last-1)))
		{
			break ;
		}
		last -- ;
	}
	*last = (char)0;
	return l ;
}

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
char * strstrip(char * s)
{
	static char l[ASCIILINESZ+1];
	char * last ;

	if (s==NULL)
	{
		return NULL ;
	}

	while (isspace((int)*s) && *s) 
	{
		s++;
	}

	memset(l, 0, ASCIILINESZ+1);
	strcpy(l, s);
	last = l + strlen(l);
	while (last > l)
	{
		if (!isspace((int)*(last-1)))
		{
			break ;
		}
		last -- ;
	}

	*last = (char)0;

	return (char*)l ;
}

}; //!namespace INI {