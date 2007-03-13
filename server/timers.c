#include <timers.h>
#ifndef __CEXTRACT__
#include <sproto.h>
#endif

static void cftimer_process_event(tag_t ob_tag);

/*****************************************************************************/
/* Processes all timers.                                                     */
/*****************************************************************************/
void cftimer_process_timers(void)
{
    int i;
    for(i=0;i<MAX_TIMERS;i++)
    {
        if (timers_table[i].mode == TIMER_MODE_CYCLES)
        {
            timers_table[i].delay --;
            if (timers_table[i].delay == 0)
            {
            /* Call object timer event */
                timers_table[i].mode = TIMER_MODE_DEAD;
                cftimer_process_event(timers_table[i].ob_tag);
            }
        }
        else if (timers_table[i].mode == TIMER_MODE_SECONDS)
        {
            if (timers_table[i].delay <= seconds())
            {
            /* Call object timer event */
                timers_table[i].mode = TIMER_MODE_DEAD;
                cftimer_process_event(timers_table[i].ob_tag);
            }
        }
    }
}

/*****************************************************************************/
/* Triggers the EVENT_TIMER of the given object                              */
/*****************************************************************************/
static void cftimer_process_event(tag_t ob_tag)
{
    object* ob = find_object(ob_tag);
    if ( ob )
        execute_event(ob, EVENT_TIMER,NULL,NULL,NULL,SCRIPT_FIX_ALL);
}

/*****************************************************************************/
/* Creates a new timer.                                                      */
/* - id    : Desired timer identifier.                                       */
/* - delay : Desired timer delay.                                            */
/* - ob    : Object that will be linked to this timer.                       */
/* - mode  : Count mode (seconds or cycles). See timers.h.                   */
/*****************************************************************************/
/* Return value:                                                             */
/*  TIMER_ERR_NONE : Timer was successfully created.                         */
/*  TIMER_ERR_ID   : Invalid ID.                                             */
/*  TIMER_ERR_MODE : Invalid mode.                                           */
/*  TIMER_ERR_OBJ  : Invalid object.                                         */
/*****************************************************************************/
int cftimer_create(int id, long delay, object* ob, int mode)
{
    if (id >= MAX_TIMERS)
        return TIMER_ERR_ID;
    if (id < 0)
        return TIMER_ERR_ID;
    if (timers_table[id].mode != TIMER_MODE_DEAD)
        return TIMER_ERR_ID;
    if ((mode != TIMER_MODE_SECONDS) && (mode != TIMER_MODE_CYCLES))
        return TIMER_ERR_MODE;
    if (ob == NULL)
        return TIMER_ERR_OBJ;
    if (find_obj_by_type_subtype(ob, EVENT_CONNECTOR, EVENT_TIMER) == NULL)
        return TIMER_ERR_OBJ;
    timers_table[id].mode = mode;
    timers_table[id].ob_tag = ob->count;
    if (mode == TIMER_MODE_CYCLES)
        timers_table[id].delay = delay;
    else
        timers_table[id].delay = seconds() + delay;
    return TIMER_ERR_NONE;
}

/*****************************************************************************/
/* Destroys an existing timer.                                               */
/* - id : Identifier of the timer to destroy.                                */
/*****************************************************************************/
/* Return value:                                                             */
/*  TIMER_ERR_NONE : No problem encountered.                                 */
/*  TIMER_ERR_ID   : Unknown ID - timer not found.                           */
/*****************************************************************************/
int cftimer_destroy(int id)
{
    if (id >= MAX_TIMERS)
        return TIMER_ERR_ID;
    if (id < 0)
        return TIMER_ERR_ID;
    timers_table[id].mode = TIMER_MODE_DEAD;
    return TIMER_ERR_NONE;
}

/*****************************************************************************/
/* Finds a free ID for a new timer.                                          */
/*****************************************************************************/
/* Return value:                                                             */
/*  TIMER_ERR_ID   : No free ID available.                                   */
/*  >0             : an available ID.                                        */
/*****************************************************************************/
int cftimer_find_free_id(void)
{
    int i;
    for(i=0;i<MAX_TIMERS;i++)
    {
        if (timers_table[i].mode == TIMER_MODE_DEAD)
            return i;
    }
    return TIMER_ERR_ID;
}

void cftimer_init(void)
{
    memset(&timers_table[0], 0, sizeof(cftimer) * MAX_TIMERS);
}
