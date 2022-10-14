///////////////////////////////////////////////////////////////////////////////
//
// Legalese:
// This file is part of the OpenNETCF time.h port for Windows CE.
// You are free to use, modify or distribute this code or any
// derivative work that you create.  This code is provided WITHOUT
// WARRANTY and OpenNETCF does NOT claim in any way that this code 
// is fit for any specific or general use and holds NO RESPONSIBILITY
// for consequences of using any of it.  It is simply provided as-is.
//
// About:
// This is part of a free and open project to provide a simply way to
// port that uses time.h functions to Windows CE.  For the latest
// code, or to submit fixes, feature additions, etc. visit:
//
// http://www.opennetcf.com
//
// Version 0.01 - March 22, 2007
//
///////////////////////////////////////////////////////////////////////////////


#ifndef __TIME_CE_H__
#define __TIME_CE_H__

//
// time_CE.h
//
 
#include <time.h>
#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _TIME_T_DEFINED
	typedef long time_t;		/* time value */
#define _TIME_T_DEFINED 	/* avoid multiple def's of time_t */
#endif
	
#ifndef _CLOCK_T_DEFINED
	typedef long clock_t;
#define _CLOCK_T_DEFINED
#endif
	
#ifndef _SIZE_T_DEFINED
	typedef unsigned int size_t;
#define _SIZE_T_DEFINED
#endif


extern LONG _localtime;
extern DWORD _isdst;
extern char tz_name[2][32];

// these pragmas help Studio to build for some CE platforms - use as necessary
//#pragma comment(linker, "/nodefaultlib:libc.lib")
//#pragma comment(linker, "/nodefaultlib:libcd.lib")
// #pragma comment(lib, "secchk.lib")

// Number of clock ticks per second. Used by the clock() function.
//
#define CLOCKS_PER_SEC		1000

// Macro for CLK_PER_SEC
//
#define CLK_TICK			CLOCKS_PER_SEC

static struct tm tmbuf;

void _tzset_ce(void);

// Convert tm to a string in the format "Www Mmm dd hh:mm:ss yyyy", 
// where Www is the weekday, Mmm the month in letters, dd the day 
// of the month, hh:mm:ss the time, and yyyy the year. The string 
// is followed by a newline and a terminating null character, 
// conforming a total of 26 characters. 
//
char * asctime_ce(const struct tm* tmptr);

// Return number of clock ticks since process start.
//
clock_t clock_ce(void);

// Convert time_t value to string in the same format as asctime.
//
char* ctime_ce(const time_t* timer);

// Returns the difference in seconds between the two times.
//
double difftime_ce(time_t timer2, time_t timer1);

// Convert a time_t value to a tm structure as UTC time. 
//
struct tm* gmtime_ce(const time_t* timer);

// Convert a time_t value to a tm structure as local time. 
//
struct tm* localtime_ce(const time_t* timer);

// Returns the Unix timestamp corresponding to the arguments given. 
// This timestamp is a long integer containing the number of seconds between the Unix Epoch 
// (January 1 1970 00:00:00 GMT) and the time specified. 
//
time_t mktime_ce(struct tm *tptr);

// Get the current time from the system clock. Stores that value in timer.
// If timer is null, the value is not stored, but it is still returned by
// the function.
//
time_t time_ce(time_t* timer);

// Format tm into a date/time string
//
size_t strftime_ce(char *s, size_t maxs, const char *f, const struct tm *t);

#define time time_ce
#define gmtime gmtime_ce
#define strftime strftime_ce

#ifdef __cplusplus
}
#endif

#endif