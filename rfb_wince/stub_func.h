#ifndef _STUB_FUNC_H_
#define _STUB_FUNC_H_

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

int chsize(int fd, long size);
int remove(const char *path);
void setbuf(FILE *stream, char *buffer);
void _tzset( void );
int rename(const char *oldname, const char *newname);

#ifdef __cplusplus
}
#endif

#endif // !_STUB_FUNC_H_