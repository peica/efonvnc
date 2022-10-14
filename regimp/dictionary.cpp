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

#include "dictionary.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*#include <unistd.h>*/


/** Maximum value size for integers and doubles. */
#define MAXVALSZ  1024

/** Minimal allocated number of entries in a dictionary */
#define DICTMINSZ 128

/** Invalid key token */
#define DICT_INVALID_KEY    ((char*)-1)

namespace INI {

/*************************************************
  Function:			mem_double
  Description:		Doubles the allocated size associated to a pointer
  Calls:			
  Called By:		
  Table Accessed:
  Table Updated:
  Input:			
  Output:
  Return:			
  Others:			'size' is the current allocated size
*************************************************/
static void * mem_double(void * ptr, int size)
{
	void    *   newptr ;

	newptr = calloc(2*size, 1);
	memcpy(newptr, ptr, size);
	free(ptr);
	return newptr ;
}

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
unsigned dictionary_hash(char * key)
{
	int     len ;
	unsigned  hash ;
	int     i ;

	len = strlen(key);
	for (hash=0, i=0 ; i<len ; i++)
	{

		hash += (unsigned)key[i] ;
		hash += (hash<<10);
		hash ^= (hash>>6) ;
	}
	hash += (hash <<3);
	hash ^= (hash >>11);
	hash += (hash <<15);
	return hash ;
}

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
dictionary * dictionary_new(int size)
{
	dictionary  * d ;

	/* If no size was specified, allocate space for DICTMINSZ */
	if (size<DICTMINSZ) size=DICTMINSZ ;

	d = (dictionary *)calloc(1, sizeof(dictionary));
	d->size = size ;
	d->val  = (char**)calloc(size, sizeof(char*));
	d->key  = (char**)calloc(size, sizeof(char*));
	d->hash = (unsigned *)calloc(size, sizeof(unsigned));
	return d ;
}

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
void dictionary_del(dictionary * d)
{
	int   i ;

	if (d==NULL) return ;
	for (i=0 ; i<d->size ; i++)
	{
		if (d->key[i]!=NULL)
		{
			free(d->key[i]);
		}
		if (d->val[i]!=NULL)
		{
			free(d->val[i]);
		}
	}

	free(d->val);
	free(d->key);
	free(d->hash);
	free(d);
	return ;
}

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
char * dictionary_get(dictionary * d, char * key, char * def)
{
	unsigned  hash ;
	int     i ;

	hash = dictionary_hash(key);
	for (i=0 ; i<d->size ; i++)
	{
		if (d->key==NULL)
		{
			continue ;
		}
		/* Compare hash */
		if (hash==d->hash[i])
		{
			/* Compare string, to avoid hash collisions */
			if (!strcmp(key, d->key[i]))
			{
				return d->val[i] ;
			}
		}
	}
	return def ;
}

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
char dictionary_getchar(dictionary * d, char * key, char def)
{
	char * v ;

	if ((v=dictionary_get(d,key,DICT_INVALID_KEY))==DICT_INVALID_KEY)
	{
		return def ;
	}else
	{
		return v[0] ;
	}
}

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
int dictionary_getint(dictionary * d, char * key, int def)
{
	char * v ;

	if ((v=dictionary_get(d,key,DICT_INVALID_KEY))==DICT_INVALID_KEY)
	{
		return def ;
	}
	else
	{
		return atoi(v);
	}
}

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
double dictionary_getdouble(dictionary * d, char * key, double def)
{
	char * v ;

	if ((v=dictionary_get(d,key,DICT_INVALID_KEY))==DICT_INVALID_KEY)
	{
		return def ;
	}
	else 
	{
		return atof(v);
	}
}

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
void dictionary_set(dictionary * d, char * key, char * val)
{
	int     i ;
	unsigned  hash ;

	if (d==NULL || key==NULL)
	{
		return ;
	}

	/* Compute hash for this key */
	hash = dictionary_hash(key) ;
	/* Find if value is already in blackboard */
	if (d->n>0)
	{
		for (i=0 ; i<d->size ; i++)
		{
			if (d->key[i]==NULL)
			{
				continue ;
			}
			if (hash==d->hash[i]) 
			{ /* Same hash value */
				if (!strcmp(key, d->key[i])) 
				{   /* Same key */
					/* Found a value: modify and return */
					if (d->val[i]!=NULL)
					{
						free(d->val[i]);
					}

					d->val[i] = val ? _strdup(val) : NULL ;
					/* Value has been modified: return */
					return ;
				}
			}
		}
	}

	/* Add a new value */
	/* See if dictionary needs to grow */
	if (d->n==d->size)
	{		
		/* Reached maximum size: reallocate blackboard */
		d->val  = (char **)mem_double(d->val,  d->size * sizeof(char*)) ;
		d->key  = (char **)mem_double(d->key,  d->size * sizeof(char*)) ;
		d->hash = (unsigned *)mem_double(d->hash, d->size * sizeof(unsigned)) ;

		/* Double size */
		d->size *= 2 ;
	}

	/* Insert key in the first empty slot */
	for (i=0 ; i<d->size ; i++)
	{
		if (d->key[i]==NULL)
		{
			/* Add key here */
			break ;
		}
	}

	/* Copy key */
	d->key[i]  = _strdup(key);
	d->val[i]  = val ? _strdup(val) : NULL ;
	d->hash[i] = hash;
	d->n ++ ;
		
	return ;
}

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
void dictionary_unset(dictionary * d, char * key)
{
	unsigned  hash ;
	int     i ;

	hash = dictionary_hash(key);
	for (i=0 ; i<d->size ; i++)
	{
		if (d->key[i]==NULL)
		{
			continue ;
		}

		/* Compare hash */
		if (hash==d->hash[i])
		{
			/* Compare string, to avoid hash collisions */
			if (!strcmp(key, d->key[i]))
			{
				/* Found key */
				break ;
			}
		}
	}
	if (i>=d->size)
	{
		/* Key not found */
		return ;
	}

	free(d->key[i]);
	d->key[i] = NULL ;
	if (d->val[i]!=NULL)
	{
		free(d->val[i]);
		d->val[i] = NULL ;
	}

	d->hash[i] = 0 ;
	d->n -- ;
	return ;
}

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
void dictionary_setint(dictionary * d, char * key, int val)
{
	char  sval[MAXVALSZ];
	sprintf(sval, "%d", val);
	dictionary_set(d, key, sval);
}

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
void dictionary_setdouble(dictionary * d, char * key, double val)
{
	char  sval[MAXVALSZ];
	sprintf(sval, "%g", val);
	dictionary_set(d, key, sval);
}

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
void dictionary_dump(dictionary * d, FILE * out)
{
	int   i ;

	if (d==NULL || out==NULL) return ;
	if (d->n<1) {
		fprintf(out, "empty dictionary\n");
		return ;
	}
	for (i=0 ; i<d->size ; i++) {
		if (d->key[i]) {
			fprintf(out, "%20s\t[%s]\n",
				d->key[i],
				d->val[i] ? d->val[i] : "UNDEF");
		}
	}
	return ;
}

}; // !namespace INI {
