/*****************************************************************************/
/* Crossfire timers support - (C) 2001 by Yann Chachkoff.                    */
/* This code is placed under the GPL.                                        */
/*****************************************************************************/

/*****************************************************************************/
/* Headers needed.                                                           */
/*****************************************************************************/

#ifndef TIMERS_H_
#define TIMERS_H_

#include <global.h>
#include <object.h>

#ifdef HAVE_TIME_H
#include <time.h>
#endif

/*****************************************************************************/
/* A timer is a kind of "clock" associated with an object. When the counter  */
/* of a timer reaches 0, it is removed from the list of active timers and an */
/* EVENT_TIMER is generated for the target object.                           */
/* Important note: don't confuse "EVENT_TIMER" and "EVENT_TIME" - the first  */
/* one is called when a timer delay has reached 0 while EVENT_TIME is        */
/* triggered each time the object gets the opportunity to move.              */
/*****************************************************************************/
/* Timer counting methods.                                                   */
/* You can either count the time in seconds or in "crossfire cycles".        */
/* If mode = TIMER_MODE_SECONDS, delay will store the "time to reach" in sec.*/
/* For example, if a timer is created at t=2044s to be triggered 10 seconds  */
/* later, delay = 2054.                                                      */
/* If mode = TIMER_MODE_CYCLES, delay will store the "time remaining", given */
/* in crossfire cycles. For example, if a timer is to be triggered 10 seconds*/
/* later, delay = 10.                                                        */
/*****************************************************************************/
/* Timer limits.                                                             */
/* You can't create more than 1000 timers (I created the timers table as a   */
/* static one not (only) because I was too lazy/incompetent to implement is  */
/* as a dynamic linked-list, but because 1) 1000 should be quite enough, 2)  */
/* 1000 does not use a lot of memory, 3) it would be easier to adapt to some */
/* form of multithreading support with a static list.                        */
/* Anyway, if you think 1000 is not enough, you can safely increase this -   */
/* memory should not be a problem in that case, given the size of a cftimer. */
/*****************************************************************************/

#define TIMER_MODE_DEAD    0 /* Used to mark a timer as unused in the list */
#define TIMER_MODE_SECONDS 1
#define TIMER_MODE_CYCLES  2

typedef struct _cftimer
{
    int     mode;
    long    delay;
    object* ob;
} cftimer;

#define MAX_TIMERS 1000

cftimer timers_table[MAX_TIMERS];

#define TIMER_ERR_NONE      0
#define TIMER_ERR_ID       -1
#define TIMER_ERR_OBJ      -2
#define TIMER_ERR_MODE     -3

#endif /* TIMERS_H_ */

