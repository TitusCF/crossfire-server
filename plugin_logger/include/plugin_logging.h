#ifndef PLUGIN_LOGGING_H
#define PLUGIN_LOGGING_H
/* Plugin configuration part */
/*****************************************************************************/
/* Plugin Configuration Part.                                                */
/* You can define the various parameters for the Plugin Logging Facility     */
/* here. Those are:                                                          */
/*****************************************************************************/
/* - WEB_DIR        : The directory where you want to put your Web pages.    */
/* - SCREENSHOT_DIR : The directory where you want to put the screenshots.   */
/* - INVSHOT_DIR    : The directory where you want to put the inventory shots*/
/* - LOG_FILE       : The name of the logfile.                               */
/*****************************************************************************/

#define WEB_DIR        "/var/www/crossfire/"
#define SCREENSHOT_DIR "screenshot/"
#define INVSHOT_DIR    "inventoryshot/"
#define LOG_FILE       "what_happened.log"

/*****************************************************************************/
/* End of the Configuration section. Do not modify anything by hand below !  */
/*****************************************************************************/

#include <plugin.h>
/*****************************************************************************/
/* The ODBC Connectivity.                                                    */
/*****************************************************************************/
#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>

#define PLUGIN_ID   "Logger"
#define PLUGIN_NAME "Crossfire Log Facilities v0.1"

#define DIALOG_SHOUT 1

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

/*****************************************************************************/
/* The Plugin Interface définition. Every plugin should have those.          */
/*****************************************************************************/
CFParm* registerHook(CFParm* PParm);
CFParm* triggerEvent(CFParm* PParm);
CFParm* initPlugin(CFParm* PParm);
CFParm* postinitPlugin(CFParm* PParm);
CFParm* removePlugin(CFParm* PParm);

extern CFParm* PlugProps;
extern f_plugin PlugHooks[1024];

#endif /* PLUGIN_LOGGING_H */

