#ifndef plugin_logging_h
/*********
 * Plugin configuration part
 * Please modify to fit your system!
 */

#define CROSSFIRE_DSN      "CrossfireDSN"
#define CROSSFIRE_USER     "postgres"
#define CROSSFIRE_PASS     "gandalf"
#define web_directory      "/var/www/html/crossfire/"
#define screenshoot_dir    "screenshoot/"
#define inventoryshoot_dir "inventoryshoot/"

/* Clocks config in term of about 0.1 second */
#define CLOCK_CYCLE         36000 /* Global clock: 1 hour */
#define CLOCK_MAP_USE       30    /* Tries to flush one map stat to database every 3 seconds*/
#define CLOCK_MAP_RATIO     100   /* Save about one map on 100 at each save clock =>5 minutes to save all*/
#define CLOCK_QUEST         600   /* Flush reseted quests to database every 5 minutes*/
#define CLOCK_PETS          10    /* flush some creature about every 18 second */
#define CLOCK_PETS_RATIO    10    /* Total 3 minutes for creature deaths cycle */
#define SERVER_PING_LATENCY 600   /* Need to be about one ping every minute so database could work*/

/*
 * End config
 *********/

/*don't care, debugging only*/
#define logging_filename "what_happened.log"

#include <plugin.h>
#include <autoconf.h>
//#include <skills.h>
#define plugin_logging_h
#define log_plugin_ident "crossfire_logger"
#define log_plugin_string "Database facilities 1.0"


#define DIALOG_SHOUT 1
#define DIALOG_SERVER_INFO 2
#define DIALOG_SERVER_NOTICE 3

#define SERVER_EVENT_STARTUP 1
#define SERVER_EVENT_PING    2044

typedef struct{
    unsigned int agilitylvl;
    unsigned int agilityexp;
    unsigned int mentallvl;
    unsigned int mentalexp;
    unsigned int physiquelvl;
    unsigned int physiqueexp;
    unsigned int magiclvl;
    unsigned int magicexp;
    unsigned int wisdomlvl;
    unsigned int wisdomexp;
    char*        god;
    unsigned int personalitylvl;
    unsigned int personalityexp;
    unsigned int generallvl;
    unsigned int generalexp;
} player_lvl;

extern CFParm* PlugProps;
extern f_plugin PlugHooks[1024];

/* The declarations for the plugin interface. Every plugin should have those.*/
CFParm* registerHook(CFParm* PParm);
CFParm* triggerEvent(CFParm* PParm);
CFParm* initPlugin(CFParm* PParm);
CFParm* postinitPlugin(CFParm* PParm);
CFParm* removePlugin(CFParm* PParm);
#endif

