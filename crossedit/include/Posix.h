/*
 * reference: Portable C Software / Mark Horton, ISBN 0-13-868959-7
 */

#if !defined(_Posix_h) && !defined(__CEXTRACT__)
#define _Posix_h

/* sys/types needs to be included before POSIX_SOURCE is defined on
 * some systems.  Next is not one of them.
 */
#if 0
#ifndef NeXT
#include <sys/types.h>
#endif

/*
 * - have to be before includes
 */
#ifndef _POSIX_SOURCE
#define _POSIX_SOURCE
#endif
#endif
/* _Ultrix_ may not be the symbol name, but if you are on an Ultrix
 * system, make sure __POSIX is defined.
 */
#if defined(ultrix)
#define __POSIX
#endif

/*
 * have to be before <dirent.h>
 */
#include <sys/types.h>

/*
 * have to be before <dirent.h>
 */
#include <sys/stat.h>

/* NeXT apparantly needs NAME_MAX defined before dirent.h is included. */

#ifndef NAME_MAX
#define NAME_MAX   255  /* 14 grr max chars in filename, no term. null */
#endif

#include <dirent.h>

/* Remove complex logic - if it isn't defined, lets define it.  Lets not
 * care what OS we might be on.
 */
#if !defined(MAXNAMELEN)
#define MAXNAMELEN 255
#endif

#if !defined(MAXNAMLEN)
#define MAXNAMLEN 255
#endif

#include <errno.h>

#include <fcntl.h>
#ifndef O_NONBLOCK
#define O_NONBLOCK O_NDELAY
#endif

#include <limits.h>
#include <locale.h>
#include <pwd.h>
#include <setjmp.h>
#include <signal.h>
#include <stdio.h>
#include <sys/times.h>
#include <sys/utsname.h>
#include <unistd.h>

/*
 *
 */
#ifndef ARG_MAX
#define ARG_MAX    20478 /* */
#endif
#ifndef CHILD_MAX
#define CHILD_MAX  32 /* */
#endif
#ifndef OPEN_MAX
#define OPEN_MAX   20 /* */
#endif
#ifndef LINK_MAX
#define LINK_MAX   1000 /* */
#endif
#ifndef MAX_CANON
#define MAX_CANON  512 /* ;always */
#endif
#ifndef MAX_INPUT
#define MAX_INPUT  512 /* ;always */
#endif
#ifndef PATH_MAX
#define PATH_MAX   1023 /* max char in pathname, no term. null ;always */
#endif
#ifndef PIPE_BUF
#define PIPE_BUF   8192      /* ;always */
#endif
#ifndef STREAM_MAX
#define STREAM_MAX OPEN_MAX  /*  */
#endif
#ifndef _POSIX_ARG_MAX 
#define _POSIX_ARG_MAX 4096
#endif

#endif /* _Posix_h */
