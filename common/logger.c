/*
 * static char *rcsid_loger_c =
 *   "$Id$ ";
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


#include <stdarg.h>
#include <global.h>
#include <funcpoint.h>

/*
 * Logs a message to stderr, or to file, and/or even to socket.
 * Or discards the message if it is of no importanse, and none have
 * asked to hear messages of that logLevel.
 *
 * See include/logger.h for possible logLevels.  Messages with llevInfo
 * and llevError are always printed, regardless of debug mode.
 */

void LOG (LogLevel logLevel, char *format, ...)
{
  char buf[20480];  /* This needs to be really really big - larger
		     * than any other buffer, since that buffer may
		     * need to be put in this one.
		     */

  va_list ap;
  va_start(ap, format);

  buf[0] = '\0';
  if (logLevel <= settings.debug)
  {
    vsprintf(buf, format, ap);
    fputs(buf, logfile);
  }
  if (!exiting && !trying_emergency_save &&
      logLevel == llevError && ++nroferrors > MAX_ERRORS) {
    exiting = 1;
    if (!trying_emergency_save)
      (*emergency_save_func) (0);
    if (!editor)
	fatal(TOO_MANY_ERRORS);
  }
  va_end(ap);
}
