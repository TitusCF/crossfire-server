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
/* Version history:                                                          */
/* 0.1 "Ophiuchus"   - Initial Alpha release                                 */
/* 0.5 "Stalingrad"  - Message length overflow corrected.                    */
/* 0.6 "Kharkov"     - Message and Write correctly redefined.                */
/* 0.7 "Koursk"      - Setting informations implemented.                     */
/*****************************************************************************/
/* Version: 0.6 Beta (also known as "Kharkov")                               */
/* Contact: yann.chachkoff@mailandnews.com                                   */
/*****************************************************************************/
/* That code is placed under the GNU General Public Licence (GPL)            */
/* (C)2001 by Chachkoff Yann (Feel free to deliver your complaints)          */
/*****************************************************************************/

/* First let's include the header file needed                                */

#include <plugin_python.h>

#define PYTHON_DEBUG   /* give us some general infos out */

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

/*****************************************************************************/
/* And now the big part - The implementation of CFPython functions in C.     */
/* All comments for those functions have the following entries:              */
/* - The name of the function;                                               */
/* - How it is called from Python;                                           */
/* - The development state.                                                  */
/* The development state can be:                                             */
/* - Unknown  : Don't know if it has been tested already or not;             */
/* - Stable   : Has been tested and works under any common case;             */
/* - Untested : Not yet tested;                                              */
/* - Unstable : Has been tested, but caused some problems/bugged.            */
/* Such a system may seem quite silly and boring, but I already got some     */
/* success while using it, so I put it here too. Feel free to change the     */
/* status field of any function that you may have tested if needed.          */
/*****************************************************************************/
/* The functions that are simple wrappers to CF id numbers are not commented */
/* with that system since they don't need debugging because they're simple.  */
/*****************************************************************************/

/*****************************************************************************/
/* Wrappers for attack numbers.                                              */
/*****************************************************************************/
#ifndef FIRE_DIRECTIONAL
#define FIRE_DIRECTIONAL 1
#endif
static PyObject* CFAttackTypePhysical(PyObject* self, PyObject* args)
{
  int val = 1;
  if (!PyArg_ParseTuple(args,"",NULL))
      return NULL;
  return Py_BuildValue("i",val);
};
static PyObject* CFAttackTypeMagic(PyObject* self, PyObject* args)
{
  int val = 2;
  if (!PyArg_ParseTuple(args,"",NULL))
      return NULL;
  return Py_BuildValue("i",val);
};
static PyObject* CFAttackTypeFire(PyObject* self, PyObject* args)
{
  int val = 4;
  if (!PyArg_ParseTuple(args,"",NULL))
      return NULL;
  return Py_BuildValue("i",val);
};
static PyObject* CFAttackTypeElectricity(PyObject* self, PyObject* args)
{
  int val = 8;
  if (!PyArg_ParseTuple(args,"",NULL))
      return NULL;
  return Py_BuildValue("i",val);
};
static PyObject* CFAttackTypeCold(PyObject* self, PyObject* args)
{
  int val = 16;
  if (!PyArg_ParseTuple(args,"",NULL))
      return NULL;
  return Py_BuildValue("i",val);
};
static PyObject* CFAttackTypeConfusion(PyObject* self, PyObject* args)
{
  int val = 32;
  if (!PyArg_ParseTuple(args,"",NULL))
      return NULL;
  return Py_BuildValue("i",val);
};
static PyObject* CFAttackTypeAcid(PyObject* self, PyObject* args)
{
  int val = 64;
  if (!PyArg_ParseTuple(args,"",NULL))
      return NULL;
  return Py_BuildValue("i",val);
};
static PyObject* CFAttackTypeDrain(PyObject* self, PyObject* args)
{
  int val = 128;
  if (!PyArg_ParseTuple(args,"",NULL))
      return NULL;
  return Py_BuildValue("i",val);
};
static PyObject* CFAttackTypeWeaponmagic(PyObject* self, PyObject* args)
{
  int val = 256;
  if (!PyArg_ParseTuple(args,"",NULL))
      return NULL;
  return Py_BuildValue("i",val);
};
static PyObject* CFAttackTypeGhosthit(PyObject* self, PyObject* args)
{
  int val = 512;
  if (!PyArg_ParseTuple(args,"",NULL))
      return NULL;
  return Py_BuildValue("i",val);
};
static PyObject* CFAttackTypePoison(PyObject* self, PyObject* args)
{
  int val = 1024;
  if (!PyArg_ParseTuple(args,"",NULL))
      return NULL;
  return Py_BuildValue("i",val);
};
static PyObject* CFAttackTypeSlow(PyObject* self, PyObject* args)
{
  int val = 2048;
  if (!PyArg_ParseTuple(args,"",NULL))
      return NULL;
  return Py_BuildValue("i",val);
};
static PyObject* CFAttackTypeParalyze(PyObject* self, PyObject* args)
{
  int val = 4096;
  if (!PyArg_ParseTuple(args,"",NULL))
      return NULL;
  return Py_BuildValue("i",val);
};
static PyObject* CFAttackTypeTurnUndead(PyObject* self, PyObject* args)
{
  int val = 8192;
  if (!PyArg_ParseTuple(args,"",NULL))
      return NULL;
  return Py_BuildValue("i",val);
};
static PyObject* CFAttackTypeFear(PyObject* self, PyObject* args)
{
  int val = 16384;
  if (!PyArg_ParseTuple(args,"",NULL))
      return NULL;
  return Py_BuildValue("i",val);
};
static PyObject* CFAttackTypeCancellation(PyObject* self, PyObject* args)
{
  int val = 32768;
  if (!PyArg_ParseTuple(args,"",NULL))
      return NULL;
  return Py_BuildValue("i",val);
};
static PyObject* CFAttackTypeDepletion(PyObject* self, PyObject* args)
{
  int val = 65536;
  if (!PyArg_ParseTuple(args,"",NULL))
      return NULL;
  return Py_BuildValue("i",val);
};
static PyObject* CFAttackTypeDeath(PyObject* self, PyObject* args)
{
  int val = 131072;
  if (!PyArg_ParseTuple(args,"",NULL))
      return NULL;
  return Py_BuildValue("i",val);
};
static PyObject* CFAttackTypeChaos(PyObject* self, PyObject* args)
{
  int val = 262144;
  if (!PyArg_ParseTuple(args,"",NULL))
      return NULL;
  return Py_BuildValue("i",val);
};
static PyObject* CFAttackTypeCounterspell(PyObject* self, PyObject* args)
{
  int val = 524288;
  if (!PyArg_ParseTuple(args,"",NULL))
      return NULL;
  return Py_BuildValue("i",val);
};
static PyObject* CFAttackTypeGodpower(PyObject* self, PyObject* args)
{
  int val = 1048576;
  if (!PyArg_ParseTuple(args,"",NULL))
      return NULL;
  return Py_BuildValue("i",val);
};
static PyObject* CFAttackTypeHolyWord(PyObject* self, PyObject* args)
{
  int val = 2097152;
  if (!PyArg_ParseTuple(args,"",NULL))
      return NULL;
  return Py_BuildValue("i",val);
};
static PyObject* CFAttackTypeBlind(PyObject* self, PyObject* args)
{
  int val = 4194304;
  if (!PyArg_ParseTuple(args,"",NULL))
      return NULL;
  return Py_BuildValue("i",val);
};

/*****************************************************************************/
/* Wrappers for Spell numbers.                                               */
/*****************************************************************************/
static PyObject* CFSpellBullet(PyObject* self, PyObject* args)
{
    int val = SP_BULLET;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellSmallFireball(PyObject* self, PyObject* args)
{
    int val = SP_S_FIREBALL;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellMediumFireball(PyObject* self, PyObject* args)
{
    int val = SP_M_FIREBALL;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellLargeFireball(PyObject* self, PyObject* args)
{                                		/*0*/
    int val = SP_L_FIREBALL;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellBurningHands(PyObject* self, PyObject* args)
{
    int val = SP_BURNING_HANDS;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellSmallLightning(PyObject* self, PyObject* args)
{
    int val = SP_S_LIGHTNING;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellLargeLightning(PyObject* self, PyObject* args)
{
    int val = SP_L_LIGHTNING;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellMagicMissile(PyObject* self, PyObject* args)
{
    int val = SP_M_MISSILE;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellCreateBomb(PyObject* self, PyObject* args)
{
    int val = SP_BOMB;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellSummonGolem(PyObject* self, PyObject* args)
{
    int val = SP_GOLEM;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellSummonFireElemental(PyObject* self, PyObject* args)
{
    int val = SP_FIRE_ELEM;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellSummonEarthElemental(PyObject* self, PyObject* args)
{
    int val = SP_EARTH_ELEM;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellSummonWaterElemental(PyObject* self, PyObject* args)
{
    int val = SP_WATER_ELEM;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellSummonAirElemental(PyObject* self, PyObject* args)
{
    int val = SP_AIR_ELEM;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellDimensionDoor(PyObject* self, PyObject* args)
{
    int val = SP_D_DOOR;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellCreateEarthWall(PyObject* self, PyObject* args)
{
    int val = SP_EARTH_WALL;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellParalyze(PyObject* self, PyObject* args)
{
    int val = SP_PARALYZE;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellIcestorm(PyObject* self, PyObject* args)
{
    int val = SP_ICESTORM;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellMagicMapping(PyObject* self, PyObject* args)
{                        		/*15*/
    int val = SP_MAGIC_MAPPING;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellTurnUndead(PyObject* self, PyObject* args)
{
    int val = SP_TURN_UNDEAD;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellFear(PyObject* self, PyObject* args)
{
    int val = SP_FEAR;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellPoisonCloud(PyObject* self, PyObject* args)
{
    int val = SP_POISON_CLOUD;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellWOW(PyObject* self, PyObject* args)
{
    int val = SP_WOW;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellDestruction(PyObject* self, PyObject* args)
{
    int val = SP_DESTRUCTION;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellPerceiveSelf(PyObject* self, PyObject* args)
{
    int val = SP_PERCEIVE;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellWOR(PyObject* self, PyObject* args)
{
    int val = SP_WOR;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellInvisibility(PyObject* self, PyObject* args)
{
    int val = SP_INVIS;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellInvisibleToUndead(PyObject* self, PyObject* args)
{
    int val = SP_INVIS_UNDEAD;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellProbe(PyObject* self, PyObject* args)
{
    int val = SP_PROBE;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellLargeBullet(PyObject* self, PyObject* args)
{
    int val = SP_LARGE_BULLET;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellImprovedInvisibility(PyObject* self, PyObject* args)
{
    int val = SP_IMPROVED_INVIS;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellHolyWord(PyObject* self, PyObject* args)
{
    int val = SP_HOLY_WORD;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellMinorHealing(PyObject* self, PyObject* args)
{
    int val = SP_MINOR_HEAL;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellMediumHealing(PyObject* self, PyObject* args)
{
    int val = SP_MED_HEAL;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellMajorHealing(PyObject* self, PyObject* args)
{
    int val = SP_MAJOR_HEAL;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellHeal(PyObject* self, PyObject* args)
{
    int val = SP_HEAL;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellCreateFood(PyObject* self, PyObject* args)
{
    int val = SP_CREATE_FOOD;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellEarthToDust(PyObject* self, PyObject* args)
{
    int val = SP_EARTH_DUST;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellArmour(PyObject* self, PyObject* args)
{
    int val = SP_ARMOUR;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellStrength(PyObject* self, PyObject* args)
{
    int val = SP_STRENGTH;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellDexterity(PyObject* self, PyObject* args)
{
    int val = SP_DEXTERITY;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellConstitution(PyObject* self, PyObject* args)
{
    int val = SP_CONSTITUTION;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellCharisma(PyObject* self, PyObject* args)
{
    int val = SP_CHARISMA;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellFireWall(PyObject* self, PyObject* args)
{
    int val = SP_FIRE_WALL;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellFrostWall(PyObject* self, PyObject* args)
{
    int val = SP_FROST_WALL;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellProtectionCold(PyObject* self, PyObject* args)
{
    int val = SP_PROT_COLD;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellProtectionElectricity(PyObject* self, PyObject* args)
{
    int val = SP_PROT_ELEC;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellProtectionFire(PyObject* self, PyObject* args)
{
    int val = SP_PROT_FIRE;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellProtectionPoison(PyObject* self, PyObject* args)
{
    int val = SP_PROT_POISON;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellProtectionSlow(PyObject* self, PyObject* args)
{
    int val = SP_PROT_SLOW;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellProtectionParalyze(PyObject* self, PyObject* args)
{
    int val = SP_PROT_PARALYZE;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellProtectionDrain(PyObject* self, PyObject* args)
{
    int val = SP_PROT_DRAIN;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellProtectionMagic(PyObject* self, PyObject* args)
{
    int val = SP_PROT_MAGIC;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellProtectionAttack(PyObject* self, PyObject* args)
{
    int val = SP_PROT_ATTACK;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellLevitate(PyObject* self, PyObject* args)
{
    int val = SP_LEVITATE;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellSmallSpeedball(PyObject* self, PyObject* args)
{
    int val = SP_SMALL_SPEEDBALL;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellLargeSpeedball(PyObject* self, PyObject* args)
{
    int val = SP_LARGE_SPEEDBALL;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellHellfire(PyObject* self, PyObject* args)
{
    int val = SP_HELLFIRE;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellFirebreath(PyObject* self, PyObject* args)
{
    int val = SP_FIREBREATH;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellLargeIcestorm(PyObject* self, PyObject* args)
{
    int val = SP_LARGE_ICESTORM;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellCharging(PyObject* self, PyObject* args)
{
    int val = SP_CHARGING;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellPolymorph(PyObject* self, PyObject* args)
{
    int val = SP_POLYMORPH;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellCancellation(PyObject* self, PyObject* args)
{
    int val = SP_CANCELLATION;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellConfusion(PyObject* self, PyObject* args)
{
    int val = SP_CONFUSION;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellMassConfusion(PyObject* self, PyObject* args)
{
    int val = SP_MASS_CONFUSION;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellSummonPetMonster(PyObject* self, PyObject* args)
{
    int val = SP_PET;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellSlow(PyObject* self, PyObject* args)
{
    int val = SP_SLOW;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellRegenerateSpellpoints(PyObject* self, PyObject* args)
{
    int val = SP_REGENERATE_SPELLPOINTS;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellCurePoison(PyObject* self, PyObject* args)
{
    int val = SP_CURE_POISON;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellProtectionConfusion(PyObject* self, PyObject* args)
{
    int val = SP_PROT_CONFUSE;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellProtectionCancellation(PyObject* self, PyObject* args)
{
    int val = SP_PROT_CANCEL;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellProtectionDepletion(PyObject* self, PyObject* args)
{
    int val = SP_PROT_DEPLETE;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellAlchemy(PyObject* self, PyObject* args)
{
    int val = SP_ALCHEMY;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellRemoveCurse(PyObject* self, PyObject* args)
{
    int val = SP_REMOVE_CURSE;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellRemoveDamnation(PyObject* self, PyObject* args)
{
    int val = SP_REMOVE_DAMNATION;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellIdentify(PyObject* self, PyObject* args)
{
    int val = SP_IDENTIFY;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellDetectMagic(PyObject* self, PyObject* args)
{
    int val = SP_DETECT_MAGIC;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellDetectMonster(PyObject* self, PyObject* args)
{
    int val = SP_DETECT_MONSTER;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellDetectEvil(PyObject* self, PyObject* args)
{
    int val = SP_DETECT_EVIL;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellDetectCurse(PyObject* self, PyObject* args)
{
    int val = SP_DETECT_CURSE;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellHeroism(PyObject* self, PyObject* args)
{
    int val = SP_HEROISM;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellAggravation(PyObject* self, PyObject* args)
{
    int val = SP_AGGRAVATION;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellFirebolt(PyObject* self, PyObject* args)
{
    int val = SP_FIREBOLT;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellFrostbolt(PyObject* self, PyObject* args)
{
    int val = SP_FROSTBOLT;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellShockwave(PyObject* self, PyObject* args)
{
    int val = SP_SHOCKWAVE;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellColorSpray(PyObject* self, PyObject* args)
{
    int val = SP_COLOR_SPRAY;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellHaste(PyObject* self, PyObject* args)
{
    int val = SP_HASTE;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellFaceOfDeath(PyObject* self, PyObject* args)
{
    int val = SP_FACE_OF_DEATH;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellBallLightning(PyObject* self, PyObject* args)
{
    int val = SP_BALL_LIGHTNING;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellMeteorSwarm(PyObject* self, PyObject* args)
{
    int val = SP_METEOR_SWARM;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellMeteor(PyObject* self, PyObject* args)
{
    int val = SP_METEOR;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellMysticFist(PyObject* self, PyObject* args)
{
    int val = SP_MYSTIC_FIST;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellRaiseDead(PyObject* self, PyObject* args)
{
    int val = SP_RAISE_DEAD;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellResurrection(PyObject* self, PyObject* args)
{
    int val = SP_RESURRECTION;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellReincarnation(PyObject* self, PyObject* args)
{
    int val = SP_REINCARNATION;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellImmuneCold(PyObject* self, PyObject* args)
{
    int val = SP_IMMUNE_COLD;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellImmuneElectricity(PyObject* self, PyObject* args)
{
    int val = SP_IMMUNE_ELEC;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellImmuneFire(PyObject* self, PyObject* args)
{
    int val = SP_IMMUNE_FIRE;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellImmunePoison(PyObject* self, PyObject* args)
{
    int val = SP_IMMUNE_POISON;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellImmuneSlow(PyObject* self, PyObject* args)
{
    int val = SP_IMMUNE_SLOW;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellImmuneParalyze(PyObject* self, PyObject* args)
{
    int val = SP_IMMUNE_PARALYZE;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellImmuneDrain(PyObject* self, PyObject* args)
{
    int val = SP_IMMUNE_DRAIN;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellImmuneMagic(PyObject* self, PyObject* args)
{
    int val = SP_IMMUNE_MAGIC;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellImmuneAttack(PyObject* self, PyObject* args)
{
    int val = SP_IMMUNE_ATTACK;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellInvulnerability(PyObject* self, PyObject* args)
{
    int val = SP_INVULNERABILITY;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellProtection(PyObject* self, PyObject* args)
{
    int val = SP_PROTECTION;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellRuneFire(PyObject* self, PyObject* args)
{
    int val = SP_RUNE_FIRE;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellRuneFrost(PyObject* self, PyObject* args)
{
    int val = SP_RUNE_FROST;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellRuneShock(PyObject* self, PyObject* args)
{
    int val = SP_RUNE_SHOCK;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellRuneBlast(PyObject* self, PyObject* args)
{
    int val = SP_RUNE_BLAST;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellRuneDeath(PyObject* self, PyObject* args)
{

    int val = SP_RUNE_DEATH;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellRuneMark(PyObject* self, PyObject* args)
{
    int val = SP_RUNE_MARK;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellBuildDirector(PyObject* self, PyObject* args)
{
    int val = SP_BUILD_DIRECTOR;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellChaosPool(PyObject* self, PyObject* args)
{
    int val = SP_CHAOS_POOL;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellBuildBulletWall(PyObject* self, PyObject* args)
{
    int val = SP_BUILD_BWALL;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellBuildLightningWall(PyObject* self, PyObject* args)
{
    int val = SP_BUILD_LWALL;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellBuildFireballWall(PyObject* self, PyObject* args)
{
    int val = SP_BUILD_FWALL;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellRuneMagic(PyObject* self, PyObject* args)
{
    int val = SP_RUNE_MAGIC;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellRuneDrainSP(PyObject* self, PyObject* args)
{
    int val = SP_RUNE_DRAINSP;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellRuneAntimagic(PyObject* self, PyObject* args)
{
    int val = SP_RUNE_ANTIMAGIC;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellRuneTransferrence(PyObject* self, PyObject* args)
{
    int val = SP_RUNE_TRANSFER;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellTransfer(PyObject* self, PyObject* args)
{
    int val = SP_TRANSFER;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellMagicDrain(PyObject* self, PyObject* args)
{
    int val = SP_MAGIC_DRAIN;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellCounterspell(PyObject* self, PyObject* args)
{
    int val = SP_COUNTER_SPELL;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellDispelRune(PyObject* self, PyObject* args)
{
    int val = SP_DISPEL_RUNE;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellCureConfusion(PyObject* self, PyObject* args)
{
    int val = SP_CURE_CONFUSION;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellRestoration(PyObject* self, PyObject* args)
{
    int val = SP_RESTORATION;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellSummonEvilMonster(PyObject* self, PyObject* args)
{
    int val = SP_SUMMON_EVIL_MONST;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellCounterwall(PyObject* self, PyObject* args)
{
    int val = SP_COUNTERWALL;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellCauseLightWounds(PyObject* self, PyObject* args)
{
    int val = SP_CAUSE_LIGHT;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellCauseMediumWounds(PyObject* self, PyObject* args)
{
    int val = SP_CAUSE_MEDIUM;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellCauseHeavyWounds(PyObject* self, PyObject* args)
{
    int val = SP_CAUSE_HEAVY;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellCharm(PyObject* self, PyObject* args)
{
    int val = SP_CHARM;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellBanishment(PyObject* self, PyObject* args)
{
    int val = SP_BANISHMENT;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellCreateMissile(PyObject* self, PyObject* args)
{
    int val = SP_CREATE_MISSILE;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellShowInvisible(PyObject* self, PyObject* args)
{
    int val = SP_SHOW_INVIS;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellXRay(PyObject* self, PyObject* args)
{
    int val = SP_XRAY;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellPacify(PyObject* self, PyObject* args)
{
    int val = SP_PACIFY;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellSummonFog(PyObject* self, PyObject* args)
{
    int val = SP_SUMMON_FOG;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellSteambolt(PyObject* self, PyObject* args)
{
    int val = SP_STEAMBOLT;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellCommandUndead(PyObject* self, PyObject* args)
{
    int val = SP_COMMAND_UNDEAD;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellHolyOrb(PyObject* self, PyObject* args)
{
    int val = SP_HOLY_ORB;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellSummonAvatar(PyObject* self, PyObject* args)
{
    int val = SP_SUMMON_AVATAR;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellHolyPossession(PyObject* self, PyObject* args)
{
    int val = SP_HOLY_POSSESSION;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellBless(PyObject* self, PyObject* args)
{
    int val = SP_BLESS;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellCurse(PyObject* self, PyObject* args)
{
    int val = SP_CURSE;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellRegeneration(PyObject* self, PyObject* args)
{
    int val = SP_REGENERATION;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellConsecrate(PyObject* self, PyObject* args)
{
    int val = SP_CONSECRATE;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellSummonCultMonsters(PyObject* self, PyObject* args)
{
    int val = SP_SUMMON_CULT;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellCauseCriticalWounds(PyObject* self, PyObject* args)
{
    int val = SP_CAUSE_CRITICAL;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellHolyWrath(PyObject* self, PyObject* args)
{
    int val = SP_HOLY_WRATH;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellRetribution(PyObject* self, PyObject* args)
{
    int val = SP_RETRIBUTION;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellFingerDeath(PyObject* self, PyObject* args)
{
    int val = SP_FINGER_DEATH;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellInsectPlague(PyObject* self, PyObject* args)
{
    int val = SP_INSECT_PLAGUE;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellHolyServant(PyObject* self, PyObject* args)
{
    int val = SP_HOLY_SERVANT;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellWallOfThorns(PyObject* self, PyObject* args)
{
    int val = SP_WALL_OF_THORNS;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellStaffToSnake(PyObject* self, PyObject* args)
{
    int val = SP_STAFF_TO_SNAKE;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellLight(PyObject* self, PyObject* args)
{
    int val = SP_LIGHT;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellDarkness(PyObject* self, PyObject* args)
{
    int val = SP_DARKNESS;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellNightfall(PyObject* self, PyObject* args)
{
    int val = SP_NIGHTFALL;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellDaylight(PyObject* self, PyObject* args)
{
    int val = SP_DAYLIGHT;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellSunSpear(PyObject* self, PyObject* args)
{
    int val = SP_SUNSPEAR;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellFaeryFire(PyObject* self, PyObject* args)
{
    int val = SP_FAERY_FIRE;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellCureBlindness(PyObject* self, PyObject* args)
{
    int val = SP_CURE_BLINDNESS;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellDarkVision(PyObject* self, PyObject* args)
{
    int val = SP_DARK_VISION;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellBulletSwarm(PyObject* self, PyObject* args)
{
    int val = SP_BULLET_SWARM;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellBulletStorm(PyObject* self, PyObject* args)
{
    int val = SP_BULLET_STORM;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellCauseManyWounds(PyObject* self, PyObject* args)
{
    int val = SP_CAUSE_MANY;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellSmallSnowstorm(PyObject* self, PyObject* args)
{
    int val = SP_S_SNOWSTORM;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellMediumSnowstorm(PyObject* self, PyObject* args)
{
    int val = SP_M_SNOWSTORM;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellLargeSnowstorm(PyObject* self, PyObject* args)
{
    int val = SP_L_SNOWSTORM;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellCureDisease(PyObject* self, PyObject* args)
{
    int val = SP_CURE_DISEASE;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellCauseEbola(PyObject* self, PyObject* args)
{
    int val = SP_CAUSE_EBOLA;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellCauseFlu(PyObject* self, PyObject* args)
{
    int val = SP_CAUSE_FLU;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellCausePlague(PyObject* self, PyObject* args)
{
    int val = SP_CAUSE_PLAGUE;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellCauseLeprosy(PyObject* self, PyObject* args)
{
    int val = SP_CAUSE_LEPROSY;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellCauseSmallPox(PyObject* self, PyObject* args)
{
    int val = SP_CAUSE_SMALLPOX;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellCausePneumonicPlague(PyObject* self, PyObject* args)
{
    int val = SP_CAUSE_PNEUMONIC_PLAGUE;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellCauseAnthrax(PyObject* self, PyObject* args)
{
    int val = SP_CAUSE_ANTHRAX;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellCauseTyphoid(PyObject* self, PyObject* args)
{
    int val = SP_CAUSE_TYPHOID;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellManaBlast(PyObject* self, PyObject* args)
{
    int val = SP_MANA_BLAST;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellSmallManaball(PyObject* self, PyObject* args)
{
    int val = SP_S_MANABALL;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellMediumManaball(PyObject* self, PyObject* args)
{
    int val = SP_M_MANABALL;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellLargeManaball(PyObject* self, PyObject* args)
{
    int val = SP_L_MANABALL;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellManabolt(PyObject* self, PyObject* args)
{
    int val = SP_MANA_BOLT;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellDancingSword(PyObject* self, PyObject* args)
{
    int val = SP_DANCING_SWORD;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellAnimateWeapon(PyObject* self, PyObject* args)
{
    int val = SP_ANIMATE_WEAPON;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellCauseCold(PyObject* self, PyObject* args)
{
    int val = SP_CAUSE_COLD;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellDivineShock(PyObject* self, PyObject* args)
{
    int val = SP_DIVINE_SHOCK;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellWindStorm(PyObject* self, PyObject* args)
{
    int val = SP_WINDSTORM;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellSanctuary(PyObject* self, PyObject* args)
{
    int val = SP_SANCTUARY;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellPeace(PyObject* self, PyObject* args)
{
    int val = SP_PEACE;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellSpiderWeb(PyObject* self, PyObject* args)
{
    int val = SP_SPIDERWEB;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellConflict(PyObject* self, PyObject* args)
{
    int val = SP_CONFLICT;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellRage(PyObject* self, PyObject* args)
{
    int val = SP_RAGE;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellForkedLightning(PyObject* self, PyObject* args)
{
    int val = SP_FORKED_LIGHTNING;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellPoisonFog(PyObject* self, PyObject* args)
{
    int val = SP_POISON_FOG;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellFlameAura(PyObject* self, PyObject* args)
{
    int val = SP_FLAME_AURA;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellVitriol(PyObject* self, PyObject* args)
{
    int val = SP_VITRIOL;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellVitriolSplash(PyObject* self, PyObject* args)
{
    int val = SP_VITRIOL_SPLASH;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellIronwoodSkin(PyObject* self, PyObject* args)
{
    int val = SP_IRONWOOD_SKIN;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFSpellWrathfullEye(PyObject* self, PyObject* args)
{
    int val = SP_WRATHFUL_EYE;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

/*****************************************************************************/
/* Wrappers for Skill Numbers                                                */
/*****************************************************************************/
static PyObject* CFSkillStealing(PyObject* self, PyObject* args)
{
    int val = SK_STEALING;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};
static PyObject* CFSkillLockpicking(PyObject* self, PyObject* args)
{
    int val = SK_LOCKPICKING;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};
static PyObject* CFSkillHiding(PyObject* self, PyObject* args)
{
    int val = SK_HIDING;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};
static PyObject* CFSkillSmithery(PyObject* self, PyObject* args)
{
    int val = SK_SMITH;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};
static PyObject* CFSkillBowyer(PyObject* self, PyObject* args)
{
    int val = SK_BOWYER;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};
static PyObject* CFSkillJeweler(PyObject* self, PyObject* args)
{
    int val = SK_JEWELER;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};
static PyObject* CFSkillAlchemy(PyObject* self, PyObject* args)
{
    int val = SK_ALCHEMY;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};
static PyObject* CFSkillThaumaturgy(PyObject* self, PyObject* args)
{
    int val = SK_THAUMATURGY;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};
static PyObject* CFSkillLiteracy(PyObject* self, PyObject* args)
{
    int val = SK_LITERACY;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};
static PyObject* CFSkillBargaining(PyObject* self, PyObject* args)
{
    int val = SK_BARGAINING;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};
static PyObject* CFSkillJumping(PyObject* self, PyObject* args)
{
    int val = SK_JUMPING;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};
static PyObject* CFSkillSenseMagic(PyObject* self, PyObject* args)
{
    int val = SK_DET_MAGIC;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};
static PyObject* CFSkillOratory(PyObject* self, PyObject* args)
{
    int val = SK_ORATORY;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};
static PyObject* CFSkillSinging(PyObject* self, PyObject* args)
{
    int val = SK_MUSIC;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};
static PyObject* CFSkillDetectCurse(PyObject* self, PyObject* args)
{
    int val = SK_DET_CURSE;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};
static PyObject* CFSkillFindTraps(PyObject* self, PyObject* args)
{
    int val = SK_FIND_TRAPS;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};
static PyObject* CFSkillMeditation(PyObject* self, PyObject* args)
{
    int val = SK_MEDITATION;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};
static PyObject* CFSkillBoxing(PyObject* self, PyObject* args)
{
    int val = SK_BOXING;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};
static PyObject* CFSkillFlameTouch(PyObject* self, PyObject* args)
{
    int val = SK_FLAME_TOUCH;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};
static PyObject* CFSkillKarate(PyObject* self, PyObject* args)
{
    int val = SK_KARATE;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};
static PyObject* CFSkillClimbing(PyObject* self, PyObject* args)
{
    int val = SK_CLIMBING;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};
static PyObject* CFSkillWoodsman(PyObject* self, PyObject* args)
{
    int val = SK_WOODSMAN;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};
static PyObject* CFSkillInscription(PyObject* self, PyObject* args)
{
    int val = SK_INSCRIPTION;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};
static PyObject* CFSkillMeleeWeapons(PyObject* self, PyObject* args)
{
    int val = SK_MELEE_WEAPON;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};
static PyObject* CFSkillThrowing(PyObject* self, PyObject* args)
{
    int val = SK_THROWING;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};
static PyObject* CFSkillSpellCasting(PyObject* self, PyObject* args)
{
    int val = SK_SPELL_CASTING;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};
static PyObject* CFSkillRemoveTraps(PyObject* self, PyObject* args)
{
    int val = SK_REMOVE_TRAP;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};
static PyObject* CFSkillSetTraps(PyObject* self, PyObject* args)
{
    int val = SK_SET_TRAP;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};
static PyObject* CFSkillUseMagicItem(PyObject* self, PyObject* args)
{
    int val = SK_USE_MAGIC_ITEM;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};
static PyObject* CFSkillPraying(PyObject* self, PyObject* args)
{
    int val = SK_PRAYING;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};
static PyObject* CFSkillClawing(PyObject* self, PyObject* args)
{
    int val = SK_CLAWING;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};
static PyObject* CFSkillLevitation(PyObject* self, PyObject* args)
{
    int val = SK_LEVITATION;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

/*****************************************************************************/
/* Stalingrad: XML Support Subsection starts here                            */
/*****************************************************************************/

/*****************************************************************************/
/* Name   : CFLoadXMLObject                                                  */
/* Python : LoadXMLObject(filename)                                          */
/* Status : Untested                                                         */
/*****************************************************************************/
/* Loads a crossfire XML-file into an object, including subobjects (if any). */
/* Note that I may have broken some XML rules (I hope I didn't, but...).     */
/*****************************************************************************/

/*****************************************************************************/
/* Name   : CFSaveXMLObject                                                  */
/* Python : SaveXMLObject(filename, object)                                  */
/* Status : Untested                                                         */
/*****************************************************************************/
/* Saves a crossfire object (subobjects included) into a file, using an XML  */
/* format. (At least I think it is mostly XML-compliant :)                   */
/*****************************************************************************/

/*****************************************************************************/
/* Stalingrad: XML Support Subsection ends here                              */
/*****************************************************************************/

/*****************************************************************************/
/* Name   : CFGetMapWidth                                                    */
/* Python : CFPython.GetMapWidth(map)                                        */
/* Status : Stable                                                           */
/*****************************************************************************/
static PyObject* CFGetMapWidth(PyObject* self, PyObject* args)
{
    int val;
    long map;
    if (!PyArg_ParseTuple(args,"l",&map))
        return NULL;
    val = ((mapstruct *)(map))->width;
    return Py_BuildValue("i",val);
};

/*****************************************************************************/
/* Name   : CFGetMapHeight                                                   */
/* Python : CFPython.GetMapHeight(map)                                       */
/* Status : Stable                                                           */
/*****************************************************************************/
static PyObject* CFGetMapHeight(PyObject* self, PyObject* args)
{
    int val;
    long map;
    if (!PyArg_ParseTuple(args,"l",&map))
        return NULL;
    val = ((mapstruct *)(map))->height;
    return Py_BuildValue("i",val);
};

/*****************************************************************************/
/* Name   : CFGetObjectAt                                                    */
/* Python : CFPython.GetObjectAt(map,x,y)                                    */
/* Status : Stable                                                           */
/*****************************************************************************/
static PyObject* CFGetObjectAt(PyObject* self, PyObject* args)
{
    int x, y;
    long map;
    long whoptr;

    if (!PyArg_ParseTuple(args,"lii",&map,&x,&y))
        return NULL;
    whoptr = (long)(get_map_ob((mapstruct *)(map),x,y));
    return Py_BuildValue("l",whoptr);
};

/*****************************************************************************/
/* Name   : CFSetValue                                                       */
/* Python : CFPython.SetValue(object,value)                                  */
/* Status : Stable                                                           */
/*****************************************************************************/

static PyObject* CFSetValue(PyObject* self, PyObject* args)
{
    long whoptr;
    int newvalue;

    if (!PyArg_ParseTuple(args,"li",&whoptr,&newvalue))
        return NULL;

    WHO->value = newvalue;
    Py_INCREF(Py_None);
    return Py_None;
};

/*****************************************************************************/
/* Name   : CFGetValue                                                       */
/* Python : CFPython.GetValue(object)                                        */
/* Status : Stable                                                           */
/*****************************************************************************/

static PyObject* CFGetValue(PyObject* self, PyObject* args)
{
    long whoptr;

    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;

    return Py_BuildValue("i",WHO->value);
};

/*****************************************************************************/
/* Name   : CFSetSkillExperience                                             */
/* Python : CFPython.SetSkillExperience(object,skillid,value)                */
/* Status : Stable                                                           */
/*****************************************************************************/

static PyObject* CFSetSkillExperience(PyObject* self, PyObject* args)
{
    object *tmp;
    object *oldchosen;

    long whoptr;

    int skill;
    long value;
    int currentxp;

    if (!PyArg_ParseTuple(args,"lil",&whoptr,&skill,&value))
        return NULL;

    /* Browse the inventory of object to find a matching skill. */
    for (tmp=WHO->inv;tmp;tmp=tmp->below)
    {
        if(tmp->type!=SKILL) continue;
        if(tmp->stats.sp!=skill) continue;

        if (tmp->exp_obj)
        {
            oldchosen = WHO->chosen_skill;
            WHO->chosen_skill = tmp;
            currentxp = tmp->exp_obj->stats.exp;
            /* Don't know how this will react if negative value
            * passed to add_exp */
            /*add_exp(WHO, value-currentxp);*/
            GCFP.Value[0] = (void *)(WHO);
            value = value - currentxp;
            GCFP.Value[1] = (void *)(&value);
            (PlugHooks[HOOK_ADDEXP])(&GCFP);
            WHO->chosen_skill = oldchosen;
            Py_INCREF(Py_None);
            return Py_None;
        };
    };
    return NULL;
};

/*****************************************************************************/
/* Name   : CFGetSkillExperience                                             */
/* Python : CFPython.GetSkillExperience(object, skill)                       */
/* Status : Stable                                                           */
/*****************************************************************************/

static PyObject* CFGetSkillExperience(PyObject* self, PyObject* args)
{
    object *tmp;
    int skill;
    long whoptr;

    if (!PyArg_ParseTuple(args,"li",&whoptr,&skill))
        return NULL;

    /* Browse the inventory of object to find a matching skill. */
    for (tmp=WHO->inv;tmp;tmp=tmp->below)
    {
        if(tmp->type!=SKILL) continue;
        if(tmp->stats.sp!=skill) continue;
        if (tmp->exp_obj)
        {
            return Py_BuildValue("l",(long)(tmp->exp_obj->stats.exp));
        };
    };
    return NULL;
};

/*****************************************************************************/
/* Name   : CFMatchString                                                    */
/* Python : CFPython.MatchString(firststr,secondstr)                         */
/* Status : Stable                                                           */
/*****************************************************************************/

static PyObject* CFMatchString(PyObject* self, PyObject* args)
{
    char *premiere;
    char *seconde;
    char *result;

    if (!PyArg_ParseTuple(args,"ss",&premiere,&seconde))
        return NULL;

    result = re_cmp(premiere, seconde);
    if (result != NULL)
    {
        return Py_BuildValue("i",1);
    } else
    {
        return Py_BuildValue("i",0);
    };
};

/*****************************************************************************/
/* Name   : CFSetCursed                                                      */
/* Python : CFPython.SetCursed(object,value)                                 */
/* Status : Stable                                                           */
/*****************************************************************************/

static PyObject* CFSetCursed(PyObject* self, PyObject* args)
{
    long whoptr;
    int value;

    if (!PyArg_ParseTuple(args,"li",&whoptr,&value))
        return NULL;

  if (value!=0)
  {
        SET_FLAG(WHO, FLAG_CURSED);
  }
  else
  {
        CLEAR_FLAG(WHO, FLAG_CURSED);
  };
  Py_INCREF(Py_None);
  return Py_None;
};

/*****************************************************************************/
/* Name   : CFActivateRune                                                   */
/* Python : CFPython.ActivateRune(object,objectwhat)                         */
/* Status : Untested                                                         */
/*****************************************************************************/

static PyObject* CFActivateRune(PyObject* self, PyObject* args)
{
    long whoptr;
    long whatptr;

    if (!PyArg_ParseTuple(args,"ll",&whoptr,&whatptr))
        return NULL;

    GCFP.Value[0] = (void *)(WHAT);
    GCFP.Value[1] = (void *)(WHO);
    (PlugHooks[HOOK_SPRINGTRAP])(&GCFP);
    /*spring_trap(WHAT,WHO); */

    Py_INCREF(Py_None);
    return Py_None;
};

/*****************************************************************************/
/* Name   : CFCheckTrigger                                                   */
/* Python : CFPython.CheckTrigger(object,objectwhat)                         */
/* Status : Untested                                                         */
/*****************************************************************************/

static PyObject* CFCheckTrigger(PyObject* self, PyObject* args)
{
    long whoptr;
    long whatptr;

    if (!PyArg_ParseTuple(args,"ll",&whoptr,&whatptr))
        return NULL;

    check_trigger(WHAT,WHO);

    Py_INCREF(Py_None);
    return Py_None;
};

/*****************************************************************************/
/* Name   : CFSetUnaggressive                                                */
/* Python : CFPython.SetUnaggressive(who,value)                              */
/* Status : Stable                                                           */
/*****************************************************************************/

static PyObject* CFSetUnaggressive(PyObject* self, PyObject* args)
{
    long whoptr;
    int value;

    if (!PyArg_ParseTuple(args,"li",&whoptr,&value))
        return NULL;

    if (value!=0)
    {
        SET_FLAG(WHO, FLAG_UNAGGRESSIVE);
    }
    else
    {
        CLEAR_FLAG(WHO, FLAG_UNAGGRESSIVE);
    };
    Py_INCREF(Py_None);
    return Py_None;
};

/*****************************************************************************/
/* Name   : CFCastAbility                                                    */
/* Python : CFPython.CastAbility(object,spell,direction,option)              */
/* Status : Stable                                                           */
/*****************************************************************************/

static PyObject* CFCastAbility(PyObject* self, PyObject* args)
{
    long whoptr;
    int spell;
    int dir;
    char* op;
    CFParm* CFR;
    int parm=1;
    int parm2 = spellNormal;
    int typeoffire = FIRE_DIRECTIONAL;

    if (!PyArg_ParseTuple(args,"liis",&whoptr,&spell,&dir,&op))
        return NULL;

    GCFP.Value[0] = (void *)(WHO);
    GCFP.Value[1] = (void *)(WHO);
    GCFP.Value[2] = (void *)(&dir);
    GCFP.Value[3] = (void *)(&spell);
    GCFP.Value[4] = (void *)(&parm);
    GCFP.Value[5] = (void *)(&parm2);
    GCFP.Value[6] = (void *)(op);
    GCFP.Value[7] = (void *)(&typeoffire);
    CFR = (PlugHooks[HOOK_CASTSPELL])(&GCFP);
    free(CFR);
    Py_INCREF(Py_None);
    return Py_None;
};

/*****************************************************************************/
/* Name   : CFGetMapPath                                                     */
/* Python : CFPython.GetMapPath(objectmap)                                   */
/* Status : Stable                                                           */
/*****************************************************************************/

static PyObject* CFGetMapPath(PyObject* self, PyObject* args)
{
    long where;
    if (!PyArg_ParseTuple(args,"l",&where))
        return NULL;

    return Py_BuildValue("s",((mapstruct *)(where))->path);
};

/*****************************************************************************/
/* Name   : CFGetMapObject                                                   */
/* Python : CFPython.GetMapObject()                                          */
/* Status : KIA                                                              */
/*****************************************************************************/
/* Remark : This function is deprecated and should not be used anymore.      */
/*****************************************************************************/

static PyObject* CFGetMapObject(PyObject* self, PyObject* args)
{
    return NULL; /* Deprecated */
};


/*****************************************************************************/
/* Name   : CFGetMessage                                                     */
/* Python : CFPython.GetMessage(object)                                      */
/* Status : Stable                                                           */
/*****************************************************************************/

static PyObject* CFGetMessage(PyObject* self, PyObject* args)
{
    /* Stalingrad: extended the buffer - added a boundary checking */
    /* (implementing this as a malloc'ed string problematic under some env.) */
    /* Now declared static to help preventing memory leaks */
    static char buf[4096];
    long whoptr;

    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;
    if (WHO->msg != NULL)
    {
        if (strlen(WHO->msg)>=4096)
        {
            printf("Warning ! Buffer overflow - The message will be truncated\n");
            strncpy(buf, WHO->msg, 4096);
            buf[4095]=0x0;
        }
        else
        {
            strncpy(buf, WHO->msg,strlen(WHO->msg));
            buf[strlen(WHO->msg)+1]=0x0;
        }
    }
    else
        buf[0] = 0x0;
    return Py_BuildValue("s",buf);
};

/*****************************************************************************/
/* Name   : CFSetMessage                                                     */
/* Python : CFPython.SetMessage(object,message)                              */
/* Status : Stable                                                           */
/*****************************************************************************/
static PyObject* CFSetMessage(PyObject* self, PyObject* args)
{
    char *txt;
    long whoptr;
    if (!PyArg_ParseTuple(args,"ls",&whoptr, &txt))
        return NULL;

    if (WHO->msg != NULL)
        free_string(WHO->msg);
    WHO->msg = add_string(txt);
    Py_INCREF(Py_None);
    return Py_None;
};

/*****************************************************************************/
/* Name   : CFGetGod                                                         */
/* Python : CFPython.GetGod(object)                                          */
/* Status : Stable                                                           */
/*****************************************************************************/

static PyObject* CFGetGod(PyObject* self, PyObject* args)
{
    long whoptr;
    CFParm* CFR;
    static char* value;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;

    GCFP.Value[0] = (void *)(WHO);
    CFR = (PlugHooks[HOOK_DETERMINEGOD])(&GCFP);
    value = (char *)(CFR->Value[0]);
    free(CFR);
    return Py_BuildValue("s",value);
};

/*****************************************************************************/
/* Name   : CFSetGod                                                         */
/* Python : CFPython.SetGod(object,godstr)                                   */
/* Status : Stable                                                           */
/*****************************************************************************/

static PyObject* CFSetGod(PyObject* self, PyObject* args)
{
    long whoptr;
    char* txt;
    char* prayname;
    object* tmp;
    CFParm* CFR0;
    CFParm* CFR;
    int value;

    if (!PyArg_ParseTuple(args,"ls",&whoptr,&txt))
        return NULL;

    prayname = add_string("praying");

    GCFP1.Value[0] = (void *)(WHO);
    GCFP1.Value[1] = (void *)(prayname);

    GCFP2.Value[0] = (void *)(WHO);
    GCFP0.Value[0] = (char *)(txt);
    CFR0 = (PlugHooks[HOOK_FINDGOD])(&GCFP0);
    tmp = (object *)(CFR0->Value[0]);
    free(CFR0);
    GCFP2.Value[1] = (void *)(tmp);

    CFR = (PlugHooks[HOOK_CMDRSKILL])(&GCFP1);
    value = *(int *)(CFR->Value[0]);
    if (value)
        (PlugHooks[HOOK_BECOMEFOLLOWER])(&GCFP2);
    free(CFR);
    free_string(prayname);
    Py_INCREF(Py_None);
    return Py_None;
};

/*****************************************************************************/
/* Name   : CFSetWeight                                                      */
/* Python : CFPython.SetWeight(object,value)                                 */
/* Status : Stable                                                           */
/*****************************************************************************/

static PyObject* CFSetWeight(PyObject* self, PyObject* args)
{
    long whoptr;
    long value;

    if (!PyArg_ParseTuple(args,"ll",&whoptr,&value))
        return NULL;

    /* I used an arbitrary bound of 32000 here */
    if (value > 32000)
    {
        printf( "SetWeight: Value must be lower than 32000\n");
        return NULL;
    }
    else if (value < 0)
    {
        printf( "(set-weight): Value must be greater than 0\n");
        return NULL;
    };
    WHO->weight = value;

    Py_INCREF(Py_None);
    return Py_None;
};

/*****************************************************************************/
/* Name   : CFReadyMap                                                       */
/* Python : CFPython.ReadyMap(name)                                          */
/* Status : Stable                                                           */
/*****************************************************************************/

static PyObject* CFReadyMap(PyObject* self, PyObject* args)
{
    char *mapname;
    mapstruct *mymap;
    int val;
    CFParm* CFR;
    if (!PyArg_ParseTuple(args,"s",&mapname))
        return NULL;

    val = 0;
    GCFP.Value[0] = (void *)(mapname);
    GCFP.Value[1] = (void *)(&val);

    printf( "Ready to call readymapname with %s %i\n",
        (char *)(GCFP.Value[0]),
        *(int *)(GCFP.Value[1])
    );
    /* mymap = ready_map_name(mapname,0); */
    CFR = (PlugHooks[HOOK_READYMAPNAME])(&GCFP);
    mymap = (mapstruct *)(CFR->Value[0]);
    printf( "Map file is %s\n",mymap->path);
    free(CFR);
    return Py_BuildValue("l",(long)(mymap));
};

/*****************************************************************************/
/* Name   : CFTeleport                                                       */
/* Python : CFPython.Teleport(object,mapptr,x,y)                             */
/* Status : Untested                                                         */
/*****************************************************************************/

static PyObject* CFTeleport(PyObject* self, PyObject* args)
{
    long whoptr;
    long where;
    int x, y;
    int val;

    if (!PyArg_ParseTuple(args,"llii",&whoptr,&where,&x,&y))
        return NULL;

    if ((out_of_map((mapstruct*)(where),x,y))==0)
    {
        int k;
        object *tmp;
        k = find_first_free_spot(WHO->arch,(mapstruct*)(where),x,y);
        if (k==-1) return NULL;
        GCFP.Value[0] = (void *)(WHO);
        (PlugHooks[HOOK_REMOVEOBJECT])(&GCFP);
/*        remove_ob(WHO); */
        for(tmp=WHO;tmp!=NULL;tmp=tmp->more)
            tmp->x=x+freearr_x[k]+(tmp->arch==NULL?0:tmp->arch->clone.x),
            tmp->y=y+freearr_y[k]+(tmp->arch==NULL?0:tmp->arch->clone.y);
        val = 0;
        GCFP.Value[0] = (void *)(WHO);
        GCFP.Value[1] = (void *)((mapstruct *)(where));
        GCFP.Value[2] = NULL;
        GCFP.Value[3] = (void *)(&val);
        /*insert_ob_in_map(WHO,(mapstruct *)(where),NULL,0); */
        free((PlugHooks[HOOK_INSERTOBJECTINMAP])(&GCFP));
    };

    Py_INCREF(Py_None);
    return Py_None;
};

/*****************************************************************************/
/* Name   : CFOutOfMap                                                       */
/* Python : CFPython.IsOutOfMap(object,x,y)                                  */
/* Status : Stable                                                           */
/*****************************************************************************/

static PyObject* CFIsOutOfMap(PyObject* self, PyObject* args)
{
    long whoptr;
    int x, y;

    if (!PyArg_ParseTuple(args,"lii",&whoptr,&x,&y))
        return NULL;

    return Py_BuildValue("i", out_of_map(WHO->map,x,y));
};

/*****************************************************************************/
/* Name   : CFPickUp                                                         */
/* Python : CFPython.Pickup(object,whatob)                                   */
/* Status : Untested                                                         */
/*****************************************************************************/

static PyObject* CFPickUp(PyObject* self, PyObject* args)
{
    long whoptr;
    long whatptr;

    if (!PyArg_ParseTuple(args,"ll",&whoptr,&whatptr))
        return NULL;

    GCFP.Value[0] = (void *)(WHO);
    GCFP.Value[1] = (void *)(WHAT);
    (PlugHooks[HOOK_PICKUP])(&GCFP);
    /*pick_up(WHO,WHAT); */
    Py_INCREF(Py_None);
    return Py_None;
};

/*****************************************************************************/
/* Name   : CFGetWeight                                                      */
/* Python : CFPython.GetWeight(object)                                       */
/* Status : Stable                                                           */
/*****************************************************************************/

static PyObject* CFGetWeight(PyObject* self, PyObject* args)
{
    long whoptr;

    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;
    printf( "GetWeight: requested target is %s\n", query_name(WHO));
    return Py_BuildValue("l",WHO->weight);
};


/*****************************************************************************/
/* Name   : CFIsCanBePicked                                                  */
/* Python : CFPython.CanBePicked(object)                                     */
/* Status : Stable                                                           */
/*****************************************************************************/

static PyObject* CFIsCanBePicked(PyObject* self, PyObject* args)
{
    long whoptr;

    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;

    return Py_BuildValue("i",QUERY_FLAG(WHO,FLAG_NO_PICK));
};

/*****************************************************************************/
/* Name   : CFGetMap                                                         */
/* Python : CFPython.GetMap(object)                                          */
/* Status : Stable                                                           */
/*****************************************************************************/

static PyObject* CFGetMap(PyObject* self, PyObject* args)
{
    long whoptr;

    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;

    return Py_BuildValue("l",(long)(WHO->map));
};

/*****************************************************************************/
/* Name   : CFSetNextObject                                                  */
/* Python : CFPython.SetNextObject(object,object)                            */
/* Status : Stable                                                           */
/*****************************************************************************/

static PyObject* CFSetNextObject(PyObject* self, PyObject* args)
{
    long whoptr;
    long whatptr;
    if (!PyArg_ParseTuple(args,"ll",&whoptr,&whatptr))
        return NULL;

    if (WHO==NULL) return NULL;

    WHO->below = WHAT;
    Py_INCREF(Py_None);
    return Py_None;
};

/*****************************************************************************/
/* Name   : CFSetPreviousObject                                              */
/* Python : CFPython.SetPreviousObject(object,object)                        */
/* Status : Stable                                                           */
/*****************************************************************************/

static PyObject* CFSetPreviousObject(PyObject* self, PyObject* args)
{
    long whoptr;
    long whatptr;
    if (!PyArg_ParseTuple(args,"ll",&whoptr,&whatptr))
        return NULL;

    if (WHO==NULL) return NULL;

    WHO->above = WHAT;
    Py_INCREF(Py_None);
    return Py_None;
};

/*****************************************************************************/
/* Name   : CFGetNextObject                                                  */
/* Python : CFPython.GetNextObject(object)                                   */
/* Status : Stable                                                           */
/*****************************************************************************/

static PyObject* CFGetNextObject(PyObject* self, PyObject* args)
{
    long whoptr;

    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;

    if (WHO==NULL) return NULL;

    return Py_BuildValue("l",(long)(WHO->below));
};

/*****************************************************************************/
/* Name   : CFGetPreviousObject                                              */
/* Python : CFPython.GetPreviousObject(object)                               */
/* Status : Stable                                                           */
/*****************************************************************************/

static PyObject* CFGetPreviousObject(PyObject* self, PyObject* args)
{
    long whoptr;

    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;

    if (WHO==NULL) return NULL;

    return Py_BuildValue("l",(long)(WHO->above));
};

/*****************************************************************************/
/* Name   : CFGetFirstObjectOnSquare                                         */
/* Python : CFPython.GetFirstObjectOnSquare(map,x,y)                         */
/* Status : Stable                                                           */
/*****************************************************************************/

static PyObject* CFGetFirstObjectOnSquare(PyObject* self, PyObject* args)
{
    long map;
    int x, y;
    object* val;
    CFParm* CFR;

    if (!PyArg_ParseTuple(args,"lii",&map,&x,&y))
        return NULL;

    GCFP.Value[0] = (mapstruct *)(map);
    GCFP.Value[1] = (void *)(&x);
    GCFP.Value[2] = (void *)(&y);
    CFR = (PlugHooks[HOOK_GETMAPOBJECT])(&GCFP);
    val = (object *)(CFR->Value[0]);
    printf( "First object is known by %s\n",query_name(val));
    free(CFR);
    return Py_BuildValue("l",(long)(val));
};

/*****************************************************************************/
/* Name   : CFSetQuantity                                                    */
/* Python : CFPython.SetQuantity(object,nrof)                                */
/* Status : Untested                                                         */
/*****************************************************************************/

static PyObject* CFSetQuantity(PyObject* self, PyObject* args)
{
    long whatptr;
    int value;

    if (!PyArg_ParseTuple(args,"li",&whatptr,&value))
        return NULL;

    /* I used an arbitrary bound of 100k here */
    if (value > 100000)
    {
        printf( "(set-quantity): Value must be lower than 100000\n");
        return NULL;
    }
    else if (value < 0)
    {
        printf( "(set-quantity): Value must be greater than 0\n");
        return NULL;
    };
    WHAT->nrof = value;
    Py_INCREF(Py_None);
    return Py_None;
};

/*****************************************************************************/
/* Name   : CFGetQuantity                                                    */
/* Python : CFPython.GetQuantity(object)                                     */
/* Status : Untested                                                         */
/*****************************************************************************/

static PyObject* CFGetQuantity(PyObject* self, PyObject* args)
{
    long whatptr;

    if (!PyArg_ParseTuple(args,"l",&whatptr))
        return NULL;

    return Py_BuildValue("l",WHAT->nrof);
};

/*****************************************************************************/
/* Name   : CFInsertObjectInside                                             */
/* Python : CFPython.InsertObjectInside(object whereobj)                     */
/* Status : Stable                                                           */
/*****************************************************************************/

static PyObject* CFInsertObjectInside(PyObject* self, PyObject* args)
{
    long whatptr;
    long whereptr;
    object *myob;

    if (!PyArg_ParseTuple(args,"ll",&whatptr,&whereptr))
        return NULL;

    myob = WHAT;
    if (!QUERY_FLAG(myob,FLAG_REMOVED))
    {
        GCFP.Value[0] = (void *)(myob);
        (PlugHooks[HOOK_REMOVEOBJECT])(&GCFP);
    }
    myob = insert_ob_in_ob(myob, WHERE);
    if (WHERE->type == PLAYER)
    {
        GCFP.Value[0] = (void *)(WHERE);
        GCFP.Value[1] = (void *)(myob);
        /*esrv_send_item(WHERE, myob); */
        (PlugHooks[HOOK_ESRVSENDITEM])(&GCFP);
    };
    Py_INCREF(Py_None);
    return Py_None;

};

/*****************************************************************************/
/* Name   : CFFindPlayer                                                     */
/* Python : CFPlayer.FindPlayer(name)                                        */
/* Status : Untested                                                         */
/*****************************************************************************/

static PyObject* CFFindPlayer(PyObject* self, PyObject* args)
{
    player *foundpl;
    object *foundob;
    CFParm *CFR;
    char* txt;

    if (!PyArg_ParseTuple(args,"s",&txt))
        return NULL;

    GCFP.Value[0] = (void *)(txt);
    CFR = (PlugHooks[HOOK_FINDPLAYER])(&GCFP);
    foundpl = (player *)(CFR->Value[0]);
    free(CFR);

    if (foundpl!=NULL)
        foundob = foundpl->ob;
    else
        foundob = NULL;
    return Py_BuildValue("l",(long)(foundob));
};

/*****************************************************************************/
/* Name   : CFApply                                                          */
/* Python : CFPython.Apply(object, whatobj, flags)                           */
/* Status : Untested                                                         */
/*****************************************************************************/

static PyObject* CFApply(PyObject* self, PyObject* args)
{
    long whoptr;
    long whatptr;
    int flags;
    CFParm* CFR;
    int retval;

    if (!PyArg_ParseTuple(args,"lli",&whoptr,&whatptr,&flags))
        return NULL;

    GCFP.Value[0] = (void *)(WHO);
    GCFP.Value[1] = (void *)(WHAT);
    GCFP.Value[2] = (void *)(&flags);
    CFR = (PlugHooks[HOOK_MANUALAPPLY])(&GCFP);
    retval = *(int *)(CFR->Value[0]);
    free(CFR);
    return Py_BuildValue("i",retval);
};

/*****************************************************************************/
/* Name   : CFDrop                                                           */
/* Python : CFPython.Drop(object, name)                                      */
/* Status : Untested                                                         */
/*****************************************************************************/

static PyObject* CFDrop(PyObject* self, PyObject* args)
{
    long whoptr;
    char* name;
    CFParm* CFR;

    if (!PyArg_ParseTuple(args,"ls",&whoptr,&name))
        return NULL;

    GCFP.Value[0] = (void *)(WHO);
    GCFP.Value[1] = (void *)(name);
    CFR = (PlugHooks[HOOK_CMDDROP])(&GCFP);
/*    command_drop(WHO,name); */
    free(CFR);
    Py_INCREF(Py_None);
    return Py_None;
};

/*****************************************************************************/
/* Name   : CFTake                                                           */
/* Python : CFPython.Take(object,name)                                       */
/* Status : Untested                                                         */
/*****************************************************************************/

static PyObject* CFTake(PyObject* self, PyObject* args)
{
    long whoptr;
    char* name;
    CFParm* CFR;

    if (!PyArg_ParseTuple(args,"ls",&whoptr,&name))
        return NULL;

    GCFP.Value[0] = (void *)(WHO);
    GCFP.Value[1] = (void *)(name);
    CFR = (PlugHooks[HOOK_CMDTAKE])(&GCFP);
    /* command_take(WHO,name); */
    free(CFR);
    Py_INCREF(Py_None);
    return Py_None;
};

/*****************************************************************************/
/* Name   : CFIsInvisible                                                    */
/* Python : CFPython.IsInvisible(object)                                     */
/* Status : Untested                                                         */
/*****************************************************************************/

static PyObject* CFIsInvisible(PyObject* self, PyObject* args)
{
    long whoptr;

    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;
    return Py_BuildValue("i",WHO->invisible);
};

/*****************************************************************************/
/* Name   : CFWhoAmI                                                         */
/* Python : CFPython.WhoAmI()                                                */
/* Status : Stable                                                           */
/*****************************************************************************/

static PyObject* CFWhoAmI(PyObject* self, PyObject* args)
{
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("l",(long)(StackWho[StackPosition]));
};

/*****************************************************************************/
/* Name   : CFWhoIsActivator                                                 */
/* Python : CFPython.WhoIsActivator()                                        */
/* Status : Stable                                                           */
/*****************************************************************************/

static PyObject* CFWhoIsActivator(PyObject* self, PyObject* args)
{
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("l",(long)(StackActivator[StackPosition]));
};

/*****************************************************************************/
/* Name   : CFWhatIsMessage                                                  */
/* Python : CFPython.WhatIsMessage()                                         */
/* Status : Stable                                                           */
/*****************************************************************************/

static PyObject* CFWhatIsMessage(PyObject* self, PyObject* args)
{
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("s",StackText[StackPosition]);
};

/*****************************************************************************/
/* Name   : CFSay                                                            */
/* Python : CFPython.Say(object,message)                                     */
/* Status : Stable                                                           */
/*****************************************************************************/

static PyObject* CFSay(PyObject* self, PyObject* args)
{
    object *who;
    long obptr;
    char *message;
    /* Stalingrad: Changed from static to dynamic buffer */
    char *buf;
    int val;

    if (!PyArg_ParseTuple(args,"ls",&obptr,&message))
        return NULL;

    who = (object *)(obptr);

    /* Stalingrad: static->dynamic buffer */
    buf = (char *)(malloc(sizeof(char)*(strlen(message)+strlen(query_name(who))+20)));
    sprintf(buf, "%s says: %s", query_name(who),message);
    val = NDI_NAVY|NDI_UNIQUE;

    GCFP.Value[0] = (void *)(&val);
    GCFP.Value[1] = (void *)(who->map);
    GCFP.Value[2] = (void *)(buf);

    (PlugHooks[HOOK_NEWINFOMAP])(&GCFP);

    /* Stalingrad: static->dynamic buffer */
    free(buf);
    Py_INCREF(Py_None);
    return Py_None;
};

/*****************************************************************************/
/* Name   : CFSetGender                                                      */
/* Python : CFPython.SetGender(object,gender_string)                         */
/* Status : Stable                                                           */
/*****************************************************************************/
static PyObject* CFSetGender(PyObject* self, PyObject* args)
{
    object *who, *walk;
    long obptr;
    char *gender;
    
    if (!PyArg_ParseTuple(args,"ls",&obptr,&gender))
        return NULL;

    who = (object *)(obptr);
    
    for(walk=who->inv;walk!=NULL;walk=walk->below)
    {
        if (!strcmp(walk->name,"GENDER_FORCE") && !strcmp(walk->arch->name,"gender_force"))
        {
            /* we find the gender of the player, now change it to new one */
            if(walk->title)
                DELETE_STRING(walk->title);
            walk->title = add_string(gender);

            who->contr->socket.ext_title_flag = 1; /* demand update to client */
            return Py_BuildValue("l",(long) (walk));
        }            
    }

    printf("Python Warning -> SetGender: Object %s has no gender_force!\n", query_name(who));
    return Py_None;
};

/*****************************************************************************/
/* Name   : CFSetRank                                                        */
/* Python : CFPython.SetRank(object,rank_string)                             */
/* Status : Stable                                                           */
/*****************************************************************************/
static PyObject* CFSetRank(PyObject* self, PyObject* args)
{
    object *who, *walk;
    long obptr;
    char *rank;
    
    if (!PyArg_ParseTuple(args,"ls",&obptr,&rank))
        return NULL;

    who = (object *)(obptr);
    
    for(walk=who->inv;walk!=NULL;walk=walk->below)
    {
        if (!strcmp(walk->name,"RANK_FORCE") && !strcmp(walk->arch->name,"rank_force"))
        {
            /* we find the rank of the player, now change it to new one */
            if(walk->title)
                DELETE_STRING(walk->title);
            if (strcmp(rank,"Mr"))
                walk->title = add_string(rank);
            
            who->contr->socket.ext_title_flag = 1; /* demand update to client */
            return Py_BuildValue("l",(long) (walk));
        }            
    }
    printf("Python Warning -> SetRank: Object %s has no rank_force!\n", query_name(who));
    return Py_None;
};

/*****************************************************************************/
/* Name   : CFSetAlignment                                                   */
/* Python : CFPython.SetAlignment(object,alignment_string)                   */
/* Status : Stable                                                           */
/*****************************************************************************/
static PyObject* CFSetAlignment(PyObject* self, PyObject* args)
{
    object *who, *walk;
    long obptr;
    char *align;
    
    if (!PyArg_ParseTuple(args,"ls",&obptr,&align))
        return NULL;

    who = (object *)(obptr);
    
    for(walk=who->inv;walk!=NULL;walk=walk->below)
    {
        if (!strcmp(walk->name,"ALIGNMENT_FORCE")  && !strcmp(walk->arch->name,"alignment_force"))
        {
            /* we find the rank of the player, now change it to new one */
            if(walk->title)
                DELETE_STRING(walk->title);

            if (!strcmp(align,"good"))
                walk->title = add_string("the Good");
            else if (!strcmp(align,"evil"))
                walk->title = add_string("the Evil");                    
            who->contr->socket.ext_title_flag = 1; /* demand update to client */
            return Py_BuildValue("l",(long) (walk));
        }            
    }
    printf("Python Warning -> SetAlignment: Object %s has no alignment_force!\n", query_name(who));
    return Py_None;
};

/*****************************************************************************/
/* Name   : CFSetGuildForce                                                  */
/* Python : CFPython.SetGuildForce(object,guild_string)                      */
/* Status : Stable                                                           */
/* Info   : Warning: This set only the title. The guild tag is in <slaying>  */
/*        : For test of a special guild, you must use GetGuild()             */
/*        : For settings inside a guild script, you can use this function    */
/*        : Because it returns the guild_force obj after setting the title   */
/*****************************************************************************/
static PyObject* CFSetGuildForce(PyObject* self, PyObject* args)
{
    object *who, *walk;
    long obptr;
    char *guild;
    
    if (!PyArg_ParseTuple(args,"ls",&obptr,&guild))
        return NULL;
    who = (object *)(obptr);
    
    for(walk=who->inv;walk!=NULL;walk=walk->below)
    {
        if (!strcmp(walk->name,"GUILD_FORCE") && !strcmp(walk->arch->name,"guild_force"))
        {
            /* we find the rank of the player, now change it to new one */
            if(walk->title)
                DELETE_STRING(walk->title);
            if (guild && strcmp(guild, ""))
                walk->title = add_string(guild);
            
            who->contr->socket.ext_title_flag = 1; /* demand update to client */
            return Py_BuildValue("l",(long) (walk));
        }            
    }
    printf("Python Warning -> SetGuild: Object %s has no guild_force!\n", query_name(who));
    return Py_None;
};

/*****************************************************************************/
/* Name   : CFGetGuildForce                                                  */
/* Python : CFPython.GetGuildForce(object,guild_string)                      */
/* Status : Stable                                                           */
/* Info   : This gets the guild_force from a inventory (should be player?)   */
/*****************************************************************************/
static PyObject* CFGetGuildForce(PyObject* self, PyObject* args)
{
    object *walk;
    long whoptr;
    
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;
    
    for(walk=WHO->inv;walk!=NULL;walk=walk->below)
    {
        if (!strcmp(walk->name,"GUILD_FORCE") && !strcmp(walk->arch->name,"guild_force"))
            return Py_BuildValue("l",(long) (walk));
    }
    printf("Python Warning -> GetGuild: Object %s has no guild_force!\n", query_name(WHO));
    return Py_None;
};


/*****************************************************************************/
/* Name   : CFSetInvisible                                                   */
/* Python : CFPython.SetInvisible(object,value)                              */
/* Status : Untested                                                         */
/*****************************************************************************/

static PyObject* CFSetInvisible(PyObject* self, PyObject* args)
{
    int value;
    long whoptr;

    if (!PyArg_ParseTuple(args,"li",&whoptr,&value))
        return NULL;

    WHO->invisible = value;
    Py_INCREF(Py_None);
    return Py_None;
};

/*****************************************************************************/
/* Name   : CFGetExperience                                                  */
/* Python : CFPython.GetExperience(object)                                   */
/* Status : Untested                                                         */
/*****************************************************************************/

static PyObject* CFGetExperience(PyObject* self, PyObject* args)
{
    long whoptr;

    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;
    return Py_BuildValue("l",WHO->stats.exp);
};

/*****************************************************************************/
/* Name   : CFGetSpeed                                                       */
/* Python : CFPython.GetSpeed(object)                                        */
/* Status : Untested                                                         */
/*****************************************************************************/

static PyObject* CFGetSpeed(PyObject* self, PyObject* args)
{
    long whoptr;

    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;

    return Py_BuildValue("d",WHO->speed);
};

/*****************************************************************************/
/* Name   : CFSetSpeed                                                       */
/* Python : CFPython.SetSpeed(object,value)                                  */
/* Status : Untested                                                         */
/*****************************************************************************/

static PyObject* CFSetSpeed(PyObject* self, PyObject* args)
{
    long whoptr;
    double value;

    if (!PyArg_ParseTuple(args,"ld",&whoptr,&value))
        return NULL;
    if (value< -9.99) return NULL;
    if (value> 9.99) return NULL;

    WHO->speed = (float) value;
    Py_INCREF(Py_None);
    return Py_None;
};

/*****************************************************************************/
/* Name   : CFGetFood                                                        */
/* Python : CFPython.GetFood(object)                                         */
/* Status : Untested                                                         */
/*****************************************************************************/

static PyObject* CFGetFood(PyObject* self, PyObject* args)
{
    long whoptr;

    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;

    return Py_BuildValue("i",WHO->stats.food);
};

/*****************************************************************************/
/* Name   : CFSetFood                                                        */
/* Python : CFPython.SetFood(object, value)                                  */
/* Status : Untested                                                         */
/*****************************************************************************/

static PyObject* CFSetFood(PyObject* self, PyObject* args)
{
    long whoptr;
    int value;

    if (!PyArg_ParseTuple(args,"li",&whoptr,&value))
        return NULL;

    if (value<0) return NULL;
    if (value>999) return NULL;

    WHO->stats.food = value;

    Py_INCREF(Py_None);
    return Py_None;
};

/*****************************************************************************/
/* Name   : CFGetGrace                                                       */
/* Python : CFPython.GetGrace(object)                                        */
/* Status : Untested                                                         */
/*****************************************************************************/

static PyObject* CFGetGrace(PyObject* self, PyObject* args)
{
    long whoptr;

    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;

    return Py_BuildValue("i",WHO->stats.grace);
};

/*****************************************************************************/
/* Name   : CFSetGrace                                                       */
/* Python : CFPython.SetGrace(object, value)                                 */
/* Status : Untested                                                         */
/*****************************************************************************/

static PyObject* CFSetGrace(PyObject* self, PyObject* args)
{
    long whoptr;
    int value;

    if (!PyArg_ParseTuple(args,"li",&whoptr,&value))
        return NULL;

    if (value<-16000) return NULL;
    if (value>16000) return NULL;

    WHO->stats.grace = value;

    Py_INCREF(Py_None);
    return Py_None;
};

/*****************************************************************************/
/* Name   : CFGetReturnValue                                                 */
/* Python : CFPython.GetReturnValue()                                        */
/* Status : Untested                                                         */
/*****************************************************************************/

static PyObject* CFGetReturnValue(PyObject* self, PyObject* args)
{
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;

    return Py_BuildValue("i",StackReturn[StackPosition]);
};

/*****************************************************************************/
/* Name   : CFSetReturnValue                                                 */
/* Python : CFPython.SetReturnValue(value)                                   */
/* Status : Untested                                                         */
/*****************************************************************************/

static PyObject* CFSetReturnValue(PyObject* self, PyObject* args)
{
    int value;
    if (!PyArg_ParseTuple(args,"i",&value))
        return NULL;

    StackReturn[StackPosition] = value;
    Py_INCREF(Py_None);
    return Py_None;
};

/*****************************************************************************/
/* Name   : CFGetDirection                                                   */
/* Python : CFPython.GetDirection(object)                                    */
/* Status : Untested                                                         */
/*****************************************************************************/

static PyObject* CFGetDirection(PyObject* self, PyObject* args)
{
    long whoptr;

    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;

    return Py_BuildValue("i",WHO->direction);
};

/*****************************************************************************/
/* Name   : CFSetDirection                                                   */
/* Python : CFPython.SetDirection(object, value)                             */
/* Status : Untested                                                         */
/*****************************************************************************/

static PyObject* CFSetDirection(PyObject* self, PyObject* args)
{
    int value;
    long whoptr;

    if (!PyArg_ParseTuple(args,"li",&whoptr,&value))
        return NULL;

    WHO->direction = value;
    SET_ANIMATION(WHO, WHO->direction);
    Py_INCREF(Py_None);
    return Py_None;
};

/*****************************************************************************/
/* Name   : CFGetLastSP                                                      */
/* Python : CFPython.GetLastSP(object)                                       */
/* Status : Untested                                                         */
/*****************************************************************************/

static PyObject* CFGetLastSP(PyObject* self, PyObject* args)
{
    long whoptr;

    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;

    return Py_BuildValue("i",WHO->last_sp);
};

/*****************************************************************************/
/* Name   : CFSetLastSP                                                      */
/* Python : CFPython.SetLastSP(object, value)                                */
/* Status : Untested                                                         */
/*****************************************************************************/

static PyObject* CFSetLastSP(PyObject* self, PyObject* args)
{
    long whoptr;
    int value;

    if (!PyArg_ParseTuple(args,"li",&whoptr,&value))
        return NULL;

    if (value<0) return NULL;
    if (value>16000) return NULL;

    WHO->last_sp = value;

    Py_INCREF(Py_None);
    return Py_None;
};

/*****************************************************************************/
/* Name   : CFGetLastGrace                                                   */
/* Python : CFPython.GetLastGrace(object)                                    */
/* Status : Untested                                                         */
/*****************************************************************************/

static PyObject* CFGetLastGrace(PyObject* self, PyObject* args)
{
    long whoptr;

    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;

    return Py_BuildValue("i",WHO->last_grace);
};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Untested                                                                */
/*****************************************************************************/

static PyObject* CFSetLastGrace(PyObject* self, PyObject* args)
{
    long whoptr;
    int value;

    if (!PyArg_ParseTuple(args,"li",&whoptr,&value))
        return NULL;

    if (value<0) return NULL;
    if (value>16000) return NULL;

    WHO->last_grace = value;

    Py_INCREF(Py_None);
    return Py_None;
};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Untested                                                                */
/*****************************************************************************/

static PyObject* CFFixObject(PyObject* self, PyObject* args)
{
    long whoptr;

    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;

    fix_player(WHO);
    Py_INCREF(Py_None);
    return Py_None;
};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Untested                                                                */
/*****************************************************************************/

static PyObject* CFSetFace(PyObject* self, PyObject* args)
{
    char* txt;
    long whoptr;
    CFParm* CFR;
    int val = UP_OBJ_FACE;

    if (!PyArg_ParseTuple(args,"ls",&whoptr,&txt))
        return NULL;

    /*WHO->animation_id = find_animation(txt); */
    /*update_object(WHO,UP_OBJ_FACE); */
    GCFP.Value[0] = (void *)(txt);
    CFR = (PlugHooks[HOOK_FINDANIMATION])(&GCFP);
    WHO->animation_id = *(int *)(CFR->Value[0]);
    free(CFR);

    GCFP.Value[0] = (void *)(WHO);
    GCFP.Value[1] = (void *)(&val);
    (PlugHooks[HOOK_UPDATEOBJECT])(&GCFP);

    Py_INCREF(Py_None);
    return Py_None;
};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Untested                                                                */
/*****************************************************************************/

static PyObject* CFGetAttackType(PyObject* self, PyObject* args)
{
    long whoptr;

    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;

    return Py_BuildValue("i",WHO->attacktype);
};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Untested                                                                */
/*****************************************************************************/

static PyObject* CFSetAttackType(PyObject* self, PyObject* args)
{
    long whoptr;
    int value;

    if (!PyArg_ParseTuple(args,"li",&whoptr,&value))
        return NULL;

    WHO->attacktype = value;

    Py_INCREF(Py_None);
    return Py_None;
};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Untested                                                                */
/*****************************************************************************/

static PyObject* CFSetDamage(PyObject* self, PyObject* args)
{
    long whoptr;
    int value;

    if (!PyArg_ParseTuple(args,"li",&whoptr,&value))
        return NULL;

    if (value<0) return NULL;
    if (value>120) return NULL;

    WHO->stats.dam = value;

    Py_INCREF(Py_None);
    return Py_None;
};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Untested                                                                */
/*****************************************************************************/

static PyObject* CFGetDamage(PyObject* self, PyObject* args)
{
    long whoptr;

    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;

    return Py_BuildValue("i",WHO->stats.dam);
};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Untested                                                                */
/*****************************************************************************/

static PyObject* CFSetBeenApplied(PyObject* self, PyObject* args)
{
    long whoptr;
    int value;

    if (!PyArg_ParseTuple(args,"li",&whoptr,&value))
        return NULL;

    if (value!=0)
        SET_FLAG(WHO,FLAG_BEEN_APPLIED);
    else
        CLEAR_FLAG(WHO,FLAG_BEEN_APPLIED);

    Py_INCREF(Py_None);
    return Py_None;
};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Untested                                                                */
/*****************************************************************************/

static PyObject* CFSetIdentified(PyObject* self, PyObject* args)
{
    long whoptr;
    int value;

    if (!PyArg_ParseTuple(args,"li",&whoptr,&value))
        return NULL;

    if (value!=0)
        SET_FLAG(WHO,FLAG_IDENTIFIED);
    else
        CLEAR_FLAG(WHO,FLAG_IDENTIFIED);

    Py_INCREF(Py_None);
    return Py_None;
};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Untested                                                                */
/*****************************************************************************/

static PyObject* CFKillObject(PyObject* self, PyObject* args)
{
    long whoptr;
    long whatptr;
    int ktype;
    int k = 1;
    CFParm* CFR;

    if (!PyArg_ParseTuple(args,"lli",&whoptr,&whatptr,&ktype))
        return NULL;

    WHAT->speed = 0;
    WHAT->speed_left = 0.0;
    update_ob_speed(WHAT);

    if(QUERY_FLAG(WHAT,FLAG_REMOVED))
    {
        printf( "Warning (from KillObject): Trying to remove removed object\n");
        return NULL;
    }
    else
    {
        WHAT->stats.hp = -1;
        GCFP.Value[0] = (void *)(WHAT);
        GCFP.Value[1] = (void *)(&k);
        GCFP.Value[2] = (void *)(WHO);
        GCFP.Value[3] = (void *)(&ktype);

        CFR = (PlugHooks[HOOK_KILLOBJECT])(&GCFP);
        free(CFR);
        /*kill_object(killed,1,killer, type); */
    };

    /* WHAT->script_str_death = NULL; */
    /* WHAT->script_death = NULL; */
    WHAT->event_hook[EVENT_DEATH] = NULL;
    WHAT->event_plugin[EVENT_DEATH] = NULL;
    WHAT->event_options[EVENT_DEATH] = NULL;

   /* This is to avoid the attack routine to continue after we called
    * killObject, since the attacked object no longer exists.
    * By fixing guile_current_other to NULL, guile_use_weapon_script will
    * return -1, meaning the attack function must be immediately terminated.
    */
    if (WHAT==StackOther[StackPosition])
    {
        StackOther[StackPosition] = NULL;
    };
    Py_INCREF(Py_None);
    return Py_None;
};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Untested                                                         */
/*****************************************************************************/

static PyObject* CFWhoIsOther(PyObject* self, PyObject* args)
{
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("l",(long)(StackOther[StackPosition]));
};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Untested                                                         */
/*****************************************************************************/

static PyObject* CFDirectionN(PyObject* self, PyObject* args)
{
    int i=1;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",i);
};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Untested                                                         */
/*****************************************************************************/

static PyObject* CFDirectionNE(PyObject* self, PyObject* args)
{
    int i=2;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",i);
};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Untested                                                                */
/*****************************************************************************/

static PyObject* CFDirectionE(PyObject* self, PyObject* args)
{
    int i=3;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",i);
};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Untested                                                                */
/*****************************************************************************/

static PyObject* CFDirectionSE(PyObject* self, PyObject* args)
{
    int i=4;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",i);
};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Untested                                                                */
/*****************************************************************************/

static PyObject* CFDirectionS(PyObject* self, PyObject* args)
{
    int i=5;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",i);
};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Untested                                                                */
/*****************************************************************************/

static PyObject* CFDirectionSW(PyObject* self, PyObject* args)
{
    int i=6;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",i);
};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Untested                                                                */
/*****************************************************************************/

static PyObject* CFDirectionW(PyObject* self, PyObject* args)
{
    int i=7;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",i);
};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Untested                                                                */
/*****************************************************************************/

static PyObject* CFDirectionNW(PyObject* self, PyObject* args)
{
    int i=8;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",i);
};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Untested                                                                */
/*****************************************************************************/

static PyObject* CFCastSpell(PyObject* self, PyObject* args)
{
    long whoptr;
    int spell;
    int dir;
    char* op;
    CFParm* CFR;
    int parm=0;
    int parm2 = spellNormal;
    int typeoffire = FIRE_DIRECTIONAL;

    if (!PyArg_ParseTuple(args,"liis",&whoptr,&spell,&dir,&op))
        return NULL;

    GCFP.Value[0] = (void *)(WHO);
    GCFP.Value[1] = (void *)(WHO);
    GCFP.Value[2] = (void *)(&dir);
    GCFP.Value[3] = (void *)(&spell);
    GCFP.Value[4] = (void *)(&parm);
    GCFP.Value[5] = (void *)(&parm2);
    GCFP.Value[6] = (void *)(op);
    GCFP.Value[7] = (void *)(&typeoffire);
    CFR = (PlugHooks[HOOK_CASTSPELL])(&GCFP);
    /*cast_spell(WHO, WHO, dir, spell, */
    /*    1,spellNormal, op); */

    free(CFR);
    Py_INCREF(Py_None);
    return Py_None;
};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Untested                                                                */
/*****************************************************************************/

static PyObject* CFForgetSpell(PyObject* self, PyObject* args)
{
    long whoptr;
    int spell;

    if (!PyArg_ParseTuple(args,"li",&whoptr,&spell))
        return NULL;

    GCFP.Value[0] = (void *)(WHO);
    GCFP.Value[1] = (void *)(&spell);
    (PlugHooks[HOOK_FORGETSPELL])(&GCFP);

    Py_INCREF(Py_None);
    return Py_None;
};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Untested                                                                */
/*****************************************************************************/

static PyObject* CFAcquireSpell(PyObject* self, PyObject* args)
{
    long whoptr;
    int spell;
    int i = 0;

    if (!PyArg_ParseTuple(args,"li",&whoptr,&spell))
        return NULL;

    GCFP.Value[0] = (void *)(WHO);
    GCFP.Value[1] = (void *)(&spell);
    GCFP.Value[2] = (void *)(&i);
    (PlugHooks[HOOK_LEARNSPELL])(&GCFP);

    Py_INCREF(Py_None);
    return Py_None;
};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Untested                                                                */
/*****************************************************************************/

static PyObject* CFDoKnowSpell(PyObject* self, PyObject* args)
{
    int spell;
    long whoptr;
    CFParm* CFR;
    int value;

    if (!PyArg_ParseTuple(args,"li",&whoptr,&spell))
        return NULL;

    GCFP.Value[0] = (void *)(WHO);
    GCFP.Value[1] = (void *)(&spell);
    CFR = (PlugHooks[HOOK_CHECKFORSPELL])(&GCFP);
    value = *(int *)(CFR->Value[0]);
    free(CFR);
    return Py_BuildValue("i",value);
};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Untested                                                                */
/*****************************************************************************/

static PyObject* CFCheckInvisibleInside(PyObject* self, PyObject* args)
{
    int whoptr;
    char *id;
    object* tmp2;

    if (!PyArg_ParseTuple(args,"ls",&whoptr,&id))
        return NULL;

    for(tmp2=WHO->inv;tmp2 !=NULL; tmp2=tmp2->below)
    {
        if(tmp2->type == FORCE && tmp2->slaying && !strcmp(tmp2->slaying,id))
            break;
    };

    return Py_BuildValue("l",(long)(tmp2));
};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Stable.                                                          */
/* Info   : The Values of a player force will effect the player.             */
/*****************************************************************************/
static PyObject* CFCreatePlayerForce(PyObject* self, PyObject* args)
{
    long whereptr;
    char* txt;
    char txt2[16];
    object *myob;
    object *where;
    CFParm* CFR;
    
    if (!PyArg_ParseTuple(args,"ls",&whereptr,&txt))
        return NULL;
    
    where = (object *)(whereptr);
    
    strcpy(txt2,"player_force");
    
    GCFP.Value[0] = (void *)(txt2);
    CFR = (PlugHooks[HOOK_GETARCHETYPE])(&GCFP);
    
    /*myob = get_archetype("player_force"); */
    myob = (object *)(CFR->Value[0]);
    free(CFR);
    
    if(!myob)
    {
        printf("Python WARNING:: CreatePlayerForce: Can't find archtype 'player_force'\n");
        return NULL;
    }
    
    /* setup the force and put it in activator */
    myob->name = add_string(txt);
    myob = insert_ob_in_ob(myob, where);

    /*esrv_send_item((object *)(gh_scm2long(where)), myob); */
    GCFP.Value[0] = (void *)(where);
    GCFP.Value[1] = (void *)(myob);
    (PlugHooks[HOOK_ESRVSENDITEM])(&GCFP);

    return Py_BuildValue("l",(long)(myob));
};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Stable                                                           */
/* Info   : The Values of a player_info object will NOT effect the player.   */
/*****************************************************************************/
static PyObject* CFCreatePlayerInfo(PyObject* self, PyObject* args)
{
    long whereptr;
    char* txt;
    char txt2[16];
    object *myob;
    object *where;
    CFParm* CFR;
    
    if (!PyArg_ParseTuple(args,"ls",&whereptr,&txt))
        return NULL;
    
    where = (object *)(whereptr);
    
    strcpy(txt2,"player_info");
    
    GCFP.Value[0] = (void *)(txt2);
    CFR = (PlugHooks[HOOK_GETARCHETYPE])(&GCFP);
    
    /*myob = get_archetype("player_info"); */
    myob = (object *)(CFR->Value[0]);
    free(CFR);
    
    
    if(!myob)
    {
        printf("Python WARNING:: CreatePlayerInfo: Cant't find archtype 'player_info'\n");
        return NULL;
    }
    
    /* setup the info and put it in activator */
    myob->name = add_string(txt);
    myob = insert_ob_in_ob(myob, where);
    
    /*esrv_send_item((object *)(gh_scm2long(where)), myob); */
    GCFP.Value[0] = (void *)(where);
    GCFP.Value[1] = (void *)(myob);
    (PlugHooks[HOOK_ESRVSENDITEM])(&GCFP);
    
    return Py_BuildValue("l",(long)(myob));
};

/*****************************************************************************/
/* Name   : CFGetPlayerInfo                                                  */
/* Python : CFPython.GetPlayerInfo(who, <name_text>)                         */
/* Status : Stable                                                           */
/*        : player_info: get first player_info in inventory with name_text   */
/*        :                                                                  */
/*****************************************************************************/
static PyObject* CFGetPlayerInfo(PyObject* self, PyObject* args)
{
    long whereptr;
    char *name;
    object *walk, *who;
    
    if (!PyArg_ParseTuple(args,"ls",&whereptr,&name))
        return NULL;

    who = (object *)(whereptr);

    /* get the first linked player_info arch in this inventory */
    for(walk=who->inv;walk!=NULL;walk=walk->below)
    {
        if (!strcmp(walk->arch->name,"player_info") &&  !strcmp(walk->name,name))
            return Py_BuildValue("l",(long)(walk));
    }

    return Py_None; /* there was non */
};


/*****************************************************************************/
/* Name   : CFGetNextPlayerInfo                                              */
/* Python : CFPython.GetNextPlayerInfo(who, player_info)                     */
/* Status : Stable                                                           */
/*        : player_info: get next player_info in inventory with same name    */
/*        :                                                                  */
/*****************************************************************************/
static PyObject* CFGetNextPlayerInfo(PyObject* self, PyObject* args)
{
    long whereptr;
    char name[128];
    object *myob, *walk;
    
    if (!PyArg_ParseTuple(args,"ll",&whereptr,&myob))
        return NULL;
    if(!myob)
        return Py_None; /* there was non left - this should avoided in scrip */

    /* thats our check paramters: arch "force_info", name of this arch */
    strncpy(name, myob->name, 127); /* 127 chars should be enough for all */
    name[63] = '\0';

    /* get the next linked player_info arch in this inventory */
    for(walk=myob->below;walk!=NULL;walk=walk->below)
    {
        if (!strcmp(walk->arch->name,"player_info") &&  !strcmp(walk->name,name))
            return Py_BuildValue("l",(long)(walk));
    }

    return Py_None; /* there was non left */
};


/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Untested                                                                */
/*****************************************************************************/
static PyObject* CFCreateInvisibleInside(PyObject* self, PyObject* args)
{
    long whereptr;
    char* txt;
    char txt2[6];
    object *myob;
    object *where;
    CFParm* CFR;

    if (!PyArg_ParseTuple(args,"ls",&whereptr,&txt))
        return NULL;

    where = (object *)(whereptr);

    strcpy(txt2,"force");

    GCFP.Value[0] = (void *)(txt2);
    CFR = (PlugHooks[HOOK_GETARCHETYPE])(&GCFP);

    /*myob = get_archetype("force"); */
    myob = (object *)(CFR->Value[0]);
    free(CFR);

    if(!myob)
    {
        printf("Python WARNING:: CFCreateInvisibleInside: Can't find archtype 'force'\n");
        return NULL;
    }
    myob->speed = 0.0;
    GCFP.Value[0] = (void *)(myob);
    (PlugHooks[HOOK_UPDATESPEED])(&GCFP);

    /*update_ob_speed(myob); */
    myob->slaying = add_string(txt);
    myob = insert_ob_in_ob(myob, where);

    GCFP.Value[0] = (void *)(where);
    GCFP.Value[1] = (void *)(myob);
  /*esrv_send_item((object *)(gh_scm2long(where)), myob); */
    (PlugHooks[HOOK_ESRVSENDITEM])(&GCFP);
    return Py_BuildValue("l",(long)(myob));
};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Untested                                                                */
/*****************************************************************************/

static PyObject* CFCreateObjectInside(PyObject* self, PyObject* args)
{
    object *myob;
    object *test;
    object *where;
    int i;
    long whereptr;
    char *tmpname;
    char *txt;
    CFParm* CFR;

    if (!PyArg_ParseTuple(args,"sl",&txt, &whereptr))
        return NULL;

    where = (object *)(whereptr);

    GCFP.Value[0] = (void *)(txt);
    CFR = (PlugHooks[HOOK_GETARCHBYOBJNAME])(&GCFP);
    myob = (object *)(CFR->Value[0]);
    free(CFR);

    if (!strncmp(query_name(myob), "singluarity",11))
    {
        free_object(myob);
        CFR = (PlugHooks[HOOK_GETARCHETYPE])(&GCFP);
        myob = (object *)(CFR->Value[0]);
        free(CFR);
    }
    else
    {
        if (strcmp(query_name(myob),txt))
        {
            for(i=strlen(query_name(myob)); i>0;i--)
            {
                tmpname = (char *)(malloc(i+1));
                strncpy(tmpname,query_name(myob),i);
                tmpname[i] = 0x0;
                if (!strcmp(query_name(myob),tmpname))
                {
                    free_string(tmpname);
                    tmpname = txt + i;
                    GCFP.Value[0] = (void *)(myob);
                    GCFP.Value[1] = (void *)(tmpname);
                    /*test = create_artifact(myob,tmpname); */
                    CFR = (PlugHooks[HOOK_CREATEARTIFACT])(&GCFP);
                    test = (object *)(CFR->Value[0]);
                    free(CFR);
                }
                else
                {
                    free_string(tmpname);
                };
            };
        };
    };

    myob = insert_ob_in_ob(myob, where);
    if (where->type == PLAYER)
    {
        GCFP.Value[0] = (void *)(where);
        GCFP.Value[1] = (void *)(myob);
        (PlugHooks[HOOK_ESRVSENDITEM])(&GCFP);
/*        esrv_send_item((object *)(gh_scm2long(where)), myob); */
    };
    return Py_BuildValue("l",(long)(myob));
};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Untested                                                                */
/*****************************************************************************/

static PyObject* CFCheckMap(PyObject* self, PyObject* args)
{
    char *what;
    char *mapstr;
    int x, y;
    object* foundob;

    if (!PyArg_ParseTuple(args,"ss(ii)",&what,&mapstr,&x,&y))
        return NULL;
    foundob = present_arch(
        find_archetype(what),
        has_been_loaded(mapstr),
        x,y
    );
    return Py_BuildValue("l",(long)(foundob));
};

/*****************************************************************************/
/* Name   : CFCheckArchInventory                                             */
/* Python : CFPython.CheckArchInventory(who, 'arch_name')                    */
/* Status : Stable                                                           */
/* Info   : This routine search explizit for a arch_name.                    */
/*****************************************************************************/
static PyObject* CFCheckArchInventory(PyObject* self, PyObject* args)
{
    long whoptr;
    char* whatstr;
    object* tmp;
    
    if (!PyArg_ParseTuple(args,"ls",&whoptr,&whatstr))
        return NULL;
    tmp = WHO->inv;

    while (tmp)
    {
        if (!strcmp(tmp->arch->name,whatstr))
            return Py_BuildValue("l",(long)(tmp));
        tmp = tmp->below;
    };

    return Py_None; /* we don't find a arch with this arch_name in the inventory */
};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Untested                                                         */
/*****************************************************************************/
static PyObject* CFCheckInventory(PyObject* self, PyObject* args)
{
    long whoptr;
    char* whatstr;
    object* tmp;
    object* foundob;

    if (!PyArg_ParseTuple(args,"ls",&whoptr,&whatstr))
        return NULL;
    tmp = WHO->inv;
    foundob = present_arch_in_ob(find_archetype(whatstr),WHO);
    if (foundob == NULL)
    {
        while (tmp)
        {
            if (!strncmp(query_name(tmp),whatstr,strlen(whatstr)))
            {
                return Py_BuildValue("l",(long)(tmp));
            };
            if (!strncmp(tmp->name,whatstr,strlen(whatstr)))
            {
                return Py_BuildValue("l",(long)(tmp));
            };
            tmp = tmp->below;
        };
    };
    return Py_BuildValue("l",(long)(foundob));
};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Untested  - hm, query name should return CF name like "name (unpaid) (cursed) */
/* and not the 'pure' name - we must test this */
/*****************************************************************************/

static PyObject* CFGetName(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;
    return Py_BuildValue("s",WHO->name);
};

/*****************************************************************************/
/* Name   : CFSetName                                                        */
/* Python : CFPython.SetName(object, name)                                   */
/* Status : Stable                                                           */
/*****************************************************************************/
static PyObject* CFSetName(PyObject* self, PyObject* args)
{
    long whoptr;
    char *txt;
    
    if (!PyArg_ParseTuple(args,"ls",&whoptr,&txt))
        return NULL;
    if (WHO->name != NULL)
        DELETE_STRING(WHO->name);
    if(txt && strcmp(txt,""))
        WHO->name = add_string(txt);
    return Py_None;
};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Stable                                                           */
/*****************************************************************************/

static PyObject* CFGetTitle(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;
    return Py_BuildValue("s",WHO->title);
};


/*****************************************************************************/
/* Name   : CFSetTitle                                                       */
/* Python : CFPython.SetTitle(object, name)                                  */
/* Status : Stable                                                           */
/*****************************************************************************/
static PyObject* CFSetTitle(PyObject* self, PyObject* args)
{
    long whoptr;
    char *txt;
    
    if (!PyArg_ParseTuple(args,"ls",&whoptr,&txt))
        return NULL;
    if (WHO->title != NULL)
        DELETE_STRING(WHO->title);
    if(txt && strcmp(txt,""))
        WHO->title = add_string(txt);
    return Py_None;
};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Stable                                                           */
/*****************************************************************************/
static PyObject* CFGetSlaying(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;
    return Py_BuildValue("s",WHO->slaying);
};

/*****************************************************************************/
/* Name   : CFSetSlaying                                                     */
/* Python : CFPython.SetSlaying(object, name)                                */
/* Status : Stable                                                           */
/*****************************************************************************/
static PyObject* CFSetSlaying(PyObject* self, PyObject* args)
{
    long whoptr;
    char *txt;

    if (!PyArg_ParseTuple(args,"ls",&whoptr,&txt))
        return NULL;
    if (WHO->slaying != NULL)
        DELETE_STRING(WHO->slaying);
    if(txt && strcmp(txt,""))
        WHO->slaying = add_string(txt);
    return Py_None;
};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Untested                                                                */
/*****************************************************************************/

static PyObject* CFCreateObject(PyObject* self, PyObject* args)
{
    object *myob;
    object *test;
    char *txt;
    int i = 0;
    char *tmpname;
    CFParm* CFR;
    int x,y;
    int val;
    long map = (long)((StackWho[StackPosition])->map);

    if (!PyArg_ParseTuple(args,"s(ii)|l",&txt, &x,&y,&map))
        return NULL;

    /*myob = get_archetype(txt); */
    /*myob = get_archetype_by_object_name(txt); */
    GCFP.Value[0] = (void *)(txt);
    CFR = (PlugHooks[HOOK_GETARCHBYOBJNAME])(&GCFP);
    myob = (object *)(CFR->Value[0]);
    free(CFR);

    if (!strncmp(query_name(myob), "singluarity",11))
    {
        free_object(myob);
        /*myob = get_archetype(txt); */
        GCFP.Value[0] = (void *)(txt);
        CFR = (PlugHooks[HOOK_GETARCHETYPE])(&GCFP);
        myob = (object *)(CFR->Value[0]);
        free(CFR);
    }
    else
    {
        if (strcmp(query_name(myob),txt))
        {
            for(i=strlen(query_name(myob)); i>0;i--)
            {
                tmpname = (char *)(malloc(i+1));
                strncpy(tmpname,query_name(myob),i);
                tmpname[i] = 0x0;
                if (!strcmp(query_name(myob),tmpname))
                {
                    free_string(tmpname);
                    tmpname = txt + i;
                    GCFP.Value[0] = (void *)(myob);
                    GCFP.Value[1] = (void *)(tmpname);
                    CFR = (PlugHooks[HOOK_CREATEARTIFACT])(&GCFP);
                    /*test = create_artifact(myob,tmpname); */
                    test = (object *)(CFR->Value[0]);
                    free(CFR);
                }
                else
                {
                    free_string(tmpname);
                };
            };
        };
    };
    myob->x = x;
    myob->y = y;
    val = 0;
    GCFP.Value[0] = (void *)(myob);
    GCFP.Value[1] = (void *)(map);
    GCFP.Value[2] = NULL;
    GCFP.Value[3] = (void *)(&val);
    /*myob = insert_ob_in_map(myob, map ,NULL,0); */
    CFR = (PlugHooks[HOOK_INSERTOBJECTINMAP])(&GCFP);
    myob = (object *)(CFR->Value[0]);
    free(CFR);
    return Py_BuildValue("l",(long)(myob));
};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Untested                                                                */
/*****************************************************************************/

static PyObject* CFRemoveObject(PyObject* self, PyObject* args)
{
    long whoptr;
    object* myob;

    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;

    myob = (object *)(whoptr);
    GCFP.Value[0] = (void *)(myob);
    (PlugHooks[HOOK_REMOVEOBJECT])(&GCFP);
    /*remove_ob(myob); */

    if (StackActivator[StackPosition]->type == PLAYER)
    {
        GCFP.Value[0] = (void *)(StackActivator[StackPosition]);
        GCFP.Value[1] = (void *)(StackActivator[StackPosition]);
        (PlugHooks[HOOK_ESRVSENDINVENTORY])(&GCFP);
/*    esrv_send_inventory(guile_current_activator[guile_stack_position], */
/*guile_current_activator[guile_stack_position]); */
    };
    free_object(myob);
    Py_INCREF(Py_None);
    return Py_None;
};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Stable                                                           */
/*****************************************************************************/

static PyObject* CFIsAlive(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;
    return Py_BuildValue("i",QUERY_FLAG(WHO,FLAG_ALIVE));
};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Stable                                                           */
/*****************************************************************************/

static PyObject* CFIsWiz(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;
    return Py_BuildValue("i",QUERY_FLAG(WHO,FLAG_WIZ));
};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Stable                                                           */
/*****************************************************************************/

static PyObject* CFWasWiz(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;
    return Py_BuildValue("i",QUERY_FLAG(WHO,FLAG_WAS_WIZ));
};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Stable                                                           */
/*****************************************************************************/

static PyObject* CFIsApplied(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;
    return Py_BuildValue("i",QUERY_FLAG(WHO,FLAG_APPLIED));
};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Stable                                                           */
/*****************************************************************************/

static PyObject* CFIsUnpaid(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;
    return Py_BuildValue("i",QUERY_FLAG(WHO,FLAG_UNPAID));
};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Stable                                                           */
/*****************************************************************************/

static PyObject* CFIsFlying(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;
    return Py_BuildValue("i",QUERY_FLAG(WHO,FLAG_FLYING));
};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Stable                                                           */
/*****************************************************************************/

static PyObject* CFIsMonster(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;
    return Py_BuildValue("i",QUERY_FLAG(WHO,FLAG_MONSTER));
};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Stable                                                           */
/*****************************************************************************/

static PyObject* CFIsFriendly(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;
    return Py_BuildValue("i",QUERY_FLAG(WHO,FLAG_FRIENDLY));
};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Stable                                                           */
/*****************************************************************************/

static PyObject* CFIsGenerator(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;
    return Py_BuildValue("i",QUERY_FLAG(WHO,FLAG_GENERATOR));
};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Stable                                                           */
/*****************************************************************************/

static PyObject* CFIsThrown(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;
    return Py_BuildValue("i",QUERY_FLAG(WHO,FLAG_IS_THROWN));
};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Stable                                                           */
/*****************************************************************************/

static PyObject* CFCanSeeInvisible(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;
    return Py_BuildValue("i",QUERY_FLAG(WHO,FLAG_SEE_INVISIBLE));
};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Stable                                                           */
/*****************************************************************************/

static PyObject* CFCanRoll(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;
    return Py_BuildValue("i",QUERY_FLAG(WHO,FLAG_CAN_ROLL));
};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Stable                                                           */
/*****************************************************************************/

static PyObject* CFIsTurnable(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;
    return Py_BuildValue("i",QUERY_FLAG(WHO,FLAG_IS_TURNABLE));
};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Stable                                                           */
/*****************************************************************************/

static PyObject* CFIsUsedUp(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;
    return Py_BuildValue("i",QUERY_FLAG(WHO,FLAG_IS_USED_UP));
};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Stable                                                           */
/*****************************************************************************/

static PyObject* CFIsIdentified(PyObject* self, PyObject* args)
{
    long whoptr;
    int retval;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;
    retval = QUERY_FLAG(WHO,FLAG_IDENTIFIED);
    return Py_BuildValue("i",retval);
};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Stable                                                           */
/*****************************************************************************/

static PyObject* CFIsSplitting(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;
    return Py_BuildValue("i",QUERY_FLAG(WHO,FLAG_SPLITTING));
};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Stable                                                           */
/*****************************************************************************/

static PyObject* CFHitBack(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;
    return Py_BuildValue("i",QUERY_FLAG(WHO,FLAG_HITBACK));
};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Stable                                                           */
/*****************************************************************************/

static PyObject* CFBlocksView(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;
    return Py_BuildValue("i",QUERY_FLAG(WHO,FLAG_BLOCKSVIEW));
};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Stable                                                           */
/*****************************************************************************/

static PyObject* CFIsUndead(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;
    return Py_BuildValue("i",QUERY_FLAG(WHO,FLAG_UNDEAD));
};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Stable                                                           */
/*****************************************************************************/

static PyObject* CFIsScared(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;
    return Py_BuildValue("i",QUERY_FLAG(WHO,FLAG_SCARED));
};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Stable                                                           */
/*****************************************************************************/

static PyObject* CFIsUnaggressive(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;
    return Py_BuildValue("i",QUERY_FLAG(WHO,FLAG_UNAGGRESSIVE));
};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Stable                                                           */
/*****************************************************************************/

static PyObject* CFReflectMissiles(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;
    return Py_BuildValue("i",QUERY_FLAG(WHO,FLAG_REFL_MISSILE));
};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Stable                                                           */
/*****************************************************************************/

static PyObject* CFReflectSpells(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;
    return Py_BuildValue("i",QUERY_FLAG(WHO,FLAG_REFL_SPELL));
};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Stable                                                           */
/*****************************************************************************/

static PyObject* CFIsRunningAway(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;
    return Py_BuildValue("i",QUERY_FLAG(WHO,FLAG_RUN_AWAY));
};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Stable                                                           */
/*****************************************************************************/

static PyObject* CFCanPassThru(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;
    return Py_BuildValue("i",QUERY_FLAG(WHO,FLAG_CAN_PASS_THRU));
};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Stable                                                           */
/*****************************************************************************/

static PyObject* CFCanPickUp(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;
    return Py_BuildValue("i",QUERY_FLAG(WHO,FLAG_PICK_UP));
};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Stable                                                           */
/*****************************************************************************/

static PyObject* CFIsUnique(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;
    return Py_BuildValue("i",QUERY_FLAG(WHO,FLAG_UNIQUE));
};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Stable                                                           */
/*****************************************************************************/

static PyObject* CFCanCastSpell(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;
    return Py_BuildValue("i",QUERY_FLAG(WHO,FLAG_CAST_SPELL));
};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Stable                                                           */
/*****************************************************************************/

static PyObject* CFCanUseScroll(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;
    return Py_BuildValue("i",QUERY_FLAG(WHO,FLAG_USE_SCROLL));
};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Stable                                                           */
/*****************************************************************************/

static PyObject* CFCanUseWand(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;
    return Py_BuildValue("i",QUERY_FLAG(WHO,FLAG_USE_RANGE));
};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Stable                                                           */
/*****************************************************************************/

static PyObject* CFCanUseBow(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;
    return Py_BuildValue("i",QUERY_FLAG(WHO,FLAG_USE_BOW));
};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Stable                                                           */
/*****************************************************************************/

static PyObject* CFCanUseArmour(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;
    return Py_BuildValue("i",QUERY_FLAG(WHO,FLAG_USE_ARMOUR));
};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Stable                                                           */
/*****************************************************************************/

static PyObject* CFCanUseWeapon(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;
    return Py_BuildValue("i",QUERY_FLAG(WHO,FLAG_USE_WEAPON));
};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Stable                                                           */
/*****************************************************************************/

static PyObject* CFCanUseRing(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;
    return Py_BuildValue("i",QUERY_FLAG(WHO,FLAG_USE_RING));
};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Stable                                                           */
/*****************************************************************************/

static PyObject* CFHasXRays(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;
    return Py_BuildValue("i",QUERY_FLAG(WHO,FLAG_XRAYS));
};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Stable                                                           */
/*****************************************************************************/

static PyObject* CFIsFloor(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;
    return Py_BuildValue("i",QUERY_FLAG(WHO,FLAG_IS_FLOOR));
};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Stable                                                           */
/*****************************************************************************/

static PyObject* CFIsLifeSaver(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;
    return Py_BuildValue("i",QUERY_FLAG(WHO,FLAG_LIFESAVE));
};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Stable                                                           */
/*****************************************************************************/

static PyObject* CFIsSleeping(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;
    return Py_BuildValue("i",QUERY_FLAG(WHO,FLAG_SLEEP));
};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Stable                                                           */
/*****************************************************************************/

static PyObject* CFStandStill(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;
    return Py_BuildValue("i",QUERY_FLAG(WHO,FLAG_STAND_STILL));
};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Stable                                                           */
/*****************************************************************************/

static PyObject* CFOnlyAttack(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;
    return Py_BuildValue("i",QUERY_FLAG(WHO,FLAG_ONLY_ATTACK));
};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Stable                                                           */
/*****************************************************************************/

static PyObject* CFIsConfused(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;
    return Py_BuildValue("i",QUERY_FLAG(WHO,FLAG_CONFUSED));
};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Stable                                                           */
/*****************************************************************************/

static PyObject* CFHasStealth(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;
    return Py_BuildValue("i",QUERY_FLAG(WHO,FLAG_STEALTH));
};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Stable                                                           */
/*****************************************************************************/

static PyObject* CFIsCursed(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;
    return Py_BuildValue("i",QUERY_FLAG(WHO,FLAG_CURSED));
};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Stable                                                           */
/*****************************************************************************/

static PyObject* CFIsDamned(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;
    return Py_BuildValue("i",QUERY_FLAG(WHO,FLAG_DAMNED));
};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Stable                                                           */
/*****************************************************************************/

static PyObject* CFIsKnownMagical(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;
    return Py_BuildValue("i",QUERY_FLAG(WHO,FLAG_KNOWN_MAGICAL));
};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Stable                                                           */
/*****************************************************************************/

static PyObject* CFIsKnownCursed(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;
    return Py_BuildValue("i",QUERY_FLAG(WHO,FLAG_KNOWN_CURSED));
};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Stable                                                           */
/*****************************************************************************/

static PyObject* CFCanUseSkill(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;
    return Py_BuildValue("i",QUERY_FLAG(WHO,FLAG_CAN_USE_SKILL));
};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Stable                                                           */
/*****************************************************************************/

static PyObject* CFHasBeenApplied(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;
    return Py_BuildValue("i",QUERY_FLAG(WHO,FLAG_BEEN_APPLIED));
};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Stable                                                           */
/*****************************************************************************/

static PyObject* CFCanUseRod(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;
    return Py_BuildValue("i",QUERY_FLAG(WHO,FLAG_USE_ROD));
};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Stable                                                           */
/*****************************************************************************/

static PyObject* CFCanUseHorn(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;
    return Py_BuildValue("i",QUERY_FLAG(WHO,FLAG_USE_HORN));
};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Stable                                                           */
/*****************************************************************************/

static PyObject* CFMakeInvisible(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;
    return Py_BuildValue("i",QUERY_FLAG(WHO,FLAG_MAKE_INVIS));
};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Stable                                                           */
/*****************************************************************************/

static PyObject* CFIsBlind(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;
    return Py_BuildValue("i",QUERY_FLAG(WHO,FLAG_BLIND));
};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Stable                                                           */
/*****************************************************************************/

static PyObject* CFCanSeeInDark(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;
    return Py_BuildValue("i",QUERY_FLAG(WHO,FLAG_SEE_IN_DARK));
};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Stable                                                           */
/*****************************************************************************/

static PyObject* CFGetAC(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;
    return Py_BuildValue("i",WHO->stats.ac);
};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Stable                                                           */
/*****************************************************************************/
static PyObject* CFGetWC(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;
    return Py_BuildValue("i",WHO->stats.wc);
};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Untested                                                                */
/*****************************************************************************/

static PyObject* CFGetCha(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;
    return Py_BuildValue("i",WHO->stats.Cha);
};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Untested                                                                */
/*****************************************************************************/

static PyObject* CFGetCon(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;
    return Py_BuildValue("i",WHO->stats.Con);
};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Untested                                                                */
/*****************************************************************************/

static PyObject* CFGetDex(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;
    return Py_BuildValue("i",WHO->stats.Dex);
};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Untested                                                                */
/*****************************************************************************/

static PyObject* CFGetHP(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;
    return Py_BuildValue("i",WHO->stats.hp);

};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Untested                                                                */
/*****************************************************************************/

static PyObject* CFGetInt(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;
    return Py_BuildValue("i",WHO->stats.Int);
};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Untested                                                                */
/*****************************************************************************/

static PyObject* CFGetPow(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;
    return Py_BuildValue("i",WHO->stats.Pow);
};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Untested                                                                */
/*****************************************************************************/

static PyObject* CFGetSP(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;
    return Py_BuildValue("i",WHO->stats.sp);
};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Untested                                                                */
/*****************************************************************************/

static PyObject* CFGetStr(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;
    return Py_BuildValue("i",WHO->stats.Str);
};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Untested                                                                */
/*****************************************************************************/

static PyObject* CFGetWis(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;
    return Py_BuildValue("i",WHO->stats.Wis);
};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Untested                                                                */
/*****************************************************************************/

static PyObject* CFGetMaxHP(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;
    return Py_BuildValue("i",WHO->stats.maxhp);
};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Untested                                                                */
/*****************************************************************************/

static PyObject* CFGetMaxSP(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;
    return Py_BuildValue("i",WHO->stats.maxsp);
};

/*****************************************************************************/
/* Name   : CFGetXPos                                                        */
/* Python : CFPython.GetXPosition(object)                                    */
/* Status : Stable                                                           */
/*****************************************************************************/

static PyObject* CFGetXPos(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;
    return Py_BuildValue("i",WHO->x);
};

/*****************************************************************************/
/* Name   : CFGetYPos                                                        */
/* Python : CFPython.GetYPosition                                            */
/* Status : Stable                                                           */
/*****************************************************************************/

static PyObject* CFGetYPos(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;
    return Py_BuildValue("i",WHO->y);
};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Untested                                                                */
/*****************************************************************************/

static PyObject* CFSetPosition(PyObject* self, PyObject* args)
{
    int x, y, k;
    long whoptr;
    CFParm* CFR;
    k = 0;

    if (!PyArg_ParseTuple(args,"l(ii)",&whoptr,&x,&y))
        return NULL;

    GCFP.Value[0] = (void *)(WHO);
    GCFP.Value[1] = (void *)(&x);
    GCFP.Value[2] = (void *)(&y);
    GCFP.Value[3] = (void *)(&k);
    GCFP.Value[4] = (void *)(NULL);

    (PlugHooks[HOOK_TRANSFEROBJECT])(&GCFP);

/*  transfer_ob(WHO, gh_scm2int(X), gh_scm2int(Y), 0, NULL); */

    free(&CFR);
    Py_INCREF(Py_None);
    return Py_None;
};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Untested                                                                */
/*****************************************************************************/

static PyObject* CFSetNickname(PyObject* self, PyObject* args)
{
    long whoptr;
    char *newnick;
    CFParm* CFR;
    /*int val = UP_OBJ_CHANGE; */

    if (!PyArg_ParseTuple(args,"ls",&whoptr,&newnick))
        return NULL;

    if (WHO->type==PLAYER)
    {
        GCFP.Value[0] = (void *)(WHO);
        GCFP.Value[1] = (void *)(newnick);
        CFR = (PlugHooks[HOOK_CMDTITLE])(&GCFP);
        free(CFR);
    }
    else
    {
        WHO->title = add_string(newnick);
        if (WHO->env != NULL)
        {
            if (WHO->env->type == PLAYER)
            {
                GCFP.Value[0] = (void *)(WHO->env);
                GCFP.Value[1] = (void *)(WHO);
                (PlugHooks[HOOK_ESRVSENDITEM])(&GCFP);
            }
        };
    };

    Py_INCREF(Py_None);
    return Py_None;
};

/*****************************************************************************/
/* Name   : CFSetAC                                                          */
/* Python : SetAC(object, value)                                             */
/* Status : Untested, AC may not survive fix_player()                        */
/*****************************************************************************/

static PyObject* CFSetAC(PyObject* self, PyObject* args)
{
    int value;
    long whoptr;

    if (!PyArg_ParseTuple(args,"li",&whoptr,&value))
        return NULL;

    if (value>120) return NULL;
    if (value<-120) return NULL;

    WHO->stats.ac = value;
    Py_INCREF(Py_None);
    return Py_None;
};

/*****************************************************************************/
/* Name   : CFSetWC                                                          */
/* Python : SetWC(object, value)                                             */
/* Status : Untested, WC may not survive fix_player()                        */
/*****************************************************************************/

static PyObject* CFSetWC(PyObject* self, PyObject* args)
{
    int value;
    long whoptr;

    if (!PyArg_ParseTuple(args,"li",&whoptr,&value))
        return NULL;

    if (value>120) return NULL;
    if (value<-120) return NULL;

    WHO->stats.wc = value;
    Py_INCREF(Py_None);
    return Py_None;
};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Untested                                                                */
/*****************************************************************************/

static PyObject* CFSetCha(PyObject* self, PyObject* args)
{
    int value;
    long whoptr;

    if (!PyArg_ParseTuple(args,"li",&whoptr,&value))
        return NULL;

    if (value>30) return NULL;
    if (value<-30) return NULL;

    WHO->stats.Cha = value;
    if (WHO->type == PLAYER)
    {
        WHO->contr->orig_stats.Cha = value;
    };
    fix_player(WHO);
    Py_INCREF(Py_None);
    return Py_None;
};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Untested                                                                */
/*****************************************************************************/

static PyObject* CFSetCon(PyObject* self, PyObject* args)
{
    int value;
    long whoptr;

    if (!PyArg_ParseTuple(args,"li",&whoptr,&value))
        return NULL;

    if (value>30) return NULL;
    if (value<-30) return NULL;

    WHO->stats.Con = value;
    if (WHO->type == PLAYER)
    {
        WHO->contr->orig_stats.Con = value;
    };
    fix_player(WHO);
    Py_INCREF(Py_None);
    return Py_None;
};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Untested                                                                */
/*****************************************************************************/

static PyObject* CFSetDex(PyObject* self, PyObject* args)
{
    int value;
    long whoptr;

    if (!PyArg_ParseTuple(args,"li",&whoptr,&value))
        return NULL;

    if (value>30) return NULL;
    if (value<-30) return NULL;

    WHO->stats.Dex = value;
    if (WHO->type == PLAYER)
    {
        WHO->contr->orig_stats.Dex = value;
    };
    fix_player(WHO);
    Py_INCREF(Py_None);
    return Py_None;
};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Untested                                                                */
/*****************************************************************************/

static PyObject* CFSetHP(PyObject* self, PyObject* args)
{
    int value;
    long whoptr;

    if (!PyArg_ParseTuple(args,"li",&whoptr,&value))
        return NULL;

    if (value>16000) return NULL;
    if (value<0) return NULL;

    WHO->stats.hp = value;
    Py_INCREF(Py_None);
    return Py_None;
};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Untested                                                                */
/*****************************************************************************/

static PyObject* CFSetInt(PyObject* self, PyObject* args)
{
    int value;
    long whoptr;

    if (!PyArg_ParseTuple(args,"li",&whoptr,&value))
        return NULL;

    if (value>30) return NULL;
    if (value<-30) return NULL;

    WHO->stats.Int = value;
    if (WHO->type == PLAYER)
    {
        WHO->contr->orig_stats.Int = value;
    };
    fix_player(WHO);
    Py_INCREF(Py_None);
    return Py_None;
};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Untested                                                                */
/*****************************************************************************/

static PyObject* CFSetMaxHP(PyObject* self, PyObject* args)
{
    int value;
    long whoptr;

    if (!PyArg_ParseTuple(args,"li",&whoptr,&value))
        return NULL;

    if (value>16000) return NULL;
    if (value<0) return NULL;

    WHO->stats.maxhp = value;
    Py_INCREF(Py_None);
    return Py_None;
};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Untested                                                                */
/*****************************************************************************/

static PyObject* CFSetMaxSP(PyObject* self, PyObject* args)
{
    int value;
    long whoptr;

    if (!PyArg_ParseTuple(args,"li",&whoptr,&value))
        return NULL;

    if (value>16000) return NULL;
    if (value<0) return NULL;

    WHO->stats.maxsp = value;
    Py_INCREF(Py_None);
    return Py_None;
};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Untested                                                                */
/*****************************************************************************/

static PyObject* CFSetPow(PyObject* self, PyObject* args)
{
    int value;
    long whoptr;

    if (!PyArg_ParseTuple(args,"li",&whoptr,&value))
        return NULL;

    if (value>30) return NULL;
    if (value<-30) return NULL;

    WHO->stats.Pow = value;
    if (WHO->type == PLAYER)
    {
        WHO->contr->orig_stats.Pow = value;
    };
    fix_player(WHO);
    Py_INCREF(Py_None);
    return Py_None;
};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Untested                                                                */
/*****************************************************************************/

static PyObject* CFSetSP(PyObject* self, PyObject* args)
{
    int value;
    long whoptr;

    if (!PyArg_ParseTuple(args,"li",&whoptr,&value))
        return NULL;

    if (value>16000) return NULL;
    if (value<0) return NULL;

    WHO->stats.sp = value;
    Py_INCREF(Py_None);
    return Py_None;
};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Untested                                                                */
/*****************************************************************************/

static PyObject* CFSetStr(PyObject* self, PyObject* args)
{
    int value;
    long whoptr;

    if (!PyArg_ParseTuple(args,"li",&whoptr,&value))
        return NULL;

    if (value>30) return NULL;
    if (value<-30) return NULL;

    WHO->stats.Str = value;
    if (WHO->type == PLAYER)
    {
        WHO->contr->orig_stats.Str = value;
    };
    fix_player(WHO);
    Py_INCREF(Py_None);
    return Py_None;
};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Untested                                                                */
/*****************************************************************************/

static PyObject* CFSetWis(PyObject* self, PyObject* args)
{
    int value;
    long whoptr;

    if (!PyArg_ParseTuple(args,"li",&whoptr,&value))
        return NULL;

    if (value>30) return NULL;
    if (value<-30) return NULL;

    WHO->stats.Wis = value;
    if (WHO->type == PLAYER)
    {
        WHO->contr->orig_stats.Wis = value;
    };
    fix_player(WHO);
    Py_INCREF(Py_None);
    return Py_None;
};

/*****************************************************************************/
/* Kharkov update : Write and Message changed.                               */
/* Write(message,dest[,color])                                               */
/* -> Writes a message to a specific player.                                 */
/* Message(message,dest[,color])                                             */
/* -> Writes a message to a specific map (given by an object in this map).   */
/*****************************************************************************/
/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Untested                                                         */
/*****************************************************************************/

static PyObject* CFMessage(PyObject* self, PyObject* args)
{
    int   color = NDI_BLUE|NDI_UNIQUE;
    char *message;
    long  whoptr;

    if (!PyArg_ParseTuple(args,"sl|i",&message,&whoptr,&color))
        return NULL;

    GCFP.Value[0] = (void *)(&color);
    GCFP.Value[1] = (void *)(WHO->map);
    GCFP.Value[2] = (void *)(message);

    (PlugHooks[HOOK_NEWINFOMAP])(&GCFP);

    Py_INCREF(Py_None);
    return Py_None;
};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Untested                                                         */
/*****************************************************************************/

static PyObject* CFWrite(PyObject* self, PyObject* args)
{
    int   zero   = 0;
    char* message;
    long  whoptr = 0;
    int   color  = NDI_UNIQUE | NDI_ORANGE;

    if (!PyArg_ParseTuple(args,"sl|i",&message,&whoptr,&color))
        return NULL;

    GCFP.Value[0] = (void *)(&color);
    GCFP.Value[1] = (void *)(&zero);
    GCFP.Value[2] = (void *)(WHO);
    GCFP.Value[3] = (void *)(message);

    (PlugHooks[HOOK_NEWDRAWINFO])(&GCFP);

    Py_INCREF(Py_None);
    return Py_None;
};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Untested                                                                */
/*****************************************************************************/

static PyObject* CFIsOfType(PyObject* self, PyObject* args)
{
    int type;
    long whoptr;
    int value;

    if (!PyArg_ParseTuple(args,"li",&whoptr,&type))
        return NULL;
    if (WHO->type==type)
        value = 1;
    else
        value = 0;
    return Py_BuildValue("i",value);
};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Untested                                                                */
/*****************************************************************************/

static PyObject* CFGetType(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;
    return Py_BuildValue("i",WHO->type);
};

/* Those replace the old get-script... and set-script... system */
/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Untested                                                                */
/*****************************************************************************/

static PyObject* CFGetEventHandler(PyObject* self, PyObject* args)
{
    long whoptr;
    int eventnr;

    if (!PyArg_ParseTuple(args,"li",&whoptr,&eventnr))
        return NULL;
    return Py_BuildValue("s",WHO->event_hook[eventnr]);
};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Untested                                                                */
/*****************************************************************************/

static PyObject* CFSetEventHandler(PyObject* self, PyObject* args)
{
    long whoptr;
    int eventnr;
    char* scriptname;

    if (!PyArg_ParseTuple(args,"lis",&whoptr, &eventnr, &scriptname))
        return NULL;

    WHO->event_hook[eventnr] = add_string(scriptname);
    Py_INCREF(Py_None);
    return Py_None;
};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Untested                                                                */
/*****************************************************************************/

static PyObject* CFGetEventPlugin(PyObject* self, PyObject* args)
{
    long whoptr;
    int eventnr;

    if (!PyArg_ParseTuple(args,"li",&whoptr, &eventnr))
        return NULL;
    return Py_BuildValue("s", WHO->event_plugin[eventnr]);
};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Untested                                                                */
/*****************************************************************************/

static PyObject* CFSetEventPlugin(PyObject* self, PyObject* args)
{
    long whoptr;
    int eventnr;
    char* scriptname;

    if (!PyArg_ParseTuple(args,"lis",&whoptr,&eventnr,&scriptname))
        return NULL;

    WHO->event_plugin[eventnr] = add_string(scriptname);
    Py_INCREF(Py_None);
    return Py_None;
};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Untested                                                                */
/*****************************************************************************/

static PyObject* CFGetEventOptions(PyObject* self, PyObject* args)
{
    long whoptr;
    int eventnr;
    static char estr[4];
    if (!PyArg_ParseTuple(args,"li",&whoptr,&eventnr))
        return NULL;
    if (WHO->event_options[eventnr] == NULL)
    {
        strcpy(estr,"");
        return Py_BuildValue("s", estr);
    };
    return Py_BuildValue("s", WHO->event_options[eventnr]);
};

/*****************************************************************************/
/* Name   :                                                                  */
/* Python :                                                                  */
/* Status : Untested                                                                */
/*****************************************************************************/

static PyObject* CFSetEventOptions(PyObject* self, PyObject* args)
{
    long whoptr;
    int eventnr;
    char* scriptname;

    if (!PyArg_ParseTuple(args,"lis",&whoptr,&eventnr,&scriptname))
        return NULL;

    WHO->event_options[eventnr] = add_string(scriptname);

    Py_INCREF(Py_None);
    return Py_None;
};

/*****************************************************************************/
/* Name   : CFLoadObject                                                     */
/* Python : LoadObject(string)                                               */
/* Status : Untested                                                         */
/*****************************************************************************/
static PyObject* CFLoadObject(PyObject* self, PyObject* args)
{
    object *whoptr;
    char *dumpob;
    CFParm* CFR;

    if (!PyArg_ParseTuple(args, "s",&dumpob))
        return NULL;

    /* First step: We create the object */
    GCFP.Value[0] = (void *)(dumpob);
    CFR = (PlugHooks[HOOK_LOADOBJECT])(&GCFP);
    whoptr = (object *)(CFR->Value[0]);
    free(CFR);

    return Py_BuildValue("l",(long)(whoptr));
};

/*****************************************************************************/
/* Name   : CFSaveObject                                                     */
/* Python : SaveObject(what)                                                 */
/* Status : Untested                                                         */
/*****************************************************************************/
static PyObject* CFSaveObject(PyObject* self, PyObject* args)
{
    long whoptr;
    static char *result;
    CFParm* CFR;

    if (!PyArg_ParseTuple(args, "l",&whoptr))
        return NULL;

    GCFP.Value[0] = (void *)(WHO);
    CFR = (PlugHooks[HOOK_DUMPOBJECT])(&GCFP);
    result = (char *)(CFR->Value[0]);
    free(CFR);

    return Py_BuildValue("s",result);
};

/*****************************************************************************/
/* Name   : CFGetIP                                                          */
/* Python : GetIP(object)                                                    */
/* Status : Untested                                                         */
/*****************************************************************************/
static PyObject* CFGetIP(PyObject* self, PyObject* args)
{
    long whoptr;
    static char *result;

    if (!PyArg_ParseTuple(args, "l",&whoptr))
        return NULL;

    if (WHO->contr!=NULL)
    {
        result = WHO->contr->socket.host;
        return Py_BuildValue("s",result);
    }
    else
    {
        printf( "PYTHON - Error - This object has no controller\n");
        return Py_BuildValue("s","");
    };
};

/*****************************************************************************/
/* Name   : CFGetInventory                                                   */
/* Python : GetInventory(object)                                             */
/* Status : Untested                                                         */
/*****************************************************************************/
static PyObject* CFGetInventory(PyObject* self, PyObject* args)
{
    long whoptr;

    if (!PyArg_ParseTuple(args, "l",&whoptr))
        return NULL;

    return Py_BuildValue("l", (long)(WHO->inv));
};

/*****************************************************************************/
/* Name   : CFGetInternalName                                                */
/* Python : GetInternalName                                                  */
/* Status : Untested                                                         */
/*****************************************************************************/

static PyObject* CFGetInternalName(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;
    return Py_BuildValue("s",WHO->name);
};

/*****************************************************************************/
/* Name   : CFRegisterCommand                                                */
/* Python : RegisterCommand(cmdname,scriptname,speed)                        */
/* Status : Untested                                                         */
/*****************************************************************************/

static PyObject* CFRegisterCommand(PyObject* self, PyObject* args)
{
    char *cmdname;
    char *scriptname;
    double cmdspeed;
    int i;

    if (!PyArg_ParseTuple(args, "ssd",&cmdname,&scriptname,&cmdspeed))
        return NULL;

    for (i=0;i<NR_CUSTOM_CMD;i++)
    {
        if (CustomCommand[i].name != NULL)
        {
            if (!strcmp(CustomCommand[i].name,cmdname))
            {
                printf( "PYTHON - This command is already registered !\n");
                return NULL;
            }
        }
    }
    for (i=0;i<NR_CUSTOM_CMD;i++)
    {
        if (CustomCommand[i].name == NULL)
        {
            CustomCommand[i].name = (char *)(malloc(sizeof(char)*strlen(cmdname)));
            CustomCommand[i].script = (char *)(malloc(sizeof(char)*strlen(scriptname)));
            strcpy(CustomCommand[i].name,cmdname);
            strcpy(CustomCommand[i].script,scriptname);
            CustomCommand[i].speed = cmdspeed;
            i = NR_CUSTOM_CMD;
        }
    };

    Py_INCREF(Py_None);
    return Py_None;
};

/*****************************************************************************/
/* Name   : CFCostFlagFTrue                                                  */
/* Python : CostFlagFTrue ()                                                 */
/* Status : Untested                                                         */
/*****************************************************************************/

static PyObject* CFCostFlagFTrue(PyObject* self, PyObject* args)
{
    int flag=F_TRUE;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",flag);
};

/*****************************************************************************/
/* Name   : CFCostFlagFBuy                                                   */
/* Python : CostFlagFBuy ()                                                  */
/* Status : Untested                                                         */
/*****************************************************************************/

static PyObject* CFCostFlagFBuy(PyObject* self, PyObject* args)
{
    int flag=F_BUY;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",flag);
};

/*****************************************************************************/
/* Name   : CFCostFlagFSell                                                  */
/* Python : CostFlagFSell ()                                                 */
/* Status : Untested                                                         */
/*****************************************************************************/

static PyObject* CFCostFlagFSell(PyObject* self, PyObject* args)
{
    int flag=F_SELL;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",flag);
};

/*****************************************************************************/
/* Name   : CFGetObjectCost                                                  */
/* Python : GetObjectCost (buyer,object,type)                                */
/* Status : Stable                                                           */
/*****************************************************************************/

static PyObject* CFGetObjectCost(PyObject* self, PyObject* args)
{
    long whoptr;
    long whatptr;
    int flag;
    int cost;
    CFParm* CFR;
    if (!PyArg_ParseTuple(args,"lli",&whoptr,&whatptr,&flag))
        return NULL;
    if ((!WHAT) || (!WHO)) return Py_BuildValue("i",0);
    GCFP.Value[0] = (void *)(WHAT);
    GCFP.Value[1] = (void *)(WHO);
    GCFP.Value[2] = (void *)(&flag);
    CFR = (PlugHooks[HOOK_QUERYCOST])(&GCFP);
    cost=*(int*)(CFR->Value[0]);
    free (CFR);
    return Py_BuildValue("i",cost);
};

/*****************************************************************************/
/* Name   : CFGetObjectMoney                                                 */
/* Python : GetObjectMoney (buyer)                                           */
/* Status : Untested                                                         */
/*****************************************************************************/

static PyObject* CFGetObjectMoney(PyObject* self, PyObject* args)
{
    long whoptr;
    int amount;
    CFParm* CFR;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;
    if (!WHO) return Py_BuildValue("i",0);
    GCFP.Value[0] = (void *)(WHO);
    CFR = (PlugHooks[HOOK_QUERYMONEY])(&GCFP);
    amount=*(int*)(CFR->Value[0]);
    free (CFR);
    return Py_BuildValue("i",amount);
};

/*****************************************************************************/
/* Name   : CFPayForItem                                                     */
/* Python : PayForItem (buyer,object)                                        */
/* Status : Untested                                                         */
/*****************************************************************************/

static PyObject* CFPayForItem(PyObject* self, PyObject* args)
{
    long whoptr;
    long whatptr;
    int val;
    CFParm* CFR;
    if (!PyArg_ParseTuple(args,"ll",&whoptr,&whatptr))
        return NULL;
    if ((!WHAT) || (!WHO)) return Py_BuildValue("i",0);
    GCFP.Value[0] = (void *)(WHAT);
    GCFP.Value[1] = (void *)(WHO);
    CFR = (PlugHooks[HOOK_PAYFORITEM])(&GCFP);
    val=*(int*)(CFR->Value[0]);
    free (CFR);
    return Py_BuildValue("i",val);
};

/*****************************************************************************/
/* Name   : CFPayAmount                                                      */
/* Python : PayAmount (buyer,value)                                          */
/* Status : Stable                                                           */
/*****************************************************************************/

static PyObject* CFPayAmount(PyObject* self, PyObject* args)
{
    long whoptr;
    int to_pay;
    int val;
    CFParm* CFR;
    if (!PyArg_ParseTuple(args,"li",&whoptr,&to_pay))
        return NULL;
    if (!WHO) return Py_BuildValue("i",0);
    GCFP.Value[0] = (void *)(&to_pay);
    GCFP.Value[1] = (void *)(WHO);
    CFR = (PlugHooks[HOOK_PAYFORAMOUNT])(&GCFP);
    val=*(int*)(CFR->Value[0]);
    free (CFR);
    return Py_BuildValue("i",val);
};

/*****************************************************************************/
/* Name   : CFSendCustomCommand                                              */
/* Python : SendCustomCommand(who, 'customcommand')                          */
/* Status : Stable                                                           */
/*****************************************************************************/
static PyObject* CFSendCustomCommand(PyObject* self, PyObject* args)
{
    long whoptr;
    char *customcmd;

    if (!PyArg_ParseTuple(args,"ls",&whoptr,&customcmd))
        return NULL;
    GCFP.Value[0] = (void *)(WHO);
    GCFP.Value[1] = (void *)(customcmd);
    (PlugHooks[HOOK_SENDCUSTOMCOMMAND])(&GCFP);
    Py_INCREF(Py_None);
    return Py_None;
};

/*****************************************************************************/
/* Name   : CFGetHumidity                                                    */
/* Python : GetHumidity(x,y,map)                                             */
/* Status : Place Holder.                                                    */
/*****************************************************************************/
/* Returns the humidity level of a given square of a map.                    */
/*****************************************************************************/
static PyObject* CFGetHumidity(PyObject* self, PyObject* args)
{
    int val=0;
    int x, y;
    long map; /* mapstruct pointer */
    if (!PyArg_ParseTuple(args,"iil",&x,&y,&map))
        return NULL;

    return Py_BuildValue("i",val);
}

/*****************************************************************************/
/* Name   : CFGetTemperature                                                 */
/* Python : GetTemperature(x,y,map)                                          */
/* Status : Place Holder.                                                    */
/*****************************************************************************/
/* Returns the temperature of a given square of a map.                       */
/*****************************************************************************/
static PyObject* CFGetTemperature(PyObject* self, PyObject* args)
{
    int val=0;
    int x, y;
    long map; /* mapstruct pointer */
    if (!PyArg_ParseTuple(args,"iil",&x,&y,&map))
        return NULL;

    return Py_BuildValue("i",val);
}

/*****************************************************************************/
/* Name   : CFGetPressure                                                    */
/* Python : GetPressure(x,y,map)                                             */
/* Status : Place Holder.                                                    */
/*****************************************************************************/
/* Returns the pressure level of a given square of a map.                    */
/*****************************************************************************/
static PyObject* CFGetPressure(PyObject* self, PyObject* args)
{
    int val=0;
    int x, y;
    long map; /* mapstruct pointer */
    if (!PyArg_ParseTuple(args,"iil",&x,&y,&map))
        return NULL;

    return Py_BuildValue("i",val);
}

/*****************************************************************************/
/* Name    : CFSetVariable                                                   */
/* Python  : SetVariable(object,args)                                        */
/*****************************************************************************/
/* Changes an object according to an argument string. Equivalent of the DM   */
/* patch command.                                                            */
/*****************************************************************************/
static PyObject* CFSetVariable(PyObject* self, PyObject* args)
{
    char *txt;
    long whoptr;
    if (!PyArg_ParseTuple(args, "ls", &whoptr, &txt))
      return NULL;
    GCFP.Value[0] = (void *)(WHO);
    GCFP.Value[1] = (void *)txt;
    (PlugHooks[HOOK_SETVARIABLE])(&GCFP);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject* CFGetMapDir(PyObject* self, PyObject* args)
{
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("s",settings.mapdir);
}
static PyObject* CFGetUniqueDir(PyObject* self, PyObject* args)
{
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("s",settings.uniquedir);
}
static PyObject* CFGetTempDir(PyObject* self, PyObject* args)
{
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("s",settings.tmpdir);
}
static PyObject* CFGetConfigurationDir(PyObject* self, PyObject* args)
{
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("s",settings.confdir);
}
static PyObject* CFGetDataDir(PyObject* self, PyObject* args)
{
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("s",settings.datadir);
}
static PyObject* CFGetLocalDir(PyObject* self, PyObject* args)
{
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("s",settings.localdir);
}
static PyObject* CFGetPlayerDir(PyObject* self, PyObject* args)
{
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("s",settings.playerdir);
}

/*****************************************************************************/
/* The Plugin Management Part.                                               */
/* Most of the functions below should exist in any CF plugin. They are used  */
/* to glue the plugin to the server core. All functions follow the same      */
/* declaration scheme (taking a CFParm* arg, returning a CFParm) to make the */
/* plugin interface as general as possible. And since the loading of modules */
/* isn't time-critical, it is never a problem. It could also make using      */
/* programming languages other than C to write plugins a little easier, but  */
/* this has yet to be proven.                                                */
/*****************************************************************************/

/*****************************************************************************/
/* Called whenever a Hook Function needs to be connected to the plugin.      */
/*****************************************************************************/
MODULEAPI CFParm* registerHook(CFParm* PParm)
{
    int Pos;
    f_plugin Hook;
    Pos = *(int*)(PParm->Value[0]);
    Hook=(f_plugin)(PParm->Value[1]);
    PlugHooks[Pos]=Hook;
    return NULL;
};

/*****************************************************************************/
/* Called whenever an event is triggered, both Local and Global ones.        */
/*****************************************************************************/
/* Two types of events exist in CF:                                          */
/* - Local events: They are triggered by a single action on a single object. */
/*                 Nearly any object can trigger a local event               */
/*                 To warn the plugin of a local event, the map-maker needs  */
/*                 to use the event... tags in the objects of their maps.    */
/* - Global events: Those are triggered by actions concerning CF as a whole. */
/*                 Those events may or may not be triggered by a particular  */
/*                 object; they can't be specified by event... tags in maps. */
/*                 The plugin should register itself for all global events it*/
/*                 wants to be aware of.                                     */
/* Why those two types ? Local Events are made to manage interactions between*/
/* objects, for example to create complex scenarios. Global Events are made  */
/* to allow logging facilities and server management. Global Events tends to */
/* require more CPU time than Local Events, and are sometimes difficult to   */
/* bind to any specific object.                                              */
/*****************************************************************************/
MODULEAPI CFParm* triggerEvent(CFParm* PParm)
{
    /*CFParm *CFP; */
    int eventcode;
    static int result;

    
    eventcode = *(int *)(PParm->Value[0]);
    printf( "PYTHON - triggerEvent:: eventcode %d\n",eventcode);
    switch(eventcode)
    {
        case EVENT_NONE:
            printf( "PYTHON - Warning - EVENT_NONE requested\n");
            break;
        case EVENT_ATTACK:
        case EVENT_APPLY:
        case EVENT_DEATH:
        case EVENT_DROP:
        case EVENT_PICKUP:
        case EVENT_SAY:
        case EVENT_STOP:
        case EVENT_TELL:
        case EVENT_TIME:
        case EVENT_THROW:
        case EVENT_TRIGGER:
        case EVENT_CLOSE:
            result = HandleEvent(PParm);
            break;
        case EVENT_BORN:
        case EVENT_CRASH:
        case EVENT_LOGIN:
        case EVENT_LOGOUT:
        case EVENT_REMOVE:
        case EVENT_SHOUT:
        case EVENT_MAPENTER:
        case EVENT_MAPLEAVE:
        case EVENT_CLOCK:
        case EVENT_MAPRESET:
            result = HandleGlobalEvent(PParm);
            break;
    };
    GCFP.Value[0] = (void *)(&result);
    return &GCFP;
};

/*****************************************************************************/
/* Handles standard global events.                                            */
/*****************************************************************************/
MODULEAPI int HandleGlobalEvent(CFParm* PParm)
{
    FILE* Scriptfile;

    if (StackPosition == MAX_RECURSIVE_CALL)
    {
        printf( "Can't execute script - No space left of stack\n");
        return 0;
    };

    StackPosition++;

    switch(*(int *)(PParm->Value[0]))
    {
        case EVENT_CRASH:
            printf( "Unimplemented for now\n");
            break;
        case EVENT_BORN:
            StackActivator[StackPosition] = (object *)(PParm->Value[1]);
            /*printf( "Event BORN generated by %s\n",query_name(StackActivator[StackPosition])); */
            Scriptfile = fopen(create_pathname("python/python_born.py"),"r");
            if (Scriptfile != NULL)
            {
                PyRun_SimpleFile(Scriptfile,create_pathname("python/python_born.py"));
                fclose(Scriptfile);
            }
            break;
        case EVENT_LOGIN:
            StackActivator[StackPosition] = ((player *)(PParm->Value[1]))->ob;
            StackWho[StackPosition] = ((player *)(PParm->Value[1]))->ob;
            StackText[StackPosition] = (char *)(PParm->Value[2]);
            /*printf( "Event LOGIN generated by %s\n",query_name(StackActivator[StackPosition])); */
            /*printf( "IP is %s\n", (char *)(PParm->Value[2])); */
            Scriptfile = fopen(create_pathname("python/python_login.py"),"r");
            if (Scriptfile != NULL)
            {
                PyRun_SimpleFile(
                    Scriptfile,create_pathname("python/python_login.py"));
                fclose(Scriptfile);
            }
            break;
        case EVENT_LOGOUT:
            StackActivator[StackPosition] = ((player *)(PParm->Value[1]))->ob;
            StackWho[StackPosition] = ((player *)(PParm->Value[1]))->ob;
            StackText[StackPosition] = (char *)(PParm->Value[2]);
            /*printf( "Event LOGOUT generated by %s\n",query_name(StackActivator[StackPosition])); */
            Scriptfile = fopen(create_pathname("python/python_logout.py"),"r");
            if (Scriptfile != NULL)
            {
                PyRun_SimpleFile(Scriptfile,create_pathname("python/python_logout.py"));
                fclose(Scriptfile);
            }
            break;
        case EVENT_REMOVE:
            StackActivator[StackPosition] = (object *)(PParm->Value[1]);
            /*printf( "Event REMOVE generated by %s\n",query_name(StackActivator[StackPosition])); */

            Scriptfile = fopen(create_pathname("python/python_remove.py"),"r");
            if (Scriptfile != NULL)
            {
                PyRun_SimpleFile(Scriptfile,create_pathname("python/python_remove.py"));
                fclose(Scriptfile);
            }
            break;
        case EVENT_SHOUT:
            StackActivator[StackPosition] = (object *)(PParm->Value[1]);
            StackText[StackPosition] = (char *)(PParm->Value[2]);
            /*printf( "Event SHOUT generated by %s\n",query_name(StackActivator[StackPosition])); */

            /*printf( "Message shout is %s\n",StackText[StackPosition]); */
            Scriptfile = fopen(create_pathname("python/python_shout.py"),"r");
            if (Scriptfile != NULL)
            {
                PyRun_SimpleFile(Scriptfile, create_pathname("python/python_shout.py"));
                fclose(Scriptfile);
            }
            break;
        case EVENT_MAPENTER:
            StackActivator[StackPosition] = (object *)(PParm->Value[1]);
            /*printf( "Event MAPENTER generated by %s\n",query_name(StackActivator[StackPosition])); */

            Scriptfile = fopen(create_pathname("python/python_mapenter.py"),"r");
            if (Scriptfile != NULL)
            {
                PyRun_SimpleFile(Scriptfile, create_pathname("python/python_mapenter.py"));
                fclose(Scriptfile);
            }
            break;
        case EVENT_MAPLEAVE:
            StackActivator[StackPosition] = (object *)(PParm->Value[1]);
            /*printf( "Event MAPLEAVE generated by %s\n",query_name(StackActivator[StackPosition])); */

            Scriptfile = fopen(create_pathname("python/python_mapleave.py"),"r");
            if (Scriptfile != NULL)
            {
                PyRun_SimpleFile(Scriptfile, create_pathname("python/python_mapleave.py"));
                fclose(Scriptfile);
            }
            break;
        case EVENT_CLOCK:
            /* printf( "Event CLOCK generated\n"); */
            Scriptfile = fopen(create_pathname("/python/python_clock.py"),"r");
            if (Scriptfile != NULL)
            {
                PyRun_SimpleFile(Scriptfile, create_pathname("python/python_clock.py"));
                fclose(Scriptfile);
            }
            break;
        case EVENT_MAPRESET:
            StackText[StackPosition] = (char *)(PParm->Value[1]);/* Map name/path */
            printf( "Event MAPRESET generated by %s\n", StackText[StackPosition]);

            Scriptfile = fopen(create_pathname("python/python_mapreset.py"),"r");
            if (Scriptfile != NULL)
            {
                PyRun_SimpleFile(Scriptfile, create_pathname("python/python_mapreset.py"));
                fclose(Scriptfile);
            }
            break;
    };
    StackPosition--;
    return 0;
};

/*****************************************************************************/
/* Handles standard local events.                                            */
/*****************************************************************************/
MODULEAPI int HandleEvent(CFParm* PParm)
{
    FILE* Scriptfile;

#ifdef PYTHON_DEBUG
    printf( "PYTHON - HandleEvent:: got script file >%s<\n",(char *)(PParm->Value[9]));
#endif
    if (StackPosition == MAX_RECURSIVE_CALL)
    {
        printf( "PYTHON - Can't execute script - No space left of stack\n");
        return 0;
    };
    StackPosition++;
    StackActivator[StackPosition]   = (object *)(PParm->Value[1]);
    StackWho[StackPosition]         = (object *)(PParm->Value[2]);
    StackOther[StackPosition]       = (object *)(PParm->Value[3]);
    StackText[StackPosition]        = (char *)(PParm->Value[4]);
    StackParm1[StackPosition]       = *(int *)(PParm->Value[5]);
    StackParm2[StackPosition]       = *(int *)(PParm->Value[6]);
    StackParm3[StackPosition]       = *(int *)(PParm->Value[7]);
    StackParm4[StackPosition]       = *(int *)(PParm->Value[8]);
    StackReturn[StackPosition]      = 0;
    /* RunPythonScript(scriptname); */
    Scriptfile = fopen(create_pathname((char *)(PParm->Value[9])),"r");
    if (Scriptfile == NULL)
    {
        printf( "PYTHON - The Script file %s can't be opened\n",(char *)(PParm->Value[9]));
        return 0;
    };
    PyRun_SimpleFile(Scriptfile, create_pathname((char *)(PParm->Value[9])));
    fclose(Scriptfile);

#ifdef PYTHON_DEBUG
    printf( "PYTHON - HandleEvent:: script loaded (%s)!\n",(char *)(PParm->Value[9]));
#endif
    if (StackParm4[StackPosition] == SCRIPT_FIX_ALL)
    {
        if (StackOther[StackPosition] != NULL)
            fix_player(StackOther[StackPosition]);
        if (StackWho[StackPosition] != NULL)
            fix_player(StackWho[StackPosition]);
        if (StackActivator[StackPosition] != NULL)
            fix_player(StackActivator[StackPosition]);
    }
    else if (StackParm4[StackPosition] == SCRIPT_FIX_ACTIVATOR)
    {
        fix_player(StackActivator[StackPosition]);
    };
    StackPosition--;
    return StackReturn[StackPosition];
};

/*****************************************************************************/
/* Plugin initialization.                                                    */
/*****************************************************************************/
/* It is required that:                                                      */
/* - The first returned value of the CFParm structure is the "internal" name */
/*   of the plugin, used by objects to identify it.                          */
/* - The second returned value is the name "in clear" of the plugin, used for*/
/*   information purposes.                                                   */
/*****************************************************************************/
MODULEAPI CFParm* initPlugin(CFParm* PParm)
{
    printf("    CFPython Plugin loading.....\n");
    Py_Initialize();
    initCFPython();
    printf( "[Done]\n");
    GCFP.Value[0] = (void *) add_string(PLUGIN_NAME);
    GCFP.Value[1] = (void *) add_string(PLUGIN_VERSION);
    return &GCFP;
};

/*****************************************************************************/
/* Used to do cleanup before killing the plugin.                             */
/*****************************************************************************/
MODULEAPI CFParm* removePlugin(CFParm* PParm)
{
        return NULL;
};

/*****************************************************************************/
/* This function is called to ask various informations to the plugin.        */
/*****************************************************************************/
MODULEAPI CFParm* getPluginProperty(CFParm* PParm)
{
    
    double dblval = 0.0;
    int i;
    if (PParm!=NULL)
    {
        if(!strcmp((char *)(PParm->Value[0]),"command?"))
        {
            if(!strcmp((char *)(PParm->Value[1]),PLUGIN_NAME))
            {
                GCFP.Value[0] = PParm->Value[1];
                GCFP.Value[1] = &cmd_aboutPython;
                GCFP.Value[2] = &dblval;
                return &GCFP;
            }
            else
            {
                for (i=0;i<NR_CUSTOM_CMD;i++)
                {
                    if (CustomCommand[i].name != NULL)
                    {
                        if (!strcmp(CustomCommand[i].name,(char *)(PParm->Value[1])))
                        {
                            printf( "PYTHON - Running command %s\n",CustomCommand[i].name);
                            GCFP.Value[0] = PParm->Value[1];
                            GCFP.Value[1] = cmd_customPython;
                            GCFP.Value[2] = &(CustomCommand[i].speed);
                            NextCustomCommand = i;
                            return &GCFP;
                        }
                    }
                }
            }
        }
        else
        {
            printf( "PYTHON - Unknown property tag: %s\n",(char *)(PParm->Value[0]));
        };
    };
    return NULL;
};

MODULEAPI int cmd_customPython(object *op, char *params)
{
    FILE* Scriptfile;
#ifdef PYTHON_DEBUG
    printf( "PYTHON - cmd_customPython called:: script file: %s\n",CustomCommand[NextCustomCommand].script);
#endif
    if (StackPosition == MAX_RECURSIVE_CALL)
    {
        printf( "PYTHON - Can't execute script - No space left of stack\n");
        return 0;
    };
    StackPosition++;
    StackActivator[StackPosition]   = op;
    StackWho[StackPosition]         = op;
    StackOther[StackPosition]       = op;
    StackText[StackPosition]        = params;
    StackReturn[StackPosition]      = 0;
    Scriptfile = fopen(create_pathname(CustomCommand[NextCustomCommand].script),"r");
    if (Scriptfile == NULL)
    {
        printf( "PYTHON - The Script file %s can't be opened\n",CustomCommand[NextCustomCommand].script);
        return 0;
    };
    PyRun_SimpleFile(Scriptfile, create_pathname(CustomCommand[NextCustomCommand].script));
    fclose(Scriptfile);
    StackPosition--;
    return StackReturn[StackPosition+1];
};

MODULEAPI int cmd_aboutPython(object *op, char *params)
{
    int color = NDI_BLUE|NDI_UNIQUE;
    char message[1024];

    sprintf(message,"%s (Pegasus)\n(C)2001 by Gros. The Plugin code is under GPL.",PLUGIN_VERSION);
    GCFP.Value[0] = (void *)(&color);
    GCFP.Value[1] = (void *)(op->map);
    GCFP.Value[2] = (void *)(message);

    (PlugHooks[HOOK_NEWINFOMAP])(&GCFP);
    return 0;
};

/*****************************************************************************/
/* The postinitPlugin function is called by the server when the plugin load  */
/* is complete. It lets the opportunity to the plugin to register some events*/
/*****************************************************************************/
MODULEAPI CFParm* postinitPlugin(CFParm* PParm)
{
    int i;
    /* We can now register some global events if we want */
    /* We'll only register the global-only events :      */
    /* BORN, CRASH, LOGIN, LOGOUT, REMOVE, and SHOUT.    */
    /* The events APPLY, ATTACK, DEATH, DROP, PICKUP, SAY*/
    /* STOP, TELL, TIME, THROW and TRIGGER are already   */
    /* handled on a per-object basis and I simply don't  */
    /* see how useful they could be for the Python stuff.*/
    /* Registering them as local would be probably useful*/
    /* for extended logging facilities.                  */

    printf( "PYTHON - Start postinitPlugin.\n");
    
    GCFP.Value[1] = (void *)(add_string(PLUGIN_NAME));
    i = EVENT_BORN;
    GCFP.Value[0] = (void *)(&i);
    (PlugHooks[HOOK_REGISTEREVENT])(&GCFP);

    i = EVENT_CRASH;
    GCFP.Value[0] = (void *)(&i);
    (PlugHooks[HOOK_REGISTEREVENT])(&GCFP);

    i = EVENT_LOGIN;
    GCFP.Value[0] = (void *)(&i);
    (PlugHooks[HOOK_REGISTEREVENT])(&GCFP);

    i = EVENT_LOGOUT;
    GCFP.Value[0] = (void *)(&i);
    (PlugHooks[HOOK_REGISTEREVENT])(&GCFP);

    i = EVENT_REMOVE;
    GCFP.Value[0] = (void *)(&i);
    (PlugHooks[HOOK_REGISTEREVENT])(&GCFP);

    i = EVENT_SHOUT;
    GCFP.Value[0] = (void *)(&i);
    (PlugHooks[HOOK_REGISTEREVENT])(&GCFP);

    i = EVENT_MAPENTER;
    GCFP.Value[0] = (void *)(&i);
    (PlugHooks[HOOK_REGISTEREVENT])(&GCFP);

    i = EVENT_MAPLEAVE;
    GCFP.Value[0] = (void *)(&i);
    (PlugHooks[HOOK_REGISTEREVENT])(&GCFP);

/*    i = EVENT_CLOCK; */
/*    GCFP.Value[0] = (void *)(&i); */
/*    (PlugHooks[HOOK_REGISTEREVENT])(&GCFP); */

    i = EVENT_MAPRESET;
    GCFP.Value[0] = (void *)(&i);
    (PlugHooks[HOOK_REGISTEREVENT])(&GCFP);

    return NULL;
};

/*****************************************************************************/
/* Initializes the Python Interpreter.                                       */
/*****************************************************************************/
static PyObject* CFPythonError;
MODULEAPI void initCFPython()
{
        PyObject *m, *d;
        int i;

        printf( "PYTHON - Start initCFPython.\n");
        
        m = Py_InitModule("CFPython", CFPythonMethods);
        d = PyModule_GetDict(m);
        CFPythonError = PyErr_NewException("CFPython.error",NULL,NULL);
        PyDict_SetItemString(d,"error",CFPythonError);
        for (i=0;i<NR_CUSTOM_CMD;i++)
        {
            CustomCommand[i].name   = NULL;
            CustomCommand[i].script = NULL;
            CustomCommand[i].speed  = 0.0;
        };
};

