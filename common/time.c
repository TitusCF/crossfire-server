/*
 * Crossfire -- cooperative multi-player graphical RPG and adventure game
 *
 * Copyright (c) 1999-2014 Mark Wedel and the Crossfire Development Team
 * Copyright (c) 1992 Frank Tore Johansen
 *
 * Crossfire is free software and comes with ABSOLUTELY NO WARRANTY. You are
 * welcome to redistribute it under certain conditions. For details, please
 * see COPYING and LICENSE.
 *
 * The authors can be reached via e-mail at <crossfire@metalforge.org>.
 */

/**
 * @file
 * In-game time functions.
 */

#include "global.h"

#include <assert.h>
#include <math.h>

#include "tod.h"

#ifndef WIN32 /* ---win32 exclude header */
#include <stdio.h>
#include <sys/types.h>
#include <sys/time.h>
#endif /* win32 */

/**
 * Gloabal variables:
 */
uint32_t max_time = MAX_TIME;
static struct timespec game_time;

/** Size of history buffer. */
#define PBUFLEN 100
static uint32_t process_utime_save[PBUFLEN];   /**< Historic data. */
static uint32_t psaveind;                      /**< Index in ::process_utime_save. */
static uint32_t process_max_utime = 0;         /**< Longest cycle time. */
static uint32_t process_min_utime = 999999999; /**< Shortest cycle time. */
static uint32_t process_tot_mtime;             /**< ? */
uint32_t pticks;                               /**< ? */
static uint32_t process_utime_long_count;      /**< ? */

/** Ingame seasons. */
static const char *const season_name[SEASONS_PER_YEAR+1] = {
    "The Season of New Year",
    "The Season of Growth",
    "The Season of Harvest",
    "The Season of Decay",
    "The Season of the Blizzard",
    "\n"
};

/** Days of the week. */
static const char *const weekdays[DAYS_PER_WEEK] = {
    "the Day of the Moon",
    "the Day of the Bull",
    "the Day of the Deception",
    "the Day of Thunder",
    "the Day of Freedom",
    "the Day of the Great Gods",
    "the Day of the Sun"
};

/** Months. */
static const char *const month_name[MONTHS_PER_YEAR] = {
    "Month of Winter",           /* 0 */
    "Month of the Ice Dragon",
    "Month of the Frost Giant",
    "Month of Valriel",
    "Month of Lythander",
    "Month of the Harvest",
    "Month of Gaea",
    "Month of Futility",
    "Month of the Dragon",
    "Month of the Sun",
    "Month of the Great Infernus",
    "Month of Ruggilli",
    "Month of the Dark Shades",
    "Month of the Devourers",
    "Month of Sorig",
    "Month of the Ancient Darkness",
    "Month of Gorokh"
};

static const char *const periodsofday[PERIODS_PER_DAY] = {
    "Night",
    "Dawn",
    "Morning",
    "Noon",
    "Evening",
    "Dusk"
};

/**
 * give access to weekday names
 */
const char *get_periodofday(const int index) {
    return ((index >= 0) && (index < PERIODS_PER_DAY)) ? periodsofday[index] : NULL;
}

/**
 * give access to month names
 */
const char *get_month_name(const int index) {
    return ((index >= 0) && (index < MONTHS_PER_YEAR)) ? month_name[index] : NULL;
}

/**
 * give access to weekday names
 */
const char *get_weekday(const int index) {
    return ((index >= 0) && (index < DAYS_PER_WEEK)) ? weekdays[index] : NULL;
}

/**
 * give access to season names
 */
const char *get_season_name(const int index) {
    return ((index >= 0) && (index < (SEASONS_PER_YEAR+1))) ? season_name[index] : NULL;
}

/**
 * Initialise all variables used in the timing routines.
 */
void reset_sleep(void) {
    int i;

    for (i = 0; i < PBUFLEN; i++)
        process_utime_save[i] = 0;
    psaveind = 0;
    process_max_utime = 0;
    process_min_utime = 999999999;
    process_tot_mtime = 0;
    pticks = 0;

    clock_gettime(CLOCK_MONOTONIC, &game_time);
}

/**
 * Adds time to our history list.
 */
static void log_time(uint32_t process_utime) {
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

/**
 * Return the difference between two timespec's in microseconds.
 */
static long timespec_diff(struct timespec *end, struct timespec *start) {
    long long sec = (long long)end->tv_sec - (long long)start->tv_sec;
    long nsec = end->tv_nsec - start->tv_nsec;
    return sec * 1e6 + nsec / 1e3;
}

/**
 * Add 'usec' microseconds to the given timespec.
 */
static void timespec_add(struct timespec *time, long usec) {
    long nsec_sum = time->tv_nsec + usec * 1e3;
    time->tv_sec += nsec_sum / 1e9;
    time->tv_nsec = nsec_sum % (long)1e9;
}

/**
 * Sleep until the next tick.
 */
void sleep_delta(void) {
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    long time_since_last_sleep = timespec_diff(&now, &game_time);
    log_time(time_since_last_sleep);

    long sleep_time = max_time - time_since_last_sleep;
    if (sleep_time > 0) {
        usleep(sleep_time);
    } else {
        process_utime_long_count++;
    }

    // Add one tick length to the last tick time.
    timespec_add(&game_time, max_time);
}

/**
 * Sets the max speed. Can be called by a DM through the speed command.
 *
 * @param t
 * new speed.
 */
void set_max_time(long t) {
    max_time = t;
}

extern unsigned long todtick;

/**
 * Computes the ingame time of the day.
 *
 * @param[out] tod
 * where to store information. Must not be NULL.
 */
void get_tod(timeofday_t *tod) {
    tod->year = todtick/HOURS_PER_YEAR;
    tod->month = (todtick/HOURS_PER_MONTH)%MONTHS_PER_YEAR;
    tod->day = (todtick%HOURS_PER_MONTH)/DAYS_PER_MONTH;
    tod->dayofweek = tod->day%DAYS_PER_WEEK;
    tod->hour = todtick%HOURS_PER_DAY;
    tod->minute = (pticks%PTICKS_PER_CLOCK)/(PTICKS_PER_CLOCK/58);
    if (tod->minute > 58)
        tod->minute = 58; /* it's imprecise at best anyhow */
    tod->weekofmonth = tod->day/WEEKS_PER_MONTH;
    if (tod->month < 3)
        tod->season = 0;
    else if (tod->month < 6)
        tod->season = 1;
    else if (tod->month < 9)
        tod->season = 2;
    else if (tod->month < 12)
        tod->season = 3;
    else
        tod->season = 4;

    if (tod->hour < 5) /*until 4:59*/
        tod->periodofday = 0;
    else if (tod->hour < 8)
        tod->periodofday = 1;
    else if (tod->hour < 13)
        tod->periodofday = 2;
    else if (tod->hour < 15)
        tod->periodofday = 3;
    else if (tod->hour < 20)
        tod->periodofday = 4;
    else if (tod->hour < 23)
        tod->periodofday = 5;
    else /*back to night*/
        tod->periodofday = 0;
}

/**
 * Prints the time.
 *
 * @param op
 * player who requested time.
 */
static void print_tod(object *op) {
    timeofday_t tod;
    const char *suf;
    int day;
    char buf1[128];

    get_tod(&tod);

    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO,
        "It is %s, on %s", time_format_time(&tod, buf1, sizeof(buf1)), weekdays[tod.dayofweek]);

    day = tod.day+1;
    if (day == 1 || ((day%10) == 1 && day > 20))
        suf = "st";
    else if (day == 2 || ((day%10) == 2 && day > 20))
        suf = "nd";
    else if (day == 3 || ((day%10) == 3 && day > 20))
        suf = "rd";
    else
        suf = "th";

    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO,
                         "The %d%s Day of the %s, Year %d",
                         day, suf, month_name[tod.month], tod.year+1);

    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO,
                         "Time of Year: %s",
                         season_name[tod.season]);
}

/**
 * Players wants to know the time. Called through the 'time' command.
 *
 * @param op
 * player who requested time.
 */
void time_info(object *op) {
    int tot = 0, long_count = 0;
    uint32_t maxt = 0, mint = 99999999, i;

    print_tod(op);

    if (!QUERY_FLAG(op, FLAG_WIZ))
        return;

    draw_ext_info_format(
        NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_DEBUG,
        "Statistics for last %d ticks:\n\tmin/avg/max = %d/%d/%d ms per tick",
        pticks, process_min_utime / 1000, process_tot_mtime / pticks,
        process_max_utime / 1000);

    draw_ext_info_format(
        NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_DEBUG,
        "\tticks longer than %d ms = %d (%d%%)", max_time / 1000,
        process_utime_long_count, 100 * process_utime_long_count / pticks);

    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_DEBUG,
                         "Time last %d ticks:",
                         MIN(pticks, PBUFLEN));

    for (i = 0; i < MIN(pticks, PBUFLEN); i++) {
        tot += process_utime_save[i];
        if (process_utime_save[i] > maxt)
            maxt = process_utime_save[i];
        if (process_utime_save[i] < mint)
            mint = process_utime_save[i];
        if (process_utime_save[i] > max_time)
            long_count++;
    }

    assert(pticks > 0);

    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_DEBUG,
                         "avg time=%dms  max time=%dms  min time=%dms",
                         tot/MIN(pticks, PBUFLEN)/1000, maxt/1000,
                         mint/1000);

    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_DEBUG,
                         "ticks longer than max time (%dms) = %d (%d%%)",
                         max_time/1000, long_count,
                         100*long_count/MIN(pticks, PBUFLEN));
}

/**
 * Return wall clock time in seconds.
 */
long seconds(void) {
    struct timespec now;
    clock_gettime(CLOCK_REALTIME, &now);
    return now.tv_sec;
}

/**
 * Formats a timestamp in Crossfire time.
 *
 * @param tod
 * the timestamp to format
 *
 * @param buf
 * the buffer to fill
 *
 * @param bufsize
 * the size of buf in bytes
 *
 * @return
 * buf
 */
const char *time_format_time(const timeofday_t *tod, char *buf, size_t bufsize)
{
    snprintf(buf, bufsize, "%d minute%s past %d o'clock %s",
        tod->minute+1,
        tod->minute+1 < 2 ? "" : "s",
        tod->hour%14 == 0 ? 14 : tod->hour%14,
        tod->hour >= 14 ? "pm" : "am");
    return buf;
}

/**
 * Calculate the number of ticks that correspond to real time.
 */
int tick_length(float seconds) {
    return (int)ceil(seconds * 1000000 / max_time);
}
