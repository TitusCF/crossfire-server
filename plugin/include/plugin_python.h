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
/* Version: 0.1 Alpha (also known as "Ophiuchus")                            */
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
#define PLUGIN_VERSION "CFPython Plugin 0.1"

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
static PyObject* CFSetGender(PyObject* self, PyObject* args);
static PyObject* CFSetRank(PyObject* self, PyObject* args);
static PyObject* CFSetAlignment(PyObject* self, PyObject* args);
static PyObject* CFSetGuild(PyObject* self, PyObject* args);
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

static PyObject* CFCreatePlayerForce(PyObject* self, PyObject* args);
static PyObject* CFCreatePlayerInfo(PyObject* self, PyObject* args);
static PyObject* CFGetPlayerInfo(PyObject* self, PyObject* args);
static PyObject* CFGetNextPlayerInfo(PyObject* self, PyObject* args);

static PyObject* CFCheckInvisibleInside(PyObject* self, PyObject* args);
static PyObject* CFCreateInvisibleInside(PyObject* self, PyObject* args);
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

/* Skill id wrappers */
static PyObject* CFSkillStealing(PyObject* self, PyObject* args);
static PyObject* CFSkillLockpicking(PyObject* self, PyObject* args);
static PyObject* CFSkillHiding(PyObject* self, PyObject* args);
static PyObject* CFSkillSmithery(PyObject* self, PyObject* args);
static PyObject* CFSkillBowyer(PyObject* self, PyObject* args);
static PyObject* CFSkillJeweler(PyObject* self, PyObject* args);
static PyObject* CFSkillAlchemy(PyObject* self, PyObject* args);
static PyObject* CFSkillThaumaturgy(PyObject* self, PyObject* args);
static PyObject* CFSkillLiteracy(PyObject* self, PyObject* args);
static PyObject* CFSkillBargaining(PyObject* self, PyObject* args);
static PyObject* CFSkillJumping(PyObject* self, PyObject* args);
static PyObject* CFSkillSenseMagic(PyObject* self, PyObject* args);
static PyObject* CFSkillOratory(PyObject* self, PyObject* args);
static PyObject* CFSkillSinging(PyObject* self, PyObject* args);
static PyObject* CFSkillDetectCurse(PyObject* self, PyObject* args);
static PyObject* CFSkillFindTraps(PyObject* self, PyObject* args);
static PyObject* CFSkillMeditation(PyObject* self, PyObject* args);
static PyObject* CFSkillBoxing(PyObject* self, PyObject* args);
static PyObject* CFSkillFlameTouch(PyObject* self, PyObject* args);
static PyObject* CFSkillKarate(PyObject* self, PyObject* args);
static PyObject* CFSkillClimbing(PyObject* self, PyObject* args);
static PyObject* CFSkillWoodsman(PyObject* self, PyObject* args);
static PyObject* CFSkillInscription(PyObject* self, PyObject* args);
static PyObject* CFSkillMeleeWeapons(PyObject* self, PyObject* args);
static PyObject* CFSkillThrowing(PyObject* self, PyObject* args);
static PyObject* CFSkillSpellCasting(PyObject* self, PyObject* args);
static PyObject* CFSkillRemoveTraps(PyObject* self, PyObject* args);
static PyObject* CFSkillSetTraps(PyObject* self, PyObject* args);
static PyObject* CFSkillUseMagicItem(PyObject* self, PyObject* args);
static PyObject* CFSkillPraying(PyObject* self, PyObject* args);
static PyObject* CFSkillClawing(PyObject* self, PyObject* args);
static PyObject* CFSkillLevitation(PyObject* self, PyObject* args);
/* Spell id wrappers */
static PyObject* CFSpellBullet(PyObject* self, PyObject* args);
static PyObject* CFSpellSmallFireball(PyObject* self, PyObject* args);
static PyObject* CFSpellMediumFireball(PyObject* self, PyObject* args);
static PyObject* CFSpellLargeFireball(PyObject* self, PyObject* args);
static PyObject* CFSpellBurningHands(PyObject* self, PyObject* args);
static PyObject* CFSpellSmallLightning(PyObject* self, PyObject* args);
static PyObject* CFSpellLargeLightning(PyObject* self, PyObject* args);
static PyObject* CFSpellMagicMissile(PyObject* self, PyObject* args);
static PyObject* CFSpellCreateBomb(PyObject* self, PyObject* args);
static PyObject* CFSpellSummonGolem(PyObject* self, PyObject* args);
static PyObject* CFSpellSummonFireElemental(PyObject* self, PyObject* args);
static PyObject* CFSpellSummonEarthElemental(PyObject* self, PyObject* args);
static PyObject* CFSpellSummonWaterElemental(PyObject* self, PyObject* args);
static PyObject* CFSpellSummonAirElemental(PyObject* self, PyObject* args);
static PyObject* CFSpellDimensionDoor(PyObject* self, PyObject* args);
static PyObject* CFSpellCreateEarthWall(PyObject* self, PyObject* args);
static PyObject* CFSpellParalyze(PyObject* self, PyObject* args);
static PyObject* CFSpellIcestorm(PyObject* self, PyObject* args);
static PyObject* CFSpellMagicMapping(PyObject* self, PyObject* args);
static PyObject* CFSpellTurnUndead(PyObject* self, PyObject* args);
static PyObject* CFSpellFear(PyObject* self, PyObject* args);
static PyObject* CFSpellPoisonCloud(PyObject* self, PyObject* args);
static PyObject* CFSpellWOW(PyObject* self, PyObject* args);
static PyObject* CFSpellDestruction(PyObject* self, PyObject* args);
static PyObject* CFSpellPerceiveSelf(PyObject* self, PyObject* args);
static PyObject* CFSpellWOR(PyObject* self, PyObject* args);
static PyObject* CFSpellInvisibility(PyObject* self, PyObject* args);
static PyObject* CFSpellInvisibleToUndead(PyObject* self, PyObject* args);
static PyObject* CFSpellProbe(PyObject* self, PyObject* args);
static PyObject* CFSpellLargeBullet(PyObject* self, PyObject* args);
static PyObject* CFSpellImprovedInvisibility(PyObject* self, PyObject* args);
static PyObject* CFSpellHolyWord(PyObject* self, PyObject* args);
static PyObject* CFSpellMinorHealing(PyObject* self, PyObject* args);
static PyObject* CFSpellMediumHealing(PyObject* self, PyObject* args);
static PyObject* CFSpellMajorHealing(PyObject* self, PyObject* args);
static PyObject* CFSpellHeal(PyObject* self, PyObject* args);
static PyObject* CFSpellCreateFood(PyObject* self, PyObject* args);
static PyObject* CFSpellEarthToDust(PyObject* self, PyObject* args);
static PyObject* CFSpellArmour(PyObject* self, PyObject* args);
static PyObject* CFSpellStrength(PyObject* self, PyObject* args);
static PyObject* CFSpellDexterity(PyObject* self, PyObject* args);
static PyObject* CFSpellConstitution(PyObject* self, PyObject* args);
static PyObject* CFSpellCharisma(PyObject* self, PyObject* args);
static PyObject* CFSpellFireWall(PyObject* self, PyObject* args);
static PyObject* CFSpellFrostWall(PyObject* self, PyObject* args);
static PyObject* CFSpellProtectionCold(PyObject* self, PyObject* args);
static PyObject* CFSpellProtectionElectricity(PyObject* self, PyObject* args);
static PyObject* CFSpellProtectionFire(PyObject* self, PyObject* args);
static PyObject* CFSpellProtectionPoison(PyObject* self, PyObject* args);
static PyObject* CFSpellProtectionSlow(PyObject* self, PyObject* args);
static PyObject* CFSpellProtectionParalyze(PyObject* self, PyObject* args);
static PyObject* CFSpellProtectionDrain(PyObject* self, PyObject* args);
static PyObject* CFSpellProtectionMagic(PyObject* self, PyObject* args);
static PyObject* CFSpellProtectionAttack(PyObject* self, PyObject* args);
static PyObject* CFSpellLevitate(PyObject* self, PyObject* args);
static PyObject* CFSpellSmallSpeedball(PyObject* self, PyObject* args);
static PyObject* CFSpellLargeSpeedball(PyObject* self, PyObject* args);
static PyObject* CFSpellHellfire(PyObject* self, PyObject* args);
static PyObject* CFSpellFirebreath(PyObject* self, PyObject* args);
static PyObject* CFSpellLargeIcestorm(PyObject* self, PyObject* args);
static PyObject* CFSpellCharging(PyObject* self, PyObject* args);
static PyObject* CFSpellPolymorph(PyObject* self, PyObject* args);
static PyObject* CFSpellCancellation(PyObject* self, PyObject* args);
static PyObject* CFSpellConfusion(PyObject* self, PyObject* args);
static PyObject* CFSpellMassConfusion(PyObject* self, PyObject* args);
static PyObject* CFSpellSummonPetMonster(PyObject* self, PyObject* args);
static PyObject* CFSpellSlow(PyObject* self, PyObject* args);
static PyObject* CFSpellRegenerateSpellpoints(PyObject* self, PyObject* args);
static PyObject* CFSpellCurePoison(PyObject* self, PyObject* args);
static PyObject* CFSpellProtectionConfusion(PyObject* self, PyObject* args);
static PyObject* CFSpellProtectionCancellation(PyObject* self, PyObject* args);
static PyObject* CFSpellProtectionDepletion(PyObject* self, PyObject* args);
static PyObject* CFSpellAlchemy(PyObject* self, PyObject* args);
static PyObject* CFSpellRemoveCurse(PyObject* self, PyObject* args);
static PyObject* CFSpellRemoveDamnation(PyObject* self, PyObject* args);
static PyObject* CFSpellIdentify(PyObject* self, PyObject* args);
static PyObject* CFSpellDetectMagic(PyObject* self, PyObject* args);
static PyObject* CFSpellDetectMonster(PyObject* self, PyObject* args);
static PyObject* CFSpellDetectEvil(PyObject* self, PyObject* args);
static PyObject* CFSpellDetectCurse(PyObject* self, PyObject* args);
static PyObject* CFSpellHeroism(PyObject* self, PyObject* args);
static PyObject* CFSpellAggravation(PyObject* self, PyObject* args);
static PyObject* CFSpellFirebolt(PyObject* self, PyObject* args);
static PyObject* CFSpellFrostbolt(PyObject* self, PyObject* args);
static PyObject* CFSpellShockwave(PyObject* self, PyObject* args);
static PyObject* CFSpellColorSpray(PyObject* self, PyObject* args);
static PyObject* CFSpellHaste(PyObject* self, PyObject* args);
static PyObject* CFSpellFaceOfDeath(PyObject* self, PyObject* args);
static PyObject* CFSpellBallLightning(PyObject* self, PyObject* args);
static PyObject* CFSpellMeteorSwarm(PyObject* self, PyObject* args);
static PyObject* CFSpellMeteor(PyObject* self, PyObject* args);
static PyObject* CFSpellMysticFist(PyObject* self, PyObject* args);
static PyObject* CFSpellRaiseDead(PyObject* self, PyObject* args);
static PyObject* CFSpellResurrection(PyObject* self, PyObject* args);
static PyObject* CFSpellReincarnation(PyObject* self, PyObject* args);
static PyObject* CFSpellImmuneCold(PyObject* self, PyObject* args);
static PyObject* CFSpellImmuneElectricity(PyObject* self, PyObject* args);
static PyObject* CFSpellImmuneFire(PyObject* self, PyObject* args);
static PyObject* CFSpellImmunePoison(PyObject* self, PyObject* args);
static PyObject* CFSpellImmuneSlow(PyObject* self, PyObject* args);
static PyObject* CFSpellImmuneParalyze(PyObject* self, PyObject* args);
static PyObject* CFSpellImmuneDrain(PyObject* self, PyObject* args);
static PyObject* CFSpellImmuneMagic(PyObject* self, PyObject* args);
static PyObject* CFSpellImmuneAttack(PyObject* self, PyObject* args);
static PyObject* CFSpellInvulnerability(PyObject* self, PyObject* args);
static PyObject* CFSpellProtection(PyObject* self, PyObject* args);
static PyObject* CFSpellRuneFire(PyObject* self, PyObject* args);
static PyObject* CFSpellRuneFrost(PyObject* self, PyObject* args);
static PyObject* CFSpellRuneShock(PyObject* self, PyObject* args);
static PyObject* CFSpellRuneBlast(PyObject* self, PyObject* args);
static PyObject* CFSpellRuneDeath(PyObject* self, PyObject* args);
static PyObject* CFSpellRuneMark(PyObject* self, PyObject* args);
static PyObject* CFSpellBuildDirector(PyObject* self, PyObject* args);
static PyObject* CFSpellChaosPool(PyObject* self, PyObject* args);
static PyObject* CFSpellBuildBulletWall(PyObject* self, PyObject* args);
static PyObject* CFSpellBuildLightningWall(PyObject* self, PyObject* args);
static PyObject* CFSpellBuildFireballWall(PyObject* self, PyObject* args);
static PyObject* CFSpellRuneMagic(PyObject* self, PyObject* args);
static PyObject* CFSpellRuneDrainSP(PyObject* self, PyObject* args);
static PyObject* CFSpellRuneAntimagic(PyObject* self, PyObject* args);
static PyObject* CFSpellRuneTransferrence(PyObject* self, PyObject* args);
static PyObject* CFSpellTransfer(PyObject* self, PyObject* args);
static PyObject* CFSpellMagicDrain(PyObject* self, PyObject* args);
static PyObject* CFSpellCounterspell(PyObject* self, PyObject* args);
static PyObject* CFSpellDispelRune(PyObject* self, PyObject* args);
static PyObject* CFSpellCureConfusion(PyObject* self, PyObject* args);
static PyObject* CFSpellRestoration(PyObject* self, PyObject* args);
static PyObject* CFSpellSummonEvilMonster(PyObject* self, PyObject* args);
static PyObject* CFSpellCounterwall(PyObject* self, PyObject* args);
static PyObject* CFSpellCauseLightWounds(PyObject* self, PyObject* args);
static PyObject* CFSpellCauseMediumWounds(PyObject* self, PyObject* args);
static PyObject* CFSpellCauseHeavyWounds(PyObject* self, PyObject* args);
static PyObject* CFSpellCharm(PyObject* self, PyObject* args);
static PyObject* CFSpellBanishment(PyObject* self, PyObject* args);
static PyObject* CFSpellCreateMissile(PyObject* self, PyObject* args);
static PyObject* CFSpellShowInvisible(PyObject* self, PyObject* args);
static PyObject* CFSpellXRay(PyObject* self, PyObject* args);
static PyObject* CFSpellPacify(PyObject* self, PyObject* args);
static PyObject* CFSpellSummonFog(PyObject* self, PyObject* args);
static PyObject* CFSpellSteambolt(PyObject* self, PyObject* args);
static PyObject* CFSpellCommandUndead(PyObject* self, PyObject* args);
static PyObject* CFSpellHolyOrb(PyObject* self, PyObject* args);
static PyObject* CFSpellSummonAvatar(PyObject* self, PyObject* args);
static PyObject* CFSpellHolyPossession(PyObject* self, PyObject* args);
static PyObject* CFSpellBless(PyObject* self, PyObject* args);
static PyObject* CFSpellCurse(PyObject* self, PyObject* args);
static PyObject* CFSpellRegeneration(PyObject* self, PyObject* args);
static PyObject* CFSpellConsecrate(PyObject* self, PyObject* args);
static PyObject* CFSpellSummonCultMonsters(PyObject* self, PyObject* args);
static PyObject* CFSpellCauseCriticalWounds(PyObject* self, PyObject* args);
static PyObject* CFSpellHolyWrath(PyObject* self, PyObject* args);
static PyObject* CFSpellRetribution(PyObject* self, PyObject* args);
static PyObject* CFSpellFingerDeath(PyObject* self, PyObject* args);
static PyObject* CFSpellInsectPlague(PyObject* self, PyObject* args);
static PyObject* CFSpellHolyServant(PyObject* self, PyObject* args);
static PyObject* CFSpellWallOfThorns(PyObject* self, PyObject* args);
static PyObject* CFSpellStaffToSnake(PyObject* self, PyObject* args);
static PyObject* CFSpellLight(PyObject* self, PyObject* args);
static PyObject* CFSpellDarkness(PyObject* self, PyObject* args);
static PyObject* CFSpellNightfall(PyObject* self, PyObject* args);
static PyObject* CFSpellDaylight(PyObject* self, PyObject* args);
static PyObject* CFSpellSunSpear(PyObject* self, PyObject* args);
static PyObject* CFSpellFaeryFire(PyObject* self, PyObject* args);
static PyObject* CFSpellCureBlindness(PyObject* self, PyObject* args);
static PyObject* CFSpellDarkVision(PyObject* self, PyObject* args);
static PyObject* CFSpellBulletSwarm(PyObject* self, PyObject* args);
static PyObject* CFSpellBulletStorm(PyObject* self, PyObject* args);
static PyObject* CFSpellCauseManyWounds(PyObject* self, PyObject* args);
static PyObject* CFSpellSmallSnowstorm(PyObject* self, PyObject* args);
static PyObject* CFSpellMediumSnowstorm(PyObject* self, PyObject* args);
static PyObject* CFSpellLargeSnowstorm(PyObject* self, PyObject* args);
static PyObject* CFSpellCureDisease(PyObject* self, PyObject* args);
static PyObject* CFSpellCauseEbola(PyObject* self, PyObject* args);
static PyObject* CFSpellCauseFlu(PyObject* self, PyObject* args);
static PyObject* CFSpellCausePlague(PyObject* self, PyObject* args);
static PyObject* CFSpellCauseLeprosy(PyObject* self, PyObject* args);
static PyObject* CFSpellCauseSmallPox(PyObject* self, PyObject* args);
static PyObject* CFSpellCausePneumonicPlague(PyObject* self, PyObject* args);
static PyObject* CFSpellCauseAnthrax(PyObject* self, PyObject* args);
static PyObject* CFSpellCauseTyphoid(PyObject* self, PyObject* args);
static PyObject* CFSpellManaBlast(PyObject* self, PyObject* args);
static PyObject* CFSpellSmallManaball(PyObject* self, PyObject* args);
static PyObject* CFSpellMediumManaball(PyObject* self, PyObject* args);
static PyObject* CFSpellLargeManaball(PyObject* self, PyObject* args);
static PyObject* CFSpellManabolt(PyObject* self, PyObject* args);
static PyObject* CFSpellDancingSword(PyObject* self, PyObject* args);
static PyObject* CFSpellAnimateWeapon(PyObject* self, PyObject* args);
static PyObject* CFSpellCauseCold(PyObject* self, PyObject* args);
static PyObject* CFSpellDivineShock(PyObject* self, PyObject* args);
static PyObject* CFSpellWindStorm(PyObject* self, PyObject* args);
static PyObject* CFSpellSanctuary(PyObject* self, PyObject* args);
static PyObject* CFSpellPeace(PyObject* self, PyObject* args);
static PyObject* CFSpellSpiderWeb(PyObject* self, PyObject* args);
static PyObject* CFSpellConflict(PyObject* self, PyObject* args);
static PyObject* CFSpellRage(PyObject* self, PyObject* args);
static PyObject* CFSpellForkedLightning(PyObject* self, PyObject* args);
static PyObject* CFSpellPoisonFog(PyObject* self, PyObject* args);
static PyObject* CFSpellFlameAura(PyObject* self, PyObject* args);
static PyObject* CFSpellVitriol(PyObject* self, PyObject* args);
static PyObject* CFSpellVitriolSplash(PyObject* self, PyObject* args);
static PyObject* CFSpellIronwoodSkin(PyObject* self, PyObject* args);
static PyObject* CFSpellWrathfullEye(PyObject* self, PyObject* args);
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

static PyObject* CFLoadObject(PyObject* self, PyObject* args);
static PyObject* CFSaveObject(PyObject* self, PyObject* args);
static PyObject* CFRegisterCommand(PyObject* self, PyObject* args);
static PyObject* CFGetValue(PyObject* self, PyObject* args);
static PyObject* CFSetValue(PyObject* self, PyObject* args);
static PyObject* CFGetMapWidth(PyObject* self, PyObject* args);
static PyObject* CFGetMapHeight(PyObject* self, PyObject* args);
static PyObject* CFGetObjectAt(PyObject* self, PyObject* args);
static PyObject* CFSetPreviousObject(PyObject* self, PyObject* args);
static PyObject* CFSetNextObject(PyObject* self, PyObject* args);
static PyObject* CFCostFlagFTrue(PyObject* self, PyObject* args);
static PyObject* CFCostFlagFBuy(PyObject* self, PyObject* args);
static PyObject* CFCostFlagFSell(PyObject* self, PyObject* args);
static PyObject* CFGetObjectCost(PyObject* self, PyObject* args);
static PyObject* CFGetObjectMoney(PyObject* self, PyObject* args);
static PyObject* CFPayForItem(PyObject* self, PyObject* args);
static PyObject* CFPayAmount(PyObject* self, PyObject* args);

/* Those are used to handle the events. The first one is used when a player  */
/* attacks with a "scripted" weapon. HandleEvent is used for all other events*/
MODULEAPI int HandleUseWeaponEvent(CFParm* CFP);
MODULEAPI int HandleEvent(CFParm* CFP);
MODULEAPI int HandleGlobalEvent(CFParm* CFP);
/* Called to start the Python Interpreter.                                   */
MODULEAPI void initCFPython();

/* The execution stack. Altough it is quite rare, a script can actually      */
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
int StackParm1[MAX_RECURSIVE_CALL];
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

        {"CreatePlayerForce",CFCreatePlayerForce,METH_VARARGS},
        {"CreatePlayerInfo",CFCreatePlayerInfo,METH_VARARGS},
        {"GetPlayerInfo",CFGetPlayerInfo,METH_VARARGS},
        {"GetNextPlayerInfo",CFGetNextPlayerInfo,METH_VARARGS},
        
        {"CheckInvisibleObjectInside",CFCheckInvisibleInside,METH_VARARGS},
        {"CreateInvisibleObjectInside",CFCreateInvisibleInside,METH_VARARGS},
        {"CreateObjectInside",CFCreateObjectInside,METH_VARARGS},
        {"CheckMap",CFCheckMap,METH_VARARGS},
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
        {"SetGender", CFSetGender, METH_VARARGS},
        {"SetRank", CFSetRank, METH_VARARGS},
        {"SetAlignment", CFSetAlignment, METH_VARARGS},
        {"SetGuild", CFSetGuild, METH_VARARGS},
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
        {"SetNextObject",CFSetNextObject,METH_VARARGS},
        {"SetPreviousObject",CFSetPreviousObject,METH_VARARGS},
        {"CostFlagFTrue",CFCostFlagFTrue,METH_VARARGS},
        {"CostFlagFBuy",CFCostFlagFBuy,METH_VARARGS},
        {"CostFlagFSell",CFCostFlagFSell,METH_VARARGS},
        {"GetObjectCost", CFGetObjectCost,METH_VARARGS},
        {"GetObjectMoney", CFGetObjectMoney,METH_VARARGS},
        {"PayForItem",CFPayForItem,METH_VARARGS},
        {"PayAmount",CFPayAmount,METH_VARARGS},
/* Skills wrappers : */
        {"SkillStealing", CFSkillStealing, METH_VARARGS},
        {"SkillLockpicking", CFSkillLockpicking, METH_VARARGS},
        {"SkillHiding", CFSkillHiding, METH_VARARGS},
        {"SkillSmithery", CFSkillSmithery, METH_VARARGS},
        {"SkillBowyer", CFSkillBowyer, METH_VARARGS},
        {"SkillJeweler", CFSkillJeweler, METH_VARARGS},
        {"SkillAlchemy", CFSkillAlchemy, METH_VARARGS},
        {"SkillThaumaturgy", CFSkillThaumaturgy, METH_VARARGS},
        {"SkillLiteracy", CFSkillLiteracy, METH_VARARGS},
        {"SkillBargaining", CFSkillBargaining, METH_VARARGS},
        {"SkillJumping", CFSkillJumping, METH_VARARGS},
        {"SkillSenseMagic", CFSkillSenseMagic, METH_VARARGS},
        {"SkillOratory", CFSkillOratory, METH_VARARGS},
        {"SkillSinging", CFSkillSinging, METH_VARARGS},
        {"SkillSenseCurse", CFSkillDetectCurse, METH_VARARGS},
        {"SkillFindTraps", CFSkillFindTraps, METH_VARARGS},
        {"SkillMeditation", CFSkillMeditation, METH_VARARGS},
        {"SkillBoxing", CFSkillBoxing, METH_VARARGS},
        {"SkillFlameTouch", CFSkillFlameTouch, METH_VARARGS},
        {"SkillKarate", CFSkillKarate, METH_VARARGS},
        {"SkillClimbing", CFSkillClimbing, METH_VARARGS},
        {"SkillWoodsman", CFSkillWoodsman, METH_VARARGS},
        {"SkillInscription", CFSkillInscription, METH_VARARGS},
        {"SkillMeleeWeapons", CFSkillMeleeWeapons, METH_VARARGS},
        {"SkillThrowing", CFSkillThrowing, METH_VARARGS},
        {"SkillSpellcasting", CFSkillSpellCasting, METH_VARARGS},
        {"SkillRemoveTraps", CFSkillRemoveTraps, METH_VARARGS},
        {"SkillSetTraps", CFSkillSetTraps, METH_VARARGS},
        {"SkillUseMagicItem", CFSkillUseMagicItem, METH_VARARGS},
        {"SkillPraying", CFSkillPraying, METH_VARARGS},
        {"SkillClawing", CFSkillClawing, METH_VARARGS},
        {"SkillLevitation", CFSkillLevitation, METH_VARARGS},
/* Spells wrappers */
        {"SpellBullet", CFSpellBullet, METH_VARARGS},
        {"SpellSmallFireball", CFSpellSmallFireball, METH_VARARGS},
        {"SpellMediumFireball", CFSpellMediumFireball, METH_VARARGS},
        {"SpellLargeFireball", CFSpellLargeFireball, METH_VARARGS},
        {"SpellBurningHands", CFSpellBurningHands, METH_VARARGS},
        {"SpellSmallLightning", CFSpellSmallLightning, METH_VARARGS},
        {"SpellLargeLightning", CFSpellLargeLightning, METH_VARARGS},
        {"SpellMagicMissile", CFSpellMagicMissile, METH_VARARGS},
        {"SpellCreateBomb", CFSpellCreateBomb, METH_VARARGS},
        {"SpellSummonGolem", CFSpellSummonGolem, METH_VARARGS},
        {"SpellSummonFireElemental", CFSpellSummonFireElemental, METH_VARARGS},
        {"SpellSummonEarthElemental", CFSpellSummonEarthElemental, METH_VARARGS},
        {"SpellSummonWaterElemental", CFSpellSummonWaterElemental, METH_VARARGS},
        {"SpellSummonAirElemental", CFSpellSummonAirElemental, METH_VARARGS},
        {"SpellDimensionDoor", CFSpellDimensionDoor, METH_VARARGS},
        {"SpellCreateEarthWall", CFSpellCreateEarthWall, METH_VARARGS},
        {"SpellParalyze", CFSpellParalyze, METH_VARARGS},
        {"SpellIcestorm", CFSpellIcestorm, METH_VARARGS},
        {"SpellMagicMapping", CFSpellMagicMapping, METH_VARARGS},
        {"SpellTurnUndead", CFSpellTurnUndead, METH_VARARGS},
        {"SpellFear", CFSpellFear, METH_VARARGS},
        {"SpellPoisonCloud", CFSpellPoisonCloud, METH_VARARGS},
        {"SpellWallOfWonder", CFSpellWOW, METH_VARARGS},
        {"SpellDestruction", CFSpellDestruction, METH_VARARGS},
        {"SpellPerceiveSelf", CFSpellPerceiveSelf, METH_VARARGS},
        {"SpellWordOfRecall", CFSpellWOR, METH_VARARGS},
        {"SpellInvisibility", CFSpellInvisibility, METH_VARARGS},
        {"SpellInvisibleToUndead", CFSpellInvisibleToUndead, METH_VARARGS},
        {"SpellProbe", CFSpellProbe, METH_VARARGS},
        {"SpellLargeBullet", CFSpellLargeBullet, METH_VARARGS},
        {"SpellImprovedInvisibility", CFSpellImprovedInvisibility, METH_VARARGS},
        {"SpellHolyWord", CFSpellHolyWord, METH_VARARGS},
        {"SpellMinorHealing", CFSpellMinorHealing, METH_VARARGS},
        {"SpellMediumHealing", CFSpellMediumHealing, METH_VARARGS},
        {"SpellMajorHealing", CFSpellMajorHealing, METH_VARARGS},
        {"SpellHeal", CFSpellHeal, METH_VARARGS},
        {"SpellCreateFood", CFSpellCreateFood, METH_VARARGS},
        {"SpellEarthToDust", CFSpellEarthToDust, METH_VARARGS},
        {"SpellArmour", CFSpellArmour, METH_VARARGS},
        {"SpellStrength", CFSpellStrength, METH_VARARGS},
        {"SpellDexterity", CFSpellDexterity, METH_VARARGS},
        {"SpellConstitution", CFSpellConstitution, METH_VARARGS},
        {"SpellCharisma", CFSpellCharisma, METH_VARARGS},
        {"SpellFireWall", CFSpellFireWall, METH_VARARGS},
        {"SpellFrostWall", CFSpellFrostWall, METH_VARARGS},
        {"SpellProtectionCold", CFSpellProtectionCold, METH_VARARGS},
        {"SpellProtectionElectricity", CFSpellProtectionElectricity, METH_VARARGS},
        {"SpellProtectionFire", CFSpellProtectionFire, METH_VARARGS},
        {"SpellProtectionPoison", CFSpellProtectionPoison, METH_VARARGS},
        {"SpellProtectionSlow", CFSpellProtectionSlow, METH_VARARGS},
        {"SpellProtectionParalyze", CFSpellProtectionParalyze, METH_VARARGS},
        {"SpellProtectionDrain", CFSpellProtectionDrain, METH_VARARGS},
        {"SpellProtectionMagic", CFSpellProtectionMagic, METH_VARARGS},
        {"SpellProtectionAttack", CFSpellProtectionAttack, METH_VARARGS},
        {"SpellLevitate", CFSpellLevitate, METH_VARARGS},
        {"SpellSmallSpeedball", CFSpellSmallSpeedball, METH_VARARGS},
        {"SpellLargeSpeedball", CFSpellLargeSpeedball, METH_VARARGS},
        {"SpellHellfire", CFSpellHellfire, METH_VARARGS},
        {"SpellFirebreath", CFSpellFirebreath, METH_VARARGS},
        {"SpellLargeIcestorm", CFSpellLargeIcestorm, METH_VARARGS},
        {"SpellCharging", CFSpellCharging, METH_VARARGS},
        {"SpellPolymorph", CFSpellPolymorph, METH_VARARGS},
        {"SpellCancellation", CFSpellCancellation, METH_VARARGS},
        {"SpellConfusion", CFSpellConfusion, METH_VARARGS},
        {"SpellMassConfusion", CFSpellMassConfusion, METH_VARARGS},
        {"SpellSummonPetMonster", CFSpellSummonPetMonster, METH_VARARGS},
        {"SpellSlow", CFSpellSlow, METH_VARARGS},
        {"SpellRegenerateSpellpoints", CFSpellRegenerateSpellpoints, METH_VARARGS},
        {"SpellCurePoison", CFSpellCurePoison, METH_VARARGS},
        {"SpellProtectionConfusion", CFSpellProtectionConfusion, METH_VARARGS},
        {"SpellProtectionCancellation", CFSpellProtectionCancellation, METH_VARARGS},
        {"SpellProtectionDepletion", CFSpellProtectionDepletion, METH_VARARGS},
        {"SpellAlchemy", CFSpellAlchemy, METH_VARARGS},
        {"SpellRemoveCurse", CFSpellRemoveCurse, METH_VARARGS},
        {"SpellRemoveDamnation", CFSpellRemoveDamnation, METH_VARARGS},
        {"SpellIdentify", CFSpellIdentify, METH_VARARGS},
        {"SpellDetectMagic", CFSpellDetectMagic, METH_VARARGS},
        {"SpellDetectMonster", CFSpellDetectMonster, METH_VARARGS},
        {"SpellDetectEvil", CFSpellDetectEvil, METH_VARARGS},
        {"SpellDetectCurse", CFSpellDetectCurse, METH_VARARGS},
        {"SpellHeroism", CFSpellHeroism, METH_VARARGS},
        {"SpellAggravation", CFSpellAggravation, METH_VARARGS},
        {"SpellFirebolt", CFSpellFirebolt, METH_VARARGS},
        {"SpellFrostbolt", CFSpellFrostbolt, METH_VARARGS},
        {"SpellShockwave", CFSpellShockwave, METH_VARARGS},
        {"SpellColorSpray", CFSpellColorSpray, METH_VARARGS},
        {"SpellHaste", CFSpellHaste, METH_VARARGS},
        {"SpellFaceOfDeath", CFSpellFaceOfDeath, METH_VARARGS},
        {"SpellBallLightning", CFSpellBallLightning, METH_VARARGS},
        {"SpellMeteorSwarm", CFSpellMeteorSwarm, METH_VARARGS},
        {"SpellMeteor", CFSpellMeteor, METH_VARARGS},
        {"SpellMysticFist", CFSpellMysticFist, METH_VARARGS},
        {"SpellRaiseDead", CFSpellRaiseDead, METH_VARARGS},
        {"SpellResurrection", CFSpellResurrection, METH_VARARGS},
        {"SpellReincarnation", CFSpellReincarnation, METH_VARARGS},
        {"SpellImmuneCold", CFSpellImmuneCold, METH_VARARGS},
        {"SpellImmuneElectricity", CFSpellImmuneElectricity, METH_VARARGS},
        {"SpellImmuneFire", CFSpellImmuneFire, METH_VARARGS},
        {"SpellImmunePoison", CFSpellImmunePoison, METH_VARARGS},
        {"SpellImmuneSlow", CFSpellImmuneSlow, METH_VARARGS},
        {"SpellImmuneParalyze", CFSpellImmuneParalyze, METH_VARARGS},
        {"SpellImmuneDrain", CFSpellImmuneDrain, METH_VARARGS},
        {"SpellImmuneMagic", CFSpellImmuneMagic, METH_VARARGS},
        {"SpellImmuneAttack", CFSpellImmuneAttack, METH_VARARGS},
        {"SpellInvulnerability", CFSpellInvulnerability, METH_VARARGS},
        {"SpellProtection", CFSpellProtection, METH_VARARGS},
        {"SpellRuneFire", CFSpellRuneFire, METH_VARARGS},
        {"SpellRuneFrost", CFSpellRuneFrost, METH_VARARGS},
        {"SpellRuneShock", CFSpellRuneShock, METH_VARARGS},
        {"SpellRuneBlast", CFSpellRuneBlast, METH_VARARGS},
        {"SpellRuneDeath", CFSpellRuneDeath, METH_VARARGS},
        {"SpellRuneMark", CFSpellRuneMark, METH_VARARGS},
        {"SpellBuildDirector", CFSpellBuildDirector, METH_VARARGS},
        {"SpellChaosPool", CFSpellChaosPool, METH_VARARGS},
        {"SpellBuildBulletWall", CFSpellBuildBulletWall, METH_VARARGS},
        {"SpellBuildLightningWall", CFSpellBuildLightningWall, METH_VARARGS},
        {"SpellBuildFireballWall", CFSpellBuildFireballWall, METH_VARARGS},
        {"SpellRuneMagic", CFSpellRuneMagic, METH_VARARGS},
        {"SpellRuneDrainSpellpoints", CFSpellRuneDrainSP, METH_VARARGS},
        {"SpellRuneAntimagic", CFSpellRuneAntimagic, METH_VARARGS},
        {"SpellRuneTransferrence", CFSpellRuneTransferrence, METH_VARARGS},
        {"SpellTransfer", CFSpellTransfer, METH_VARARGS},
        {"SpellMagicDrain", CFSpellMagicDrain, METH_VARARGS},
        {"SpellCounterspell", CFSpellCounterspell, METH_VARARGS},
        {"SpellDispelRune", CFSpellDispelRune, METH_VARARGS},
        {"SpellCureConfusion", CFSpellCureConfusion, METH_VARARGS},
        {"SpellRestoration", CFSpellRestoration, METH_VARARGS},
        {"SpellSummonEvilMonster", CFSpellSummonEvilMonster, METH_VARARGS},
        {"SpellCounterwall", CFSpellCounterwall, METH_VARARGS},
        {"SpellCauseLightWounds", CFSpellCauseLightWounds, METH_VARARGS},
        {"SpellCauseMediumWounds", CFSpellCauseMediumWounds, METH_VARARGS},
        {"SpellCauseHeavyWounds", CFSpellCauseHeavyWounds, METH_VARARGS},
        {"SpellCharm", CFSpellCharm, METH_VARARGS},
        {"SpellBanishment", CFSpellBanishment, METH_VARARGS},
        {"SpellCreateMissile", CFSpellCreateMissile, METH_VARARGS},
        {"SpellShowInvisible", CFSpellShowInvisible, METH_VARARGS},
        {"SpellXRay", CFSpellXRay, METH_VARARGS},
        {"SpellPacify", CFSpellPacify, METH_VARARGS},
        {"SpellSummonFog", CFSpellSummonFog, METH_VARARGS},
        {"SpellSteambolt", CFSpellSteambolt, METH_VARARGS},
        {"SpellCommandUndead", CFSpellCommandUndead, METH_VARARGS},
        {"SpellHolyOrb", CFSpellHolyOrb, METH_VARARGS},
        {"SpellSummonAvatar", CFSpellSummonAvatar, METH_VARARGS},
        {"SpellHolyPossession", CFSpellHolyPossession, METH_VARARGS},
        {"SpellBless", CFSpellBless, METH_VARARGS},
        {"SpellCurse", CFSpellCurse, METH_VARARGS},
        {"SpellRegeneration", CFSpellRegeneration, METH_VARARGS},
        {"SpellConsecrate", CFSpellConsecrate, METH_VARARGS},
        {"SpellSummonCultMonsters", CFSpellSummonCultMonsters, METH_VARARGS},
        {"SpellCauseCriticalWounds", CFSpellCauseCriticalWounds, METH_VARARGS},
        {"SpellHolyWrath", CFSpellHolyWrath, METH_VARARGS},
        {"SpellRetribution", CFSpellRetribution, METH_VARARGS},
        {"SpellFingerDeath", CFSpellFingerDeath, METH_VARARGS},
        {"SpellInsectPlague", CFSpellInsectPlague, METH_VARARGS},
        {"SpellHolyServant", CFSpellHolyServant, METH_VARARGS},
        {"SpellWallOfThorns", CFSpellWallOfThorns, METH_VARARGS},
        {"SpellStaffToSnake", CFSpellStaffToSnake, METH_VARARGS},
        {"SpellSpellLight", CFSpellLight, METH_VARARGS},
        {"SpellDarkness", CFSpellDarkness, METH_VARARGS},
        {"SpellNightfall", CFSpellNightfall, METH_VARARGS},
        {"SpellDaylight", CFSpellDaylight, METH_VARARGS},
        {"SpellSunSpear", CFSpellSunSpear, METH_VARARGS},
        {"SpellFaeryFire", CFSpellFaeryFire, METH_VARARGS},
        {"SpellCureBlindness", CFSpellCureBlindness, METH_VARARGS},
        {"SpellDarkVision", CFSpellDarkVision, METH_VARARGS},
        {"SpellBulletSwarm", CFSpellBulletSwarm, METH_VARARGS},
        {"SpellBulletStorm", CFSpellBulletStorm, METH_VARARGS},
        {"SpellCauseManyWounds", CFSpellCauseManyWounds, METH_VARARGS},
        {"SpellSmallSnowstorm", CFSpellSmallSnowstorm, METH_VARARGS},
        {"SpellMediumSnowstorm", CFSpellMediumSnowstorm, METH_VARARGS},
        {"SpellLargeSnowstorm", CFSpellLargeSnowstorm, METH_VARARGS},
        {"SpellCureDisease", CFSpellCureDisease, METH_VARARGS},
        {"SpellCauseEbola", CFSpellCauseEbola, METH_VARARGS},
        {"SpellCauseFlu", CFSpellCauseFlu, METH_VARARGS},
        {"SpellCausePlague", CFSpellCausePlague, METH_VARARGS},
        {"SpellCauseLeprosy", CFSpellCauseLeprosy, METH_VARARGS},
        {"SpellCauseSmallPox", CFSpellCauseSmallPox, METH_VARARGS},
        {"SpellCausePneumonicPlague", CFSpellCausePneumonicPlague, METH_VARARGS},
        {"SpellCauseAnthrax", CFSpellCauseAnthrax, METH_VARARGS},
        {"SpellCauseTyphoid", CFSpellCauseTyphoid, METH_VARARGS},
        {"SpellManaBlast", CFSpellManaBlast, METH_VARARGS},
        {"SpellSmallManaball", CFSpellSmallManaball, METH_VARARGS},
        {"SpellMediumManaball", CFSpellMediumManaball, METH_VARARGS},
        {"SpellLargeManaball", CFSpellLargeManaball, METH_VARARGS},
        {"SpellManabolt", CFSpellManabolt, METH_VARARGS},
        {"SpellDancingSword", CFSpellDancingSword, METH_VARARGS},
        {"SpellAnimateWeapon", CFSpellAnimateWeapon, METH_VARARGS},
        {"SpellCauseCold", CFSpellCauseCold, METH_VARARGS},
        {"SpellDivineShock", CFSpellDivineShock, METH_VARARGS},
        {"SpellWindStorm", CFSpellWindStorm, METH_VARARGS},
        {"SpellSanctuary", CFSpellSanctuary, METH_VARARGS},
        {"SpellPeace", CFSpellPeace, METH_VARARGS},
        {"SpellSpiderWeb", CFSpellSpiderWeb, METH_VARARGS},
        {"SpellConflict", CFSpellConflict, METH_VARARGS},
        {"SpellRage", CFSpellRage, METH_VARARGS},
        {"SpellForkedLightning", CFSpellForkedLightning, METH_VARARGS},
        {"SpellPoisonFog", CFSpellPoisonFog, METH_VARARGS},
        {"SpellFlameAura", CFSpellFlameAura, METH_VARARGS},
        {"SpellVitriol", CFSpellVitriol, METH_VARARGS},
        {"SpellVitriolSplash", CFSpellVitriolSplash, METH_VARARGS},
        {"SpellIronwoodSkin", CFSpellIronwoodSkin, METH_VARARGS},
        {"SpellWrathfullEye", CFSpellWrathfullEye, METH_VARARGS},
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
    char *name;    /* The name of the command, as known in the game.    */
    char *script;  /* The name of the script file to bind.              */
    double speed;   /* The speed of the command execution.                   */
} PythonCmd;

/* This plugin allows up to 1024 custom commands.                            */
#define NR_CUSTOM_CMD 1024
PythonCmd CustomCommand[NR_CUSTOM_CMD];
/* This one contains the index of the next command that needs to be run. I do*/
/* not like the use of such a global variable, but it is the most convenient */
/* way I found to pass the command index to cmd_customPython.                */
int NextCustomCommand;
#endif /*PLUGIN_PYTHON_H*/
