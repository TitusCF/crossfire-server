#include <version.h>
#include <global.h>
#include <guile/gh.h>
#include <object.h>
#ifndef __CEXTRACT__
#include <sproto.h>
#endif
#ifdef HAVE_TIME_H
#include <time.h>
#endif
#include <../random_maps/random_map.h>
#include <../random_maps/rproto.h>

#define WHO ((object *)(gh_scm2long(who)))
#define WHAT ((object *)(gh_scm2long(what)))

static int guile_stack_position = 0;

/* Error handler */
static void error_handler(const char *err_str)
{
  printf("-------------------\n");
  printf("%s\n",err_str);
  printf("-------------------\n");
};

/* GROS: Initialize the Guile Crossfire extensions functions.
 */

void guile_init_functions()
{
  LOG(llevInfo, "Guile-Scheme interpreter support by Gros\n");
  LOG(llevInfo, "Guile Interpreter initializing...\n");
  gh_new_procedure("activate-rune", Script_activateRune,2,0,0);
  gh_new_procedure("check_trigger", Script_checkTrigger,2,0,0);
  gh_new_procedure("set-unaggressive", Script_setUnaggressive,2,0,0);
  gh_new_procedure("cast-ability", Script_castAbility,4,0,0);
  gh_new_procedure("get-map-path", Script_getMapPath,1,0,0);
  gh_new_procedure("get-map-object", Script_getMapObject,1,0,0);
  gh_new_procedure("get-message", Script_getMessage,1,0,0);
  gh_new_procedure("get-god", Script_getGod,1,0,0);
  gh_new_procedure("set-god", Script_setGod,2,0,0);
  gh_new_procedure("set-weight", Script_setWeight,2,0,0);
  gh_new_procedure("ready-map", Script_readyMap,1,0,0);
  gh_new_procedure("teleport", Script_teleport,4,0,0);
  gh_new_procedure("out-of-map?", Script_outOfMap_p,3,0,0);
  gh_new_procedure("pick-up", Script_pickUp,2,0,0);
  gh_new_procedure("get-weight", Script_getWeight,1,0,0);
  gh_new_procedure("can-be-picked?", Script_NoPick_p,1,0,0);
  gh_new_procedure("get-map", Script_getMap,1,0,0);
  gh_new_procedure("get-next-object", Script_getNextObject,1,0,0);
  gh_new_procedure("get-previous-object", Script_getPreviousObject,1,0,0);
  gh_new_procedure("get-first-object-on-square", Script_getFirstOnSquare, 3,0,0);
  gh_new_procedure("set-quantity", Script_setQuantity,2,0,0);
  gh_new_procedure("get-quantity", Script_getQuantity,1,0,0);
  gh_new_procedure("insert-object-inside", Script_insertObjectInside,2,0,0);
  gh_new_procedure("find-player", Script_findPlayer,1,0,0);
  gh_new_procedure("apply",Script_apply,3,0,0);
  gh_new_procedure("drop",Script_drop,2,0,0);
  gh_new_procedure("take",Script_take,2,0,0);
  gh_new_procedure("get-scriptfire-quantum", Script_getScriptQuantum,0,0,0);
  gh_new_procedure("get-remaining-scriptfire-quantum", Script_getRemainingQuantum,0,0,0);
  gh_new_procedure("invisible?", Script_invisible_p,1,0,0);
  gh_new_procedure("set-invisible", Script_setInvisible,2,0,0);
  gh_new_procedure("get-experience", Script_getXP,1,0,0);
  gh_new_procedure("set-experience", Script_setXP,2,0,0);
  gh_new_procedure("get-speed", Script_getSpeed,1,0,0);
  gh_new_procedure("set-speed", Script_setSpeed,2,0,0);
  gh_new_procedure("get-food", Script_getFP,1,0,0);
  gh_new_procedure("set-food",Script_setFP,2,0,0);
  gh_new_procedure("get-grace",Script_getGP,1,0,0);
  gh_new_procedure("set-grace",Script_setGP,2,0,0);
  gh_new_procedure("get-return-value", Script_getReturnValue,0,0,0);
  gh_new_procedure("set-return-value", Script_setReturnValue,1,0,0);
  gh_new_procedure("get-direction", Script_getDirection, 1,0,0);
  gh_new_procedure("set-direction", Script_setDirection, 2,0,0);
  gh_new_procedure("get-script-death", Script_getScriptDeath,1,0,0);
  gh_new_procedure("get-script-load", Script_getScriptLoad,1,0,0);
  gh_new_procedure("get-script-apply", Script_getScriptApply,1,0,0);
  gh_new_procedure("get-script-say", Script_getScriptSay,1,0,0);
  gh_new_procedure("get-script-trigger", Script_getScriptTrigger,1,0,0);
  gh_new_procedure("get-script-time", Script_getScriptTime,1,0,0);
  gh_new_procedure("get-script-attack", Script_getScriptAttack,1,0,0);
  gh_new_procedure("get-script-drop", Script_getScriptDrop,1,0,0);
  gh_new_procedure("get-script-throw", Script_getScriptThrow,1,0,0);
  gh_new_procedure("get-script-stop", Script_getScriptStop,1,0,0);
  gh_new_procedure("set-script-death", Script_setScriptDeath,2,0,0);
  gh_new_procedure("set-script-load", Script_setScriptLoad,2,0,0);
  gh_new_procedure("set-script-apply", Script_setScriptApply,2,0,0);
  gh_new_procedure("set-script-say", Script_setScriptSay,2,0,0);
  gh_new_procedure("set-script-trigger", Script_setScriptTrigger,2,0,0);
  gh_new_procedure("set-script-time", Script_setScriptTime,2,0,0);
  gh_new_procedure("set-script-attack", Script_setScriptAttack,2,0,0);
  gh_new_procedure("set-script-drop", Script_setScriptDrop,2,0,0);
  gh_new_procedure("set-script-throw", Script_setScriptThrow,2,0,0);
  gh_new_procedure("set-script-stop", Script_setScriptStop,2,0,0);
  gh_new_procedure("get-last-sp", Script_getLastSP,1,0,0);
  gh_new_procedure("set-last-sp", Script_setLastSP,2,0,0);
  gh_new_procedure("get-last-grace", Script_getLastGP,1,0,0);
  gh_new_procedure("set-last-grace", Script_setLastGP,2,0,0);
  gh_new_procedure("fix-object", Script_fixObject,1,0,0);
  gh_new_procedure("teleport", Script_teleport,4,0,0);
  gh_new_procedure("set-face", Script_setFace,2,0,0);
  gh_new_procedure("get-attack-type", Script_getAttackType, 1,0,0);
  gh_new_procedure("set-attack-type", Script_setAttackType, 2,0,0);
  gh_new_procedure("set-damage", Script_setDamage,2,0,0);
  gh_new_procedure("get-damage", Script_getDamage,1,0,0);
  gh_new_procedure("set-been-applied", Script_setBeenApplied,2,0,0);
  gh_new_procedure("set-identified", Script_setIdentified,2,0,0);
  gh_new_procedure("kill-object", Script_killObject,3,0,0);
  gh_new_procedure("who-is-other", Script_whoIsOther,0,0,0);
  gh_new_procedure("direction-north",Script_directionNorth,0,0,0);
  gh_new_procedure("direction-north-east",Script_directionNE,0,0,0);
  gh_new_procedure("direction-east",Script_directionEast,0,0,0);
  gh_new_procedure("direction-south-east",Script_directionSE,0,0,0);
  gh_new_procedure("direction-south",Script_directionSouth,0,0,0);
  gh_new_procedure("direction-south-west",Script_directionSW,0,0,0);
  gh_new_procedure("direction-west",Script_directionWest,0,0,0);
  gh_new_procedure("direction-north-west",Script_directionNW,0,0,0);
  gh_new_procedure("cast-spell", Script_castSpell,4,0,0);
  gh_new_procedure("forget-spell", Script_forgetSpell,2,0,0);
  gh_new_procedure("acquire-spell", Script_acquireSpell, 2,0,0);
  gh_new_procedure("know-spell?", Script_knowSpell_p, 2,0,0);
  gh_new_procedure("check-invisible-object-inside", Script_checkInvisibleObjectInside, 2,0,0);
  gh_new_procedure("create-invisible-object-inside", Script_createInvisibleObjectInside, 2,0,0);
  gh_new_procedure("create-object-inside", Script_createObjectInside, 2,0,0);
  gh_new_procedure("check-map", Script_checkArchetype, 4,0,0);
  gh_new_procedure("check-inventory", Script_checkInventory, 2,0,0);
  gh_new_procedure("get-name", Script_getName, 1,0,0);
  gh_new_procedure("create-object", Script_createObject, 3,0,0);
  gh_new_procedure("remove-object", Script_removeObject, 1,0,0);
  gh_new_procedure("alive?", Script_isAlive_p, 1,0,0);
  gh_new_procedure("wiz?", Script_isWiz_p, 1,0,0);
  gh_new_procedure("was-wiz?", Script_wasWiz_p, 1,0,0);
  gh_new_procedure("applied?", Script_isApplied_p, 1,0,0);
  gh_new_procedure("unpaid?", Script_isUnpaid_p, 1,0,0);
  gh_new_procedure("flying?", Script_isFlying_p, 1,0,0);
  gh_new_procedure("monster?", Script_isMonster_p, 1,0,0);
  gh_new_procedure("friendly?", Script_isFriendly_p, 1,0,0);
  gh_new_procedure("generator?", Script_isGenerator_p, 1,0,0);
  gh_new_procedure("thrown?", Script_isThrown_p, 1,0,0);
  gh_new_procedure("can-see-invisible?", Script_canSeeInvisible_p, 1,0,0);
  gh_new_procedure("can-roll?", Script_canRoll_p, 1,0,0);
  gh_new_procedure("turnable?", Script_isTurnable_p, 1,0,0);
  gh_new_procedure("used-up?", Script_isUsedUp_p, 1,0,0);
  gh_new_procedure("identified?", Script_isIdentified_p, 1,0,0);
  gh_new_procedure("splitting?", Script_isSplitting_p, 1,0,0);
  gh_new_procedure("hitback?", Script_hitback_p, 1,0,0);
  gh_new_procedure("blocksview?", Script_blocksview_p, 1,0,0);
  gh_new_procedure("undead?", Script_isUndead_p, 1,0,0);
  gh_new_procedure("scared?", Script_isScared_p, 1,0,0);
  gh_new_procedure("unaggressive?", Script_isUnaggressive_p, 1,0,0);
  gh_new_procedure("reflect-missiles?", Script_reflectMissiles_p, 1,0,0);
  gh_new_procedure("reflect-spells?", Script_reflectSpells_p, 1,0,0);
  gh_new_procedure("run-away?", Script_runAway_p, 1,0,0);
  gh_new_procedure("can-pass-thru?", Script_canPassThru_p, 1,0,0);
  gh_new_procedure("can-pick-up?", Script_canPickUp_p, 1,0,0);
  gh_new_procedure("unique?", Script_isUnique_p, 1,0,0);
  gh_new_procedure("can-cast-spell?", Script_canCastSpell_p, 1,0,0);
  gh_new_procedure("can-use-scroll?", Script_canUseScroll_p, 1,0,0);
  gh_new_procedure("can-use-wand?", Script_canUseWand_p, 1,0,0);
  gh_new_procedure("can-use-bow?", Script_canUseBow_p, 1,0,0);
  gh_new_procedure("can-use-armour?", Script_canUseArmour_p, 1,0,0);
  gh_new_procedure("can-use-weapon?", Script_canUseWeapon_p, 1,0,0);
  gh_new_procedure("can-use-ring?", Script_canUseRing_p, 1,0,0);
  gh_new_procedure("has-xrays?", Script_hasXRays_p, 1,0,0);
  gh_new_procedure("floor?", Script_isFloor_p, 1,0,0);
  gh_new_procedure("lifesaver?", Script_isLifesave_p, 1,0,0);
  gh_new_procedure("sleeping?", Script_isSleeping_p, 1,0,0);
  gh_new_procedure("stand-still?", Script_standStill_p, 1,0,0);
  gh_new_procedure("only-attack?", Script_onlyAttack_p, 1,0,0);
  gh_new_procedure("confused?", Script_isConfused_p, 1,0,0);
  gh_new_procedure("stealth?", Script_hasStealth_p, 1,0,0);
  gh_new_procedure("cursed?", Script_isCursed_p, 1,0,0);
  gh_new_procedure("damned?", Script_isDamned_p, 1,0,0);
  gh_new_procedure("known-magical?", Script_knownMagical_p, 1,0,0);
  gh_new_procedure("known-cursed?", Script_knownCursed_p, 1,0,0);
  gh_new_procedure("can-use-skill?", Script_canUseSkill_p, 1,0,0);
  gh_new_procedure("been-applied?", Script_beenApplied_p, 1,0,0);
  gh_new_procedure("can-use-rod?", Script_canUseRod_p, 1,0,0);
  gh_new_procedure("can-use-horn?", Script_canUseHorn_p, 1,0,0);
  gh_new_procedure("make-invisible?", Script_makeInvisible_p, 1,0,0);
  gh_new_procedure("blind?", Script_isBlind_p, 1,0,0);
  gh_new_procedure("can-see-in-dark?", Script_canSeeInDark_p, 1,0,0);
  gh_new_procedure("get-ac", Script_getAC,1,0,0);
  gh_new_procedure("get-cha", Script_getCha,1,0,0);
  gh_new_procedure("get-con", Script_getCon,1,0,0);
  gh_new_procedure("get-dex", Script_getDex,1,0,0);
  gh_new_procedure("get-hp", Script_getHP,1,0,0);
  gh_new_procedure("get-int", Script_getInt,1,0,0);
  gh_new_procedure("get-pow", Script_getPow,1,0,0);
  gh_new_procedure("get-sp", Script_getSP,1,0,0);
  gh_new_procedure("get-str", Script_getPow,1,0,0);
  gh_new_procedure("get-wis", Script_getWis,1,0,0);
  gh_new_procedure("get-max-hp", Script_getMaxHP,1,0,0);
  gh_new_procedure("get-max-sp", Script_getMaxSP,1,0,0);
  gh_new_procedure("get-x-position", Script_getXPosition,1,0,0);
  gh_new_procedure("get-y-position", Script_getYPosition,1,0,0);
  gh_new_procedure("set-position", Script_setPosition,3,0,0);
  gh_new_procedure("set-title", Script_setTitle,2,0,0);
  gh_new_procedure("set-ac", Script_setAC,1,0,0);
  gh_new_procedure("set-cha", Script_setCha,2,0,0);
  gh_new_procedure("set-con", Script_setCon,2,0,0);
  gh_new_procedure("set-dex", Script_setDex,2,0,0);
  gh_new_procedure("set-hp", Script_setHP,2,0,0);
  gh_new_procedure("set-int", Script_setInt,2,0,0);
  gh_new_procedure("set-max-hp", Script_setMaxHP,2,0,0);
  gh_new_procedure("set-max-sp", Script_setMaxSP,2,0,0);
  gh_new_procedure("set-pow", Script_setPow,2,0,0);
  gh_new_procedure("set-sp", Script_setSP,2,0,0);
  gh_new_procedure("set-str", Script_setStr,2,0,0);
  gh_new_procedure("set-wis", Script_setWis,2,0,0);
  gh_new_procedure("who-am-I", Script_whoAmI,0,0,0);
  gh_new_procedure("who-is-activator", Script_whoIsActivator,0,0,0);
  gh_new_procedure("what-message", Script_whatMessage, 0,0,0);
  gh_new_procedure("crossfire-message", Script_crossfireMessage,1,1,0);
  gh_new_procedure("crossfire-say", Script_crossfireSay, 2,0,0);
  gh_new_procedure("crossfire-write", Script_crossfireWrite, 2,1,0);

  gh_new_procedure("is-type?", Script_isType,2,0,0);
  gh_new_procedure("get-type", Script_getType,1,0,0);

  guile_max_quantum = 1000;
  guile_init_type_functions();
  guile_init_spell_functions();
  guile_stack_position = 0;

  LOG(llevInfo, "Done.\n");
};

void guile_run(char *scriptfile)
{
  gh_eval_file(scriptfile);
};

int guile_call_event(object *first, object *second, object *third, int flags, char *text, int dam, int wc, char *scriptname, int fixthem)
{
  char buf[MAX_BUF];
  guile_stack_position++;
  guile_current_activator[guile_stack_position] = first;
  guile_current_who[guile_stack_position]       = second;
  guile_current_other[guile_stack_position]     = third;
  guile_current_text[guile_stack_position]      = text;
  guile_current_dam[guile_stack_position]       = dam;
  guile_current_wc[guile_stack_position]        = wc;
  guile_current_flags[guile_stack_position]     = flags;
  guile_return_value[guile_stack_position] = 0;
  guile_current_quantum[guile_stack_position] = guile_max_quantum;
  gfec_eval_file(create_pathname(scriptname),error_handler);
  if (fixthem == SCRIPT_FIX_ALL)
  {
    if (third != NULL) fix_player(third);
    if (second != NULL) fix_player(second);
    if (first != NULL) fix_player(first);
  }
  else
  {
    if (fixthem == SCRIPT_FIX_ACTIVATOR)
      fix_player(first);
  };
  guile_stack_position--;
  return guile_return_value[guile_stack_position+1];
};

int guile_call_event_str(object *first, object *second, object *third, int flags, char *text, int dam, int wc, char *scriptname, int fixthem)
{
  char buf[MAX_BUF];
  guile_stack_position++;
  guile_current_activator[guile_stack_position] = first;
  guile_current_who[guile_stack_position]       = second;
  guile_current_other[guile_stack_position]     = third;
  guile_current_text[guile_stack_position]      = text;
  guile_current_dam[guile_stack_position]       = dam;
  guile_current_wc[guile_stack_position]        = wc;
  guile_current_flags[guile_stack_position]     = flags;
  guile_return_value[guile_stack_position] = 0;
  guile_current_quantum[guile_stack_position] = guile_max_quantum;
  gfec_eval_string(scriptname,error_handler);
  if (fixthem == SCRIPT_FIX_ALL)
  {
    if (third != NULL) fix_player(third);
    if (second != NULL) fix_player(second);
    if (first != NULL) fix_player(first);
  }
  else
  {
    if (fixthem == SCRIPT_FIX_ACTIVATOR)
      fix_player(first);
  };
  guile_stack_position--;
  return guile_return_value[guile_stack_position+1];
};

int guile_use_weapon_script(object *hitter, object *hit, int base_dam, int base_wc)
{
  char buf[MAX_BUF];
  guile_stack_position++;
  guile_current_activator[guile_stack_position] = hitter;
  guile_current_who[guile_stack_position]       = hitter->current_weapon;
  guile_current_other[guile_stack_position]     = hit;
  guile_current_text[guile_stack_position]      = "";
  guile_current_dam[guile_stack_position]       = base_dam;
  guile_current_wc[guile_stack_position]        = base_wc;
  guile_current_flags[guile_stack_position]     = 0;
  guile_return_value[guile_stack_position] = 0;
  guile_current_quantum[guile_stack_position] = guile_max_quantum;
  if (hitter->current_weapon->script_attack != NULL)
  {
    gfec_eval_file(create_pathname(hitter->current_weapon->script_attack),error_handler);
  }
  else
  {
    gfec_eval_string(hitter->current_weapon->script_str_attack,error_handler);
  };
  fix_player(hitter);
  fix_player(hitter->current_weapon);
  if (guile_current_other[guile_stack_position] !=NULL)
  {
        fix_player(hit);
        guile_stack_position--;
        return 0;
  }
  else
  {
        guile_stack_position--;
        return -1;
  };
};

/* These come from 'guile fancy error catching' (gfec) source. */
/* Evaluates a script file, passing any error to error_handler */
void gfec_eval_file(const char *file, gfec_error_handler error_handler)
{
  char *err_msg = NULL;
  scm_internal_stack_catch(SCM_BOOL_T,(scm_catch_body_t)gh_eval_file, (void*)file, (scm_catch_handler_t)gfec_catcher,(void*)&err_msg);
  if (err_msg != NULL)
  {
    error_handler(err_msg);
    free(err_msg);
  };
  return;
};

/* Evaluates a script string, passing any error to error_handler */
void gfec_eval_string(const char *str, gfec_error_handler error_handler)
{
  char *err_msg = NULL;
  scm_internal_stack_catch(SCM_BOOL_T,(scm_catch_body_t)gh_eval_str, (void*)str, (scm_catch_handler_t)gfec_catcher,(void*)&err_msg);
  if (err_msg != NULL)
  {
    error_handler(err_msg);
    free(err_msg);
  };
  return;
};

/* Manages the error catching */
/* Normally, this should display an error stack trace, but for some
 * unknown reason, it does not work. Another problem is that the
 * guile error handling functions are not available under Win32, so
 * I decided to comment the stuff out until a decent solution could
 * be found.
 */
static SCM gfec_catcher(void *data, SCM tag, SCM throw_args)
{
  printf("Warning ! Script error !\n");
  /*
  SCM the_stack;
  SCM port=scm_mkstrport(SCM_INUM0,scm_make_string(SCM_MAKINUM(200),SCM_UNDEFINED),SCM_OPN | SCM_WRTNG , "error_handler");

  if (gh_list_p(throw_args) && gh_length(throw_args) >= 4)
  {
    SCM fn = gh_car(throw_args);
    SCM format = gh_cadr(throw_args);
    SCM args = gh_caddr(throw_args);
    SCM other_data = gh_car(gh_cdddr(throw_args));
    if (fn != SCM_BOOL_F)
    {
      scm_puts("Function:",port);
      scm_display(fn,port);
      scm_puts(", ",port);
      scm_display(tag,port);
      scm_newline(port);
    };
    if (gh_string_p(format))
    {
      scm_puts("Error:",port);
      scm_display_error_message(format,args,port);
    };
    if (other_data != SCM_BOOL_F)
    {
      scm_puts("Other data:",port);
      scm_display(other_data,port);
      scm_newline(port);
      scm_newline(port);
    };
  };
  the_stack = scm_fluid_ref(SCM_CDR(scm_the_last_stack_fluid));
  if (the_stack != SCM_BOOL_F)
  {
    scm_display_backtrace(the_stack, port, SCM_UNDEFINED, SCM_UNDEFINED);
  };
  LOG(llevError, "Warning ! Error in Script !\n");
  SCM_DEFER_INTS;
*/
/* For some unknown reason this does not work */
/*  *(char **)data = strdup(SCM_CHARS(SCM_CDR(SCM_STREAM(port))));*/
/*  SCM_ALLOW_INTS; */
  return gh_int2scm(1);
};


player *find_player(char *plname)
{
  player *pl;
  for(pl=first_player;pl!=NULL;pl=pl->next)
  {
    if(pl->ob != NULL && !QUERY_FLAG(pl->ob,FLAG_REMOVED) && !strcmp(query_name(pl->ob),plname))
        return pl;
  };
  return NULL;
};

object *create_artifact(object *op, char *artifactname)
{
        artifactlist *al;
        artifact *art;
        char *temptitle;
        al = find_artifactlist(op->type);
        if (al==NULL)
                return NULL;
        for (art=al->items; art!=NULL; art=art->next)
        {
                temptitle = (char *)(malloc(strlen(art->item->name) + 5));
                strcpy(temptitle, " of ");
                strcat(temptitle, art->item->name);
                if (!strcmp (temptitle, artifactname))
                {
                        give_artifact_abilities(op, art->item);
                }

                free(temptitle);
        };
        return NULL;
}

SCM Script_activateRune(SCM who, SCM what)
{
  spring_trap(WHAT,WHO);
};

SCM Script_checkTrigger(SCM who, SCM what)
{
  check_trigger(WHAT,WHO);
};

SCM Script_setUnaggressive(SCM who, SCM value)
{

  if (gh_scm2bool(value))
  {
    SET_FLAG(WHO, FLAG_UNAGGRESSIVE);
  }
  else
  {
    CLEAR_FLAG(WHO, FLAG_UNAGGRESSIVE);
  };
};

SCM Script_getMapPath(SCM where)
{
  return gh_str02scm(((mapstruct *)(gh_scm2long(where)))->path);
};

SCM Script_getMapObject(SCM where)
{
  return gh_long2scm((long)(((mapstruct *)(gh_scm2long(where)))->map_object));
};

SCM Script_getMessage(SCM who)
{
  char buf[MAX_BUF];
  strncpy(buf, WHO->msg,strlen(WHO->msg)-1);
  buf[strlen(WHO->msg)-1]=0x0;
  return gh_str02scm(buf);
};

SCM Script_getGod(SCM who)
{
  return gh_str02scm(determine_god(WHO));
};

SCM Script_setGod(SCM who, SCM value)
{
  int *length = 0;
  char *txt = gh_scm2newstr(value, length);
  if (command_rskill(WHO, "praying"))
    become_follower(WHO, find_god(txt));
  free(txt);
};

SCM Script_setWeight(SCM who, SCM value)
{
  WHO->weight = gh_scm2long(value);
};

SCM Script_readyMap(SCM mapname)
{
  int *length = 0;
  char *txt = gh_scm2newstr(mapname, length);
  mapstruct *mymap = ready_map_name(txt,0);
  free(txt);
  return gh_long2scm((long)(mymap));
};

SCM Script_teleport(SCM who, SCM where, SCM x, SCM y)
{
  if ((out_of_map((mapstruct *)(gh_scm2long(where)),gh_scm2int(x),gh_scm2int(y)))==0)
  {
    int k=find_first_free_spot(WHO->arch,(mapstruct *)(gh_scm2long(where)),gh_scm2int(x),gh_scm2int(y));
    object *tmp;
    if (k==-1) return gh_int2scm(0);
    remove_ob(WHO);
    for(tmp=WHO;tmp!=NULL;tmp=tmp->more)
      tmp->x=gh_scm2int(x)+freearr_x[k]+(tmp->arch==NULL?0:tmp->arch->clone.x),
      tmp->y=gh_scm2int(y)+freearr_y[k]+(tmp->arch==NULL?0:tmp->arch->clone.y);
    insert_ob_in_map(WHO,(mapstruct *)(gh_scm2long(where)),NULL);
  };
};

SCM Script_outOfMap_p(SCM who, SCM x, SCM y)
{
  return gh_bool2scm(out_of_map(WHO->map, gh_scm2int(x),gh_scm2int(y)));
};

SCM Script_pickUp(SCM who, SCM what)
{
  pick_up(WHO,WHAT);
};

SCM Script_getWeight(SCM who)
{
  return gh_long2scm(WHO->weight);
};

SCM Script_NoPick_p(SCM who)
{
  return gh_bool2scm(QUERY_FLAG(WHO,FLAG_NO_PICK));
};

SCM Script_getNextObject(SCM who)
{
  if (WHO==NULL) return gh_long2scm(0);
  return gh_long2scm((long)(WHO->below));
};

SCM Script_getPreviousObject(SCM who)
{
  if (WHO==NULL) return gh_long2scm(0);
  return gh_long2scm((long)(WHO->above));
};

SCM Script_getFirstOnSquare(SCM where, SCM x, SCM y)
{
  return gh_long2scm((long)(get_map_ob((mapstruct *)(gh_scm2long(where)),gh_scm2int(x),gh_scm2int(y))));
};

SCM Script_getMap(SCM who)
{
  gh_long2scm((long)(WHO->map));
};

SCM Script_setQuantity(SCM what, SCM value)
{
  WHAT->nrof = gh_scm2int(value);
};

SCM Script_getQuantity(SCM what)
{
  return gh_long2scm(WHAT->nrof);
};

SCM Script_insertObjectInside(SCM what, SCM where)
{
  object *myob;
  myob = WHAT;
  myob = insert_ob_in_ob(myob, (object *)(gh_scm2long(where)));
  if (((object *)(gh_scm2long(where)))->type == PLAYER)
  {
    esrv_send_item((object *)(gh_scm2long(where)), myob);
  };
};

SCM Script_findPlayer(SCM plname)
{
  int *length = 0;
  player *foundpl;
  object *foundob;
  char *txt = gh_scm2newstr(plname, length);
  foundpl = find_player(txt);
  if (foundpl!=NULL)
    foundob = foundpl->ob;
  else
    foundob = NULL;
  free(txt);
  return gh_long2scm((long)foundob);
};

SCM Script_apply(SCM who, SCM what, SCM flags)
{
        return gh_int2scm(manual_apply(WHO, ((object *)(gh_scm2long(what))), gh_scm2int(flags)));
};

SCM Script_drop(SCM who, SCM value)
{
        int *length = 0;
        char *txt = gh_scm2newstr(value, length);
        command_drop(WHO, txt);
        free(txt);
};

SCM Script_take(SCM who, SCM value)
{
        int *length = 0;
        char *txt = gh_scm2newstr(value, length);
        command_take(WHO, txt);
        free(txt);
};

SCM Script_getScriptQuantum()
{
        return gh_long2scm(guile_max_quantum);
};

SCM Script_getRemainingQuantum()
{
        return gh_long2scm(guile_current_quantum[guile_stack_position]);
};

SCM Script_invisible_p(SCM who)
{
        return gh_bool2scm(WHO->invisible);
};

SCM Script_setInvisible(SCM who, SCM value)
{
        WHO->invisible = gh_scm2bool(value);
};

SCM Script_getXP(SCM who)
{
        return gh_long2scm(WHO->stats.exp);
};

SCM Script_setXP(SCM who, SCM value)
{
        WHO->stats.exp = gh_scm2long(value);
};
SCM Script_getReturnValue()
{
        return gh_int2scm(guile_return_value[guile_stack_position]);
};

SCM Script_setReturnValue(SCM value)
{
        guile_return_value[guile_stack_position] = gh_scm2int(value);
};

SCM Script_getDirection(SCM who)
{
        return gh_int2scm(WHO->direction);
};

SCM Script_setDirection(SCM who, SCM value)
{
        WHO->direction = gh_scm2int(value);
        SET_ANIMATION(WHO, WHO->direction);
};

SCM Script_getScriptDeath(SCM who)
{
  return gh_str02scm(WHO->script_death);
};
SCM Script_getScriptLoad(SCM who)
{
  return gh_str02scm(WHO->script_load);
};
SCM Script_getScriptApply(SCM who)
{
  return gh_str02scm(WHO->script_apply);
};
SCM Script_getScriptSay(SCM who)
{
  return gh_str02scm(WHO->script_say);
};
SCM Script_getScriptTrigger(SCM who)
{
  return gh_str02scm(WHO->script_trigger);
};
SCM Script_getScriptTime(SCM who)
{
  return gh_str02scm(WHO->script_time);
};
SCM Script_getScriptAttack(SCM who)
{
  return gh_str02scm(WHO->script_attack);
};
SCM Script_getScriptDrop(SCM who)
{
  return gh_str02scm(WHO->script_drop);
};
SCM Script_getScriptThrow(SCM who)
{
  return gh_str02scm(WHO->script_throw);
};
SCM Script_getScriptStop(SCM who)
{
  return gh_str02scm(WHO->script_stop);
};


SCM Script_setScriptDeath(SCM who, SCM value)
{
  int *length = 0;
  char *txt = gh_scm2newstr(value, length);
  WHO->script_death = add_string(txt);
  free(txt);
};
SCM Script_setScriptLoad(SCM who, SCM value)
{
  int *length = 0;
  char *txt = gh_scm2newstr(value, length);
  WHO->script_load = add_string(txt);
  free(txt);
};
SCM Script_setScriptApply(SCM who, SCM value)
{
  int *length = 0;
  char *txt = gh_scm2newstr(value, length);
  WHO->script_apply = add_string(txt);
  free(txt);
};
SCM Script_setScriptSay(SCM who, SCM value)
{
  int *length = 0;
  char *txt = gh_scm2newstr(value, length);
  WHO->script_say = add_string(txt);
  free(txt);
};
SCM Script_setScriptTrigger(SCM who, SCM value)
{
  int *length = 0;
  char *txt = gh_scm2newstr(value, length);
  WHO->script_trigger = add_string(txt);
  free(txt);
};
SCM Script_setScriptTime(SCM who, SCM value)
{
  int *length = 0;
  char *txt = gh_scm2newstr(value, length);
  if (strlen(txt))
  {
    WHO->script_time = add_string(txt);
  }
  else
  {
    WHO->script_time = NULL;
  };
  free(txt);
};
SCM Script_setScriptAttack(SCM who, SCM value)
{
  int *length = 0;
  char *txt = gh_scm2newstr(value, length);
  WHO->script_attack = add_string(txt);
  free(txt);
};
SCM Script_setScriptDrop(SCM who, SCM value)
{
  int *length = 0;
  char *txt = gh_scm2newstr(value, length);
  WHO->script_drop = add_string(txt);
  free(txt);
};
SCM Script_setScriptThrow(SCM who, SCM value)
{
  int *length = 0;
  char *txt = gh_scm2newstr(value, length);
  WHO->script_throw = add_string(txt);
  free(txt);
};
SCM Script_setScriptStop(SCM who, SCM value)
{
  int *length = 0;
  char *txt = gh_scm2newstr(value, length);
  WHO->script_stop = add_string(txt);
  free(txt);
};

SCM Script_setSpeed(SCM who, SCM value)
{
        WHO->speed = gh_scm2double(value);
};
SCM Script_getSpeed(SCM who)
{
        return gh_double2scm(WHO->speed);
};

SCM Script_getLastSP(SCM who)
{
        return gh_int2scm(WHO->last_sp);
};

SCM Script_setLastSP(SCM who, SCM value)
{
        WHO->last_sp = gh_scm2int(value);
};

SCM Script_getLastGP(SCM who)
{
        return gh_int2scm(WHO->last_grace);
};

SCM Script_setLastGP(SCM who, SCM value)
{
        WHO->last_grace = gh_scm2int(value);
};

SCM Script_fixObject(SCM who)
{
        fix_player(WHO);
};

SCM Script_setFace(SCM who, SCM value)
{
  int *length = 0;
  char *txt = gh_scm2newstr(value, length);
  WHO->animation_id = find_animation(txt);
  SET_ANIMATION(WHO, WHO->direction);
  update_object(WHO);
};

SCM Script_setAttackType(SCM who, SCM value)
{
        WHO->attacktype = gh_scm2long(value);
};

SCM Script_getAttackType(SCM who)
{
        return gh_long2scm(WHO->attacktype);
};

SCM Script_setDamage(SCM who, SCM value)
{
        WHO->stats.dam = gh_scm2int(value);
};

SCM Script_getDamage(SCM who)
{
        gh_int2scm(WHO->stats.dam);
};

SCM Script_setBeenApplied(SCM who, SCM value)
{
  if (gh_scm2bool(value))
  {
        SET_FLAG(WHO, FLAG_BEEN_APPLIED);
  }
  else
  {
        CLEAR_FLAG(WHO, FLAG_BEEN_APPLIED);
  };
};

SCM Script_setIdentified(SCM who, SCM value)
{
  if (gh_scm2bool(value))
  {
        SET_FLAG(WHO, FLAG_IDENTIFIED);
  }
  else
  {
        CLEAR_FLAG(WHO, FLAG_IDENTIFIED);
  };
};

SCM Script_killObject(SCM killer, SCM killed, SCM type)
{
  ((object *)(gh_scm2long(killed)))->speed = 0;
  ((object *)(gh_scm2long(killed)))->speed_left = 0.0;

  update_ob_speed((object *)(gh_scm2long(killed)));

   if(QUERY_FLAG(((object *)(gh_scm2long(killed))) ,FLAG_REMOVED))
  {
        LOG(llevDebug, "Warning in (kill-object): Trying to remove removed object (rejected)\n");
  }
  else
  {
        ((object *)(gh_scm2long(killed)))->stats.hp = -1;
        kill_object(((object *)(gh_scm2long(killed))),1,((object *)(gh_scm2long(killer))), gh_scm2int(type));
  };
  ((object *)(gh_scm2long(killed)))->script_str_death = NULL;


  /* This is to avoid the attack routine to continue after we called killObject, since the attacked
   * object no longer exists. By fixing guile_current_other to NULL, guile_use_weapon_script will
   * return -1, meaning the attack function must be immediately terminated. */
  if (((object *)(gh_scm2long(killed)))==guile_current_other[guile_stack_position])
  {
        guile_current_other[guile_stack_position] = NULL;
  };
};

SCM Script_directionNorth()
{
  return gh_int2scm(1);
};
SCM Script_directionNE()
{
  return gh_int2scm(2);
};
SCM Script_directionEast()
{
  return gh_int2scm(3);
};
SCM Script_directionSE()
{
  return gh_int2scm(4);
};
SCM Script_directionSouth()
{
  return gh_int2scm(5);
};
SCM Script_directionSW()
{
  return gh_int2scm(6);
};
SCM Script_directionWest()
{
  return gh_int2scm(7);
};
SCM Script_directionNW()
{
  return gh_int2scm(8);
};

SCM Script_castAbility(SCM who, SCM spell, SCM direction, SCM options)
{
  int *length = 0;
  char *txt = gh_scm2newstr(options, length);
  cast_spell(WHO, WHO, gh_scm2int(direction), gh_scm2int(spell), 1,spellNormal, txt );
};


SCM Script_castSpell(SCM who, SCM spell, SCM direction, SCM options)
{
  int *length = 0;
  char *txt = gh_scm2newstr(options, length);
  cast_spell(WHO, WHO, gh_scm2int(direction), gh_scm2int(spell), 0,spellNormal, txt );
};

SCM Script_forgetSpell(SCM who, SCM spell)
{
  do_forget_spell(WHO, gh_scm2int(spell));
};

SCM Script_acquireSpell(SCM who, SCM spell)
{
  do_learn_spell(WHO, gh_scm2int(spell), 0);
};

SCM Script_knowSpell_p(SCM who, SCM spell)
{
  return gh_bool2scm(check_spell_known(WHO, gh_scm2int(spell)));
};

SCM Script_checkInvisibleObjectInside(SCM idname, SCM who)
{
  /* From move_marker inside time.c */

  object *tmp2;
  int *length = 0;
  char *txt = gh_scm2newstr(idname, length);
  for(tmp2=WHO->inv;tmp2 !=NULL; tmp2=tmp2->below)
  {
    if(tmp2->type == FORCE && tmp2->slaying && !strcmp(tmp2->slaying,txt)) break;
  };
  return gh_long2scm((long)(tmp2));
};


SCM Script_checkArchetype(SCM what, SCM mapinfo, SCM x, SCM y)
{
  int *length = 0;
  char *txt = gh_scm2newstr(what, length);
  object *foundob = present_arch(find_archetype(txt),has_been_loaded(gh_scm2newstr(mapinfo, length)),gh_scm2long(x),gh_scm2long(y));
  free(txt);
  return gh_long2scm((long)(foundob));
};

SCM Script_checkInventory(SCM who, SCM what)
{
  int *length = 0;
  char *txt = gh_scm2newstr(what, length);
  object *tmp = WHO->inv;
  object *foundob = present_arch_in_ob(find_archetype(txt),WHO);
  if (foundob == NULL)
  {
    while (tmp)
    {
      if (!strcmp(tmp->name,txt))
      {
        free(txt);
        return gh_long2scm((long)(tmp));
      };
      tmp = tmp->below;
    };
  };
  free(txt);
  return gh_long2scm((long)(foundob));
};

SCM Script_getName(SCM who)
{
  return gh_str02scm(WHO->name);
};

/*
 * Creates a new object on the map.
 * It is roughly equivalent of the arch...end map structure
 */
SCM Script_createObject(SCM archname, SCM x, SCM y)
{
  object *myob;
  object *test;
  int *length = 0;
  char *txt = gh_scm2newstr(archname, length);
  int i = 0;
  char *tmpname;
  myob = get_archetype(txt);
  myob = get_archetype_by_object_name(txt);
  if (!strncmp(query_name(myob), "singluarity",11))
  {
        free_object(myob);
        myob = get_archetype(txt);
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
                                test = create_artifact(myob,tmpname);
                        }
                        else
                        {
                                free_string(tmpname);
                        };
                };
        };
  };
  free(txt);
  myob->x = gh_scm2long(x);
  myob->y = gh_scm2long(y);
  myob = insert_ob_in_map(myob, guile_current_who[guile_stack_position]->map ,NULL);
  return gh_long2scm((long)(myob));
};

SCM Script_createObjectInside(SCM archname, SCM where)
{
  object *myob;
  int i;
  char *tmpname;
  object *test;
  int *length = 0;
  char *txt = gh_scm2newstr(archname, length);
  myob = get_archetype_by_object_name(txt);
  if (!strncmp(query_name(myob), "singluarity",11))
  {
        free_object(myob);
        myob = get_archetype(txt);
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
                                test = create_artifact(myob,tmpname);
                        }
                        else
                        {
                                free_string(tmpname);
                        };
                };
        };
  };

  myob = insert_ob_in_ob(myob, (object *)(gh_scm2long(where)));
  free(txt);
  if (((object *)(gh_scm2long(where)))->type == PLAYER)
  {
    esrv_send_item((object *)(gh_scm2long(where)), myob);
  };
  return gh_long2scm((long)(myob));
};

SCM Script_createInvisibleObjectInside(SCM idname, SCM where)
{
  object *myob;
  int *length = 0;
  char *txt = gh_scm2newstr(idname, length);
  myob = get_archetype("force");
  myob->speed = 0.0;
  update_ob_speed(myob);
  myob->slaying = add_string(txt);
  myob = insert_ob_in_ob(myob, (object *)(gh_scm2long(where)));
  free(txt);
  esrv_send_item((object *)(gh_scm2long(where)), myob);
  return gh_long2scm((long)(myob));
};

SCM Script_removeObject(SCM what)
{
  object *myob = (object *)(gh_scm2long(what));
  remove_ob(myob);
  if (guile_current_activator[guile_stack_position]->type == PLAYER)
    esrv_send_inventory(guile_current_activator[guile_stack_position], guile_current_activator[guile_stack_position]);
  free_object(myob);
};

SCM Script_whoIsOther()
{
  return gh_long2scm((long)guile_current_other[guile_stack_position]);
};

SCM Script_whoAmI()
{
  return gh_long2scm((long)guile_current_who[guile_stack_position]);
};

SCM Script_whoIsActivator()
{
  return gh_long2scm((long)guile_current_activator[guile_stack_position]);
};

SCM Script_whatMessage()
{
  char buf[MAX_BUF];
  int *length = 0;
  sprintf(buf, "Message is %s\n", gh_scm2newstr(gh_str02scm(guile_current_text[guile_stack_position]), length));
  return gh_str02scm(guile_current_text[guile_stack_position]);
};

SCM Script_crossfireSay(SCM who, SCM message)
{
  int *length = 0;
  char buf[MAX_BUF];
  object *npc;
  char *txt;
  npc = (object *)(gh_scm2long(who));
  txt = gh_scm2newstr(message, length);
  sprintf(buf, "The %s says:", query_name(npc));
  new_info_map(NDI_NAVY|NDI_UNIQUE, npc->map, buf);
  new_info_map(NDI_NAVY|NDI_UNIQUE, npc->map, txt);
  free(txt);
};

SCM Script_crossfireWrite(SCM who, SCM message, SCM color)
{
  int *length = 0;
  char buf[MAX_BUF];
  object *npc;
  char *txt;
  int flags;
  npc = (object *)(gh_scm2long(who));
  txt = gh_scm2newstr(message, length);

  if (color == SCM_UNDEFINED)
  {
    flags = NDI_BLUE|NDI_UNIQUE;
  }
  else
  {
    flags = gh_scm2int(color);
  };

  new_info_map(flags, npc->map, txt);
  free(txt);
};

SCM Script_crossfireMessage(SCM message, SCM color)
{
  int *length = 0;
  char buf[MAX_BUF];
  char *txt;
  int flags;
  txt = gh_scm2newstr(message, length);
  if (color == SCM_UNDEFINED)
  {
    flags = NDI_BLUE|NDI_UNIQUE;
  }
  else
  {
    flags = gh_scm2int(color);
  };

  new_draw_info(flags, 0, guile_current_activator[guile_stack_position], txt);
  free(txt);
};

SCM Script_isAlive_p(SCM who)
{
  return gh_bool2scm(QUERY_FLAG(WHO, FLAG_ALIVE));
};
SCM Script_isWiz_p(SCM who)
{
  return gh_bool2scm(QUERY_FLAG(WHO, FLAG_WIZ));
};
SCM Script_wasWiz_p(SCM who)
{
  return gh_bool2scm(QUERY_FLAG(WHO, FLAG_WAS_WIZ));
};
SCM Script_isApplied_p(SCM who)
{
  return gh_bool2scm(QUERY_FLAG(WHO, FLAG_APPLIED));
};
SCM Script_isUnpaid_p(SCM who)
{
  return gh_bool2scm(QUERY_FLAG(WHO, FLAG_UNPAID));
};
SCM Script_isFlying_p(SCM who)
{
  return gh_bool2scm(QUERY_FLAG(WHO, FLAG_FLYING));
};
SCM Script_isMonster_p(SCM who)
{
  return gh_bool2scm(QUERY_FLAG(WHO, FLAG_MONSTER));
};
SCM Script_isFriendly_p(SCM who)
{
  return gh_bool2scm(QUERY_FLAG(WHO, FLAG_FRIENDLY));
};
SCM Script_isGenerator_p(SCM who)
{
  return gh_bool2scm(QUERY_FLAG(WHO, FLAG_GENERATOR));
};
SCM Script_isThrown_p(SCM who)
{
  return gh_bool2scm(QUERY_FLAG(WHO, FLAG_IS_THROWN));
};
SCM Script_canSeeInvisible_p(SCM who)
{
  return gh_bool2scm(QUERY_FLAG(WHO, FLAG_SEE_INVISIBLE));
};
SCM Script_canRoll_p(SCM who)
{
  return gh_bool2scm(QUERY_FLAG(WHO, FLAG_CAN_ROLL));
};
SCM Script_isTurnable_p(SCM who)
{
  return gh_bool2scm(QUERY_FLAG(WHO, FLAG_IS_TURNABLE));
};
SCM Script_isUsedUp_p(SCM who)
{
  return gh_bool2scm(QUERY_FLAG(WHO, FLAG_IS_USED_UP));
};
SCM Script_isIdentified_p(SCM who)
{
  return gh_bool2scm(QUERY_FLAG(WHO, FLAG_IDENTIFIED));
};
SCM Script_isSplitting_p(SCM who)
{
  return gh_bool2scm(QUERY_FLAG(WHO, FLAG_SPLITTING));
};
SCM Script_hitback_p(SCM who)
{
  return gh_bool2scm(QUERY_FLAG(WHO, FLAG_HITBACK));
};
SCM Script_blocksview_p(SCM who)
{
  return gh_bool2scm(QUERY_FLAG(WHO, FLAG_BLOCKSVIEW));
};
SCM Script_isUndead_p(SCM who)
{
  return gh_bool2scm(QUERY_FLAG(WHO, FLAG_UNDEAD));
};
SCM Script_isScared_p(SCM who)
{
  return gh_bool2scm(QUERY_FLAG(WHO, FLAG_SCARED));
};
SCM Script_isUnaggressive_p(SCM who)
{
  return gh_bool2scm(QUERY_FLAG(WHO, FLAG_UNAGGRESSIVE));
};
SCM Script_reflectMissiles_p(SCM who)
{
  return gh_bool2scm(QUERY_FLAG(WHO, FLAG_REFL_MISSILE));
};
SCM Script_reflectSpells_p(SCM who)
{
  return gh_bool2scm(QUERY_FLAG(WHO, FLAG_REFL_SPELL));
};
SCM Script_runAway_p(SCM who)
{
  return gh_bool2scm(QUERY_FLAG(WHO, FLAG_RUN_AWAY));
};
SCM Script_canPassThru_p(SCM who)
{
  return gh_bool2scm(QUERY_FLAG(WHO, FLAG_CAN_PASS_THRU));
};
SCM Script_canPickUp_p(SCM who)
{
  return gh_bool2scm(QUERY_FLAG(WHO, FLAG_PICK_UP));
};
SCM Script_isUnique_p(SCM who)
{
  return gh_bool2scm(QUERY_FLAG(WHO, FLAG_UNIQUE));
};
SCM Script_canCastSpell_p(SCM who)
{
  return gh_bool2scm(QUERY_FLAG(WHO, FLAG_CAST_SPELL));
};
SCM Script_canUseScroll_p(SCM who)
{
  return gh_bool2scm(QUERY_FLAG(WHO, FLAG_USE_SCROLL));
};
SCM Script_canUseWand_p(SCM who)
{
  return gh_bool2scm(QUERY_FLAG(WHO, FLAG_USE_WAND));
};
SCM Script_canUseBow_p(SCM who)
{
  return gh_bool2scm(QUERY_FLAG(WHO, FLAG_USE_BOW));
};
SCM Script_canUseArmour_p(SCM who)
{
  return gh_bool2scm(QUERY_FLAG(WHO, FLAG_USE_ARMOUR));
};
SCM Script_canUseWeapon_p(SCM who)
{
  return gh_bool2scm(QUERY_FLAG(WHO, FLAG_USE_WEAPON));
};
SCM Script_canUseRing_p(SCM who)
{
  return gh_bool2scm(QUERY_FLAG(WHO, FLAG_USE_RING));
};
SCM Script_hasXRays_p(SCM who)
{
  return gh_bool2scm(QUERY_FLAG(WHO, FLAG_XRAYS));
};
SCM Script_isFloor_p(SCM who)
{
  return gh_bool2scm(QUERY_FLAG(WHO, FLAG_IS_FLOOR));
};
SCM Script_isLifesave_p(SCM who)
{
  return gh_bool2scm(QUERY_FLAG(WHO, FLAG_LIFESAVE));
};
SCM Script_isSleeping_p(SCM who)
{
  return gh_bool2scm(QUERY_FLAG(WHO, FLAG_SLEEP));
};
SCM Script_standStill_p(SCM who)
{
  return gh_bool2scm(QUERY_FLAG(WHO, FLAG_STAND_STILL));
};
SCM Script_onlyAttack_p(SCM who)
{
  return gh_bool2scm(QUERY_FLAG(WHO, FLAG_ONLY_ATTACK));
};
SCM Script_isConfused_p(SCM who)
{
  return gh_bool2scm(QUERY_FLAG(WHO, FLAG_CONFUSED));
};
SCM Script_hasStealth_p(SCM who)
{
  return gh_bool2scm(QUERY_FLAG(WHO, FLAG_STEALTH));
};
SCM Script_isCursed_p(SCM who)
{
  return gh_bool2scm(QUERY_FLAG(WHO, FLAG_CURSED));
};
SCM Script_isDamned_p(SCM who)
{
  return gh_bool2scm(QUERY_FLAG(WHO, FLAG_DAMNED));
};
SCM Script_knownMagical_p(SCM who)
{
  return gh_bool2scm(QUERY_FLAG(WHO, FLAG_KNOWN_MAGICAL));
};
SCM Script_knownCursed_p(SCM who)
{
  return gh_bool2scm(QUERY_FLAG(WHO, FLAG_KNOWN_CURSED));
};
SCM Script_canUseSkill_p(SCM who)
{
  return gh_bool2scm(QUERY_FLAG(WHO, FLAG_CAN_USE_SKILL));
};
SCM Script_beenApplied_p(SCM who)
{
  return gh_bool2scm(QUERY_FLAG(WHO, FLAG_BEEN_APPLIED));
};
SCM Script_canUseRod_p(SCM who)
{
  return gh_bool2scm(QUERY_FLAG(WHO, FLAG_USE_ROD));
};
SCM Script_canUseHorn_p(SCM who)
{
  return gh_bool2scm(QUERY_FLAG(WHO, FLAG_USE_HORN));
};
SCM Script_makeInvisible_p(SCM who)
{
  return gh_bool2scm(QUERY_FLAG(WHO, FLAG_MAKE_INVIS));
};
SCM Script_isBlind_p(SCM who)
{
  return gh_bool2scm(QUERY_FLAG(WHO, FLAG_BLIND));
};
SCM Script_canSeeInDark_p(SCM who)
{
  return gh_bool2scm(QUERY_FLAG(WHO, FLAG_SEE_IN_DARK));
};

/*
 * Sets the position of object who to (X,Y)
 */
SCM Script_setPosition(SCM who, SCM X, SCM Y)
{
  transfer_ob(WHO, gh_scm2int(X), gh_scm2int(Y), 0, NULL);
};

/*
 * Returns the X position of object 'who'
 */
SCM Script_getXPosition(SCM who)
{
  return gh_long2scm(WHO->x);
};

/*
 * Returns the Y position of object 'who'
 */
SCM Script_getYPosition(SCM who)
{
  return gh_long2scm(WHO->y);
};

/*
 * Sets the title of object who to newname.
 */
SCM Script_setTitle(SCM who, SCM newname)
{
  int *length = 0;
  char *txt = gh_scm2newstr(newname, length);
  WHO->title = add_string(txt);
  free(txt);
};

/*
 * Sets the AC value of object 'who' to 'newvalue'
 */
SCM Script_setAC(SCM who, SCM newvalue)
{
  WHO->stats.ac = gh_scm2long(newvalue);
};

/*
 * Returns the AC value of object 'who'
 */
SCM Script_getAC(SCM who)
{
  return gh_long2scm(WHO->stats.ac);
};

/*
 * Returns the HitPoints value of object 'who'
 */
SCM Script_getHP(SCM who)
{
  return gh_long2scm(WHO->stats.hp);
};

/*
 * Returns the SpellPoints value of object 'who'
 */
SCM Script_getSP(SCM who)
{
  return gh_long2scm(WHO->stats.sp);
};

/*
 * Returns the GracePoints value of object 'who'
 */
SCM Script_getGP(SCM who)
{
  return gh_long2scm(WHO->stats.grace);
};

/*
 * Returns the FoodPoints value of object 'who'
 */
SCM Script_getFP(SCM who)
{
  return gh_long2scm(WHO->stats.food);
};

/*
 * Returns the maximum HitPoints value of object 'who'
 */
SCM Script_getMaxHP(SCM who)
{
  return gh_long2scm(WHO->stats.maxhp);
};

/*
 * Returns the maximum SpellPoints value of object 'who'
 */
SCM Script_getMaxSP(SCM who)
{
  return gh_long2scm(WHO->stats.maxsp);
};

/*
 * Sets the value of HitPoints of object 'who' to 'newvalue'
 */
SCM Script_setHP(SCM who, SCM newvalue)
{
  WHO->stats.hp = gh_scm2long(newvalue);
};

/*
 * Sets the value of SpellPoints of object 'who' to 'newvalue'
 */
SCM Script_setSP(SCM who, SCM newvalue)
{
  WHO->stats.sp = gh_scm2long(newvalue);
};

/*
 * Sets the value of FoodPoints of object 'who' to 'newvalue'
 */
SCM Script_setFP(SCM who, SCM newvalue)
{
  WHO->stats.food = gh_scm2long(newvalue);
};

/*
 * Sets the value of GracePoints of object 'who' to 'newvalue'
 */
SCM Script_setGP(SCM who, SCM newvalue)
{
  WHO->stats.grace = gh_scm2long(newvalue);
};

/*
 * Sets the maximum value of HitPoints of object 'who' to 'newvalue'
 */
SCM Script_setMaxHP(SCM who, SCM newvalue)
{
  WHO->stats.maxhp = gh_scm2long(newvalue);
};

/*
 * Sets the maximum value of SpellPoints of object 'who' to 'newvalue'
 */
SCM Script_setMaxSP(SCM who, SCM newvalue)
{
  WHO->stats.maxsp = gh_scm2long(newvalue);
};


/*
 * Sets the Charisma value of the object who to newvalue
 */
SCM Script_setCha(SCM who, SCM newvalue)
{
  char buf[MAX_BUF];
  WHO->stats.Cha = gh_scm2long(newvalue);
  if (WHO->type == PLAYER)
  {
      WHO->contr->orig_stats.Cha = gh_scm2long(newvalue);
  };
  fix_player(WHO);
};

/*
 * Sets the Strength value of object who to newvalue
 */
SCM Script_setStr(SCM who, SCM newvalue)
{
  WHO->stats.Str = gh_scm2long(newvalue);
  if (WHO->type == PLAYER)
  {
      WHO->contr->orig_stats.Str = gh_scm2long(newvalue);
  };
  fix_player(WHO);
};

/*
 * Sets the Dexterity value of object who to newvalue
 */
SCM Script_setDex(SCM who, SCM newvalue)
{
  WHO->stats.Dex = gh_scm2long(newvalue);
  if (WHO->type == PLAYER)
  {
      WHO->contr->orig_stats.Dex = gh_scm2long(newvalue);
  };
  fix_player(WHO);
};

/*
 * Sets the Intelligence value of object who to newvalue
 */
SCM Script_setInt(SCM who, SCM newvalue)
{
  WHO->stats.Int = gh_scm2long(newvalue);
  if (WHO->type == PLAYER)
  {
      WHO->contr->orig_stats.Int = gh_scm2long(newvalue);
  };
  fix_player(WHO);
};

/*
 * Sets the Wisdom value of object who to newvalue
 */
SCM Script_setWis(SCM who, SCM newvalue)
{
  WHO->stats.Wis = gh_scm2long(newvalue);
  if (WHO->type == PLAYER)
  {
      WHO->contr->orig_stats.Wis = gh_scm2long(newvalue);
  };
  fix_player(WHO);
};

/*
 * Sets the Power value of object who to newvalue
 */
SCM Script_setPow(SCM who, SCM newvalue)
{
  WHO->stats.Pow = gh_scm2long(newvalue);
  if (WHO->type == PLAYER)
  {
      WHO->contr->orig_stats.Pow = gh_scm2long(newvalue);
  };
  fix_player(WHO);
};

/*
 * Sets the Constitution value of object who to newvalue
 */
SCM Script_setCon(SCM who, SCM newvalue)
{
  WHO->stats.Con = gh_scm2long(newvalue);
  if (WHO->type == PLAYER)
  {
      WHO->contr->orig_stats.Con = gh_scm2long(newvalue);
  };
  fix_player(WHO);
};

/*
 * Gets the Constitution value of object 'who'
 */
SCM Script_getCon(SCM who)
{
  return gh_long2scm(WHO->stats.Con);
};

/*
 * Gets the Strength value of object 'who'
 */
SCM Script_getStr(SCM who)
{
  return gh_long2scm(WHO->stats.Str);
};

/*
 * Gets the Dexterity value of object 'who'
 */
SCM Script_getDex(SCM who)
{
  return gh_long2scm(WHO->stats.Dex);
};

/*
 * Gets the Intelligence value of object 'who'
 */
SCM Script_getInt(SCM who)
{
  return gh_long2scm(WHO->stats.Int);
};

/*
 * Gets the Wisdom value of object 'who'
 */
SCM Script_getWis(SCM who)
{
  return gh_long2scm(WHO->stats.Wis);
};

/*
 * Gets the Power value of object 'who'
 */
SCM Script_getPow(SCM who)
{
  return gh_long2scm(WHO->stats.Pow);
};

/*
 * Gets the Charisma value of object 'who'
 */
SCM Script_getCha(SCM who)
{
  return gh_long2scm(WHO->stats.Cha);
};

/*
 * Returns the type of object 'who'
 */
SCM Script_getType(SCM who)
{
        return gh_int2scm(WHO->type);
};

/*
 * Tests if object 'who' is of type 'type'
 */
SCM Script_isType(SCM who, SCM what)
{
        return gh_bool2scm(WHO->type==what);
};


