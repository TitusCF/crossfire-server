/*
 * This is the plugin support stuff.
 */
#ifdef PLUGINS
#ifndef PLUGPROTO_H_
#define PLUGPROTO_H_

/* Event ID codes */
#define EVENT_NONE     0
#define EVENT_APPLY    1
#define EVENT_ATTACK   2
#define EVENT_BORN     3 /*New player coming*/
#define EVENT_CLOCK    4 /* Global time event */
#define EVENT_CLOSE    5
#define EVENT_CRASH    6
#define EVENT_DEATH    7
#define EVENT_DROP     8
#define EVENT_GDEATH   9 /* Global Death event */
#define EVENT_GKILL    10 /* Global Kill event */
#define EVENT_LOGIN    11
#define EVENT_LOGOUT   12
#define EVENT_MAPENTER 13
#define EVENT_MAPLEAVE 14
#define EVENT_MAPRESET 15
#define EVENT_PICKUP   16
#define EVENT_REMOVE   17 /*Player removal*/
#define EVENT_SAY      18
#define EVENT_SHOUT    19
#define EVENT_STOP     20
#define EVENT_TELL     21
#define EVENT_TIME     22
#define EVENT_THROW    23
#define EVENT_TRIGGER  24
#define NR_EVENTS 25

/* Hook codes */
#define HOOK_NONE               0
#define HOOK_LOG                1
#define HOOK_NEWINFOMAP         2
#define HOOK_SPRINGTRAP         3
#define HOOK_CASTSPELL          4
#define HOOK_CMDRSKILL          5
#define HOOK_BECOMEFOLLOWER     6
#define HOOK_PICKUP             7
#define HOOK_GETMAPOBJECT       8
#define HOOK_ESRVSENDITEM       9
#define HOOK_FINDPLAYER         10
#define HOOK_MANUALAPPLY        11
#define HOOK_CMDDROP            12
#define HOOK_CMDTAKE            13
#define HOOK_CMDTITLE           14
#define HOOK_TRANSFEROBJECT     15
#define HOOK_KILLOBJECT         16
#define HOOK_LEARNSPELL         17
#define HOOK_FORGETSPELL        18
#define HOOK_CHECKFORSPELL      19
#define HOOK_ESRVSENDINVENTORY  20
#define HOOK_CREATEARTIFACT     21
#define HOOK_GETARCHETYPE       22
#define HOOK_UPDATESPEED        23
#define HOOK_UPDATEOBJECT       24
#define HOOK_FINDANIMATION      25
#define HOOK_GETARCHBYOBJNAME   26
#define HOOK_INSERTOBJECTINMAP  27
#define HOOK_READYMAPNAME       28
#define HOOK_ADDEXP             29
#define HOOK_DETERMINEGOD       30
#define HOOK_FINDGOD            31
#define HOOK_REGISTEREVENT      32
#define HOOK_UNREGISTEREVENT    33
#define HOOK_DUMPOBJECT         34
#define HOOK_LOADOBJECT         35
#define NR_OF_HOOKS             36

typedef struct _CFParm
{
    int dparm;
    int     Type[15];/* 15 should be enough for anything */
    void*   Value[15];
} CFParm;

/* Generic plug-in function prototype */
typedef CFParm* (*f_plugin) (CFParm* PParm);
typedef void (*f_debug) (int i);

typedef struct _CFPlugin
{
    f_plugin        eventfunc;          /*Event handlers */
    f_plugin        initfunc;           /*Plugin Initialization function */
    f_plugin        pinitfunc;          /*Plugin Post-Init. function */
    f_plugin        endfunc;            /*Plugin closing function */
    f_plugin        hookfunc;           /*Plugin CF-functions hooker function*/
    f_plugin        propfunc;           /*Plugin getProperty function */
    f_debug         debugfunc;
    void*           libptr;             /*Plugin pointer */
    char            id[MAX_BUF];        /*Plugin identification string */
    char            fullname[MAX_BUF];  /*Plugin full name */
    int             gevent[NR_EVENTS];  /*Global events registered*/
} CFPlugin;

#endif /*PLUGPROTO_H_*/
#endif /*PLUGINS*/