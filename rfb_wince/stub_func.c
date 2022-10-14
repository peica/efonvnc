
#if defined(_WIN32_WCE)

#if !defined(TEST_ON_PC)

#include <windows.h>

#define BUFSIZ  512

#define _IOFBF          0x0000
#define _IOLBF          0x0040
#define _IONBF          0x0004

// long timezone;

// char *_tzname[2];

int chsize(int fd, long size)
{	
	HANDLE hFD = (HANDLE)fd;

	if (GetFileSize(hFD, NULL) == 0xFFFFFFFF)
	{
		// we meet a invalid file descriptor
		return -1;
	}

	if (size == 0)
	{
		return -1;
	}

	SetFilePointer(hFD, size, 0, FILE_BEGIN);
	SetEndOfFile(hFD);	
	return 0;
}

int GetLocaleInfoA(
  IN LCID     Locale,
    IN LCTYPE   LCType,
    OUT LPSTR  lpLCData,
    IN int      cchData
)
{
	return (0);
}

int GetLocaleInfoW(
   IN LCID     Locale,
    IN LCTYPE   LCType,
    OUT LPWSTR  lpLCData,
    IN int      cchData
)
{
	return (0);
}

void _tzset( void )
{

}

void setbuf(FILE *stream, char *buffer)
{
	// NOTE: Peica don't confirm the buffer size is equal to 512,
	//		 so the user must care on it.
	setvbuf(stream, buffer, _IOFBF, BUFSIZ);
}

int remove(const char *path)
{
	WCHAR path_w[MAX_PATH + 1];

	if (path == 0 || strlen(path) == 0)
		return -1;

	memset(path_w, 
		0, 
		sizeof(path_w));

	mbstowcs(path_w, 
		path, 
		sizeof(path_w) - 1);

	return DeleteFile(path_w) 
		? 0 : -1;
}

int rename(const char *oldname, const char *newname)
{
	WCHAR oldname_w[MAX_PATH + 1];
	WCHAR newname_w[MAX_PATH + 1];
	
	memset(oldname_w, 0, sizeof(oldname_w));

	memset(newname_w, 0, sizeof(newname_w));

	mbstowcs(oldname_w, oldname, 
		sizeof(oldname_w) - 1);

	mbstowcs(newname_w, newname, 
		sizeof(newname_w) - 1);

	return MoveFile(oldname_w, newname_w) 
		? 0 : -1;
}

#endif
#endif