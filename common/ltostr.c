/*
 * static char *rcsid_ltostr_c =
 *   "$Id$";
 */

/*
    CrossFire, A Multiplayer game for X-windows

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

    The author can be reached via e-mail to frankj@ifi.uio.no.
*/

#include <global.h>

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
