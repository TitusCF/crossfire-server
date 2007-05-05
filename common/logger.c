/*
 * static char *rcsid_loger_c =
 *   "$Id$ ";
 */

int reopen_logfile = 0; /* May be set in SIGHUP handler */

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

/**
 * @file logger.c
 * This handles logging, to file or strerr/stdout.
 */

#include <stdarg.h>
#include <global.h>
#include <funcpoint.h>

/**
 * Human-readable name of log levels.
 */
static char* loglevel_names[] = {"[Error]   ",
                                 "[Info]    ",
                                 "[Debug]   ",
                                 "[Monster] "};

/**
 * Logs a message to stderr, or to file.
 * Or discards the message if it is of no importance, and none have
 * asked to hear messages of that logLevel.
 *
 * See include/logger.h for possible logLevels.  Messages with llevInfo
 * and llevError are always printed, regardless of debug mode.
 *
 * @param logLevel
 * level of the message
 * @param format
 * message to log. Works like printf() and such
 */
void LOG (LogLevel logLevel, const char *format, ...)
{
    char buf[20480];  /* This needs to be really really big - larger
                       * than any other buffer, since that buffer may
                       * need to be put in this one.
                       */

    va_list ap;
    va_start(ap, format);

    buf[0] = '\0';
    if (logLevel <= settings.debug) {
        vsprintf(buf, format, ap);
#ifdef WIN32 /* ---win32 change log handling for win32 */
        fputs(loglevel_names[logLevel], logfile);    /* wrote to file or stdout */
        fputs(buf, logfile);    /* wrote to file or stdout */
#ifdef DEBUG				/* if we have a debug version, we want see ALL output */
        fflush(logfile);    /* so flush this! */
#endif
        if(logfile != stderr) {   /* if was it a logfile wrote it to screen too */ 
            fputs(loglevel_names[logLevel], stderr); 
            fputs(buf, stderr); 
        }
#else /* not WIN32 */
    if (reopen_logfile) {
        reopen_logfile = 0;
        if (fclose(logfile) != 0) {
            /* stderr has been closed if -detach was used, but it's better
             * to try to report about this anyway. */
            perror("tried to close log file after SIGHUP in logger.c:LOG()");
        }
        if ((logfile = fopen(settings.logfilename, "a")) == NULL) {
            /* There's likely to be something very wrong with the OS anyway
             * if reopening fails. */
            perror("tried to open log file after SIGHUP in logger.c:LOG()");
            emergency_save(0);
            clean_tmp_files();
            exit(1);
        }
        setvbuf(logfile, NULL, _IOLBF, 0);
        LOG(llevInfo,"logfile reopened\n");
    }

    fputs(loglevel_names[logLevel], logfile);
    fputs(buf, logfile);
#endif
    }
    if (!exiting && !trying_emergency_save &&
        logLevel == llevError && ++nroferrors > MAX_ERRORS) {
        exiting = 1;
        if (!trying_emergency_save)
            emergency_save(0);
    }
    va_end(ap);
}
