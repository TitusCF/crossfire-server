/*
 * static char *rcsid_ban_c =
 *   "$Id$";
 */

/*
 * Ban.c
 * Code was grabbed from the netrek source and modified to work with 
 * crossfire. This function checks a file in the lib directory for any
 * banned players. If it finds one it returns a 1. Wildcards can be used.
 */ 

#include <global.h>
#include <sproto.h>
#ifndef WIN32 /* ---win32 : remove unix headers */
#include <sys/ioctl.h>
#endif /* win32 */
#ifdef hpux
#include <sys/ptyio.h>
#endif

#ifndef WIN32 /* ---win32 : remove unix headers */
#include <errno.h>
#include <stdio.h>
#include <sys/file.h>
#endif /* win32 */

int checkbanned(char *login, char *host)
{
  FILE  *bannedfile;
  char  buf[MAX_BUF];
  char  log_buf[64], host_buf[64], line_buf[160];
  char  *indexpos;
  int   num1;
  int   Hits=0;                 /* Hits==2 means we're banned */
  int	loop=0;
  
 while (loop < 2) {	/* have to check both ban files now */

		/* First time through look for BANFILE */

	 if (loop == 0){	
  	 sprintf (buf, "%s/%s", settings.confdir, BANFILE);
  		if ((bannedfile = fopen(buf, "r")) == NULL) {
    	LOG (llevDebug, "Could not find file Banned file\n");
    	loop++;	
  		}
	}
		
		/* Second time through look for BANISHFILE */

  	if (loop == 1){
  sprintf (buf, "%s/%s", settings.localdir, BANISHFILE);
		if ((bannedfile = fopen(buf, "r")) == NULL) {
    	LOG (llevDebug, "Could not find file Banish file\n");
    	return(0);
  		}
	}

	/* Do the actual work here checking for banned IPs */
  
  while(fgets(line_buf, 160, bannedfile) != NULL) {
    /* Split line up */
    if((*line_buf=='#')||(*line_buf=='\n'))
      continue;
    if ((indexpos = (char *) strrchr(line_buf, '@')) == 0) {
      LOG (llevDebug, "Bad line in banned file\n");
      continue;
    }
    num1 = indexpos - line_buf;
    strncpy(log_buf, line_buf, num1); /* copy login name into log_buf */
    log_buf[num1] = '\0';
    strncpy(host_buf, indexpos + 1, 64); /* copy host name into host_buf */
    /* Cut off any extra spaces on the host buffer */
    indexpos = host_buf;
    while (!isspace(*indexpos))
      indexpos++;
    *indexpos = '\0';

    /*
      LOG (llevDebug, "Login: <%s>; host: <%s>\n", login, host);
      LOG (llevDebug, "    Checking Banned <%s> and <%s>.\n",log_buf,host_buf);
    */
    if(*log_buf=='*')

      Hits=1;
    else if (!strcmp(login, log_buf))
      Hits=1;
    if(Hits==1)
      {
        if (*host_buf == '*'){  /* Lock out any host */
          Hits++;
          break;                /* break out now. otherwise Hits will get reset
                                   to one */
        }
        else if(strstr(host,host_buf)!=NULL){ /* Lock out subdomains (eg, "*@usc.edu" */
          Hits++;
          break;                /* break out now. otherwise Hits will get reset
                                   to one */
        }
        else if (!strcmp(host, host_buf)){ /* Lock out specific host */
          Hits++;
          break;                /* break out now. otherwise Hits will get reset
                                   to one */
        }
      }
  }
  fclose(bannedfile);
  if(Hits>=2)
    return(1);
  loop++;  
 }
	return(0);
}
