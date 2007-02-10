/*
 * static char *rcsid_porting_c =
 *   "$Id$";
 */

/*
    CrossFire, A Multiplayer game for X-windows

    Copyright (C) 2006 Mark Wedel & Crossfire Development Team
    Copyright (C) 1992 Frank Tore Johansen

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

    The authors can be reached via e-mail at crossfire-devel@real-time.com
*/

/**
 * @file porting.c
 * This file contains various functions that are not really unique for
 * crossfire, but rather provides what should be standard functions 
 * for systems that do not have them.  In this way, most of the
 * nasty system dependent stuff is contained here, with the program
 * calling these functions.
 */


#ifdef WIN32 /* ---win32 exclude/include headers */
#include "process.h"
#define pid_t int  /* we include it non global, because there is a redefinition in python.h */
#else
#include <ctype.h>
#include <sys/stat.h>
#include <sys/wait.h>

#include <sys/param.h>
#include <stdio.h>

/* Need to pull in the HAVE_... values somehow */
/* win32 reminder: always put this in a ifndef win32 block */
#include <autoconf.h>
#endif


#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <stdarg.h>
/* Has to be after above includes so we don't redefine some values */
#include "global.h"

/** Used to generate temporary unique name. */
static unsigned int curtmp = 0;

/*****************************************************************************
 * File related functions
 ****************************************************************************/

/**
 * A replacement for the tempnam() function since it's not defined
 * at some unix variants.
 *
 * @param dir
 * directory where to create the file. Can be NULL, in which case NULL is returned.
 * @param pfx
 * prefix to create unique name. Can be NULL.
 * @return
 * path to temporary file, or NULL if failure. Must be freed by caller.
 */
char *tempnam_local(const char *dir, const char *pfx)
{
    char *name;
    pid_t pid=getpid();

/* HURD does not have a hard limit, but we do */
#ifndef MAXPATHLEN
#define MAXPATHLEN 4096
#endif

    if (!pfx)
        pfx = "cftmp.";

    /* This is a pretty simple method - put the pid as a hex digit and
     * just keep incrementing the last digit.  Check to see if the file
     * already exists - if so, we'll just keep looking - eventually we should
     * find one that is free.
     */
    if (dir!=NULL) {
    if (!(name = (char *) malloc(MAXPATHLEN)))
        return(NULL);
        do {
#ifdef HAVE_SNPRINTF
            (void)snprintf(name, MAXPATHLEN, "%s/%s%hx.%d", dir, pfx, pid, curtmp);
#else
            (void)sprintf(name,"%s/%s%hx%d", dir, pfx, pid, curtmp);
#endif
            curtmp++;
        } while (access(name, F_OK)!=-1);
        return(name);
    }
  return(NULL);
}



/**
 * This function removes everything in the directory, and the directory itself.
 *
 * Errors are LOG() to error level.
 *
 * @param path
 * directory to remove.
 *
 * @note
 * will fail if any file has a name starting by .
 */
void remove_directory(const char *path)
{
    DIR *dirp;
    char buf[MAX_BUF];
    struct stat statbuf;
    int status;

    if ((dirp=opendir(path))!=NULL) {
        struct dirent *de;

        for (de=readdir(dirp); de; de = readdir(dirp)) {
            /* Don't remove '.' or '..'  In  theory we should do a better 
             * check for .., but the directories we are removing are fairly
             * limited and should not have dot files in them.
             */
            if (de->d_name[0] == '.') continue;

            /* Linux actually has a type field in the dirent structure,
             * but that is not portable - stat should be portable
             */
            status=stat(de->d_name, &statbuf);
            if ((status!=-1) && (S_ISDIR(statbuf.st_mode))) {
                sprintf(buf,"%s/%s", path, de->d_name);
                remove_directory(buf);
                continue;
            }
            sprintf(buf,"%s/%s", path, de->d_name);
            if (unlink(buf)) {
                LOG(llevError,"Unable to remove %s\n", path);
            }
        }
        closedir(dirp);
    }
    if (rmdir(path)) {
        LOG(llevError,"Unable to remove directory %s\n", path);
    }
}

#if defined(sgi)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define popen fixed_popen

/**
 * Executes a command in the background through a call to /bin/sh.
 *
 * @param command
 * command which will be launched.
 * @param type
 * whether we want to read or write to that command. Must be "r" or "w".
 * @return
 * pointer to stream to command, NULL on failure.
 * @note
 * for SGI only.
 *
 * @todo
 * is this actually used?
 */
FILE *popen_local(const char *command, const char *type)
{
    int     fd[2];
    int     pd;
    FILE    *ret;
    if (!strcmp(type,"r"))
    {
        pd=STDOUT_FILENO;
    }
    else if (!strcmp(type,"w"))
    {
        pd=STDIN_FILENO;
    }
    else
    {
        return NULL;
    }
    if (pipe(fd)!=-1)
    {
        switch (fork())
        {
        case -1:
            close(fd[0]);
            close(fd[1]);
            break;
        case 0:
            close(fd[0]);
            if ((fd[1]==pd)||(dup2(fd[1],pd)==pd))
            {
                if (fd[1]!=pd)
                {
                    close(fd[1]);
                }
                execl("/bin/sh","sh","-c",command,NULL);
                close(pd);
            }
            exit(1);
            break;
        default:
            close(fd[1]);
            if (ret=fdopen(fd[0],type))
            {
                return ret;
            }
            close(fd[0]);
            break;
        }
    }
    return NULL;
}

#endif /* defined(sgi) */


/*****************************************************************************
 * String related function
 ****************************************************************************/



/**
 * A replacement of strdup(), since it's not defined at some
 * unix variants.
 *
 * @param str
 * string to duplicate.
 * @return
 * copy, needs to be freed by caller. NULL on memory allocation error.
 */
char *strdup_local(const char *str) {
    char *c=(char *)malloc(strlen(str)+1);
    if (c!=NULL)
        strcpy(c,str);
    return c;
}

/** Converts x to number */
#define DIGIT(x)        (isdigit(x) ? (x) - '0' : \
islower (x) ? (x) + 10 - 'a' : (x) + 10 - 'A')
#define MBASE ('z' - 'a' + 1 + 10)

/**
 * Converts a string to long.
 *
 * A replacement of strtol() since it's not defined at
 * many unix systems.
 *
 * @param str
 * string to convert.
 * @param ptr
 * will point to first invalid character in str.
 * @param base
 * base to consider to convert to long.
 *
 * @todo
 * check weird -+ handling (missing break?)
 */
long strtol_local(str, ptr, base)
        register char *str;
        char **ptr;
        register int base;
{
    register long val;
    register int c;
    int xx, neg = 0;

    if (ptr != (char **) 0)
        *ptr = str;         /* in case no number is formed */
    if (base < 0 || base > MBASE)
        return (0);         /* base is invalid */
    if (!isalnum (c = *str)) {
        while (isspace (c))
            c = *++str;
        switch (c) {
            case '-':
                neg++;
            case '+':
                c = *++str;
        }
    }
    if (base == 0) {
        if (c != '0')
        base = 10;
        else {
        if (str[1] == 'x' || str[1] == 'X')
            base = 16;
        else
            base = 8;
        }
    }
    /*
    ** For any base > 10, the digits incrementally following
    ** 9 are assumed to be "abc...z" or "ABC...Z"
    */
    if (!isalnum (c) || (xx = DIGIT (c)) >= base)
        return 0;           /* no number formed */
    if (base == 16 && c == '0' && isxdigit (str[2]) &&
      (str[1] == 'x' || str[1] == 'X'))
        c = *(str += 2);    /* skip over leading "0x" or "0X" */
    for (val = -DIGIT (c); isalnum (c = *++str) && (xx = DIGIT (c)) < base;)
        /* accumulate neg avoids surprises near
        MAXLONG */
        val = base * val - xx;
    if (ptr != (char **) 0)
        *ptr = str;
    return (neg ? val : -val);
}

/**
 * Case-insensitive comparaison of strings.
 *
 * This seems to be lacking on some system.
 *
 * @param s1
 * @param s2
 * strings to compare.
 * @param n
 * maximum number of chars to compare.
 * @return
 * @li -1 if s1 is less than s2
 * @li 0 if s1 equals s2
 * @li 1 if s1 is greater than s2
 */
#if !defined(HAVE_STRNCASECMP)
int strncasecmp(const char *s1, const char *s2, int n)
{
    register int c1, c2;

    while (*s1 && *s2 && n) {
        c1 = tolower(*s1);
        c2 = tolower(*s2);
        if (c1 != c2)
        return (c1 - c2);
        s1++;
        s2++;
        n--;
    }
    if (!n)
        return(0);
    return (int) (*s1 - *s2);
}
#endif

#if !defined(HAVE_STRCASECMP)
/**
 * Case-insensitive comparaison of strings.
 *
 * This seems to be lacking on some system.
 *
 * @param s1
 * @param s2
 * strings to compare.
 * @return
 * @li -1 if s1 is less than s2
 * @li 0 if s1 equals s2
 * @li 1 if s1 is greater than s2
 */
int strcasecmp(const char *s1, const char*s2)
{
    register int c1, c2;

    while (*s1 && *s2) {
        c1 = tolower(*s1);
        c2 = tolower(*s2);
        if (c1 != c2)
        return (c1 - c2);
        s1++;
        s2++;
    }
    if (*s1=='\0' && *s2=='\0')
        return 0;
    return (int) (*s1 - *s2);
}
#endif

/**
 * Finds a substring in a string, in a case-insensitive manner.
 *
 * @param s
 * string we're searching into.
 * @param find
 * string we're searching for.
 * @return
 * pointer to first occurrence of find in s, NULL if not found.
 */
const char *strcasestr_local(const char *s, const char *find)
{
    char c, sc;
    size_t len;

    if ((c = *find++) != 0) {
        c = tolower(c);
        len = strlen(find);
        do {
            do {
                 if ((sc = *s++) == 0)
                     return NULL;
            } while (tolower(sc) != c);
        } while (strncasecmp(s, find, len) != 0);
        s--;
     }
     return (char *)s;
}

#if !defined(HAVE_SNPRINTF)
/**
 * Formats to a string, in a size-safe way.
 *
 * @param dest
 * where to write.
 * @param max
 * max length of dest.
 * @param format
 * format specifier, and arguments.
 * @return
 * number of chars written to dest.
 *
 * @warning
 * this function will abort() if there is an overflow.
 *
 * @todo
 * try to do something better than abort()?
 */
int snprintf(char *dest, int max, const char *format, ...)
{
    va_list var;
    int ret;

    va_start(var, format);
    ret = vsprintf(dest, format, var);
    va_end(var);
    if (ret > max) abort();

    return ret;
}
#endif


/**
 * This takes an err number and returns a string with a description of
 * the error.
 *
 * @param errnum
 * error we want the description of.
 * @return
 * pointer to description.
 *
 * @note
 * this function will return a dummy string if strerror() doesn't exist on the current platform.
 * @todo
 * find a way to remove static buffer but keep easy logging stuff.
 */
char *strerror_local(int errnum)
{
    static error[MAX_BUF];
#if defined(HAVE_STRERROR)
    strerror_r(errnum, error, MAX_BUF);
#else
    strncpy(error, "strerror_local not implemented", MAX_BUF);
#endif
    return error;
}

/**
 * Computes the square root.
 * Based on (n+1)^2 = n^2 + 2n + 1
 * given that	1^2 = 1, then
 *		2^2 = 1 + (2 + 1) = 1 + 3 = 4
 * 		3^2 = 4 + (4 + 1) = 4 + 5 = 1 + 3 + 5 = 9
 * 		4^2 = 9 + (6 + 1) = 9 + 7 = 1 + 3 + 5 + 7 = 16
 *		...
 * In other words, a square number can be express as the sum of the
 * series n^2 = 1 + 3 + ... + (2n-1)
 *
 * @param n
 * number of which to compute the root.
 * @return
 * square root.
 */
int isqrt(int n)
{
    int result, sum, prev;
    result = 0;
    prev = sum = 1;
    while (sum <= n) {
        prev += 2;
        sum += prev;
        ++result;
    }
    return result;
}


/**
 * Converts a long to a string.
 *
 * @param n
 * long to convert.
 * @return
 * char-pointer to a static array, in which a representation
 * of the decimal number given will be stored.
 *
 * @todo
 * remove static buffer?
 */
char *ltostr10(signed long n) {
    static char buf[12]; /* maximum size is n=-2 billion, i.e. 11 characters+1
                            character for the trailing nul character */
    snprintf(buf, sizeof(buf), "%ld", n);
    return buf;
}

/**
 * Converts a double to a string.
 *
 * @param v
 * double to convert.
 * @return
 * char-pointer to a static array, in which a representation
 * of the decimal number given will be stored.
 *
 * @todo
 * remove static buffer?
 */
char *doubletostr10(double v){
    static char tbuf[200];
    sprintf(tbuf,"%f",v);
    return tbuf;
}

/**
 * A fast routine which appends the name and decimal number specified
 * to the given buffer.
 * Could be faster, though, if the strcat()s at the end could be changed
 * into alternate strcat which returned a pointer to the _end_, not the
 * start!
 *
 * Hey good news, it IS faster now, according to changes in get_ob_diff
 * Completly redone prototype and made define in loader.l. See changes there.
 * Didn't touch those for speed reason (don't use them anymore) .
 *                                                             Tchize
 *
 * @param buf
 * buffer we're appending to.
 * @param name
 * name of the long we're saving.
 * @param n
 * value to save.
 *
 * @todo
 * use safe string functions?
 */
void save_long(char *buf, const char *name, long n) {
    char buf2[MAX_BUF];
    strcpy(buf2,name);
    strcat(buf2," ");
    strcat(buf2,ltostr10(n));
    strcat(buf2,"\n");
    strcat(buf,buf2);
}

/**
 * Appends the name and value of a long long.
 *
 * @param buf
 * buffer we're appending to.
 * @param name
 * name of the long we're saving.
 * @param n
 * value to save.
 *
 * @todo
 * use safe string functions?
 */
void save_long_long(char *buf, char *name, sint64 n) {
    char buf2[MAX_BUF];

    sprintf(buf2,"%s %" FMT64 "\n", name, n);
    strcat(buf,buf2);
}

/**
 * This is a list of the suffix, uncompress and compress functions.  Thus,
 * if you have some other compress program you want to use, the only thing
 * that needs to be done is to extended this.
 * The first entry must be NULL - this is what is used for non
 * compressed files.
 */
const char *uncomp[NROF_COMPRESS_METHODS][3] = {
    {NULL, NULL, NULL},
    {".Z", UNCOMPRESS, COMPRESS},
    {".gz", GUNZIP, GZIP},
    {".bz2", BUNZIP, BZIP}
};


/**
 * Open and possibly uncompress a file.
 *
 * @param ext
 * the extension if the file is compressed.
 * @param uncompressor
 * the command to uncompress the file if the file is compressed.
 * @param name
 * the base file name without compression extension
 * @param flag
 * only used for compressed files:
 * @li if set, uncompress and open the file
 * @li if unset, uncompress the file via pipe
 * @param[out] compressed
 * set to zero if the file was uncompressed
 * @return
 * pointer to opened file, NULL on failure.
 *
 * @note
 * will set ::errno if an error occurs.
 */
static FILE *open_and_uncompress_file(const char *ext, const char *uncompressor, const char *name, int flag, int *compressed) {
    struct stat st;
    char buf[MAX_BUF];
    char buf2[MAX_BUF];
    int ret;

    if (ext == NULL) {
        ext = "";
    }

    if (strlen(name)+strlen(ext) >= sizeof(buf)) {
        errno = ENAMETOOLONG; /* File name too long */
        return NULL;
    }
    sprintf(buf, "%s%s", name, ext);

    if (stat(buf, &st) != 0) {
        return NULL;
    }

    if (!S_ISREG(st.st_mode)) {
        errno = EISDIR;         /* Not a regular file */
        return NULL;
    }

    if (uncompressor == NULL) {
        /* open without uncompression */

        return fopen(buf, "rb");
    }

    /* The file name buf (and its substring name) is passed as an argument to a
     * shell command, therefore check for characters that could confuse the
     * shell.
     */
    if (strpbrk(buf, "'\\\r\n") != NULL) {
        errno = ENOENT;         /* Pretend the file does not exist */
        return NULL;
    }

    if (!flag) {
        /* uncompress via pipe */

        if (strlen(uncompressor)+4+strlen(buf)+1 >= sizeof(buf2)) {
            errno = ENAMETOOLONG;       /* File name too long */
            return NULL;
        }
        sprintf(buf2, "%s < '%s'", uncompressor, buf);

        return popen(buf2, "r");
    }

    /* remove compression from file, then open file */

    if (stat(name, &st) == 0 && !S_ISREG(st.st_mode)) {
        errno = EISDIR;
        return NULL;
    }

    if (strlen(uncompressor)+4+strlen(buf)+5+strlen(name)+1 >= sizeof(buf2)) {
        errno = ENAMETOOLONG;   /* File name too long */
        return NULL;
    }
    sprintf(buf2, "%s < '%s' > '%s'", uncompressor, buf, name);

    ret = system(buf2);
    if (!WIFEXITED(ret) || WEXITSTATUS(ret) != 0) {
        LOG(llevError, "system(%s) returned %d\n", buf2, ret);
        errno = ENOENT;
        return NULL;
    }

    unlink(buf);                /* Delete the original */
    *compressed = 0;            /* Change to "uncompressed file" */
    chmod(name, st.st_mode);    /* Copy access mode from compressed file */

    return fopen(name, "rb");
}

/**
 * open_and_uncompress() first searches for the original filename. If it exist,
 * then it opens it and returns the file-pointer.
 *
 * If not, it does two things depending on the flag. If the flag is set, it
 * tries to create the original file by appending a compression suffix to name
 * and uncompressing it. If the flag is not set, it creates a pipe that is used
 * for reading the file (NOTE - you can not use fseek on pipes).
 *
 * The compressed pointer is set to nonzero if the file is compressed (and
 * thus, fp is actually a pipe.) It returns 0 if it is a normal file.
 *
 * @param name
 * the base file name without compression extension
 * @param flag
 * only used for compressed files:
 * @li if set, uncompress and open the file
 * @li if unset, uncompress the file via pipe
 * @param[out] compressed
 * set to zero if the file was uncompressed
 * @return
 * pointer to opened file, NULL on failure.
 *
 * @note
 * will set ::errno if an error occurs.
 */
FILE *open_and_uncompress(const char *name, int flag, int *compressed) {
    size_t i;
    FILE *fp;

    for (i = 0; i < NROF_COMPRESS_METHODS; i++) {
        *compressed = i;
        fp = open_and_uncompress_file(uncomp[i][0], uncomp[i][1], name, flag, compressed);
        if (fp != NULL) {
            return fp;
        }
    }

    errno = ENOENT;
    return NULL;
}

/**
 * Closes specified file.
 *
 * @param fp
 * file to close.
 * @param compressed
 * whether the file was compressed or not. Set by open_and_uncompress().
 */
void close_and_delete(FILE *fp, int compressed) {
    if (compressed)
        pclose(fp);
    else
        fclose(fp);
}

/**
 * Checks if any directories in the given path doesn't exist, and creates if necessary.
 *
 * @param filename
 * file path we'll want to access. Can be NULL.
 *
 * @note
 * will LOG() to debug and error.
 */
void make_path_to_file (const char *filename)
{
    char buf[MAX_BUF], *cp = buf;
    struct stat statbuf;

    if (!filename || !*filename)
        return;
    strcpy (buf, filename);
    LOG(llevDebug, "make_path_tofile %s...", filename);
    while ((cp = strchr (cp + 1, (int) '/'))) {
        *cp = '\0';
#if 0
        LOG(llevDebug, "\n Checking %s...", buf);
#endif
        if (stat(buf, &statbuf) || !S_ISDIR (statbuf.st_mode)) {
            LOG(llevDebug, "Was not dir...");
            if (mkdir (buf, SAVE_DIR_MODE)) {
                LOG(llevError, "Cannot mkdir %s: %s\n", buf, strerror_local(errno));
                return;
            }
#if 0
            LOG(llevDebug, "Made dir.");
        } else
            LOG(llevDebug, "Was dir");
#else
        }
#endif
        *cp = '/';
    }
    LOG(llevDebug,"\n");
}
