/*****************************************************************************/
/* CFPython - A Python module for Crossfire RPG.                             */
/*****************************************************************************/
/* The goal of this module is to provide support for Python scripts into     */
/* Crossfire. Guile support existed before, but it was put directly in the   */
/* code, a thing that prevented easy building of Crossfire on platforms that */
/* didn't have a Guile port. And Guile was seen as difficult to learn and was*/
/* also less popular than Python in the Crossfire Community.                 */
/* So, I finally decided to replace Guile with Python and made it a separate */
/* module since it is not a "critical part" of the code. Of course, it also  */
/* means that it will never be as fast as it could be, but it allows more    */
/* flexibility (and although it is not as fast as compiled-in code, it should*/
/* be fast enough for nearly everything on most today computers).            */
/*****************************************************************************/
/* Please note that it is still very beta - some of the functions may not    */
/* work as expected and could even cause the server to crash.                */
/*****************************************************************************/
/* Version: 0.7 Beta  (also known as "Koursk")                               */
/* Contact: yann.chachkoff@mailandnews.com                                   */
/*****************************************************************************/
/* That code is placed under the GNU General Public Licence (GPL)            */
/* (C)2001 by Chachkoff Yann (Feel free to deliver your complaints)          */
/*****************************************************************************/

#ifndef PLUGIN_PYTHON_H
#define PLUGIN_PYTHON_H

/* First the required header files - only the CF module interface and Python */
#include <Python.h>
#include <plugin.h>

/* Well, not quite only... Some constants are in skills.h too (SK_...)       */
#include <skills.h>

#undef MODULEAPI
#ifdef WIN32
#ifdef PYTHON_PLUGIN_EXPORTS
#define MODULEAPI __declspec(dllexport)
#else
#define MODULEAPI __declspec(dllimport)
#endif

#else
#define MODULEAPI
#endif

#define PLUGIN_NAME    "Python"
#define PLUGIN_VERSION "CFPython Plugin 0.7 (Koursk)"

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
MODULEAPI CFParm* registerHook(CFParm* PParm);
MODULEAPI CFParm* triggerEvent(CFParm* PParm);
MODULEAPI CFParm* initPlugin(CFParm* PParm);
MODULEAPI CFParm* postinitPlugin(CFParm* PParm);
MODULEAPI CFParm* removePlugin(CFParm* PParm);
MODULEAPI CFParm* getPluginProperty(CFParm* PParm);

/* This one is used to cleanly pass args to the CF core */
static CFParm GCFP;
static CFParm GCFP0;
static CFParm GCFP1;
static CFParm GCFP2;

/* Those are the new Python instructions, as implemented in C.               */
static PyObject* CFCheckArchInventory(PyObject* self, PyObject* args);
static PyObject* CFGetName(PyObject* self, PyObject* args);
static PyObject* CFSetName(PyObject* self, PyObject* args);
static PyObject* CFGetTitle(PyObject* self, PyObject* args);
static PyObject* CFSetTitle(PyObject* self, PyObject* args);
static PyObject* CFGetSlaying(PyObject* self, PyObject* args);
static PyObject* CFSetSlaying(PyObject* self, PyObject* args);
static PyObject* CFSetMessage(PyObject* self, PyObject* args);


static PyObject* CFSetSkillExperience(PyObject* self, PyObject* args);
static PyObject* CFGetSkillExperience(PyObject* self, PyObject* args);
static PyObject* CFMatchString(PyObject* self, PyObject* args);
static PyObject* CFSetCursed(PyObject* self, PyObject* args);
static PyObject* CFSetDamned(PyObject* self, PyObject* args);
static PyObject* CFActivateRune(PyObject* self, PyObject* args);
static PyObject* CFCheckTrigger(PyObject* self, PyObject* args);
static PyObject* CFSetUnaggressive(PyObject* self, PyObject* args);
static PyObject* CFCastAbility(PyObject* self, PyObject* args);
static PyObject* CFGetMapPath(PyObject* self, PyObject* args);
static PyObject* CFGetMapObject(PyObject* self, PyObject* args);
static PyObject* CFGetMessage(PyObject* self, PyObject* args);
static PyObject* CFGetGod(PyObject* self, PyObject* args);
static PyObject* CFSetGod(PyObject* self, PyObject* args);
static PyObject* CFSetWeight(PyObject* self, PyObject* args);
static PyObject* CFReadyMap(PyObject* self, PyObject* args);
static PyObject* CFTeleport(PyObject* self, PyObject* args);
static PyObject* CFIsOutOfMap(PyObject* self, PyObject* args);
static PyObject* CFPickUp(PyObject* self, PyObject* args);
static PyObject* CFGetWeight(PyObject* self, PyObject* args);
static PyObject* CFIsCanBePicked(PyObject* self, PyObject* args);
static PyObject* CFGetMap(PyObject* self, PyObject* args);
static PyObject* CFGetNextObject(PyObject* self, PyObject* args);
static PyObject* CFGetPreviousObject(PyObject* self, PyObject* args);
static PyObject* CFGetFirstObjectOnSquare(PyObject* self, PyObject* args);
static PyObject* CFSetQuantity(PyObject* self, PyObject* args);
static PyObject* CFGetQuantity(PyObject* self, PyObject* args);
static PyObject* CFInsertObjectInside(PyObject* self, PyObject* args);
static PyObject* CFFindPlayer(PyObject* self, PyObject* args);
static PyObject* CFApply(PyObject* self, PyObject* args);
static PyObject* CFDrop(PyObject* self, PyObject* args);
static PyObject* CFTake(PyObject* self, PyObject* args);
static PyObject* CFIsInvisible(PyObject* self, PyObject* args);
static PyObject* CFWhoAmI(PyObject* self, PyObject* args);
static PyObject* CFWhoIsActivator(PyObject* self, PyObject* args);
static PyObject* CFWhatIsMessage(PyObject* self, PyObject* args);
static PyObject* CFSay(PyObject* self, PyObject* args);
static PyObject* CFSetInvisible(PyObject* self, PyObject* args);
static PyObject* CFGetExperience(PyObject* self, PyObject* args);
static PyObject* CFGetSpeed(PyObject* self, PyObject* args);
static PyObject* CFSetSpeed(PyObject* self, PyObject* args);
static PyObject* CFGetFood(PyObject* self, PyObject* args);
static PyObject* CFSetFood(PyObject* self, PyObject* args);
static PyObject* CFGetGrace(PyObject* self, PyObject* args);
static PyObject* CFSetGrace(PyObject* self, PyObject* args);
static PyObject* CFGetReturnValue(PyObject* self, PyObject* args);
static PyObject* CFSetReturnValue(PyObject* self, PyObject* args);
static PyObject* CFGetDirection(PyObject* self, PyObject* args);
static PyObject* CFSetDirection(PyObject* self, PyObject* args);
static PyObject* CFGetLastSP(PyObject* self, PyObject* args);
static PyObject* CFSetLastSP(PyObject* self, PyObject* args);
static PyObject* CFGetLastGrace(PyObject* self, PyObject* args);
static PyObject* CFSetLastGrace(PyObject* self, PyObject* args);
static PyObject* CFFixObject(PyObject* self, PyObject* args);
static PyObject* CFSetFace(PyObject* self, PyObject* args);
static PyObject* CFGetAttackType(PyObject* self, PyObject* args);
static PyObject* CFSetAttackType(PyObject* self, PyObject* args);
static PyObject* CFSetDamage(PyObject* self, PyObject* args);
static PyObject* CFGetDamage(PyObject* self, PyObject* args);
static PyObject* CFSetBeenApplied(PyObject* self, PyObject* args);
static PyObject* CFSetIdentified(PyObject* self, PyObject* args);
static PyObject* CFKillObject(PyObject* self, PyObject* args);
static PyObject* CFWhoIsOther(PyObject* self, PyObject* args);
static PyObject* CFDirectionN(PyObject* self, PyObject* args);
static PyObject* CFDirectionNE(PyObject* self, PyObject* args);
static PyObject* CFDirectionE(PyObject* self, PyObject* args);
static PyObject* CFDirectionSE(PyObject* self, PyObject* args);
static PyObject* CFDirectionS(PyObject* self, PyObject* args);
static PyObject* CFDirectionSW(PyObject* self, PyObject* args);
static PyObject* CFDirectionW(PyObject* self, PyObject* args);
static PyObject* CFDirectionNW(PyObject* self, PyObject* args);
static PyObject* CFCastSpell(PyObject* self, PyObject* args);
static PyObject* CFForgetSpell(PyObject* self, PyObject* args);
static PyObject* CFAcquireSpell(PyObject* self, PyObject* args);
static PyObject* CFDoKnowSpell(PyObject* self, PyObject* args);
static PyObject* CFCheckInvisibleObjectInside(PyObject* self, PyObject* args);
static PyObject* CFCreateInvisibleObjectInside(PyObject* self, PyObject* args);
static PyObject* CFCreateObjectInside(PyObject* self, PyObject* args);
static PyObject* CFCheckMap(PyObject* self, PyObject* args);
static PyObject* CFCheckInventory(PyObject* self, PyObject* args);
static PyObject* CFGetName(PyObject* self, PyObject* args);
static PyObject* CFCreateObject(PyObject* self, PyObject* args);
static PyObject* CFRemoveObject(PyObject* self, PyObject* args);
static PyObject* CFIsAlive(PyObject* self, PyObject* args);
static PyObject* CFIsWiz(PyObject* self, PyObject* args);
static PyObject* CFWasWiz(PyObject* self, PyObject* args);
static PyObject* CFIsApplied(PyObject* self, PyObject* args);
static PyObject* CFIsUnpaid(PyObject* self, PyObject* args);
static PyObject* CFIsFlying(PyObject* self, PyObject* args);
static PyObject* CFIsMonster(PyObject* self, PyObject* args);
static PyObject* CFIsFriendly(PyObject* self, PyObject* args);
static PyObject* CFIsGenerator(PyObject* self, PyObject* args);
static PyObject* CFIsThrown(PyObject* self, PyObject* args);
static PyObject* CFCanSeeInvisible(PyObject* self, PyObject* args);
static PyObject* CFCanRoll(PyObject* self, PyObject* args);
static PyObject* CFIsTurnable(PyObject* self, PyObject* args);
static PyObject* CFIsUsedUp(PyObject* self, PyObject* args);
static PyObject* CFIsIdentified(PyObject* self, PyObject* args);
static PyObject* CFIsSplitting(PyObject* self, PyObject* args);
static PyObject* CFHitBack(PyObject* self, PyObject* args);
static PyObject* CFBlocksView(PyObject* self, PyObject* args);
static PyObject* CFIsUndead(PyObject* self, PyObject* args);
static PyObject* CFIsScared(PyObject* self, PyObject* args);
static PyObject* CFIsUnaggressive(PyObject* self, PyObject* args);
static PyObject* CFReflectMissiles(PyObject* self, PyObject* args);
static PyObject* CFReflectSpells(PyObject* self, PyObject* args);
static PyObject* CFIsRunningAway(PyObject* self, PyObject* args);
static PyObject* CFCanPassThru(PyObject* self, PyObject* args);
static PyObject* CFCanPickUp(PyObject* self, PyObject* args);
static PyObject* CFIsUnique(PyObject* self, PyObject* args);
static PyObject* CFCanCastSpell(PyObject* self, PyObject* args);
static PyObject* CFCanUseScroll(PyObject* self, PyObject* args);
static PyObject* CFCanUseWand(PyObject* self, PyObject* args);
static PyObject* CFCanUseBow(PyObject* self, PyObject* args);
static PyObject* CFCanUseArmour(PyObject* self, PyObject* args);
static PyObject* CFCanUseWeapon(PyObject* self, PyObject* args);
static PyObject* CFCanUseRing(PyObject* self, PyObject* args);
static PyObject* CFHasXRays(PyObject* self, PyObject* args);
static PyObject* CFIsFloor(PyObject* self, PyObject* args);
static PyObject* CFIsLifeSaver(PyObject* self, PyObject* args);
static PyObject* CFIsSleeping(PyObject* self, PyObject* args);
static PyObject* CFStandStill(PyObject* self, PyObject* args);
static PyObject* CFOnlyAttack(PyObject* self, PyObject* args);
static PyObject* CFIsConfused(PyObject* self, PyObject* args);
static PyObject* CFHasStealth(PyObject* self, PyObject* args);
static PyObject* CFIsCursed(PyObject* self, PyObject* args);
static PyObject* CFIsDamned(PyObject* self, PyObject* args);
static PyObject* CFIsKnownMagical(PyObject* self, PyObject* args);
static PyObject* CFIsKnownCursed(PyObject* self, PyObject* args);
static PyObject* CFCanUseSkill(PyObject* self, PyObject* args);
static PyObject* CFHasBeenApplied(PyObject* self, PyObject* args);
static PyObject* CFCanUseRod(PyObject* self, PyObject* args);
static PyObject* CFCanUseHorn(PyObject* self, PyObject* args);
static PyObject* CFMakeInvisible(PyObject* self, PyObject* args);
static PyObject* CFIsBlind(PyObject* self, PyObject* args);
static PyObject* CFCanSeeInDark(PyObject* self, PyObject* args);
static PyObject* CFGetAC(PyObject* self, PyObject* args);
static PyObject* CFGetCha(PyObject* self, PyObject* args);
static PyObject* CFGetCon(PyObject* self, PyObject* args);
static PyObject* CFGetDex(PyObject* self, PyObject* args);
static PyObject* CFGetHP(PyObject* self, PyObject* args);
static PyObject* CFGetInt(PyObject* self, PyObject* args);
static PyObject* CFGetPow(PyObject* self, PyObject* args);
static PyObject* CFGetSP(PyObject* self, PyObject* args);
static PyObject* CFGetStr(PyObject* self, PyObject* args);
static PyObject* CFGetWis(PyObject* self, PyObject* args);
static PyObject* CFGetMaxHP(PyObject* self, PyObject* args);
static PyObject* CFGetMaxSP(PyObject* self, PyObject* args);
static PyObject* CFGetXPos(PyObject* self, PyObject* args);
static PyObject* CFGetYPos(PyObject* self, PyObject* args);
static PyObject* CFSetPosition(PyObject* self, PyObject* args);
static PyObject* CFSetNickname(PyObject* self, PyObject* args);
static PyObject* CFSetAC(PyObject* self, PyObject* args);
static PyObject* CFSetCha(PyObject* self, PyObject* args);
static PyObject* CFSetCon(PyObject* self, PyObject* args);
static PyObject* CFSetDex(PyObject* self, PyObject* args);
static PyObject* CFSetHP(PyObject* self, PyObject* args);
static PyObject* CFSetInt(PyObject* self, PyObject* args);
static PyObject* CFSetMaxHP(PyObject* self, PyObject* args);
static PyObject* CFSetMaxSP(PyObject* self, PyObject* args);
static PyObject* CFSetPow(PyObject* self, PyObject* args);
static PyObject* CFSetSP(PyObject* self, PyObject* args);
static PyObject* CFSetStr(PyObject* self, PyObject* args);
static PyObject* CFSetWis(PyObject* self, PyObject* args);
static PyObject* CFMessage(PyObject* self, PyObject* args);
static PyObject* CFWrite(PyObject* self, PyObject* args);
static PyObject* CFIsOfType(PyObject* self, PyObject* args);
static PyObject* CFGetType(PyObject* self, PyObject* args);
static PyObject* CFGetEventHandler(PyObject* self, PyObject* args);
static PyObject* CFSetEventHandler(PyObject* self, PyObject* args);
static PyObject* CFGetEventPlugin(PyObject* self, PyObject* args);
static PyObject* CFSetEventPlugin(PyObject* self, PyObject* args);
static PyObject* CFGetEventOptions(PyObject* self, PyObject* args);
static PyObject* CFSetEventOptions(PyObject* self, PyObject* args);
static PyObject* CFGetIP(PyObject* self, PyObject* args);
static PyObject* CFGetInventory(PyObject* self, PyObject* args);
static PyObject* CFGetInternalName(PyObject* self, PyObject* args);
static PyObject* CFSetVariable(PyObject* self, PyObject* args);
static PyObject* CFDecreaseObjectNr(PyObject* self, PyObject* args);

static PyObject* CFAttackTypePhysical(PyObject* self, PyObject* args);
static PyObject* CFAttackTypeMagic(PyObject* self, PyObject* args);
static PyObject* CFAttackTypeFire(PyObject* self, PyObject* args);
static PyObject* CFAttackTypeElectricity(PyObject* self, PyObject* args);
static PyObject* CFAttackTypeCold(PyObject* self, PyObject* args);
static PyObject* CFAttackTypeConfusion(PyObject* self, PyObject* args);
static PyObject* CFAttackTypeAcid(PyObject* self, PyObject* args);
static PyObject* CFAttackTypeDrain(PyObject* self, PyObject* args);
static PyObject* CFAttackTypeWeaponmagic(PyObject* self, PyObject* args);
static PyObject* CFAttackTypeGhosthit(PyObject* self, PyObject* args);
static PyObject* CFAttackTypePoison(PyObject* self, PyObject* args);
static PyObject* CFAttackTypeSlow(PyObject* self, PyObject* args);
static PyObject* CFAttackTypeParalyze(PyObject* self, PyObject* args);
static PyObject* CFAttackTypeTurnUndead(PyObject* self, PyObject* args);
static PyObject* CFAttackTypeFear(PyObject* self, PyObject* args);
static PyObject* CFAttackTypeCancellation(PyObject* self, PyObject* args);
static PyObject* CFAttackTypeDepletion(PyObject* self, PyObject* args);
static PyObject* CFAttackTypeDeath(PyObject* self, PyObject* args);
static PyObject* CFAttackTypeChaos(PyObject* self, PyObject* args);
static PyObject* CFAttackTypeCounterspell(PyObject* self, PyObject* args);
static PyObject* CFAttackTypeGodpower(PyObject* self, PyObject* args);
static PyObject* CFAttackTypeHolyWord(PyObject* self, PyObject* args);
static PyObject* CFAttackTypeBlind(PyObject* self, PyObject* args);
static PyObject* CFAttackTypeLifeStealing(PyObject* self, PyObject* args);
static PyObject* CFAttackTypeDisease(PyObject* self, PyObject* args);

static PyObject* CFEventApply(PyObject* self, PyObject* args);
static PyObject* CFEventAttack(PyObject* self, PyObject* args);
static PyObject* CFEventDeath(PyObject* self, PyObject* args);
static PyObject* CFEventDrop(PyObject* self, PyObject* args);
static PyObject* CFEventPickup(PyObject* self, PyObject* args);
static PyObject* CFEventSay(PyObject* self, PyObject* args);
static PyObject* CFEventStop(PyObject* self, PyObject* args);
static PyObject* CFEventTime(PyObject* self, PyObject* args);
static PyObject* CFEventThrow(PyObject* self, PyObject* args);
static PyObject* CFEventTrigger(PyObject* self, PyObject* args);
static PyObject* CFEventClose(PyObject* self, PyObject* args);
static PyObject* CFEventTimer(PyObject* self, PyObject* args);

static PyObject* CFLoadObject(PyObject* self, PyObject* args);
static PyObject* CFSaveObject(PyObject* self, PyObject* args);
static PyObject* CFRegisterCommand(PyObject* self, PyObject* args);
static PyObject* CFGetValue(PyObject* self, PyObject* args);
static PyObject* CFSetValue(PyObject* self, PyObject* args);
static PyObject* CFGetMapWidth(PyObject* self, PyObject* args);
static PyObject* CFGetMapHeight(PyObject* self, PyObject* args);
static PyObject* CFGetObjectAt(PyObject* self, PyObject* args);
static PyObject* CFCostFlagFTrue(PyObject* self, PyObject* args);
static PyObject* CFCostFlagFBuy(PyObject* self, PyObject* args);
static PyObject* CFCostFlagFSell(PyObject* self, PyObject* args);
static PyObject* CFCostFlagFNoBargain(PyObject* self, PyObject* args);
static PyObject* CFCostFlagFIdentified(PyObject* self, PyObject* args);
static PyObject* CFCostFlagFNotCursed(PyObject* self, PyObject* args);
static PyObject* CFGetObjectCost(PyObject* self, PyObject* args);
static PyObject* CFGetObjectMoney(PyObject* self, PyObject* args);
static PyObject* CFPayForItem(PyObject* self, PyObject* args);
static PyObject* CFPayAmount(PyObject* self, PyObject* args);
static PyObject* CFSendCustomCommand(PyObject* self, PyObject* args);
static PyObject* CFGetHumidity(PyObject* self, PyObject* args);
static PyObject* CFGetTemperature(PyObject* self, PyObject* args);
static PyObject* CFGetPressure(PyObject* self, PyObject* args);
static PyObject* CFGetWC(PyObject* self, PyObject* args);
static PyObject* CFSetWC(PyObject* self, PyObject* args);

/* Setting values - datapaths */
static PyObject* CFGetMapDir(PyObject* self, PyObject* args);
static PyObject* CFGetUniqueDir(PyObject* self, PyObject* args);
static PyObject* CFGetTempDir(PyObject* self, PyObject* args);
static PyObject* CFGetConfigurationDir(PyObject* self, PyObject* args);
static PyObject* CFGetDataDir(PyObject* self, PyObject* args);
static PyObject* CFGetLocalDir(PyObject* self, PyObject* args);
static PyObject* CFGetPlayerDir(PyObject* self, PyObject* args);

/* Those are used to handle the events. The first one is used when a player  */
/* attacks with a "scripted" weapon. HandleEvent is used for all other events*/
MODULEAPI int HandleUseWeaponEvent(CFParm* CFP);
MODULEAPI int HandleEvent(CFParm* CFP);
MODULEAPI int HandleGlobalEvent(CFParm* CFP);
/* Called to start the Python Interpreter.                                   */
MODULEAPI void initCFPython();

/* The execution stack. Although it is quite rare, a script can actually     */
/* trigger another script. The stack is used to keep track of those multiple */
/* levels of execution. A recursion stack of size 100 shout be sufficient.   */
/* If for some reason you think it is not enough, simply increase its size.  */
/* The code will still work, but the plugin will eat more memory.            */
#define MAX_RECURSIVE_CALL 100
static int StackPosition=0;
object* StackActivator[MAX_RECURSIVE_CALL];
object* StackWho[MAX_RECURSIVE_CALL];
object* StackOther[MAX_RECURSIVE_CALL];
char* StackText[MAX_RECURSIVE_CALL];
uint32 StackParm1[MAX_RECURSIVE_CALL];
int StackParm2[MAX_RECURSIVE_CALL];
int StackParm3[MAX_RECURSIVE_CALL];
int StackParm4[MAX_RECURSIVE_CALL];
int StackReturn[MAX_RECURSIVE_CALL];

/* Here are the Python Declaration Table, used by the interpreter to make    */
/* an interface with the C code                                              */
static PyMethodDef CFPythonMethods[] =
{
        {"SetMessage", CFSetMessage, METH_VARARGS},
        {"GetName",CFGetName,METH_VARARGS},
        {"SetName", CFSetName, METH_VARARGS},
        {"GetTitle",CFGetTitle,METH_VARARGS},
        {"SetTitle", CFSetTitle, METH_VARARGS},
        {"GetSlaying",CFGetSlaying,METH_VARARGS},
        {"SetSlaying", CFSetSlaying, METH_VARARGS},
        {"SetSkillExperience", CFSetSkillExperience, METH_VARARGS},
        {"GetSkillExperience", CFGetSkillExperience, METH_VARARGS},
        {"MatchString", CFMatchString, METH_VARARGS},
        {"SetCursed", CFSetCursed, METH_VARARGS},
        {"SetDamned", CFSetDamned, METH_VARARGS},
        {"ActivateRune", CFActivateRune, METH_VARARGS},
        {"CheckTrigger", CFCheckTrigger, METH_VARARGS},
        {"SetUnaggressive", CFSetUnaggressive, METH_VARARGS},
        {"CastAbility", CFCastAbility, METH_VARARGS},
        {"GetMapPath", CFGetMapPath, METH_VARARGS},
        {"GetMapObject", CFGetMapObject, METH_VARARGS},
        {"GetMessage", CFGetMessage, METH_VARARGS},
        {"GetGod", CFGetGod, METH_VARARGS},
        {"SetGod", CFSetGod, METH_VARARGS},
        {"SetWeight", CFSetWeight, METH_VARARGS},
        {"ReadyMap", CFReadyMap, METH_VARARGS},
        {"Teleport", CFTeleport, METH_VARARGS},
        {"IsOutOfMap", CFIsOutOfMap, METH_VARARGS},
        {"PickUp", CFPickUp, METH_VARARGS},
        {"GetWeight", CFGetWeight, METH_VARARGS},
        {"IsCanBePicked", CFIsCanBePicked, METH_VARARGS},
        {"GetMap", CFGetMap, METH_VARARGS},
        {"GetNextObject", CFGetNextObject, METH_VARARGS},
        {"GetPreviousObject", CFGetPreviousObject, METH_VARARGS},
        {"GetFirstObjectOnSquare", CFGetFirstObjectOnSquare, METH_VARARGS},
        {"SetQuantity", CFSetQuantity, METH_VARARGS},
        {"GetQuantity", CFGetQuantity, METH_VARARGS},
        {"InsertObjectInside", CFInsertObjectInside, METH_VARARGS},
        {"FindPlayer", CFFindPlayer, METH_VARARGS},
        {"Apply", CFApply, METH_VARARGS},
        {"Drop", CFDrop, METH_VARARGS},
        {"Take", CFTake, METH_VARARGS},
        {"IsInvisible", CFIsInvisible, METH_VARARGS},
        {"SetInvisible",CFSetInvisible,METH_VARARGS},
        {"GetExperience",CFGetExperience,METH_VARARGS},
        {"GetSpeed",CFGetSpeed,METH_VARARGS},
        {"SetSpeed",CFSetSpeed,METH_VARARGS},
        {"GetFood",CFGetFood,METH_VARARGS},
        {"SetFood",CFSetFood,METH_VARARGS},
        {"GetGrace",CFGetGrace,METH_VARARGS},
        {"SetGrace",CFSetGrace,METH_VARARGS},
        {"GetReturnValue",CFGetReturnValue,METH_VARARGS},
        {"SetReturnValue",CFSetReturnValue,METH_VARARGS},
        {"GetDirection",CFGetDirection,METH_VARARGS},
        {"SetDirection",CFSetDirection,METH_VARARGS},
        {"GetLastSP",CFGetLastSP,METH_VARARGS},
        {"SetLastSP",CFSetLastSP,METH_VARARGS},
        {"GetLastGrace",CFGetLastGrace,METH_VARARGS},
        {"SetLastGrace",CFSetLastGrace,METH_VARARGS},
        {"FixObject",CFFixObject,METH_VARARGS},
        {"SetFace",CFSetFace,METH_VARARGS},
        {"GetAttackType",CFGetAttackType,METH_VARARGS},
        {"SetAttackType",CFSetAttackType,METH_VARARGS},
        {"SetDamage",CFSetDamage,METH_VARARGS},
        {"GetDamage",CFGetDamage,METH_VARARGS},
        {"SetBeenApplied",CFSetBeenApplied,METH_VARARGS},
        {"SetIdentified",CFSetIdentified,METH_VARARGS},
        {"KillObject",CFKillObject,METH_VARARGS},
        {"WhoIsOther",CFWhoIsOther,METH_VARARGS},
        {"DirectionN",CFDirectionN,METH_VARARGS},
        {"DirectionNE",CFDirectionNE,METH_VARARGS},
        {"DirectionE",CFDirectionE,METH_VARARGS},
        {"DirectionSE",CFDirectionSE,METH_VARARGS},
        {"DirectionS",CFDirectionS,METH_VARARGS},
        {"DirectionSW",CFDirectionSW,METH_VARARGS},
        {"DirectionW",CFDirectionW,METH_VARARGS},
        {"DirectionNW",CFDirectionNW,METH_VARARGS},
        {"CastSpell",CFCastSpell,METH_VARARGS},
        {"ForgetSpell",CFForgetSpell,METH_VARARGS},
        {"AcquireSpell",CFAcquireSpell,METH_VARARGS},
        {"DoKnowSpell",CFDoKnowSpell,METH_VARARGS},
        {"CheckInvisibleObjectInside",CFCheckInvisibleObjectInside,METH_VARARGS},
        {"CreateInvisibleObjectInside",CFCreateInvisibleObjectInside,METH_VARARGS},
        {"CreateObjectInside",CFCreateObjectInside,METH_VARARGS},
        {"CheckMap",CFCheckMap,METH_VARARGS},
        {"CheckArchInventory",CFCheckArchInventory,METH_VARARGS},
        {"CheckInventory",CFCheckInventory,METH_VARARGS},
        {"CreateObject",CFCreateObject,METH_VARARGS},
        {"RemoveObject",CFRemoveObject,METH_VARARGS},
        {"IsAlive",CFIsAlive,METH_VARARGS},
        {"IsDungeonMaster",CFIsWiz,METH_VARARGS},
        {"WasDungeonMaster",CFWasWiz,METH_VARARGS},
        {"IsApplied",CFIsApplied,METH_VARARGS},
        {"IsUnpaid",CFIsUnpaid,METH_VARARGS},
        {"IsFlying",CFIsFlying,METH_VARARGS},
        {"IsMonster",CFIsMonster,METH_VARARGS},
        {"IsFriendly",CFIsFriendly,METH_VARARGS},
        {"IsGenerator",CFIsGenerator,METH_VARARGS},
        {"IsThrown",CFIsThrown,METH_VARARGS},
        {"CanSeeInvisible",CFCanSeeInvisible,METH_VARARGS},
        {"CanRoll",CFCanRoll,METH_VARARGS},
        {"IsTurnable",CFIsTurnable,METH_VARARGS},
        {"IsUsedUp",CFIsUsedUp,METH_VARARGS},
        {"IsIdentified",CFIsIdentified,METH_VARARGS},
        {"IsSplitting",CFIsSplitting,METH_VARARGS},
        {"HitBack",CFHitBack,METH_VARARGS},
        {"BlocksView",CFBlocksView,METH_VARARGS},
        {"IsUndead",CFIsUndead,METH_VARARGS},
        {"IsScared",CFIsScared,METH_VARARGS},
        {"IsUnaggressive",CFIsUnaggressive,METH_VARARGS},
        {"ReflectMissiles",CFReflectMissiles,METH_VARARGS},
        {"ReflectSpells",CFReflectSpells,METH_VARARGS},
        {"IsRunningAway",CFIsRunningAway,METH_VARARGS},
        {"CanPassThru",CFCanPassThru,METH_VARARGS},
        {"CanPickUp",CFCanPickUp,METH_VARARGS},
        {"IsUnique",CFIsUnique,METH_VARARGS},
        {"CanCastSpell",CFCanCastSpell,METH_VARARGS},
        {"CanUseScroll",CFCanUseScroll,METH_VARARGS},
        {"CanUseWand",CFCanUseWand,METH_VARARGS},
        {"CanUseBow",CFCanUseBow,METH_VARARGS},
        {"CanUseArmour",CFCanUseArmour,METH_VARARGS},
        {"CanUseWeapon",CFCanUseWeapon,METH_VARARGS},
        {"CanUseRing",CFCanUseRing,METH_VARARGS},
        {"HasXRays",CFHasXRays,METH_VARARGS},
        {"IsFloor",CFIsFloor,METH_VARARGS},
        {"IsLifesaver",CFIsLifeSaver,METH_VARARGS},
        {"IsSleeping",CFIsSleeping,METH_VARARGS},
        {"StandStill",CFStandStill,METH_VARARGS},
        {"OnlyAttack",CFOnlyAttack,METH_VARARGS},
        {"IsConfused",CFIsConfused,METH_VARARGS},
        {"HasStealth",CFHasStealth,METH_VARARGS},
        {"IsCursed",CFIsCursed,METH_VARARGS},
        {"IsDamned",CFIsDamned,METH_VARARGS},
        {"IsKnownMagical",CFIsKnownMagical,METH_VARARGS},
        {"IsKnownCursed",CFIsKnownCursed,METH_VARARGS},
        {"CanUseSkill",CFCanUseSkill,METH_VARARGS},
        {"HasBeenApplied",CFHasBeenApplied,METH_VARARGS},
        {"CanUseRod",CFCanUseRod,METH_VARARGS},
        {"CanUseHorn",CFCanUseHorn,METH_VARARGS},
        {"MakeInvisible",CFMakeInvisible,METH_VARARGS},
        {"IsBlind",CFIsBlind,METH_VARARGS},
        {"CanSeeInDark",CFCanSeeInDark,METH_VARARGS},
        {"GetAC",CFGetAC,METH_VARARGS},
        {"GetCharisma",CFGetCha,METH_VARARGS},
        {"GetConstitution",CFGetCon,METH_VARARGS},
        {"GetDexterity",CFGetDex,METH_VARARGS},
        {"GetHP",CFGetHP,METH_VARARGS},
        {"GetIntelligence",CFGetInt,METH_VARARGS},
        {"GetPower",CFGetPow,METH_VARARGS},
        {"GetSP",CFGetSP,METH_VARARGS},
        {"GetStrength",CFGetStr,METH_VARARGS},
        {"GetWisdom",CFGetWis,METH_VARARGS},
        {"GetMaxHP",CFGetMaxHP,METH_VARARGS},
        {"GetMaxSP",CFGetMaxSP,METH_VARARGS},
        {"GetXPosition",CFGetXPos,METH_VARARGS},
        {"GetYPosition",CFGetYPos,METH_VARARGS},
        {"SetPosition",CFSetPosition,METH_VARARGS},
        {"SetNickname",CFSetNickname,METH_VARARGS},
        {"SetAC",CFSetAC,METH_VARARGS},
        {"SetCharisma",CFSetCha,METH_VARARGS},
        {"SetConstitution",CFSetCon,METH_VARARGS},
        {"SetDexterity",CFSetDex,METH_VARARGS},
        {"SetHP",CFSetHP,METH_VARARGS},
        {"SetIntelligence",CFSetInt,METH_VARARGS},
        {"SetMaxHP",CFSetMaxHP,METH_VARARGS},
        {"SetMaxSP",CFSetMaxSP,METH_VARARGS},
        {"SetPower",CFSetPow,METH_VARARGS},
        {"SetSP",CFSetSP,METH_VARARGS},
        {"SetStrength",CFSetStr,METH_VARARGS},
        {"SetWisdom",CFSetWis,METH_VARARGS},
        {"Message",CFMessage,METH_VARARGS},
        {"Write",CFWrite,METH_VARARGS},
        {"IsOfType",CFIsOfType,METH_VARARGS},
        {"GetType",CFGetType,METH_VARARGS},
        {"GetEventHandler",CFGetEventHandler,METH_VARARGS},
        {"SetEventHandler",CFSetEventHandler,METH_VARARGS},
        {"GetEventPlugin",CFGetEventPlugin,METH_VARARGS},
        {"SetEventPlugin",CFSetEventPlugin,METH_VARARGS},
        {"GetEventOptions",CFGetEventOptions,METH_VARARGS},
        {"SetEventOptions",CFSetEventOptions,METH_VARARGS},
        {"Say", CFSay, METH_VARARGS},
        {"WhoAmI", CFWhoAmI, METH_VARARGS},
        {"WhoIsActivator", CFWhoIsActivator, METH_VARARGS},
        {"WhatIsMessage", CFWhatIsMessage, METH_VARARGS},
        {"SaveObject",CFSaveObject,METH_VARARGS},
        {"LoadObject",CFLoadObject,METH_VARARGS},
        {"GetIP",CFGetIP,METH_VARARGS},
        {"GetInventory",CFGetInventory,METH_VARARGS},
        {"GetInternalName",CFGetInternalName,METH_VARARGS},
        {"RegisterCommand",CFRegisterCommand,METH_VARARGS},
        {"GetValue",CFGetValue,METH_VARARGS},
        {"SetValue",CFSetValue,METH_VARARGS},
        {"GetMapWidth",CFGetMapWidth,METH_VARARGS},
        {"GetMapHeight",CFGetMapHeight,METH_VARARGS},
        {"GetObjectAt",CFGetObjectAt,METH_VARARGS},
        {"CostFlagFTrue",CFCostFlagFTrue,METH_VARARGS},
        {"CostFlagFBuy",CFCostFlagFBuy,METH_VARARGS},
        {"CostFlagFSell",CFCostFlagFSell,METH_VARARGS},
        {"CostFlagFNoBargain",CFCostFlagFNoBargain,METH_VARARGS},
        {"CostFlagFIdentified",CFCostFlagFIdentified,METH_VARARGS},
        {"CostFlagFNotCursed",CFCostFlagFNotCursed,METH_VARARGS},
        {"GetObjectCost", CFGetObjectCost,METH_VARARGS},
        {"GetObjectMoney", CFGetObjectMoney,METH_VARARGS},
        {"PayForItem",CFPayForItem,METH_VARARGS},
        {"PayAmount",CFPayAmount,METH_VARARGS},
        {"SendCustomCommand",CFSendCustomCommand,METH_VARARGS},
        {"GetHumidity",CFGetHumidity, METH_VARARGS},
        {"GetTemperature",CFGetTemperature, METH_VARARGS},
        {"GetPressure",CFGetPressure, METH_VARARGS},
        {"GetMapDirectory", CFGetMapDir, METH_VARARGS},
        {"GetUniqueDirectory", CFGetUniqueDir, METH_VARARGS},
        {"GetTempDirectory", CFGetTempDir, METH_VARARGS},
        {"GetConfigurationDirectory", CFGetConfigurationDir, METH_VARARGS},
        {"GetLocalDirectory", CFGetLocalDir, METH_VARARGS},
        {"GetPlayerDirectory", CFGetPlayerDir, METH_VARARGS},
        {"GetDataDirectory", CFGetDataDir, METH_VARARGS},
        {"GetWC", CFGetWC, METH_VARARGS},
        {"SetWC", CFSetWC, METH_VARARGS},
        {"SetVariable", CFSetVariable, METH_VARARGS},
        {"DecreaseObjectNr", CFDecreaseObjectNr, METH_VARARGS},
/* Attack Types Wrappers */
        {"AttackTypePhysical", CFAttackTypePhysical, METH_VARARGS},
        {"AttackTypeMagic", CFAttackTypeMagic, METH_VARARGS},
        {"AttackTypeFire", CFAttackTypeFire, METH_VARARGS},
        {"AttackTypeElectricity", CFAttackTypeElectricity, METH_VARARGS},
        {"AttackTypeCold", CFAttackTypeCold, METH_VARARGS},
        {"AttackTypeConfusion", CFAttackTypeConfusion, METH_VARARGS},
        {"AttackTypeAcid", CFAttackTypeAcid, METH_VARARGS},
        {"AttackTypeDrain", CFAttackTypeDrain, METH_VARARGS},
        {"AttackTypeWeaponmagic", CFAttackTypeWeaponmagic, METH_VARARGS},
        {"AttackTypeGhosthit", CFAttackTypeGhosthit, METH_VARARGS},
        {"AttackTypePoison", CFAttackTypePoison, METH_VARARGS},
        {"AttackTypeSlow", CFAttackTypeSlow, METH_VARARGS},
        {"AttackTypeParalyze", CFAttackTypeParalyze, METH_VARARGS},
        {"AttackTypeTurnUndead", CFAttackTypeTurnUndead, METH_VARARGS},
        {"AttackTypeFear", CFAttackTypeFear, METH_VARARGS},
        {"AttackTypeCancellation", CFAttackTypeCancellation, METH_VARARGS},
        {"AttackTypeDepletion", CFAttackTypeDepletion, METH_VARARGS},
        {"AttackTypeDeath", CFAttackTypeDeath, METH_VARARGS},
        {"AttackTypeChaos", CFAttackTypeChaos, METH_VARARGS},
        {"AttackTypeCounterspell", CFAttackTypeCounterspell, METH_VARARGS},
        {"AttackTypeGodpower", CFAttackTypeGodpower, METH_VARARGS},
        {"AttackTypeHolyWord", CFAttackTypeHolyWord, METH_VARARGS},
        {"AttackTypeBlind", CFAttackTypeBlind, METH_VARARGS},
        {"AttackTypeLifeStealing", CFAttackTypeLifeStealing, METH_VARARGS},
        {"AttackTypeDisease", CFAttackTypeDisease, METH_VARARGS},
/* Event Type Wrappers */
        {"EventApply", CFEventApply, METH_VARARGS},
        {"EventAttack", CFEventAttack, METH_VARARGS},
        {"EventDeath", CFEventDeath, METH_VARARGS},
        {"EventDrop", CFEventDrop, METH_VARARGS},
        {"EventPickup", CFEventPickup, METH_VARARGS},
        {"EventSay", CFEventSay, METH_VARARGS},
        {"EventStop", CFEventStop, METH_VARARGS},
        {"EventTime", CFEventTime, METH_VARARGS},
        {"EventThrow", CFEventThrow, METH_VARARGS},
        {"EventTrigger", CFEventTrigger, METH_VARARGS},
        {"EventClose", CFEventClose, METH_VARARGS},
        {"EventTimer", CFEventTimer, METH_VARARGS},
        {NULL, NULL}
};

/*****************************************************************************/
/* Commands management part.                                                 */
/* It is now possible to add commands to crossfire. The following stuff was  */
/* created to handle such commands.                                          */
/*****************************************************************************/

/* The "About Python" stuff. Bound to "python" command.                      */
MODULEAPI int cmd_aboutPython(object *op, char *params);
/* The following one handles all custom Python command calls.                */
MODULEAPI int cmd_customPython(object *op, char *params);

/* This structure is used to define one python-implemented crossfire command.*/
typedef struct PythonCmdStruct
{
    char *name;    /* The name of the command, as known in the game.         */
    char *script;  /* The name of the script file to bind.                   */
    double speed;  /* The speed of the command execution.                    */
} PythonCmd;

/* This plugin allows up to 1024 custom commands.                            */
#define NR_CUSTOM_CMD 1024
PythonCmd CustomCommand[NR_CUSTOM_CMD];
/* This one contains the index of the next command that needs to be run. I do*/
/* not like the use of such a global variable, but it is the most convenient */
/* way I found to pass the command index to cmd_customPython.                */
int NextCustomCommand;

#endif /*PLUGIN_PYTHON_H*/
