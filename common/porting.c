/*
 * static char *rcsid_porting_c =
 *   "$Id$";
 */

/*
    CrossFire, A Multiplayer game for X-windows

    Copyright (C) 2002 Mark Wedel & Crossfire Development Team
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

/* This file contains various functions that are not really unique for
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

/* Has to be after above includes so we don't redefine some values */
#include "global.h"

static unsigned int curtmp = 0;

/*****************************************************************************
 * File related functions
 ****************************************************************************/

/*
 * A replacement for the tempnam() function since it's not defined
 * at some unix variants.
 */

char *tempnam_local(char *dir, char *pfx)
{
    char *f, *name;
    pid_t pid=getpid();

    if (!(name = (char *) malloc(MAXPATHLEN)))
	return(NULL);

    if (!pfx)
	pfx = "cftmp.";

    /* This is a pretty simple method - put the pid as a hex digit and
     * just keep incrementing the last digit.  Check to see if the file
     * already exists - if so, we'll just keep looking - eventually we should
     * find one that is free.
     */
    if ((f = (char *)dir)!=NULL) {
	do {
#ifdef HAVE_SNPRINTF
	    (void)snprintf(name, MAXPATHLEN, "%s/%s%hx.%d", f, pfx, pid, curtmp);
#else
	    (void)sprintf(name,"%s/%s%hx%d", f, pfx, pid, curtmp);
#endif
	    curtmp++;
	} while (access(name, F_OK)!=-1);
	return(name);
    }
  return(NULL);
}



/* This function removes everything in the directory. */
void remove_directory(const char *path)
{
    DIR *dirp;
    char buf[MAX_BUF];
    struct stat statbuf;
    int status;

    if ((dirp=opendir(path))!=NULL) {
	struct dirent *de;

	for (de=readdir(dirp); de; de = readdir(dirp)) {
	    status=stat(de->d_name, &statbuf);
	    /* Linus actually has a type field in the dirent structure,
	     * but that is not portable - stat should be portable
	     */
	    if ((status!=-1) && (S_ISDIR(statbuf.st_mode))) {
		sprintf(buf,"%s/%s", path, de->d_name);
		remove_directory(buf);
		continue;
	    }
	    /* Don't remove '.' or '..'  In  theory we should do a better 
	     * check for .., but the directories we are removing are fairly
	     * limited and should not have dot files in them.
	     */
	    if (de->d_name[0] == '.') continue;
	    sprintf(buf,"%s/%s", path, de->d_name);
	    if (unlink(buf)) {
		LOG(llevError,"Unable to remove directory %s\n", path);
	    }
	}
	closedir(dirp);
    }
    if (unlink(path)) {
	LOG(llevError,"Unable to remove directory %s\n", path);
    }
}

#if defined(sgi)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define popen fixed_popen

FILE *popen_local(const char *command, const char *type)
{
	int		fd[2];
	int		pd;
	FILE	*ret;
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



/*
 * A replacement of strdup(), since it's not defined at some
 * unix variants.
 */
char *strdup_local(char *str) {
  char *c=(char *)malloc(sizeof(char)*(strlen(str)+1));
  strcpy(c,str);
  return c;
}


#define DIGIT(x)        (isdigit(x) ? (x) - '0' : \
islower (x) ? (x) + 10 - 'a' : (x) + 10 - 'A')
#define MBASE ('z' - 'a' + 1 + 10)

/*
 * A replacement of strtol() since it's not defined at
 * many unix systems.
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

/* This seems to be lacking on some system */
#if !defined(HAVE_STRNCASECMP)
int strncasecmp(char *s1, char *s2, int n)
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
int strcasecmp(char *s1, char*s2)
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


/* This takes an err number and returns a string with a description of
 * the error.
 */
char *strerror_local(int errnum)
{
#if defined(HAVE_STRERROR)
    return(strerror(errnum));
#else
    return("strerror_local not implemented");
#endif
}

/*
 * Based on (n+1)^2 = n^2 + 2n + 1
 * given that	1^2 = 1, then
 *		2^2 = 1 + (2 + 1) = 1 + 3 = 4
 * 		3^2 = 4 + (4 + 1) = 4 + 5 = 1 + 3 + 5 = 9
 * 		4^2 = 9 + (6 + 1) = 9 + 7 = 1 + 3 + 5 + 7 = 16
 *		...
 * In other words, a square number can be express as the sum of the
 * series n^2 = 1 + 3 + ... + (2n-1)
 */
int
isqrt(n)
int n;
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


/*
 * returns a char-pointer to a static array, in which a representation
 * of the decimal number given will be stored.
 */

char *ltostr10(signed long n) {
  static char buf[10];
  char *cp=buf+9;
  long flag;

  *cp='\0';
  if(n<0)
    flag= n = -n;
  else
    flag=0;
  do {
    *(--cp) = '0'+n%10;
    n/=10;
  } while(n);
  if(flag)
    *(--cp)='-';
  return cp;
}

/*
 * A fast routine which appends the name and decimal number specified
 * to the given buffer.
 * Could be faster, though, if the strcat()s at the end could be changed
 * into alternate strcat which returned a pointer to the _end_, not the
 * start!
 */

void save_long(char *buf, char *name, long n) {
#if 0 /* This doesn't work, since buf is always the beginning */
  char *cp, *var;
  for(cp=buf;*name!='\0';)
    *cp++ = *name++;
  *cp++=' ';
  for(var=ltostr10(n);*var!='\0';)
    *cp++ = *name++;
  *cp='\0';
#else
  char buf2[MAX_BUF];
  strcpy(buf2,name);
  strcat(buf2," ");
  strcat(buf2,ltostr10(n));
  strcat(buf2,"\n");
  strcat(buf,buf2);
#endif
}

/* This is a list of the suffix, uncompress and compress functions.  Thus,
 * if you have some other compress program you want to use, the only thing
 * that needs to be done is to extended this.
 * The first entry must be NULL - this is what is used for non
 * compressed files.
 */
char *uncomp[NROF_COMPRESS_METHODS][3] = {
    {NULL, NULL, NULL},
    {".Z", UNCOMPRESS, COMPRESS},
    {".gz", GUNZIP, GZIP},
    {".bz2", BUNZIP, BZIP}
};


/*
 * open_and_uncompress() first searches for the original filename.
 * if it exist, then it opens it and returns the file-pointer.
 * if not, it does two things depending on the flag.  If the flag
 * is set, it tries to create the original file by uncompressing a .Z file.
 * If the flag is not set, it creates a pipe that is used for
 * reading the file (NOTE - you can not use fseek on pipes)
 *
 * The compressed pointer is set to nonzero if the file is
 * compressed (and thus,  fp is actually a pipe.)  It returns 0
 * if it is a normal file
 *
 * (Note, the COMPRESS_SUFFIX is used instead of ".Z", thus it can easily
 * be changed in the config file.)
 */

FILE *open_and_uncompress(char *name,int flag, int *compressed) {
  FILE *fp;
  char buf[MAX_BUF],buf2[MAX_BUF], *bufend;
  int try_once = 0;

  strcpy(buf, name);
  bufend = buf + strlen(buf);

/*  LOG(llevDebug, "open_and_uncompress(%s)\n", name);
*/

  /* strip off any compression prefixes that may exist */
  for (*compressed = 0; *compressed < NROF_COMPRESS_METHODS; (*compressed)++) {
    if ((uncomp[*compressed][0]) &&
      (!strcmp(uncomp[*compressed][0], bufend - strlen(uncomp[*compressed][0])))) {
	buf[strlen(buf) - strlen(uncomp[*compressed][0])] = '\0';
	bufend = buf + strlen(buf);
    }
  }
  for (*compressed = 0; *compressed < NROF_COMPRESS_METHODS; (*compressed)++) {
    struct stat statbuf;

    if (uncomp[*compressed][0])
        strcpy(bufend, uncomp[*compressed][0]);
    if (stat(buf, &statbuf)) {

/*      LOG(llevDebug, "Failed to stat %s\n", buf);
*/
      continue;
    }
/*    LOG(llevDebug, "Found file %s\n", buf);
*/
    if (uncomp[*compressed][0]) {
      strcpy(buf2, uncomp[*compressed][1]);
      strcat(buf2, " < ");
      strcat(buf2, buf);
      if (flag) {
        int i;
        if (try_once) {
          LOG(llevError, "Failed to open %s after decompression.\n", name);
          return NULL;
        }
        try_once = 1;
        strcat(buf2, " > ");
        strcat(buf2, name);
        LOG(llevDebug, "system(%s)\n", buf2);
        if ((i=system(buf2))) {
          LOG(llevError, "system(%s) returned %d\n", buf2, i);
          return NULL;
        }
        unlink(buf);		/* Delete the original */
        *compressed = '\0';	/* Restart the loop from the beginning */
        chmod(name, statbuf.st_mode);
        continue;
      }
      if ((fp = popen(buf2, "r")) != NULL)
        return fp;
    } else if((fp=fopen(name,"r"))!=NULL) {
      struct stat statbuf;
      if (fstat (fileno (fp), &statbuf) || ! S_ISREG (statbuf.st_mode)) {
        LOG (llevDebug, "Can't open %s - not a regular file\n", name);
        (void) fclose (fp);
        errno = EISDIR;
        return NULL;
      }
      return fp;
    }
  }
  LOG(llevDebug, "Can't open %s\n", name);
  return NULL;
}

/*
 * See open_and_uncompress().
 */

void close_and_delete(FILE *fp, int compressed) {
  if (compressed)
    pclose(fp);
  else
    fclose(fp);
}

/*
 * If any directories in the given path doesn't exist, they are created.
 */

void make_path_to_file (char *filename)
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
	    if (mkdir (buf, 0777)) {
		perror ("Couldn't make path to file");
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

