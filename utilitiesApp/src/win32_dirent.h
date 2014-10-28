#ifndef WIN32_DIRENT_INCLUDED
#define WIN32_DIRENT_INCLUDED

/*

    Declaration of POSIX directory browsing functions and types for Win32.

    Author:  Kevlin Henney (kevlin@acm.org, kevlin@curbralan.com)
    History: Created March 1997. Updated June 2003.
	         Modified for EPICS build system Freddie Akeroyd 28/10/2014 (freddie.akeroyd@stfc.ac.uk)
    Rights:  See end of file.
    
*/

#ifdef __cplusplus
extern "C"
{
#endif

#include <shareLib.h>

typedef struct DIR DIR;

struct dirent
{
    char *d_name;
};

epicsShareFunc DIR           *opendir(const char *);
epicsShareFunc int           closedir(DIR *);
epicsShareFunc struct dirent *readdir(DIR *);
epicsShareFunc void          rewinddir(DIR *);

/*

    Copyright Kevlin Henney, 1997, 2003. All rights reserved.

    Permission to use, copy, modify, and distribute this software and its
    documentation for any purpose is hereby granted without fee, provided
    that this copyright and permissions notice appear in all copies and
    derivatives.
    
    This software is supplied "as is" without express or implied warranty.

    But that said, if there are any problems please get in touch.

*/

#ifdef __cplusplus
}
#endif

#endif
