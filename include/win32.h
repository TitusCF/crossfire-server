#ifndef WIN32_H
#define WIN32_H

#ifndef UNICODE
#define UNICODE
#endif

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <dirent.h>
#include <time.h>
#include <direct.h>
#include <sys/stat.h>   /* somewhat odd, but you don't get stat here with __STDC__ */

#ifndef socklen_t
#define socklen_t int /* Doesn't exist, just a plain int */
#endif

#ifndef WIN32
#define WIN32
#endif

#define NAMLEN(dirent) strlen((dirent)->d_name)

/* Function prototypes */
extern DIR *opendir(const char *);
extern struct dirent *readdir(DIR *);
extern int closedir(DIR *);
extern void rewinddir(DIR *);
extern int strcasecmp(const char *s1, const char *s2);
extern const char *strcasestr(const char *s, const char *find);
extern void service_register();
extern void service_unregister();
extern void service_handle();

#ifndef S_ISGID
#define S_ISGID 0002000
#endif
#ifndef S_IWOTH
#define S_IWOTH 0000200
#endif
#ifndef S_IWGRP
#define S_IWGRP 0000020
#endif
#ifndef S_IWUSR
#define S_IWUSR 0000002
#endif
#ifndef S_IROTH
#define S_IROTH 0000400
#endif
#ifndef S_IRGRP
#define S_IRGRP 0000040
#endif
#ifndef S_IRUSR
#define S_IRUSR 0000004
#endif

/* For Win32 service */
extern int bRunning;

#if 0

/**
 * @file
 * Structures and types used to implement opendir/readdir/closedir
 * on Windows 95/NT and set the loe level defines.
 *
 * Also some Windows-specific includes and tweaks.
 */

#if !defined(AFX_STDAFX_H__31666CA1_2474_11D5_AE6C_F07569C10000__INCLUDED_)
#define AFX_STDAFX_H__31666CA1_2474_11D5_AE6C_F07569C10000__INCLUDED_

/* Define the version here.  In Unixland, it's defined on the command line now. */
#define VERSION "1.9.1 (trunk) snapshot 20061229"

#pragma warning(disable: 4761) /* integral size mismatch in argument; conversion supplied */

#if _MSC_VER > 1000
#pragma once
#endif /* _MSC_VER > 1000 */

#endif /* !defined(AFX_STDAFX_H__31666CA1_2474_11D5_AE6C_F07569C10000__INCLUDED_) */

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>
#include <winsock2.h>
#include <time.h>
#include <math.h>

#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <process.h>

#define __STDC__ 1      /* something odd, CF want this, but don'T include it */
                        /* before the standard includes */

#define snprintf _snprintf

/* include all needed autoconfig.h defines */
#define CS_LOGSTATS
#define MAXPATHLEN 256
#define HAVE_STRTOL
#define HAVE_STRERROR

/* Many defines to redirect unix functions or fake standard unix values */
#define inline __inline
#define unlink(__a) _unlink(__a)
#define mkdir(__a, __b) mkdir(__a)
#define getpid() _getpid()
#define popen(__a, __b) _popen(__a, __b)
#define pclose(__a) _pclose(__a)
#define vsnprintf _vsnprintf
#define strtok_r(x, y, z) strtok(x, y)

#define R_OK 6          /* for __access() */
#define F_OK 6

#define PREFIXDIR ""

#define S_ISDIR(x) (((x)&S_IFMT) == S_IFDIR)
#define S_ISREG(x) (((x)&S_IFMT) == S_IFREG)

#define WIFEXITED(x) 1
#define WEXITSTATUS(x) x

/* Location of read-only machine independent data */
#define DATADIR "share"
#define LIBDIR "share"
#define CONFDIR "share"

/* Location of changeable single system data (temp maps, hiscore, etc) */
#define LOCALDIR "var"

/* Suffix for libraries */
#define PLUGIN_SUFFIX ".dll"

/* struct dirent - same as Unix */

typedef struct dirent {
    long d_ino;                         /* inode (always 1 in WIN32) */
    off_t d_off;                        /* offset to this dirent */
    unsigned short d_reclen;            /* length of d_name */
    char d_name[_MAX_FNAME+1];          /* filename (null terminated) */
}dirent;

#define NAMLEN(dirent) strlen((dirent)->d_name)


#define HAVE_LIBCURL

/* Win32's Sleep takes milliseconds, not seconds. */
#define sleep(x) Sleep(x*1000)

/**
 * Type definitions for fixed-size integer types. This is only required on
 * Windows when compiling with Visual Studio because autoconf makes sure that
 * certain types exist.
 */

typedef signed char     int8_t;
typedef signed int      int32_t;
typedef signed short    int16_t;
typedef unsigned char   uint8_t;
typedef unsigned int    uint32_t;
typedef unsigned short  uint16_t;

/* Python plugin stuff defines SIZEOF_LONG_LONG as 8, and besides __int64 is a 64b type on MSVC...
 * So let's force the typedef */
typedef unsigned __int64        uint64_t;
typedef signed __int64          int64_t;
/* Needed for experience */
#define atoll   _atoi64

#define FMT64                   "I64d"
#define FMT64U                  "I64u"

/* To reduce number of warnings */
#pragma warning(disable: 4244) /* conversion from 'xxx' to 'yyy', possible loss of data */
#pragma warning(disable: 4305) /* initializing float f = 0.05; instead of f = 0.05f; */

#endif /* WIN32_H */

#endif
