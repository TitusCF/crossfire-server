/*
 *  CrossEdit - game world editor
 *  Copyright (C) 1993 Jarkko Sonninen & Petri Heinila
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *  The authors can be reached via e-mail to Jarkko.Sonninen@lut.fi
 *  or Petri.Heinila@lut.fi .
 */

#include <Posix.h>
#include <Ansi.h>
#include <Str.h>

#include <global.h>
#include <debug.h>

#ifdef StrHasGeneratedSource
#include <StrG.c>
#endif

/**********************************************************************
 * private 
 **********************************************************************/


/**********************************************************************
 * public 
 **********************************************************************/

/*
 * member: makes absolute pathname from relative
 * src   : migration ???
 * return: result
 */
char *StrPathNormalize (char *this,char *src) {
    char *p, *q;
    char buf[MAX_BUF];
    static char path[MAX_BUF];

    LOG(llevDebug,"path before normalization >%s<>%s<\n", src, this);

    if (*this == '/') {
	strcpy (buf, this);

    } else {
	strcpy (buf, src);
	if ((p = strrchr (buf, '/')))
	    p[1] = '\0';
	else
	    strcpy (buf, "/");
	strcat (buf, this);
    }

    q = p = buf;
    while ((q = strstr (q, "//")))
	p = ++q;	

    *path = '\0';
    q = path;
    p = strtok (p, "/");
    while (p) {
	if (!strcmp (p, "..")) {
	    q = strrchr (path, '/');
	    if (q)
		*q = '\0';
	    else {
		*path = '\0';
		LOG (llevError, "Illegal path.\n");
	    }
	} else {
	    strcat (path, "/");
	    strcat (path, p);
	}
	p = strtok (NULL, "/");
    }
    LOG(llevDebug,"path after normalization >%s<\n", path);

    return (path);
}

/*
 * function: make new abs. path after cd
 * cwd     : current working directory, this have to be abs. path
 *           this have to be size PATH_MAX + 1, result is put to this
 * cd      : migration
 * return  : pointer to cwd, or NULL on error 
 */
char *StrPathCd(char *cwd,const char *cd)
{
    char *fields[PATH_MAX / 2 - 1],*fld,cdMove[PATH_MAX+1];
    char clt[PATH_MAX+1];
    int numFields,i;

    if(*cwd != '/') return NULL;
    if(*cd == '/') return strcpy(cwd,cd);

    /*** load cwd by parts ***/
    numFields = 0;
    if((fld = strtok(cwd,"/"))) {
	do {
	    fields[numFields] = fld;
	    numFields++;
	} while((fld = strtok(NULL,"/")));
    }
    /*** traverse path by cd ***/
    numFields--; /* filename in cwd off */
    strcpy(cdMove,cd);
    if((fld = strtok(cdMove,"/"))) {
	do {
	    if(!strcmp(fld,".")) continue;
	    if(!strcmp(fld,"..")) {
		numFields--;
	    } else {
		fields[numFields] = fld;
		numFields++;
	    }
	} while((fld = strtok(NULL,"/")));
    }
    /*** collect them ***/
    strcpy(clt,"");
    for(i=0; i < numFields; i++) {
	strcat(clt,"/");
	strcat(clt,fields[i]);
    }
    return strcpy(cwd,clt);
}

/*
 * function: genrestes reletive cd argument, in which can move from
 *           cwd to dst
 * cwd     : currenntr working directory, gerenerated values is subtituted to
 *           this
 * dst     : destination directory
 * return  : pointer to cwd, or NULL on error 
 */
char *StrPathGenCd(char *cwd,const char *dstArg)
{
#if 0
    int i,j,cnt;
    char buf[BUFSIZ];

    for(i=0; dst[i] && cwd[i] && dst[i] == cwd[i]; i++);

    debug2("cwd:%s dst:%s\n",cwd + i,dst + i); 

    if(strtok(cwd + i,"/")) {
	cnt += 1;
	while(strtok(NULL,"/")) cnt++;
    }
    buf[0] = 0;
    if(cnt > 0) {
	for(j=0; j < cnt; j++) strcat(buf,"../");
	strcat(buf,dst + i);
    } else {
	strcpy(buf,dst + i + 1);
    }
#else
    char dst[PATH_MAX+1];   
    char *cwdFields[PATH_MAX / 2 - 1],*dstFields[PATH_MAX / 2 - 1];   
    int cwdNum,dstNum,i,start;
    char *fld,buf[PATH_MAX+1];

    /*** save dst ***/
    strcpy(dst,dstArg);
    /*** load cwd by parts ***/
    cwdNum = 0;
    if((fld = strtok(cwd,"/"))) {
	do {
	    cwdFields[cwdNum] = fld;
	    cwdNum++;
	} while((fld = strtok(NULL,"/")));
    }
    /*** load dst by parts ***/
    dstNum = 0;
    if((fld = strtok(dst,"/"))) {
	do {
	    dstFields[dstNum] = fld;
	    dstNum++;
	} while((fld = strtok(NULL,"/")));
    }
    strcpy(buf,".");
    /*** wile same ***/
    for(start=0; 
	start < cwdNum && start < dstNum &&
	!strcmp(cwdFields[start],dstFields[start]); 
	start++);
    /*** whilke guing up***/
    for(i=start; i < cwdNum - 1; i++)
	strcat(buf,"/..");
#if 0   
    if(*buf)
	buf[strlen(buf)-1] = '\0';
#endif
    /*** while gong down ***/
    for(i=start; i < dstNum; i++) {
	strcat(buf,"/");
	strcat(buf,dstFields[i]);
    }
#endif 
    return strcpy(cwd,buf);
}

/*
 * function: gets base-part of filename as basename(1)
 * basename: allocated space for name
 * filename: destination directory
 * return  : pointer to basename, or NULL on error 
 */
char *StrBasename(char *basename,const char *filename)
{
    char buf[PATH_MAX+1],*fields[PATH_MAX / 2 - 1],*fld;
    int num;

    strcpy(buf,filename);
    /*** load cwd by parts ***/
    num = 0;
    if((fld = strtok(buf,"/"))) {
	do {
	    fields[num] = fld;
	    num++;
	} while((fld = strtok(NULL,"/")));
    } else {
	return strcpy(basename,"");
    }
    return strcpy(basename,fields[num - 1]);
}

/*** end of Str.c ***/
