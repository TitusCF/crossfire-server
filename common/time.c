/*
 * static char *rcsid_time_c =
 *    "$Id$";
 */

/*
    CrossFire, A Multiplayer game for X-windows

    Copyright (C) 1994 Mark Wedel
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

    The author can be reached via e-mail to master@rahul.net
*/

#include <global.h>
#include <funcpoint.h>

#ifndef WIN32 /* ---win32 exclude header */
#include <stdio.h>
#include <sys/types.h>
#include <sys/time.h>
#endif /* win32 */

/*
 * Gloabal variables:
 */
long max_time = MAX_TIME;
struct timeval last_time;

#define PBUFLEN 100
long process_utime_save[PBUFLEN];
long psaveind;
long process_max_utime = 0;
long process_min_utime = 999999999;
long process_tot_mtime;
long pticks;
long process_utime_long_count;

/* 0.94.1 - change to GETTIMEOFDAY macro - SNI systems only one one option.
 * rather than have complex #ifdefs throughout the file, lets just figure
 * it out once, here at the top.
 * Have no idea if that is the right symbol to check on for NetBSD,
 * but NetBSD does use 2 params.
 */

#ifdef GETTIMEOFDAY_TWO_ARGS
#define GETTIMEOFDAY(last_time) gettimeofday(last_time, (struct timezone *) NULL);
#else
#define GETTIMEOFDAY(last_time) gettimeofday(last_time);
#endif

/*
 * Initialise all variables used in the timing routines. 
 */

void
reset_sleep()
{
  int i;
  for(i = 0; i < PBUFLEN; i++)
    process_utime_save[i] = 0;
  psaveind = 0;
  process_max_utime = 0;
  process_min_utime = 999999999;
  process_tot_mtime = 0;
  pticks = 0;

  (void) GETTIMEOFDAY(&last_time);
}

void
log_time(long process_utime)
{
  pticks++;
  if (++psaveind >= PBUFLEN)
    psaveind = 0;
  process_utime_save[psaveind] = process_utime;
  if (process_utime > process_max_utime)
    process_max_utime = process_utime;
  if (process_utime < process_min_utime)
    process_min_utime = process_utime;
  process_tot_mtime += process_utime/1000;
}

/*
 * enough_elapsed_time will return true if the time passed since
 * last tick is more than max-time.
 */

int
enough_elapsed_time()
{
  static struct timeval new_time;
  long elapsed_utime;

  (void) GETTIMEOFDAY(&new_time);

  elapsed_utime = (new_time.tv_sec - last_time.tv_sec) * 1000000 +
                  new_time.tv_usec - last_time.tv_usec;
  if (elapsed_utime > max_time) {
    log_time(elapsed_utime);
    last_time.tv_sec = new_time.tv_sec;
    last_time.tv_usec = new_time.tv_usec;
    return 1;
  }
  return 0;
}

/*
 * sleep_delta checks how much time has elapsed since last tick.
 * If it is less than max_time, the remaining time is slept with select().
 */

void
sleep_delta()
{
  static struct timeval new_time;
  long sleep_sec, sleep_usec;

  (void) GETTIMEOFDAY(&new_time);

  sleep_sec = last_time.tv_sec - new_time.tv_sec;
  sleep_usec = max_time - (new_time.tv_usec - last_time.tv_usec);

  /* This is very ugly, but probably the fastest for our use: */
  while (sleep_usec < 0) {
    sleep_usec += 1000000;
    sleep_sec -= 1;
  }
  while (sleep_usec > 1000000) {
    sleep_usec -= 1000000;
    sleep_sec +=1;
  }

  log_time((new_time.tv_sec - last_time.tv_sec)*1000000
           + new_time.tv_usec - last_time.tv_usec);

  if (sleep_sec >= 0 && sleep_usec > 0) {
    static struct timeval sleep_time;
    sleep_time.tv_sec = sleep_sec;
    sleep_time.tv_usec = sleep_usec;
    select(0, NULL, NULL, NULL, &sleep_time);
  }
  else
    process_utime_long_count++;
  /*
   * Set last_time to when we're expected to wake up:
   */
  last_time.tv_usec += max_time;
  while (last_time.tv_usec > 1000000) {
    last_time.tv_usec -= 1000000;
    last_time.tv_sec++;
  }
  /*
   * Don't do too much catching up:
   * (Things can still get jerky on a slow/loaded computer)
   */
  if (last_time.tv_sec * 1000000 + last_time.tv_usec <
      new_time.tv_sec * 1000000 + new_time.tv_usec)
  {
    last_time.tv_sec = new_time.tv_sec;
    last_time.tv_usec = new_time.tv_usec;
  }
}

void
set_max_time(long t) {
  max_time = t;
}

void
time_info(object *op)
{
  int tot = 0, maxt = 0, mint = 99999999, long_count = 0, i;

  (*draw_info_func) (NDI_UNIQUE, 0,op,"Total time:");
  sprintf(errmsg,"ticks=%ld  time=%ld.%2ld",
          pticks, process_tot_mtime/1000, process_tot_mtime%1000);
  (*draw_info_func) (NDI_UNIQUE, 0,op,errmsg);
  sprintf(errmsg,"avg time=%ldms  max time=%ldms  min time=%ldms",
          process_tot_mtime/pticks, process_max_utime/1000,
          process_min_utime/1000);
  (*draw_info_func) (NDI_UNIQUE, 0,op,errmsg);
  sprintf(errmsg,"ticks longer than max time (%ldms) = %ld (%ld%%)",
          max_time/1000,
          process_utime_long_count, 100*process_utime_long_count/pticks);
  (*draw_info_func) (NDI_UNIQUE, 0,op,errmsg);

  sprintf(errmsg,"Time last %ld ticks:", pticks > PBUFLEN ? PBUFLEN : pticks);
  (*draw_info_func) (NDI_UNIQUE, 0,op,errmsg);

  for (i = 0; i < (pticks > PBUFLEN ? PBUFLEN : pticks); i++) {
    tot += process_utime_save[i];
    if (process_utime_save[i] > maxt) maxt = process_utime_save[i];
    if (process_utime_save[i] < mint) mint = process_utime_save[i];
    if (process_utime_save[i] > max_time) long_count++;
  }

  sprintf(errmsg,"avg time=%ldms  max time=%dms  min time=%dms",
          tot/(pticks > PBUFLEN ? PBUFLEN : pticks)/1000, maxt/1000,
          mint/1000);
  (*draw_info_func) (NDI_UNIQUE, 0,op,errmsg);
  sprintf(errmsg,"ticks longer than max time (%ldms) = %d (%ld%%)",
          max_time/1000, long_count,
          100*long_count/(pticks > PBUFLEN ? PBUFLEN : pticks));
  (*draw_info_func) (NDI_UNIQUE, 0,op,errmsg);
}

long
seconds()
{
  struct timeval now;

  (void) GETTIMEOFDAY(&now);
  return now.tv_sec;
}
