/*****************************************************************************/
/*  CrossFire, A Multiplayer game for the X Window System                    */
/*                                                                           */
/*****************************************************************************/

/*
 *  This code is placed under the GNU General Public Licence (GPL)
 *
 *  Copyright (C) 2001-2005 by Chachkoff Yann                               
 *  Copyright (C) 2006      by Marc Lehmann <cf@schmorpd.e>                 
 *                                                                          
 *  This program is free software; you can redistribute it and/or modify    
 *  it under the terms of the GNU General Public License as published by    
 *  the Free Software Foundation; either version 2 of the License, or       
 *  (at your option) any later version.                                     
 *                                                                          
 *  This program is distributed in the hope that it will be useful,         
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of          
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           
 *  GNU General Public License for more details.                            
 *                                                                          
 *  You should have received a copy of the GNU General Public License       
 *  along with this program; if not, write to the Free Software             
 *  Foundation, Inc. 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA 
*/

#include <EXTERN.h>
#include <perl.h>
#include <XSUB.h>

#undef save_long // clashes with libproto.h

#define PLUGIN_NAME    "perl"
#define PLUGIN_VERSION "cfperl 0.2"

#ifndef __CEXTRACT__
#include <plugin.h>
#endif

#undef MODULEAPI
#ifdef WIN32
#else
#define MODULEAPI
#endif

#include <plugin_common.h>

#include <stdarg.h>

#include "perlxsi.c"

typedef object    object_ornull;
typedef mapstruct mapstruct_ornull;

static f_plug_api gethook;
static f_plug_api registerGlobalEvent;
static f_plug_api unregisterGlobalEvent;
static f_plug_api systemDirectory;
static f_plug_api object_set_property;
static f_plug_api map_get_map;
static f_plug_api object_insert;

typedef struct
{
    object*     who;
    object*     activator;
    object*     third;
    char        message[1024];
    int         fix;
    int         event_code;
    char        extension[1024]; // name field, should invoke specific perl extension
    char        options[1024];   // slaying field of event_connectors
    int         returnvalue;
} CFPContext;

//static int current_command = -999;

static HV *obj_cache;
static PerlInterpreter *perl;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// garbage collect some perl objects, if possible
// all objects no longer referenced and empty are
// eligible for destruction.
void
clean_obj_cache ()
{
  static int count;

  if (++count & 7)
    return;
  
  int todo = 1000;
  do
    {
      I32 klen;
      char *key;
      HE *he = hv_iternext (obj_cache);

      if (he)
        {
          SV *sv = hv_iterval (obj_cache, he);

          // empty and unreferenced? nuke it
          if (SvREFCNT (sv) == 1 && SvREFCNT (SvRV (sv)) == 1 && !HvFILL ((HV *)(SvRV (sv))))
            {
              hv_delete (obj_cache, HeKEY (he), HeKLEN (he), G_DISCARD);
              todo++;
            }
        }
      else
        break;
    }
  while (--todo);
}

static SV *
newSVptr (void *ptr, const char *klass)
{
  SV *sv;

  if (!ptr)
    return &PL_sv_undef;

  sv = newSV (0);
  sv_magic (sv, 0, PERL_MAGIC_ext, (char *)ptr, 0);
  return sv_bless (newRV_noinc (sv), gv_stashpv (klass, 1));
}

static SV *
newSVptr_cached (void *ptr, const char *klass)
{
  SV *sv, **he;

  if (!ptr)
    return &PL_sv_undef;

  he = hv_fetch (obj_cache, (char *)&ptr, sizeof (ptr), 0);

  if (he)
    sv = *he;
  else
    {
      HV *hv = newHV ();
      sv_magic ((SV *)hv, 0, PERL_MAGIC_ext, (char *)ptr, 0);
      sv = sv_bless (newRV_noinc ((SV *)hv), gv_stashpv (klass, 1));
      hv_store (obj_cache, (char *)&ptr, sizeof (ptr), sv, 0);
    }

  return newSVsv (sv);
}

static void
clearSVptr (SV *sv)
{
  if (SvROK (sv))
    sv = SvRV (sv);

  hv_clear ((HV *)sv);
  sv_unmagic (sv, PERL_MAGIC_ext);
}

static long
SvPTR (SV *sv, const char *klass)
{
  if (!sv_derived_from (sv, klass))
    croak ("object of type %s expected", klass);

  MAGIC *mg = mg_find (SvRV (sv), PERL_MAGIC_ext);

  if (!mg)
    croak ("perl code used %s object, but C object is already destroyed, caught", klass);

  return (long)mg->mg_ptr;
}

static long
SvPTR_ornull (SV *sv, const char *klass)
{
  if (SvOK (sv))
    return SvPTR (sv, klass);
  else
    return 0;
}

SV *
newSVcfapi (int type, ...)
{
  SV *sv;

  va_list args;
  va_start (args, type);

  switch (type)
    {
      case CFAPI_INT:
        sv = newSViv (*va_arg (args, int *));
        break;

      case CFAPI_LONG:
        sv = newSViv (*va_arg (args, long *));
        break;

      case CFAPI_DOUBLE:
        sv = newSViv (*va_arg (args, double *));
        break;

      case CFAPI_STRING:
        {
          char *str = va_arg (args, char *);
          sv = str ? newSVpv (str, 0) : &PL_sv_undef;
        }
        break;

      case CFAPI_POBJECT:
        {
          object *obj = va_arg (args, object *);

          if (!obj)
            sv = &PL_sv_undef;
          else
            switch (*(int *)cf_object_get_property (obj, CFAPI_OBJECT_PROP_TYPE))
              {
                case MAP:
                  sv = newSVptr_cached (obj, "cf::object::map");
                  break;

                case PLAYER:
                  sv = newSVptr_cached (obj, "cf::object::player");
                  break;

                default:
                  sv = newSVptr_cached (obj, "cf::object");
                  break;
              }
        }
        break;

      case CFAPI_PMAP:
        sv = newSVptr (va_arg (args, mapstruct *), "cf::map");
        break;

      case CFAPI_PPLAYER:
        sv = newSVptr (va_arg (args, player *), "cf::player");
        break;

      case CFAPI_PARCH:
        sv = newSVptr (va_arg (args, archetype *), "cf::arch");
        break;

      case CFAPI_PPARTY:
        sv = newSVptr (va_arg (args, partylist *), "cf::party");
        break;

      case CFAPI_PREGION:
        sv = newSVptr (va_arg (args, region *), "cf::region");
        break;

      default:
        assert (("unhandled type in newSVcfapi", 0));
    }

  va_end (args);

  return sv;
}

/////////////////////////////////////////////////////////////////////////////

void
inject_event (const char *func, CFPContext *context)
{
  dSP;

  ENTER;
  SAVETMPS;

  PUSHMARK (SP);

  HV *hv = newHV ();
#define hv_context(type,addr,expr) hv_store (hv, #expr, sizeof (#expr) - 1, newSVcfapi (type, addr context->expr), 0)
  hv_context (CFAPI_POBJECT, ,who);
  hv_context (CFAPI_POBJECT, ,activator);
  hv_context (CFAPI_POBJECT, ,third);
  hv_context (CFAPI_STRING , ,message);
  hv_context (CFAPI_INT    ,&,fix);
  hv_context (CFAPI_INT    ,&,event_code);
  hv_context (CFAPI_STRING , ,options);
  hv_context (CFAPI_STRING , ,extension);

  XPUSHs (sv_2mortal (newRV_noinc ((SV *)hv)));

  PUTBACK;
  int count = call_pv (func, G_SCALAR | G_EVAL);
  SPAGAIN;

  if (SvTRUE (ERRSV))
    LOG (llevError, "event '%d' callback evaluation error: %s", context->event_code, SvPV_nolen (ERRSV));

  context->returnvalue = count > 0 ? POPi : 0;

  PUTBACK;
  FREETMPS;
  LEAVE;
}
  
/////////////////////////////////////////////////////////////////////////////

int
initPlugin (const char *iversion, f_plug_api gethooksptr)
{
  gethook = gethooksptr;
  printf (PLUGIN_VERSION " init\n");

  return 0;
}

static CommArray_s rtn_cmd;

int
runPluginCommand (object *obj, char *params)
{
  dSP;

  ENTER;
  SAVETMPS;

  PUSHMARK (SP);

  EXTEND (SP, 3);
  PUSHs (sv_2mortal (newSVpv (rtn_cmd.name, 0)));
  PUSHs (sv_2mortal (newSVcfapi (CFAPI_POBJECT, obj)));

  if (params)
    PUSHs (sv_2mortal (newSVpv (params, 0)));

  PUTBACK;
  int count = call_pv ("cf::inject_command", G_SCALAR | G_EVAL);
  SPAGAIN;

  if (SvTRUE (ERRSV))
    LOG (llevError, "command '%s' callback evaluation error: %s", rtn_cmd.name, SvPV_nolen (ERRSV));

  int returnvalue = count > 0 ? POPi : -1;

  PUTBACK;
  FREETMPS;
  LEAVE;

  return returnvalue;
}

void *
getPluginProperty (int *type, ...)
{
  va_list args;
  char *propname;
  int i;
  va_start (args, type);
  propname = va_arg (args, char *);
  //printf ("Property name: %s\n", propname);
 
  if (!strcmp (propname, "command?"))
    {
      if (!perl)
        return NULL;

      const char *cmdname = va_arg (args, const char *);
      HV *hv = get_hv ("cf::COMMAND", 1);
      SV **svp = hv_fetch (hv, cmdname, strlen (cmdname) + 1, 0);

      va_end (args);

      if (svp)
        {
          // this is totaly broken, should stash it into %COMMAND
          rtn_cmd.name = cmdname;
          rtn_cmd.time = SvNV (*svp);
          rtn_cmd.func = runPluginCommand;

          return &rtn_cmd;
        }
    }
  else if (!strcmp (propname, "Identification"))
    {
      va_end (args);
      return PLUGIN_NAME;
    }
  else if (!strcmp (propname, "FullName"))
    {
      va_end (args);
      return PLUGIN_VERSION;
    }
  else
    va_end (args);

  return NULL;
}

void *globalEventListener (int *type, ...);

int
postInitPlugin ()
{
  int hooktype = 1;
  int rtype = 0;

  printf (PLUGIN_VERSION " post init\n");

  registerGlobalEvent = gethook (&rtype, hooktype, "cfapi_system_register_global_event");
  unregisterGlobalEvent = gethook (&rtype, hooktype, "cfapi_system_unregister_global_event");
  systemDirectory = gethook (&rtype, hooktype, "cfapi_system_directory");
  object_set_property = gethook (&rtype, hooktype, "cfapi_object_set_property");
  map_get_map = gethook (&rtype, hooktype, "cfapi_map_get_map");
  object_insert = gethook (&rtype, hooktype, "cfapi_object_insert");

  cf_init_plugin (gethook);

  /* Pick the global events you want to monitor from this plugin */
  registerGlobalEvent (NULL, EVENT_BORN, PLUGIN_NAME, globalEventListener);
  registerGlobalEvent (NULL, EVENT_CLOCK, PLUGIN_NAME, globalEventListener);
  //registerGlobalEvent (NULL, EVENT_CRASH, PLUGIN_NAME, globalEventListener);
  registerGlobalEvent (NULL, EVENT_PLAYER_DEATH, PLUGIN_NAME, globalEventListener);
  registerGlobalEvent (NULL, EVENT_GKILL, PLUGIN_NAME, globalEventListener);
  registerGlobalEvent (NULL, EVENT_LOGIN, PLUGIN_NAME, globalEventListener);
  registerGlobalEvent (NULL, EVENT_LOGOUT, PLUGIN_NAME, globalEventListener);
  registerGlobalEvent (NULL, EVENT_MAPENTER, PLUGIN_NAME, globalEventListener);
  registerGlobalEvent (NULL, EVENT_MAPLEAVE, PLUGIN_NAME, globalEventListener);
  registerGlobalEvent (NULL, EVENT_MAPRESET, PLUGIN_NAME, globalEventListener);
  registerGlobalEvent (NULL, EVENT_REMOVE, PLUGIN_NAME, globalEventListener);
  registerGlobalEvent (NULL, EVENT_SHOUT, PLUGIN_NAME, globalEventListener);
  registerGlobalEvent (NULL, EVENT_TELL, PLUGIN_NAME, globalEventListener);
  registerGlobalEvent (NULL, EVENT_MUZZLE, PLUGIN_NAME, globalEventListener);
  registerGlobalEvent (NULL, EVENT_KICK, PLUGIN_NAME, globalEventListener);
  registerGlobalEvent (NULL, EVENT_FREE_OB, PLUGIN_NAME, globalEventListener);

  char *argv[] = {
    "",
    "-e"
    "BEGIN {"
    "   cf->bootstrap;"
    "   unshift @INC, cf::datadir ();"
    "}"
    ""
    "use cf;"
  };

  perl = perl_alloc ();
  perl_construct (perl);

  if (perl_parse (perl, xs_init, 2, argv, (char **)NULL) || perl_run (perl))
    {
      printf ("unable to initialize perl-interpreter, continuing without.\n");

      perl_destruct (perl);
      perl_free (perl);
      perl = 0;
    }
  else
    {
      obj_cache = newHV ();
    }

  return 0;
}

void *
globalEventListener (int *type, ...)
{
  va_list args;
  static int rv = 0;
  CFPContext context;
  char *buf;
  player *pl;
  object *op;

  if (!perl)
    return;

  memset (&context, 0, sizeof (context));

  va_start (args, type);
  context.event_code = va_arg (args, int);

  switch (context.event_code)
    {
      case EVENT_CRASH:
        printf ("Unimplemented for now\n");
        break;

      case EVENT_MAPENTER:
      case EVENT_MAPLEAVE:
      case EVENT_FREE_OB:
      case EVENT_BORN:
      case EVENT_REMOVE:
        context.activator = va_arg (args, object *);
        break;

      case EVENT_PLAYER_DEATH:
        context.who = va_arg (args, object *);
        break;

      case EVENT_GKILL:
        context.who = va_arg (args, object *);
        context.activator = va_arg (args, object *);
        break;

      case EVENT_LOGIN:
      case EVENT_LOGOUT:
        pl = va_arg (args, player *);
        context.activator = pl->ob;
        buf = va_arg (args, char *);
        if (buf != 0)
          strncpy (context.message, buf, sizeof (context.message));
        break;

      case EVENT_SHOUT:
      case EVENT_MUZZLE:
      case EVENT_KICK:
        context.activator = va_arg (args, object *);
        buf = va_arg (args, char *);
        if (buf != 0)
          strncpy (context.message, buf, sizeof (context.message));
        break;

      case EVENT_CLOCK:
        clean_obj_cache ();
        break;

      case EVENT_TELL:
        break;

      case EVENT_MAPRESET:
        buf = va_arg (args, char *);
        if (buf != 0)
          strncpy (context.message, buf, sizeof (context.message));
        break;
    }

  va_end (args);

  if (context.event_code == EVENT_FREE_OB)
    {
      SV *sv = hv_delete (obj_cache, (char *)&context.activator, sizeof (void *), 0);

      if (sv)
        clearSVptr (sv);
    }
  else
    inject_event ("cf::inject_global_event", &context);
  
  rv = context.returnvalue;

  return &rv;
}

void *
eventListener (int *type, ...)
{
  static int rv = 0;
  va_list args;
  char *buf;
  CFPContext context;

  if (!perl)
    return;

  memset (&context, 0, sizeof (context));

  va_start (args, type);

  context.who = va_arg (args, object *);
  context.event_code = va_arg (args, int);
  context.activator = va_arg (args, object *);
  context.third = va_arg (args, object *);

  buf = va_arg (args, char *);
  if (buf != 0)
    strncpy (context.message, buf, sizeof (context.message));

  context.fix = va_arg (args, int);
  strncpy (context.extension, va_arg (args, char *), sizeof (context.extension));
  strncpy (context.options, va_arg (args, char *), sizeof (context.options));
  context.returnvalue = 0;
  va_end (args);

  inject_event ("cf::inject_event", &context);
  
  rv = context.returnvalue;
  return &rv;
}

int
closePlugin ()
{
  printf (PLUGIN_VERSION " closing\n");

  if (perl)
    {
      perl_destruct (perl);
      perl_free (perl);
      perl = 0;
    }

  return 0;
}

MODULE = cf             PACKAGE = cf         PREFIX = cf_

BOOT:
{
  HV *stash = gv_stashpv ("cf", 1);

  static const struct {
    const char *name;
    IV iv;
  } *civ, const_iv[] = {
#   define const_iv(name) { # name, (IV)name },
    const_iv (llevError)
    const_iv (llevInfo)
    const_iv (llevDebug)
    const_iv (llevMonster)

    const_iv (PLAYER)
    const_iv (ROD)
    const_iv (TREASURE)
    const_iv (POTION)
    const_iv (FOOD)
    const_iv (POISON)
    const_iv (BOOK)
    const_iv (CLOCK)
    const_iv (LIGHTNING)
    const_iv (ARROW)
    const_iv (BOW)
    const_iv (WEAPON)
    const_iv (ARMOUR)
    const_iv (PEDESTAL)
    const_iv (ALTAR)
    const_iv (CONFUSION)
    const_iv (LOCKED_DOOR)
    const_iv (SPECIAL_KEY)
    const_iv (MAP)
    const_iv (DOOR)
    const_iv (KEY)
    const_iv (TIMED_GATE)
    const_iv (TRIGGER)
    const_iv (GRIMREAPER)
    const_iv (MAGIC_EAR)
    const_iv (TRIGGER_BUTTON)
    const_iv (TRIGGER_ALTAR)
    const_iv (TRIGGER_PEDESTAL)
    const_iv (SHIELD)
    const_iv (HELMET)
    const_iv (HORN)
    const_iv (MONEY)
    const_iv (CLASS)
    const_iv (GRAVESTONE)
    const_iv (AMULET)
    const_iv (PLAYERMOVER)
    const_iv (TELEPORTER)
    const_iv (CREATOR)
    const_iv (SKILL)
    const_iv (EXPERIENCE)
    const_iv (EARTHWALL)
    const_iv (GOLEM)
    const_iv (THROWN_OBJ)
    const_iv (BLINDNESS)
    const_iv (GOD)
    const_iv (DETECTOR)
    const_iv (TRIGGER_MARKER)
    const_iv (DEAD_OBJECT)
    const_iv (DRINK)
    const_iv (MARKER)
    const_iv (HOLY_ALTAR)
    const_iv (PLAYER_CHANGER)
    const_iv (BATTLEGROUND)
    const_iv (PEACEMAKER)
    const_iv (GEM)
    const_iv (FIREWALL)
    const_iv (ANVIL)
    const_iv (CHECK_INV)
    const_iv (MOOD_FLOOR)
    const_iv (EXIT)
    const_iv (ENCOUNTER)
    const_iv (SHOP_FLOOR)
    const_iv (SHOP_MAT)
    const_iv (RING)
    const_iv (FLOOR)
    const_iv (FLESH)
    const_iv (INORGANIC)
    const_iv (SKILL_TOOL)
    const_iv (LIGHTER)
    const_iv (TRAP_PART)
    const_iv (WALL)
    const_iv (LIGHT_SOURCE)
    const_iv (MISC_OBJECT)
    const_iv (MONSTER)
    const_iv (SPAWN_GENERATOR)
    const_iv (LAMP)
    const_iv (DUPLICATOR)
    const_iv (TOOL)
    const_iv (SPELLBOOK)
    const_iv (BUILDFAC)
    const_iv (CLOAK)
    const_iv (SPINNER)
    const_iv (GATE)
    const_iv (BUTTON)
    const_iv (CF_HANDLE)
    const_iv (HOLE)
    const_iv (TRAPDOOR)
    const_iv (SIGN)
    const_iv (BOOTS)
    const_iv (GLOVES)
    const_iv (SPELL)
    const_iv (SPELL_EFFECT)
    const_iv (CONVERTER)
    const_iv (BRACERS)
    const_iv (POISONING)
    const_iv (SAVEBED)
    const_iv (POISONCLOUD)
    const_iv (FIREHOLES)
    const_iv (WAND)
    const_iv (SCROLL)
    const_iv (DIRECTOR)
    const_iv (GIRDLE)
    const_iv (FORCE)
    const_iv (POTION_EFFECT)
    const_iv (EVENT_CONNECTOR)
    const_iv (CLOSE_CON)
    const_iv (CONTAINER)
    const_iv (ARMOUR_IMPROVER)
    const_iv (WEAPON_IMPROVER)
    const_iv (SKILLSCROLL)
    const_iv (DEEP_SWAMP)
    const_iv (IDENTIFY_ALTAR)
    const_iv (MENU)
    const_iv (RUNE)
    const_iv (TRAP)
    const_iv (POWER_CRYSTAL)
    const_iv (CORPSE)
    const_iv (DISEASE)
    const_iv (SYMPTOM)
    const_iv (BUILDER)
    const_iv (MATERIAL)
    const_iv (ITEM_TRANSFORMER)
    const_iv (QUEST)

    const_iv (ST_BD_BUILD)
    const_iv (ST_BD_REMOVE)
    const_iv (ST_MAT_FLOOR)
    const_iv (ST_MAT_WALL)
    const_iv (ST_MAT_ITEM)

    const_iv (AT_PHYSICAL)
    const_iv (AT_MAGIC)
    const_iv (AT_FIRE)
    const_iv (AT_ELECTRICITY)
    const_iv (AT_COLD)
    const_iv (AT_CONFUSION)
    const_iv (AT_ACID)
    const_iv (AT_DRAIN)
    const_iv (AT_WEAPONMAGIC)
    const_iv (AT_GHOSTHIT)
    const_iv (AT_POISON)
    const_iv (AT_SLOW)
    const_iv (AT_PARALYZE)
    const_iv (AT_TURN_UNDEAD)
    const_iv (AT_FEAR)
    const_iv (AT_CANCELLATION)
    const_iv (AT_DEPLETE)
    const_iv (AT_DEATH)
    const_iv (AT_CHAOS)
    const_iv (AT_COUNTERSPELL)
    const_iv (AT_GODPOWER)
    const_iv (AT_HOLYWORD)
    const_iv (AT_BLIND)
    const_iv (AT_INTERNAL)
    const_iv (AT_LIFE_STEALING)
    const_iv (AT_DISEASE)

    const_iv (QUEST_IN_PROGRESS)
    const_iv (QUEST_DONE_QUEST)
    const_iv (QUEST_DONE_TASK)
    const_iv (QUEST_START_QUEST)
    const_iv (QUEST_END_QUEST)
    const_iv (QUEST_START_TASK)
    const_iv (QUEST_END_TASK)
    const_iv (QUEST_OVERRIDE)
    const_iv (QUEST_ON_ACTIVATE)

    const_iv (WEAP_HIT)
    const_iv (WEAP_SLASH)
    const_iv (WEAP_PIERCE)
    const_iv (WEAP_CLEAVE)
    const_iv (WEAP_SLICE)
    const_iv (WEAP_STAB)
    const_iv (WEAP_WHIP)
    const_iv (WEAP_CRUSH)
    const_iv (WEAP_BLUD)

    const_iv (FLAG_ALIVE)
    const_iv (FLAG_WIZ)
    const_iv (FLAG_REMOVED)
    const_iv (FLAG_FREED)
    const_iv (FLAG_WAS_WIZ)
    const_iv (FLAG_APPLIED)
    const_iv (FLAG_UNPAID)
    const_iv (FLAG_USE_SHIELD)
    const_iv (FLAG_NO_PICK)
    const_iv (FLAG_ANIMATE)
    const_iv (FLAG_MONSTER)
    const_iv (FLAG_FRIENDLY)
    const_iv (FLAG_GENERATOR)
    const_iv (FLAG_IS_THROWN)
    const_iv (FLAG_AUTO_APPLY)
    const_iv (FLAG_TREASURE)
    const_iv (FLAG_PLAYER_SOLD)
    const_iv (FLAG_SEE_INVISIBLE)
    const_iv (FLAG_CAN_ROLL)
    const_iv (FLAG_OVERLAY_FLOOR)
    const_iv (FLAG_IS_TURNABLE)
    const_iv (FLAG_IS_USED_UP)
    const_iv (FLAG_IDENTIFIED)
    const_iv (FLAG_REFLECTING)
    const_iv (FLAG_CHANGING)
    const_iv (FLAG_SPLITTING)
    const_iv (FLAG_HITBACK)
    const_iv (FLAG_STARTEQUIP)
    const_iv (FLAG_BLOCKSVIEW)
    const_iv (FLAG_UNDEAD)
    const_iv (FLAG_SCARED)
    const_iv (FLAG_UNAGGRESSIVE)
    const_iv (FLAG_REFL_MISSILE)
    const_iv (FLAG_REFL_SPELL)
    const_iv (FLAG_NO_MAGIC)
    const_iv (FLAG_NO_FIX_PLAYER)
    const_iv (FLAG_IS_LIGHTABLE)
    const_iv (FLAG_TEAR_DOWN)
    const_iv (FLAG_RUN_AWAY)
    const_iv (FLAG_PICK_UP)
    const_iv (FLAG_UNIQUE)
    const_iv (FLAG_NO_DROP)
    const_iv (FLAG_WIZCAST)
    const_iv (FLAG_CAST_SPELL)
    const_iv (FLAG_USE_SCROLL)
    const_iv (FLAG_USE_RANGE)
    const_iv (FLAG_USE_BOW)
    const_iv (FLAG_USE_ARMOUR)
    const_iv (FLAG_USE_WEAPON)
    const_iv (FLAG_USE_RING)
    const_iv (FLAG_READY_RANGE)
    const_iv (FLAG_READY_BOW)
    const_iv (FLAG_XRAYS)
    const_iv (FLAG_NO_APPLY)
    const_iv (FLAG_IS_FLOOR)
    const_iv (FLAG_LIFESAVE)
    const_iv (FLAG_NO_STRENGTH)
    const_iv (FLAG_SLEEP)
    const_iv (FLAG_STAND_STILL)
    const_iv (FLAG_RANDOM_MOVE)
    const_iv (FLAG_ONLY_ATTACK)
    const_iv (FLAG_CONFUSED)
    const_iv (FLAG_STEALTH)
    const_iv (FLAG_WIZPASS)
    const_iv (FLAG_IS_LINKED)
    const_iv (FLAG_CURSED)
    const_iv (FLAG_DAMNED)
    const_iv (FLAG_SEE_ANYWHERE)
    const_iv (FLAG_KNOWN_MAGICAL)
    const_iv (FLAG_KNOWN_CURSED)
    const_iv (FLAG_CAN_USE_SKILL)
    const_iv (FLAG_BEEN_APPLIED)
    const_iv (FLAG_READY_SCROLL)
    const_iv (FLAG_USE_ROD)
    const_iv (FLAG_USE_HORN)
    const_iv (FLAG_MAKE_INVIS)
    const_iv (FLAG_INV_LOCKED)
    const_iv (FLAG_IS_WOODED)
    const_iv (FLAG_IS_HILLY)
    const_iv (FLAG_READY_SKILL)
    const_iv (FLAG_READY_WEAPON)
    const_iv (FLAG_NO_SKILL_IDENT)
    const_iv (FLAG_BLIND)
    const_iv (FLAG_SEE_IN_DARK)
    const_iv (FLAG_IS_CAULDRON)
    const_iv (FLAG_NO_STEAL)
    const_iv (FLAG_ONE_HIT)
    const_iv (FLAG_CLIENT_SENT)
    const_iv (FLAG_BERSERK)
    const_iv (FLAG_NEUTRAL)
    const_iv (FLAG_NO_ATTACK)
    const_iv (FLAG_NO_DAMAGE)
    const_iv (FLAG_OBJ_ORIGINAL)
    const_iv (FLAG_OBJ_SAVE_ON_OVL)
    const_iv (FLAG_ACTIVATE_ON_PUSH)
    const_iv (FLAG_ACTIVATE_ON_RELEASE)
    const_iv (FLAG_IS_WATER)
    const_iv (FLAG_CONTENT_ON_GEN)
    const_iv (FLAG_IS_A_TEMPLATE)
    const_iv (FLAG_IS_BUILDABLE)
    const_iv (FLAG_AFK)

    const_iv (NDI_BLACK)
    const_iv (NDI_WHITE)
    const_iv (NDI_NAVY)
    const_iv (NDI_RED)
    const_iv (NDI_ORANGE)
    const_iv (NDI_BLUE)
    const_iv (NDI_DK_ORANGE)
    const_iv (NDI_GREEN)
    const_iv (NDI_LT_GREEN)
    const_iv (NDI_GREY)
    const_iv (NDI_BROWN)
    const_iv (NDI_GOLD)
    const_iv (NDI_TAN)
    const_iv (NDI_MAX_COLOR)
    const_iv (NDI_COLOR_MASK)
    const_iv (NDI_UNIQUE)
    const_iv (NDI_ALL)

    const_iv (F_APPLIED)
    const_iv (F_LOCATION)
    const_iv (F_UNPAID)
    const_iv (F_MAGIC)
    const_iv (F_CURSED)
    const_iv (F_DAMNED)
    const_iv (F_OPEN)
    const_iv (F_NOPICK)
    const_iv (F_LOCKED)

    const_iv (P_BLOCKSVIEW)
    const_iv (P_NO_MAGIC)
    const_iv (P_IS_ALIVE)
    const_iv (P_NO_CLERIC)
    const_iv (P_NEED_UPDATE)
    const_iv (P_NO_ERROR)
    const_iv (P_OUT_OF_MAP)
    const_iv (P_NEW_MAP)

    const_iv (UP_OBJ_INSERT)
    const_iv (UP_OBJ_REMOVE)
    const_iv (UP_OBJ_CHANGE)
    const_iv (UP_OBJ_FACE)

    const_iv (INS_NO_MERGE)
    const_iv (INS_ABOVE_FLOOR_ONLY)
    const_iv (INS_NO_WALK_ON)
    const_iv (INS_ON_TOP)
    const_iv (INS_BELOW_ORIGINATOR)
    const_iv (INS_MAP_LOAD)

    const_iv (WILL_APPLY_HANDLE)
    const_iv (WILL_APPLY_TREASURE)
    const_iv (WILL_APPLY_EARTHWALL)
    const_iv (WILL_APPLY_DOOR)
    const_iv (WILL_APPLY_FOOD)
  };

  for (civ = const_iv + sizeof (const_iv) / sizeof (const_iv [0]); civ-- > const_iv; )
    newCONSTSUB (stash, (char *)civ->name, newSViv (civ->iv));

  static const struct {
    const char *name;
    IV iv;
  } *event, event_list[] = {
#   define const_event(name) { # name, (IV)EVENT_ ## name },
    const_event (NONE)
    const_event (APPLY)
    const_event (ATTACK)
    const_event (DEATH)
    const_event (DROP)
    const_event (PICKUP)
    const_event (SAY)
    const_event (STOP)
    const_event (TIME)
    const_event (THROW)
    const_event (TRIGGER)
    const_event (CLOSE)
    const_event (TIMER)
    const_event (MOVE)

    const_event (BORN)
    const_event (CLOCK)
    const_event (CRASH)
    const_event (PLAYER_DEATH)
    const_event (GKILL)
    const_event (LOGIN)
    const_event (LOGOUT)
    const_event (MAPENTER)
    const_event (MAPLEAVE)
    const_event (MAPRESET)
    const_event (REMOVE)
    const_event (SHOUT)
    const_event (TELL)
    const_event (MUZZLE)
    const_event (KICK)
    //const_event (FREE_OB)
  };

  AV *av = get_av ("cf::EVENT", 1);

  for (event = event_list + sizeof (event_list) / sizeof (event_list [0]); event-- > event_list; )
    av_store (av, event->iv, newSVpv ((char *)event->name, 0));

  static const struct {
    int dtype;
    const char *name;
    IV idx;
  } *cprop, prop_table[] = {
#   define prop(type, name) { type, # name, (IV) CFAPI_ ## name },
    prop (CFAPI_INT, MAP_PROP_FLAGS)
    prop (CFAPI_INT, MAP_PROP_DIFFICULTY)
    prop (CFAPI_STRING, MAP_PROP_PATH)
    prop (CFAPI_STRING, MAP_PROP_TMPNAME)
    prop (CFAPI_STRING, MAP_PROP_NAME)
    prop (CFAPI_INT, MAP_PROP_RESET_TIME)
    prop (CFAPI_INT, MAP_PROP_RESET_TIMEOUT)
    prop (CFAPI_INT, MAP_PROP_PLAYERS)
    prop (CFAPI_INT, MAP_PROP_DARKNESS)
    prop (CFAPI_INT, MAP_PROP_WIDTH)
    prop (CFAPI_INT, MAP_PROP_HEIGHT)
    prop (CFAPI_INT, MAP_PROP_ENTER_X)
    prop (CFAPI_INT, MAP_PROP_ENTER_Y)
    prop (CFAPI_INT, MAP_PROP_TEMPERATURE)
    prop (CFAPI_INT, MAP_PROP_PRESSURE)
    prop (CFAPI_INT, MAP_PROP_HUMIDITY)
    prop (CFAPI_INT, MAP_PROP_WINDSPEED)
    prop (CFAPI_INT, MAP_PROP_WINDDIR)
    prop (CFAPI_INT, MAP_PROP_SKY)
    prop (CFAPI_INT, MAP_PROP_WPARTX)
    prop (CFAPI_INT, MAP_PROP_WPARTY)
    prop (CFAPI_STRING, MAP_PROP_MESSAGE)
    prop (CFAPI_PMAP, MAP_PROP_NEXT)
    prop (CFAPI_PREGION, MAP_PROP_REGION)
    prop (CFAPI_POBJECT, OBJECT_PROP_OB_ABOVE)
    prop (CFAPI_POBJECT, OBJECT_PROP_OB_BELOW)
    prop (CFAPI_POBJECT, OBJECT_PROP_NEXT_ACTIVE_OB)
    prop (CFAPI_POBJECT, OBJECT_PROP_PREV_ACTIVE_OB)
    prop (CFAPI_POBJECT, OBJECT_PROP_INVENTORY)
    prop (CFAPI_POBJECT, OBJECT_PROP_ENVIRONMENT)
    prop (CFAPI_POBJECT, OBJECT_PROP_HEAD)
    prop (CFAPI_POBJECT, OBJECT_PROP_CONTAINER)
    prop (CFAPI_PMAP, OBJECT_PROP_MAP)
    prop (CFAPI_INT, OBJECT_PROP_COUNT)
    prop (CFAPI_INT, OBJECT_PROP_REFCOUNT)
    prop (CFAPI_STRING, OBJECT_PROP_NAME)
    prop (CFAPI_STRING, OBJECT_PROP_NAME_PLURAL)
    prop (CFAPI_STRING, OBJECT_PROP_TITLE)
    prop (CFAPI_STRING, OBJECT_PROP_RACE)
    prop (CFAPI_STRING, OBJECT_PROP_SLAYING)
    prop (CFAPI_STRING, OBJECT_PROP_SKILL)
    prop (CFAPI_STRING, OBJECT_PROP_MESSAGE)
    prop (CFAPI_STRING, OBJECT_PROP_LORE)
    prop (CFAPI_INT, OBJECT_PROP_X)
    prop (CFAPI_INT, OBJECT_PROP_Y)
    prop (CFAPI_DOUBLE, OBJECT_PROP_SPEED)
    prop (CFAPI_DOUBLE, OBJECT_PROP_SPEED_LEFT)
    prop (CFAPI_INT, OBJECT_PROP_NROF)
    prop (CFAPI_INT, OBJECT_PROP_DIRECTION)
    prop (CFAPI_INT, OBJECT_PROP_FACING)
    prop (CFAPI_INT, OBJECT_PROP_TYPE)
    prop (CFAPI_INT, OBJECT_PROP_SUBTYPE)
    prop (CFAPI_INT, OBJECT_PROP_CLIENT_TYPE)
    prop (CFAPI_INT, OBJECT_PROP_ATTACK_TYPE)
    prop (CFAPI_INT, OBJECT_PROP_PATH_ATTUNED)
    prop (CFAPI_INT, OBJECT_PROP_PATH_REPELLED)
    prop (CFAPI_INT, OBJECT_PROP_PATH_DENIED)
    prop (CFAPI_INT, OBJECT_PROP_MATERIAL)
    prop (CFAPI_STRING, OBJECT_PROP_MATERIAL_NAME)
    prop (CFAPI_INT, OBJECT_PROP_MAGIC)
    prop (CFAPI_INT, OBJECT_PROP_VALUE)
    prop (CFAPI_INT, OBJECT_PROP_LEVEL)
    prop (CFAPI_INT, OBJECT_PROP_LAST_HEAL)
    prop (CFAPI_INT, OBJECT_PROP_LAST_SP)
    prop (CFAPI_INT, OBJECT_PROP_LAST_GRACE)
    prop (CFAPI_INT, OBJECT_PROP_LAST_EAT)
    prop (CFAPI_INT, OBJECT_PROP_INVISIBLE_TIME)
    prop (CFAPI_INT, OBJECT_PROP_PICK_UP)
    prop (CFAPI_INT, OBJECT_PROP_ITEM_POWER)
    prop (CFAPI_INT, OBJECT_PROP_GEN_SP_ARMOUR)
    prop (CFAPI_INT, OBJECT_PROP_WEIGHT)
    prop (CFAPI_INT, OBJECT_PROP_WEIGHT_LIMIT)
    prop (CFAPI_INT, OBJECT_PROP_CARRYING)
    prop (CFAPI_INT, OBJECT_PROP_GLOW_RADIUS)
    prop (CFAPI_LONG, OBJECT_PROP_PERM_EXP)
    prop (CFAPI_POBJECT, OBJECT_PROP_CURRENT_WEAPON)
    prop (CFAPI_POBJECT, OBJECT_PROP_ENEMY)
    prop (CFAPI_POBJECT, OBJECT_PROP_ATTACKED_BY)
    prop (CFAPI_INT, OBJECT_PROP_RUN_AWAY)
    prop (CFAPI_POBJECT, OBJECT_PROP_CHOSEN_SKILL)
    prop (CFAPI_INT, OBJECT_PROP_HIDDEN)
    prop (CFAPI_INT, OBJECT_PROP_MOVE_STATUS)
    prop (CFAPI_INT, OBJECT_PROP_MOVE_TYPE)
    prop (CFAPI_POBJECT, OBJECT_PROP_SPELL_ITEM)
    prop (CFAPI_DOUBLE, OBJECT_PROP_EXP_MULTIPLIER)
    prop (CFAPI_PARCH, OBJECT_PROP_ARCHETYPE)
    prop (CFAPI_PARCH, OBJECT_PROP_OTHER_ARCH)
    prop (CFAPI_STRING, OBJECT_PROP_CUSTOM_NAME)
    prop (CFAPI_INT, OBJECT_PROP_ANIM_SPEED)
    prop (CFAPI_INT, OBJECT_PROP_FRIENDLY)
    prop (CFAPI_STRING, OBJECT_PROP_SHORT_NAME)
    prop (CFAPI_INT, OBJECT_PROP_MAGICAL)
    prop (CFAPI_INT, OBJECT_PROP_LUCK)
    prop (CFAPI_LONG, OBJECT_PROP_EXP)
    prop (CFAPI_POBJECT, OBJECT_PROP_OWNER)
    prop (CFAPI_POBJECT, OBJECT_PROP_PRESENT)
    prop (CFAPI_INT, OBJECT_PROP_CHEATER)
    prop (CFAPI_INT, OBJECT_PROP_MERGEABLE)
    prop (CFAPI_INT, OBJECT_PROP_PICKABLE)
    prop (CFAPI_INT, OBJECT_PROP_STR)
    prop (CFAPI_INT, OBJECT_PROP_DEX)
    prop (CFAPI_INT, OBJECT_PROP_CON)
    prop (CFAPI_INT, OBJECT_PROP_WIS)
    prop (CFAPI_INT, OBJECT_PROP_INT)
    prop (CFAPI_INT, OBJECT_PROP_POW)
    prop (CFAPI_INT, OBJECT_PROP_CHA)
    prop (CFAPI_INT, OBJECT_PROP_WC)
    prop (CFAPI_INT, OBJECT_PROP_AC)
    prop (CFAPI_INT, OBJECT_PROP_HP)
    prop (CFAPI_INT, OBJECT_PROP_SP)
    prop (CFAPI_INT, OBJECT_PROP_GP)
    prop (CFAPI_INT, OBJECT_PROP_FP)
    prop (CFAPI_INT, OBJECT_PROP_MAXHP)
    prop (CFAPI_INT, OBJECT_PROP_MAXSP)
    prop (CFAPI_INT, OBJECT_PROP_MAXGP)
    prop (CFAPI_INT, OBJECT_PROP_DAM)
    prop (CFAPI_STRING, OBJECT_PROP_GOD)
    prop (CFAPI_STRING, OBJECT_PROP_ARCH_NAME)
    prop (CFAPI_INT, OBJECT_PROP_INVISIBLE)
    prop (CFAPI_INT, OBJECT_PROP_FACE)
  };

  HV *prop_type = get_hv ("cf::PROP_TYPE", 1);
  HV *prop_idx  = get_hv ("cf::PROP_IDX", 1);

  for (cprop = prop_table + sizeof (prop_table) / sizeof (prop_table [0]); cprop-- > prop_table; )
    {
      hv_store (prop_type, cprop->name, strlen (cprop->name), newSViv (cprop->dtype), 0);
      hv_store (prop_idx,  cprop->name, strlen (cprop->name), newSViv (cprop->idx  ), 0);
    }
}

void
LOG (int level, char *msg)
	PROTOTYPE: $$
	C_ARGS: level, "%s", msg

char *
cf_get_maps_directory (char *path)
	PROTOTYPE: $
	ALIAS: maps_directory = 0

char *
mapdir ()
	PROTOTYPE:
	ALIAS:
        mapdir    = 0
        uniquedir = 1
        tmpdir    = 2
        confdir   = 3
        localdir  = 4
        playerdir = 5
        datadir   = 6
        CODE:
{
        int unused_type;
        RETVAL = (char *)systemDirectory (&unused_type, ix);
}
	OUTPUT: RETVAL

int
cf_find_animation (char *text)
	PROTOTYPE: $

MODULE = cf        PACKAGE = cf::object         PREFIX = cf_object_

SV *
get_property (object *obj, int type, int idx)
	CODE:
        RETVAL = newSVcfapi (type, cf_object_get_property (obj, idx));
	OUTPUT: RETVAL

SV *
set_property (object *obj, int type, int idx, SV *newval)
	CODE:
        switch (type)
          {
            case CFAPI_INT:
              cf_object_set_int_property (obj, idx, SvIV (newval));
              break;
            case CFAPI_LONG:
              cf_object_set_long_property (obj, idx, SvNV (newval));
              break;
            case CFAPI_DOUBLE:
              {
                int unused_type;
                object_set_property (&unused_type, obj, idx, (double)SvNV (newval));
              }
              break;
            case CFAPI_STRING:
              cf_object_set_string_property (obj, idx, SvOK (newval) ? SvPV_nolen (newval) : 0);
              break;
            case CFAPI_POBJECT:
              {
                 int unused_type;
                 object_set_property (&unused_type, obj, idx, (object *)SvPTR_ornull (newval, "cf::object"));
              }
              break;
            default:
              croak ("unhandled type '%d' in set_property '%d'", type, idx);
          }

# missing properties

void 
set_attacktype (object *obj, U32 attacktype)
	CODE:
        obj->attacktype = attacktype;

U32
get_attacktype (object *obj)
	ALIAS:
        attacktype = 0
	CODE:
        RETVAL = obj->attacktype;
	OUTPUT: RETVAL

void 
set_food (object *obj, int food)
	CODE:
        obj->stats.food = food;

int
get_food (object *obj)
	ALIAS:
        food = 0
	CODE:
        RETVAL = obj->stats.food;
	OUTPUT: RETVAL

void
inv (object *obj)
	PROTOTYPE: $
        PPCODE:
{
	object *o;
	for (o = obj->inv; o; o = o->below)
          XPUSHs (sv_2mortal (newSVcfapi (CFAPI_POBJECT, o)));
}

int cf_object_get_resistance (object *op, int rtype)
        ALIAS: resistance = 0

int cf_object_get_flag (object *op, int flag)
	ALIAS: flag = 0

void cf_object_set_flag (object *op, int flag, int value)

void cf_object_move (object *op, int dir, object *originator = op)

void cf_object_apply (object *op, object *author, int flags = 0)

void cf_object_apply_below (object *op)

void cf_object_remove (object *op)

void cf_object_free (object *op)

object *cf_object_present_archname_inside (object *op, char *whatstr)

int cf_object_transfer (object *op, int x, int y, int r, object *orig)

int cf_object_change_map (object *op, int x, int y, mapstruct *map)

object *cf_object_clone (object *op, int clonetype = 0)

int cf_object_pay_item (object *op, object *buyer)

int cf_object_pay_amount (object *op, double amount)

int cf_object_cast_spell (object *caster, object *ctoo, int dir, object *spell_ob, char *stringarg = 0)

int cf_object_cast_ability (object *caster, object *ctoo, int dir, object *sp_, char *stringarg = 0)

void cf_object_learn_spell (object *op, object *sp)

void cf_object_forget_spell (object *op, object *sp)

object *cf_object_check_for_spell (object *op, char *spellname)

int cf_object_query_money (object *op)
	ALIAS: money = 0

int cf_object_query_cost (object *op, object *who, int flags)
	ALIAS: cost = 0

void cf_object_activate_rune (object *op , object *victim)

int cf_object_check_trigger (object *op, object *cause)

int cf_object_out_of_map (object *op, int x, int y)

void cf_object_drop (object *op, object *author)

void cf_object_take (object *op, object *author)

void cf_object_say (object *op, char *msg)

void cf_object_speak (object *op, char *msg)

object *cf_object_insert_object (object *op, object *container)

const char *cf_object_get_msg (object *ob)
	ALIAS: msg = 0

object *cf_object_insert_in_ob (object *ob, object *where)

int cf_object_teleport (object *op, mapstruct *map, int x, int y)

void cf_object_update (object *op, int flags)

void cf_object_pickup (object *op, object *what)

char *cf_object_get_key (object *op, char *keyname)
	ALIAS: key = 0

void cf_object_set_key (object *op, char *keyname, char *value)

object *cf_create_object_by_name (const char *name)

MODULE = cf        PACKAGE = cf::object         PREFIX = cf_

void cf_fix_object (object *pl)
	ALIAS: fix = 0

object *cf_insert_ob_in_ob (object *ob, object *where)

# no clean way to get an object from an archetype - stupid idiotic
# dumb kludgy misdesigned plug-in api slowly gets on my nerves.

object *new (const char *archetype = 0)
	PROTOTYPE: ;$
        CODE:
        RETVAL = archetype ? get_archetype (archetype) : cf_create_object ();
        OUTPUT:
	RETVAL

object *insert_ob_in_map_at (object *ob, mapstruct *where, object_ornull *orig, int flag, int x, int y)
	PROTOTYPE: $$$$$$
        CODE:
{
	int unused_type;
        RETVAL = (object *)object_insert (&unused_type, ob, 0, where, orig, flag, x, y);
}

object *get_nearest_player (object *ob)
	ALIAS: nearest_player = 0
        PREINIT:
        extern object *get_nearest_player (object *);

void rangevector (object *ob, object *other, int flags = 0)
	PROTOTYPE: $$;$
        PPCODE:
{
        rv_vector rv;
        get_rangevector (ob, other, &rv, flags);
        EXTEND (SP, 5);
        PUSHs (newSVuv (rv.distance));
        PUSHs (newSViv (rv.distance_x));
        PUSHs (newSViv (rv.distance_y));
        PUSHs (newSViv (rv.direction));
        PUSHs (newSVcfapi (CFAPI_POBJECT, rv.part));
}

bool on_same_map_as (object *ob, object *other)
	CODE:
        RETVAL = on_same_map (ob, other);
	OUTPUT: RETVAL

char *
base_name (object *ob, int plural)
        CODE:
        RETVAL = cf_query_base_name (ob, plural);
        OUTPUT: RETVAL


MODULE = cf        PACKAGE = cf::object::player PREFIX = cf_player_

player *player (object *op)
	CODE:
        RETVAL = cf_player_find (cf_query_name (op));
	OUTPUT: RETVAL

void cf_player_message (object *obj, char *txt, int flags = NDI_ORANGE | NDI_UNIQUE)

object *cf_player_send_inventory (object *op)

player *contr (object *op)
	CODE:
        RETVAL = op->contr;
        OUTPUT: RETVAL

char *cf_player_get_ip (object *op)
	ALIAS: ip = 0

object *cf_player_get_marked_item (object *op)
	ALIAS: marked_item = 0

void cf_player_set_marked_item (object *op, object *ob)

partylist *cf_player_get_party (object *op)
	ALIAS: party = 0

void cf_player_set_party (object *op, partylist *party)


MODULE = cf        PACKAGE = cf::object::map    PREFIX = cf_

MODULE = cf        PACKAGE = cf::player         PREFIX = cf_player_

player *cf_player_find (char *name)
	PROTOTYPE: $

void cf_player_move (player *pl, int dir)

void MapNewmapCmd (player *pl)

# nonstandard
object *ob (player *pl)
	CODE:
        RETVAL = pl->ob;
        OUTPUT: RETVAL

player *first ()
	CODE:
        RETVAL = first_player;
        OUTPUT: RETVAL

player *next (player *pl)
	CODE:
        RETVAL = pl->next;
        OUTPUT: RETVAL

void
list ()
	PPCODE:
{
	player *pl;
        for (pl = first_player; pl; pl = pl->next)
          XPUSHs (newSVcfapi (CFAPI_PPLAYER, pl));
}


MODULE = cf        PACKAGE = cf::map            PREFIX = cf_map_

SV *
get_property (mapstruct *obj, int type, int idx)
	CODE:
        RETVAL = newSVcfapi (type, cf_map_get_property (obj, idx));
	OUTPUT: RETVAL

SV *
set_property (mapstruct *obj, int type, int idx, SV *newval)
	CODE:
        switch (type)
          {
            case CFAPI_INT:
              cf_map_set_int_property (obj, idx, SvIV (newval));
              break;
            default:
              croak ("unhandled type '%d' in set_property '%d'", type, idx);
          }

mapstruct *new (int width, int height)
	PROTOTYPE:
	CODE:
{
        int unused_type;
        RETVAL = map_get_map (&unused_type, 0, width, height);
}
	OUTPUT:
        RETVAL

mapstruct *cf_map_get_map (char *name)
	PROTOTYPE: $
	ALIAS: map = 0

mapstruct *cf_map_get_first ()
	PROTOTYPE:
	ALIAS: first = 0

object *cf_map_insert_object_there (mapstruct *where, object *op, object *originator, int flags)

object *cf_map_insert_object (mapstruct *where, object* op, int x, int y)

object* cf_map_present_arch_by_name (mapstruct *map, const char* str, int nx, int ny)
        C_ARGS: str, map, nx, ny

#int cf_map_get_flags (mapstruct* map, mapstruct** nmap, I16 x, I16 y, I16 *nx, I16 *ny)

void
at (mapstruct *obj, unsigned int x, unsigned int y)
	PROTOTYPE: $$$
        INIT:
	if (x >= MAP_WIDTH (obj) || y >= MAP_HEIGHT (obj)) XSRETURN_EMPTY;
        PPCODE:
{
	object *o;
	for (o = GET_MAP_OB (obj, x, y); o; o = o->above)
          XPUSHs (sv_2mortal (newSVcfapi (CFAPI_POBJECT, o)));
}

SV *
bot_at (mapstruct *obj, unsigned int x, unsigned int y)
	PROTOTYPE: $$$
        ALIAS:
          top_at        = 1
          flags_at      = 2
          light_at      = 3
          move_block_at = 4
          move_slow_at  = 5
          move_on_at    = 6
          move_off_at   = 7
        INIT:
	if (x >= MAP_WIDTH (obj) || y >= MAP_HEIGHT (obj)) XSRETURN_UNDEF;
        CODE:
        switch (ix)
          {
            case 0: RETVAL = newSVcfapi (CFAPI_POBJECT, GET_MAP_OB         (obj, x, y)); break;
            case 1: RETVAL = newSVcfapi (CFAPI_POBJECT, GET_MAP_TOP        (obj, x, y)); break;
            case 2: RETVAL = newSVuv    (               GET_MAP_FLAGS      (obj, x, y)); break;
            case 3: RETVAL = newSViv    (               GET_MAP_LIGHT      (obj, x, y)); break;
            case 4: RETVAL = newSVuv    (               GET_MAP_MOVE_BLOCK (obj, x, y)); break;
            case 5: RETVAL = newSVuv    (               GET_MAP_MOVE_SLOW  (obj, x, y)); break;
            case 6: RETVAL = newSVuv    (               GET_MAP_MOVE_ON    (obj, x, y)); break;
            case 7: RETVAL = newSVuv    (               GET_MAP_MOVE_OFF   (obj, x, y)); break;
          }
        OUTPUT:
        RETVAL


MODULE = cf        PACKAGE = cf::arch           PREFIX = cf_archetype_

archetype *cf_archetype_get_first()
	PROTOTYPE:
	ALIAS: first = 0

archetype *cf_archetype_get_next (archetype *arch)
	ALIAS: next = 0

archetype *cf_archetype_get_head (archetype *arch)
	ALIAS: head = 0

archetype *cf_archetype_get_more (archetype *arch)
	ALIAS: more = 0

const char *cf_archetype_get_name (archetype *arch)
	ALIAS: name = 0

MODULE = cf        PACKAGE = cf::party          PREFIX = cf_party_

partylist *cf_party_get_first ()
	PROTOTYPE:
        ALIAS: first = 0

partylist *cf_party_get_next (partylist *party)
        ALIAS: next = 0

const char *cf_party_get_name (partylist *party)

const char *cf_party_get_password (partylist *party)
        ALIAS: password = 0

player *cf_party_get_first_player (partylist *party)
        ALIAS: first_player = 0

player *cf_party_get_next_player (partylist *party, player *op)
        ALIAS: next_player = 0


MODULE = cf        PACKAGE = cf::region         PREFIX = cf_region_

region  *cf_region_get_first ()
	PROTOTYPE:
        ALIAS: first = 0

const char *cf_region_get_name (region *reg)
        ALIAS: name = 0

region *cf_region_get_next (region *reg)
        ALIAS: next = 0

region *cf_region_get_parent (region *reg)
        ALIAS: parent = 0

const char *cf_region_get_longname (region *reg)
        ALIAS: longname = 0

const char *cf_region_get_message (region *reg)
        ALIAS: message = 0


