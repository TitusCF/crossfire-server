/*****************************************************************************/
/* Animation plugin for Crossfire.                                           */
/*****************************************************************************/
/* Contact: tchize@mailandnews.com                                           */
/*****************************************************************************/
/* That code is placed under the GNU General Public Licence (GPL)            */
/* (C)2001 by tchize (Delbecq David)                                         */
/*****************************************************************************/

#ifndef PLUGIN_ANIMATOR_H
#define PLUGIN_ANIMATOR_H

/* First the required header files - only the CF module interface.           */
#include <plugin.h>
/* Well, not quite only... Some constants are in skills.h too (SK_...)       */
#include <skills.h>
#include <object.h>
#include <define.h>
enum time_enum {time_second, time_tick};
struct CFanimation_struct;
struct CFmovement_struct;
typedef int (*CFAnimRunFunc) (struct CFanimation_struct* animation, long int id, void* parameters);
typedef long int (*CFAnimInitFunc) (char* name,char* parameters,struct CFmovement_struct*);
typedef struct CFmovement_struct
    {
    struct CFanimation_struct* parent;
    CFAnimRunFunc func;
    void* parameters;
    long int id;
    int tick;
    struct CFmovement_struct* next;
    } CFmovement;
typedef struct CFanimation_struct
    {
    char* name;
    object* victim;
    int paralyze;
    int invisible;
    int wizard;
    int unique;
    int verbose;
    int ghosted;
    int errors_allowed;
    object* corpse;
    long int tick_left;
    enum time_enum time_representation;
    struct CFmovement_struct* nextmovement;
    struct CFanimation_struct* nextanimation;
    } CFanimation;
typedef struct
    {
    char* name;
    CFAnimInitFunc funcinit;
    CFAnimRunFunc funcrun;
    } CFanimationHook;
extern CFanimationHook animationbox[];
extern int animationcount;
int get_boolean (char* string,int* bool);

/* The plugin properties and hook functions. A hook function is a pointer to */
/* a CF function wrapper. Basically, most CF functions that could be of any  */
/* use to the plugin have "wrappers", functions that all have the same args  */
/* and all returns the same type of data (CFParm); pointers to those functs. */
/* are passed to the plugin when it is initialized. They are what I called   */
/* "Hooks". It means that using any server function is slower from a plugin  */
/* than it is from the "inside" of the server code, because all arguments    */
/* need to be passed back and forth in a CFParm structure, but the difference*/
/* is not a problem, unless for time-critical code sections. Using such hooks*/
/* may of course sound complicated, but it allows much greater flexibility.  */
CFParm* PlugProps;
f_plugin PlugHooks[1024];

/* Some practical stuff, often used in the plugin                            */
#define WHO ((object *)(whoptr))
#define WHAT ((object *)(whatptr))
#define WHERE ((object *)(whereptr))

/* The declarations for the plugin interface. Every plugin should have those.*/
char* hook_add_string (char* text);
char* hook_add_refcount (char* text);
void hook_free_string (char* text);
int hook_move_player (object* player, int dir);
int hook_move_ob (object* what, int dir, object* originator);
void hook_scroll_map (object* player, int dir);

CFParm* registerHook(CFParm* PParm);
CFParm* triggerEvent(CFParm* PParm);
CFParm* initPlugin(CFParm* PParm);
CFParm* postinitPlugin(CFParm* PParm);
CFParm* removePlugin(CFParm* PParm);
CFParm* getPluginProperty(CFParm* PParm);

/* This one is used to cleanly pass args to the CF core */
extern CFParm GCFP;
extern CFParm GCFP0;
extern CFParm GCFP1;
extern CFParm GCFP2;

/* Those are used to handle the events.                                      */
int HandleEvent(CFParm* CFP);
int HandleGlobalEvent(CFParm* CFP);

/* The execution stack. Altough it is quite rare, a script can actually      */
/* trigger another script. The stack is used to keep track of those multiple */
/* levels of execution. A recursion stack of size 100 shout be sufficient.   */
/* If for some reason you think it is not enough, simply increase its size.  */
/* The code will still work, but the plugin will eat more memory.            */

#define MAX_RECURSIVE_CALL 100
extern int StackPosition;
extern object* StackActivator[MAX_RECURSIVE_CALL];
extern object* StackWho[MAX_RECURSIVE_CALL];
extern object* StackOther[MAX_RECURSIVE_CALL];
extern char* StackText[MAX_RECURSIVE_CALL];
extern int StackParm1[MAX_RECURSIVE_CALL];
extern int StackParm2[MAX_RECURSIVE_CALL];
extern int StackParm3[MAX_RECURSIVE_CALL];
extern int StackParm4[MAX_RECURSIVE_CALL];
extern int StackReturn[MAX_RECURSIVE_CALL];

#endif /*PLUGIN_ANIMATOR_H*/
