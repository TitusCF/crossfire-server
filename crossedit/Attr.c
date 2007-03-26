/*
	Copyright (C) 1993 Jarkko Sonninen & Petri Heinila

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

	The authors can be reached via e-mail to Jarkko.Sonninen@lut.fi
	or Petri.Heinila@lut.fi .
*/



#include "Posix.h"
#include "Attr.h"
#include "X11.h"
#include "CrList.h"
#include "CrFace.h"
#include "CrEdit.h"
#include "Cnv.h"
#include "Edit.h"
#include "App.h"
#include "loader.h"
#include "debug.h"

static void AttrReset(Attr self);

/* variables, combination bit */
#define T_Path      (1<<I_Path)
#define T_X         (1<<I_X)
#define T_Y         (1<<I_Y)
#define T_Weight    (1<<I_Weight)
#define T_Connect   (1<<I_Connect)
#define T_Hp        (1<<I_Hp)
#define T_Trigger   (1<<I_Trigger)
#define T_Sacrifice (1<<I_Sacrifice)
#define T_Count     (1<<I_Count)
#define T_Lockcode  (1<<I_Lockcode) /* slaying - field */
#define T_Direction (1<<I_Direction)
#define T_Rotation  (1<<I_Rotation)
#define T_NoPick    (1<<I_NoPick)
#define T_Unique    (1<<I_Unique)
#define T_WeightL   (1<<I_WeightL)
#define T_Brand     (1<<I_Brand)
#define T_Maker     (1<<I_Maker)

/*** types ar Combinations ***/
#define T_Exit      (T_Path | T_X | T_Y )
#define T_Trapdoor  (T_Weight | T_X | T_Y )
#define T_Connected (T_Connect )
#define T_Pit       (T_Connect | T_X | T_Y )
#define T_Monster   (T_Hp )
#define T_Pedestal  (T_Connected | T_Trigger )
#define T_Altar     (T_Connected | T_Sacrifice | T_Count )
#define T_Button    (T_Weight | T_Connected)
#define T_Director  (T_Connected | T_Direction | T_Rotation)
#define T_Lockdoor  (T_Lockcode)
#define T_Key       (T_Lockcode | T_Unique)
#define T_Container (T_Lockcode | T_WeightL | T_Brand | T_Unique | T_NoPick)
#define T_Sign      0
#define T_Map       (T_X | T_Y | T_Path)

#define T_Default   0

/*
 * transfer type from game to editor, return negative on error
 *
 * Basically, for certain things, we want to have default values set up.
 */
int GetType (object *tmp)
{
    if (!tmp)
	return (-1);

    if (tmp->head)
	tmp = tmp->head;

    switch (tmp->type) {
    case TELEPORTER:
		return T_Connected|T_Exit;
    case PLAYER_CHANGER:
    case EXIT:
	return T_Exit;
    case TRAPDOOR:
	return T_Trapdoor;
    case HOLE:
	return T_Pit;
    case BUTTON:
    case TRIGGER_BUTTON:
	return T_Button;
	 case CREATOR:
		return T_Connected|T_Maker;
	 case CONVERTER:
		return T_Maker|T_Lockcode;
    case GATE:
    case CF_HANDLE:
    case TIMED_GATE:
    case MAGIC_EAR:
    case TRIGGER:
	return T_Connected;
    case CHECK_INV:
	return T_Connected | T_Lockcode;
    case ALTAR:
    case TRIGGER_ALTAR:
	return T_Altar;
    case PEDESTAL:
    case TRIGGER_PEDESTAL:
	return T_Pedestal;
    case BOOK:
    case SIGN:
	return T_Sign;
	 case MARKER:
    case LOCKED_DOOR:
	return T_Lockdoor;
    case SPECIAL_KEY:
	return T_Key;
    case MAP: /* shouldn't happen ... */
	return T_Map;
    case DIRECTOR:
    case FIREWALL:
	return (NUM_ANIMATIONS(tmp) > 0)
	    ? T_Director : T_Connected;
    case CONTAINER:
	return T_Container;
    default:
	if (QUERY_FLAG(tmp, FLAG_MONSTER))
	    return T_Monster;
	else
	    return T_Default;
    }
    /* return -1; */
}

/**********************************************************************
 *Section: get value from object for displying
 *Commentary:
 * Remeber, the strings from obejct may be NULL's
 **********************************************************************/

static void getX (object *ob, char *str, XtPointer c) {
    sprintf(str,"%d",EXIT_X(ob));
}

static void getY (object *ob, char *str, XtPointer c) {
    sprintf(str,"%d",EXIT_Y(ob));
}

static void getPath (object *ob, char *str, XtPointer c) {
    strcpy (str, EXIT_PATH(ob) ? EXIT_PATH(ob) : "");
}

static void getWeight (object *ob, char *str, XtPointer c) {
    sprintf(str,"%d",ob->weight);
}

/*** connected ***/
static void getConnect (object *ob, char *str, XtPointer c) {
    sprintf(str,"%d",get_button_value(ob));
}

static void getHp (object *ob, char *str, XtPointer c) {
    sprintf(str,"%d",ob->stats.hp);
}

#define NotUsed "(not-used)"

static void getTrigger (object *ob, char *str, XtPointer c) {
    if(!ob->slaying || !*ob->slaying)
	sprintf(str,NotUsed);
    else
	strcpy (str, ob->slaying);
}

/*** sacrifice ***/
static void getSacrifice (object *ob, char *str, XtPointer c) {
    if(!ob->slaying || !*ob->slaying) {
	if(!ob->arch->clone.slaying)
	    LOG(llevError,"missing sacrifice for altar\n");
	strcpy (str, ob->arch->clone.slaying);
    } else
	strcpy (str, ob->slaying);
}

static void getCount (object *ob, char *str, XtPointer c) {
    sprintf(str,"%d",ob->stats.food);
}

/*** lockcode ***/
static void getLockcode (object *ob, char *str, XtPointer c) {
    if(!ob->slaying || !*ob->slaying) {
	sprintf(str,NotUsed);
    } else
	strcpy (str, ob->slaying);
}

/*** direction ***/
static void getDirection (object *ob, char *str, XtPointer c) {
    sprintf(str,"%d",ob->stats.maxsp);
}

/*** rotation ***/
static void getRotation (object *ob, char *str, XtPointer c) {
    sprintf(str,"%d",ob->stats.sp);
}

/*** unique ***/
static void getUnique (object *ob, char *str, XtPointer c) {
    *str = QUERY_FLAG(ob, FLAG_UNIQUE) ? ~0 : 0;
}

/*** no pick ***/
static void getNoPick (object *ob, char *str, XtPointer c) {
    *str = QUERY_FLAG(ob, FLAG_NO_PICK) ? ~0 : 0;
}

/*** weight limit ***/
static void getWeightL (object *ob, char *str, XtPointer c) {
    sprintf(str,"%d",ob->weight_limit);
}

/*** brand ***/
static void getBrand (object *ob, char *str, XtPointer c) {
    if(!ob->race || !*ob->race)
	sprintf(str,NotUsed);
    else
	sprintf(str,"%s",ob->race);
}

/*** brand ***/
static void getMakes (object *ob, char *str, XtPointer c) {
    if(!ob->other_arch)
	sprintf(str,NotUsed);
    else
	sprintf(str,"%s",ob->other_arch->name);
}

/*
 * putValue functions
 *
 */

/*** coord ***/
static void putX (object *ob, char *str, XtPointer c) {
    EXIT_X(ob) = atoi(str);
}

static void putY (object *ob, char *str, XtPointer c) {
    EXIT_Y(ob) = atoi(str);
}

/*** path ***/
static void putPath (object *ob, char *str, XtPointer c) {
    if(EXIT_PATH(ob)) free_string(EXIT_PATH(ob));
    EXIT_PATH(ob) = NULL;
    if(strlen(str)) EXIT_PATH(ob) = add_string(str);
}

/*** sacrifice ***/
static void putSacrifice (object *ob, char *str, XtPointer c) {
    if(ob->slaying) free_string(ob->slaying);
    ob->slaying = add_string(str);
}

/*** trigger ***/
static void putTrigger (object *ob, char *str, XtPointer c) {
    if(!strcmp(str,NotUsed))
	ob->slaying = NULL;
    else {
	if(ob->slaying) free_string(ob->slaying);
	ob->slaying = add_string(str);
    }
}

/*** weight ***/
static void putWeight (object *ob, char *str, XtPointer c) {
    ob->weight = atoi(str);
}

/*** connect ***/
static void putConnect (object *ob, char *str, XtPointer c) {
    if (QUERY_FLAG(ob, FLAG_IS_LINKED))
	remove_button_link(ob);
    add_button_link(ob, ob->map, atoi(str));
}

/*** hp ***/
static void putHp (object *ob, char *str, XtPointer c) {
    ob->stats.hp = atoi(str);
}

/*** count ***/
static void putCount (object *ob, char *str, XtPointer c) {
    ob->stats.food = atoi(str);
}

/*** lockcode ***/
static void putLockcode (object *ob, char *str, XtPointer c) {
    if(!strcmp(str,NotUsed))
       ob->slaying = NULL;
    else {
	if(ob->slaying) free_string(ob->slaying);
	ob->slaying = add_string(str);
    }
}

/*** direction ***/
static void putDirection (object *ob, char *str, XtPointer c) {
    ob->stats.maxsp = atoi(str);
    animate_object (ob, ob->direction);
}

/*** rotation ***/
static void putRotation (object *ob, char *str, XtPointer c) {
    ob->stats.sp = atoi(str);
    animate_object (ob, ob->direction);
}

/*** unique ***/
static void putUnique (object *ob, char *str, XtPointer c) {
    *str ? SET_FLAG(ob, FLAG_UNIQUE) : CLEAR_FLAG(ob, FLAG_UNIQUE);
}

/*** no pick ***/
static void putNoPick (object *ob, char *str, XtPointer c) {
    *str ? SET_FLAG(ob, FLAG_NO_PICK) : CLEAR_FLAG(ob, FLAG_NO_PICK);
}

/*** weight limit ***/
static void putWeightL (object *ob, char *str, XtPointer c) {
       debug0("NO putWeightL\n");
}

/*** brand ***/
static void putBrand (object *ob, char *str, XtPointer c) {
    if(!strcmp(str,NotUsed))
	ob->race = NULL;
    else {
	if(ob->race) free_string(ob->race);
	ob->race = add_string(str);
    }
}

static void putMakes (object *ob, char *str, XtPointer c) {
    if(!strcmp(str,NotUsed))
	ob->other_arch = NULL;
    else {
		ob->other_arch = find_archetype(str);
    }
}


/**********************************************************************
 * tags
 **********************************************************************/

AttrDef AttrDescription[] = {
    {"Path",		TypeString, getPath,		putPath},
    {"X",		TypeString, getX,		putX},
    {"Y",		TypeString, getY,		putY},
    {"Weight",		TypeString, getWeight,		putWeight},
    {"Connect",		TypeString, getConnect,		putConnect},
    {"Hp",		TypeString, getHp,		putHp},
    {"Trigger",		TypeString, getTrigger,		putTrigger},
    {"Sacrifice",	TypeString, getSacrifice,	putSacrifice},
    {"Count",		TypeString, getCount,		putCount},
    {"Lockcode",	TypeString, getLockcode,	putLockcode},
    {"Direction",	TypeString, getDirection,	putDirection},
    {"Rotation",	TypeString, getRotation,	putRotation},
    {"No Pick",	        TypeToggle, getNoPick,		putNoPick},
    {"Unique",	        TypeToggle, getUnique,		putUnique},
    {"WeightL",	        TypeString, getWeightL,		putWeightL},
    {"Brand",	        TypeString, getBrand,		putBrand},
	 {"Makes",  TypeString, getMakes, putMakes},  /* other_arch */
    {NULL,		0,		NULL,		NULL}
};

static char *allowed_variables[] = {

    "name", "race", "slaying", "other_arch", "last_heal", "last_sp",
    "last_eat", "speed", "speed_left", "slow_move", "face", "Str", "Dex",
    "Con", "Wis", "Cha", "Int", "Pow", "hp", "maxhp", "sp", "maxsp", "exp",
    "food", "dam", "wc", "ac", "nrof", "level", "direction", "type",
    "material", "value", "weight", "carrying", "immune", "protected",
    "attacktype", "vulnerable", "path_attuned", "path_repelled",
    "path_denied", "invisible", "magic", "state", "alive", "applied",
    "unpaid", "need_an", "need_ie", "no_pick", "no_pass", "walk_on",
    "walk_off", "fly_on", "fly_off", "monster",
    "neutral", "no_attack", "no_damage", "friendly",
    "generator", "is_thrown", "auto_apply", "treasure", "apply_once",
    "see_invisible", "can_roll", "is_turning", "is_turnable", "is_used_up",
    "identified", "reflecting", "changing", "splitting", "hitback",
    "startequip", "blocksview", "undead", "scared", "unaggressive",
    "reflect_missile", "reflect_spell", "no_magic", "wiz", "was_wiz",
    "tear_down", "luck", "run_away", "pass_thru", "can_pass_thru",
    "pick_up", "anim_speed", "container", "no_drop", "no_pretext",
    "will_apply", "random_movement", "can_apply", "can_cast_spell",
    "can_use_scroll", "can_use_wand", "can_use_bow", "can_use_armour",
    "can_use_weapon", "can_use_ring", "has_ready_wand", "has_ready_bow",
    "xrays", "is_floor", "lifesave", "no_strength", "sleep", "stand_still",
    "random_move", "only_attack", "armour", "attack_movement", "move_state",
    "confused", "stealth", "connected", "cursed", "damned", "see_anywhere",
    "known_magical", "known_cursed", "can_use_skill", "been_applied",
    "title", "has_ready_rod", "can_use_rod", "has_ready_horn",
    "can_use_horn", "expmul", "unique", "make_invisible", "is_wooded",
    "is_hilly", "has_ready_skill", "has_ready_weapon", "no_skill_ident",
    "glow_radius", "is_blind", "can_see_in_dark", "is_cauldron",
    "randomitems", "is_dust", "no_steal", "one_hit","berserk",
    "sub_type", "sub_type2","casting_speed", "elevation",
    "save_on_overlay",
/* GROS - These are hooks for script events */
    "script_load","script_apply","script_say","script_trigger", "script_time",
    "script_attack","script_drop", "script_throw", "script_stop", "script_death", "current_weapon_script",
    "start_script_load", "start_script_apply","start_script_say","start_script_trigger","start_script_time",
    "start_script_attack","start_script_drop","start_script_throw","start_script_stop", "start_script_death",
    "end_script_load", "end_script_apply","end_script_say","end_script_trigger","end_script_time",
    "end_script_attack","end_script_drop","end_script_throw","end_script_stop", "end_script_death",

#ifdef NPC_PROG
    "npc_status", "npc_program",
#endif

    /* Resistances */ "resist_physical", "resist_magic", "resist_fire",
   "resist_electricity", "resist_cold", "resist_confusion", "resist_acid",
   "resist_drain", "resist_weaponmagic", "resist_ghosthit", "resist_poison",
   "resist_slow", "resist_paralyze", "resist_turn_undead", "resist_fear",
   "resist_cancellation", "resist_deplete", "resist_death", "resist_chaos",
   "resist_counterspell", "resist_godpower", "resist_holyword",
   "resist_blind", "elevation"

};

#define ALLOWED_VARIABLES (sizeof(allowed_variables) / sizeof (char *))

/**********************************************************************
 * widgets
 **********************************************************************/

/*
 * member: create tag widgets
 * parent: parent container
 */
static void AttrTagsCreate(Attr self,Widget parent)
{
    int i;
    self->attrnumber = 0;
    while (self->desc[self->attrnumber].label)
	self->attrnumber++;

    self->tags = (AttrTags*)XtCalloc(self->attrnumber,sizeof(AttrTags));
    for(i=0; i < self->attrnumber; i++) {
	if (self->desc[i].type == TypeString) {
	    self->tags[i].cont = XtVaCreateWidget
		("box",boxWidgetClass,parent,
		 XtNorientation,XtorientHorizontal,
		 NULL);
	    XtVaCreateManagedWidget
		("label",labelWidgetClass,self->tags[i].cont,
		 XtNlabel,self->desc[i].label,
		 NULL);
	    self->tags[i].value = XtVaCreateManagedWidget
		("value",asciiTextWidgetClass,self->tags[i].cont,
		 XtNtype,XawAsciiString,
		 XtNeditType,XawtextEdit,
		 NULL);
	}
    }
    for(i=0; i < self->attrnumber; i++) {
	if (self->desc[i].type == TypeToggle) {
	    self->tags[i].cont = XtVaCreateWidget
		("box",boxWidgetClass,parent,
		 XtNorientation,XtorientHorizontal,
		 NULL);

	    self->tags[i].value = XtVaCreateManagedWidget
		("toggle",toggleWidgetClass, self->tags[i].cont,
		 XtNlabel,self->desc[i].label,
		 NULL);
	}
    }
}


/*
 * callback: receive ok
 */
static void AttrOkCb(Widget w,XtPointer client,XtPointer call)
{
    Attr self = (Attr)client;

    AttrApply(self);
    AttrDestroy (self);
}

/*
 * receive apply
 */
static void AttrApplyCb(Widget w,XtPointer client,XtPointer call)
{
  Attr self = (Attr)client;
  AttrApply(self);
}

/*
 * receive cancel
 */
static void AttrCancelCb(Widget w,XtPointer client,XtPointer call)
{
  Attr self = (Attr)client;

  AttrDestroy(self);
}

/*
 * receive dump
 */
static void AttrDumpCb(Widget w,XtPointer client,XtPointer call)
{
    char buf[HUGE_BUF];
  Attr self = (Attr)client;

  dump_object(self->op, buf, sizeof(buf));
  CnvBrowseShowString(self->dump,buf);
}

/**********************************************************************
 * widget-message
 **********************************************************************/


static CrListNode AttrInventoryNext(XtPointer client,XtPointer call)
{
    Attr self = (Attr)client;
    CrListNode retNode = (CrListNode)call;
    static struct _CrListNode node;
    object *op = NULL;

    if (!self->op)
	return (CrListNode)NULL;

    if(retNode) { /* next */
	op = ((object *)retNode->ptr)->below;
    } else {      /* begin */
	op = self->op->inv;
    }

    if(op) {
	node.face = op->face;
	node.name = op->name;
	node.ptr = (XtPointer)op;
	return &node;
    }
    return (CrListNode)NULL;
}

/*
 * callback: insert object
 */
static void InsertCb(Widget w,XtPointer client,XtPointer call)
{
    Attr self = (Attr)client;
    /* CrListCall ret = (CrListCall)call; */
    object *obj;

    if((obj = AppItemGetObject(self->app)) && AttrGetObject(self)) {
	debug1("Attr-InsertCb() %s\n",obj->name);
	(void) insert_ob_in_ob(object_create_clone(obj),AttrGetObject(self));
    }
}

/*
 * create recursively attributes from inventory
 */
static void AttrInventorySelectCb(Widget w,XtPointer client,XtPointer call)
{
    Attr self = (Attr)client;
    CrListCall p = (CrListCall)call;
    object *ob = (object *)p->node;

    if (self->attr) {
	AttrDestroy (self->attr);
    }

    self->attr = AttrCreate("attr",self->app, ob,
			    AttrDescription, GetType(ob), self->client);
}

/*
 * callback: delete object from look window
 */
static void DeleteCb(Widget w,XtPointer client,XtPointer call)
{
    Attr self = (Attr)client;
    CrListCall ret = (CrListCall)call;
    object *obj = ret->node;

    debug1("Attr-DeleteCb() %s\n",obj->name);
    if (self->attr && self->attr->op == obj)
	AttrDestroy (self->attr);
    remove_ob(obj);
    free_object(obj);
}

/**********************************************************************
 * widget - variables
 **********************************************************************/

/*
 *
 */
static void AttrVarSelectCb(Widget w,XtPointer client,XtPointer call)
{
    Attr self = (Attr)client;
    XawListReturnStruct *ret = (XawListReturnStruct*)call;
    XtVaSetValues(self->iw.var,
		  XtNstring,ret->string,
		  NULL);
    XtPopdown(self->vars.shell);
}

/*
 *
 */
static void AttrVarCancelCb(Widget w,XtPointer client,XtPointer call)
{
    Attr self = (Attr)client;
    XtPopdown(self->vars.shell);
}

/*
 * compare funtion for sorting in PathListGet()
 */
static int StrCmp (const void **s1, const void **s2)
{
    return strcmp (*s1, *s2);
}

/*
 * create widget layout for selectin variable from list
 */
static void AttrVarLayout(Attr self,Widget parent)
{
  Widget form,cancel,view;

  self->vars.shell = XtVaCreatePopupShell
    ("vars",transientShellWidgetClass,parent,
     NULL);
  form = XtVaCreateManagedWidget
    ("form",formWidgetClass,self->vars.shell,
     NULL);
  view = XtVaCreateManagedWidget
    ("view",viewportWidgetClass,form,
     NULL);
  self->vars.list = XtVaCreateManagedWidget
    ("list",listWidgetClass,view,
     NULL);
  XtAddCallback(self->vars.list,XtNcallback,AttrVarSelectCb,(XtPointer)self);
  /*** sort varibales ***/

  qsort(allowed_variables, ALLOWED_VARIABLES,
	sizeof(char *),(int (*)())StrCmp);

  XawListChange(self->vars.list,allowed_variables,
		ALLOWED_VARIABLES, 0, True);
  cancel = XtVaCreateManagedWidget
    ("cancel",commandWidgetClass,form,
     XtNfromVert,view,
     NULL);
  XtAddCallback(cancel,XtNcallback,AttrVarCancelCb,(XtPointer)self);
  CnvCenterWidget(self->vars.shell);
}

/*
 *
 */
static void AttrVarGetCb(Widget w,XtPointer client,XtPointer call)
{
  Attr self = (Attr)client;
  XtPopup(self->vars.shell,XtGrabExclusive);
}

/**********************************************************************
 * members
 **********************************************************************/

static void AppLayout(Attr self,Widget parent, char *name)
{
    Widget form,view1,pane;
    Widget ok,apply,cancel;
    Widget view;

    self->shell = XtVaCreatePopupShell
	(name, topLevelShellWidgetClass, parent, NULL);
    form = XtVaCreateManagedWidget("form",formWidgetClass,self->shell,NULL);

    self->iw.name = XtVaCreateManagedWidget
	("name",asciiTextWidgetClass,form,
	 XtNtype,XawAsciiString,
	 XtNeditType,XawtextEdit,
	 XtNresize,False,
	 XtNadjust,False,
	 NULL);

    self->iw.face = XtVaCreateManagedWidget
	("face",crFaceWidgetClass,form,
	 XtNfromVert,self->iw.name,
	 XtNresize,True,
	 XtNadjust,False,
	 XtNobject,self->op,
	 NULL);

    /*** dump ***/
    self->iw.exact = XtVaCreateManagedWidget
	("exact",commandWidgetClass,form,
	 XtNfromVert,self->iw.face,
	 XtNresize,True,
	 XtNadjust,False,
	 NULL);
    XtAddCallback(self->iw.exact,XtNcallback,AttrDumpCb,(XtPointer)self);

    /*** inventory ***/
    view = XtVaCreateManagedWidget
	("inventory",viewportWidgetClass,form,
	 XtNfromVert,self->iw.name,
	 XtNfromHoriz,self->iw.face,
	 NULL);

    self->iw.inv = XtVaCreateManagedWidget
	("list",crListWidgetClass,view,
	 XtNpackage, self,
	 XtNnext, AttrInventoryNext,
	 NULL);
    XtAddCallback(self->iw.inv,XtNselectCallback,AttrInventorySelectCb,
		  (XtPointer)self);
    XtAddCallback(self->iw.inv,XtNinsertCallback,InsertCb,
		  (XtPointer)self);
    XtAddCallback(self->iw.inv,XtNdeleteCallback,DeleteCb,
		  (XtPointer)self);

    /*** multi ***/
    view1 = XtVaCreateManagedWidget
	("view",viewportWidgetClass,form,
	 /*XtNallowVert,True,*/
	 XtNforceBars,True,
	 XtNfromVert,self->iw.exact,
	 NULL);
    pane = XtVaCreateManagedWidget
	("pane",boxWidgetClass,view1,
	 XtNorientation,XtorientVertical,
	 NULL);

    /*** variable setting ***/
    AttrVarLayout(self,parent);

    self->iw.msg = XtVaCreateManagedWidget
	("msg",asciiTextWidgetClass,form,
	 XtNtype,XawAsciiString,
	 XtNeditType,XawtextEdit,
	 XtNfromHoriz,NULL,
	 XtNfromVert,view1,
	 NULL);


  self->iw.vars = XtVaCreateManagedWidget
    ("vars",commandWidgetClass,form,
     XtNfromVert,self->iw.msg,
     NULL);
  XtAddCallback(self->iw.vars,XtNcallback,AttrVarGetCb,(XtPointer)self);
  self->iw.var = XtVaCreateManagedWidget
    ("var",asciiTextWidgetClass,form,
     XtNfromHoriz,self->iw.vars,
     XtNfromVert,self->iw.msg,
     XtNtype,XawAsciiString,
     XtNeditType,XawtextEdit,
     NULL);

  /*** reponses ***/
  ok = XtVaCreateManagedWidget
    ("ok",commandWidgetClass,form,
     XtNfromVert,self->iw.vars,
     NULL);
  XtAddCallback(ok,XtNcallback,AttrOkCb,(XtPointer)self);
  apply = XtVaCreateManagedWidget
    ("apply",commandWidgetClass,form,
     XtNfromVert,self->iw.vars,
     XtNfromHoriz,ok,
     NULL);
  XtAddCallback(apply,XtNcallback,AttrApplyCb,(XtPointer)self);
  cancel = XtVaCreateManagedWidget
    ("cancel",commandWidgetClass,form,
     XtNfromVert,self->iw.vars,
     XtNfromHoriz,apply,
     NULL);
  XtAddCallback(cancel,XtNcallback,AttrCancelCb,(XtPointer)self);

  AttrTagsCreate(self,pane);
}

/*
 * - create object attribute editor from given object
 * - change values in future of given object
 * - struct Attr don't have to be initialized, but have to be
 *   allocated anyway.
 * - create widgets & popup window
 */
Attr AttrCreate(char *name, App app, object *ob,
		AttrDef *desc, unsigned long flags, Edit edit)
{
  Attr self = (Attr) XtMalloc (sizeof(struct _Attr));

  if (ob->head)
    ob = ob->head;
  self->op = ob;
  self->app = app;
  self->client = edit;
  self->attr = NULL;

  self->desc = desc;
  AppLayout (self, self->app->shell, name);
  AttrChange(self,self->op, flags, self->client);

  self->dump = CnvBrowseCreate("dump", self->app->shell, NULL);
  XtPopup(self->shell,XtGrabNone);
  self->isup = True;
  return self;
}

/*
 * change object to another
 */
void AttrChange(Attr self,object *ob, unsigned long flags, Edit edit)
{
  char buf[BUFSIZ];
  int i, mask = 1;

  if (!self)
      return;

  if (self->attr)
      AttrDestroy (self->attr);

  if (ob && ob->head)
    ob = ob->head;
  self->op = ob;

  if(!ob) {
      AttrReset(self);
      XtVaSetValues(self->shell,
		    XtNtitle,"",
		    NULL);
       return;
  }

  self->flags = flags;

  /*** name ***/
  XtVaSetValues(self->iw.name,
		XtNstring,self->op->name,
		NULL);

  /*** object ***/
  XtVaSetValues(self->iw.face,
		XtNobject,self->op,
		NULL);

  /*** message ***/
  XtVaSetValues(self->iw.msg,
		XtNstring,self->op->msg,
		NULL);

  /*** inventory ***/
  XtVaSetValues(self->iw.inv,
		XtNpackage,self,
		NULL);

  /* get attribute value */
  for (i = 0; self->desc[i].label; i++, mask <<= 1)
      if(self->flags & mask) {
	  self->desc[i].getValue (ob, buf, (XtPointer) self->client);
	  if (self->desc[i].type == TypeString) {
	      XtVaSetValues(self->tags[i].value,
			    XtNstring, buf,
			    NULL);
	  } else if (self->desc[i].type == TypeToggle) {
	      XtVaSetValues(self->tags[i].value,
			    XtNstate, *buf ? TRUE : FALSE,
			    NULL);
	  }
      }

  /*** update ***/
  for(i=0; self->desc[i].label; i++) {
      XtUnmanageChild(self->tags[i].cont);
  }
  for(i=0; self->desc[i].label; i++) {
      if(self->flags & (1 << i)) {
	  XtManageChild(self->tags[i].cont);
      }
  }
  sprintf(buf,"Attr: %s",self->op->name);
  XtVaSetValues(self->shell,
		XtNtitle,buf,
		XtNiconName,buf,
		NULL);
  self->modified = False;
}

static void AttrReset(Attr self)
{
    int i;
    debug0("Attr-Reset()\n");
    /*** name ***/
    XtVaSetValues(self->iw.name,
		  XtNstring,NULL,
		  NULL);
    /*** object ***/
    XtVaSetValues(self->iw.face,
		  XtNobject,NULL,
		  NULL);
    /*** message ***/
    XtVaSetValues(self->iw.msg,
		  XtNstring,NULL,
		  NULL);
    /*** inventory ***/
    XtVaSetValues(self->iw.inv,
		  XtNpackage,self,
		  NULL);

    for(i=0; self->desc[i].label; i++) {
	XtUnmanageChild(self->tags[i].cont);
    }
}

/*
 * popdown window & destroy widgets
 */
void AttrDestroy(Attr self)
{
    Attr tmp2;

    if (self->attr)
	AttrDestroy (self->attr);
    XtDestroyWidget(self->shell);
    XtFree((char*)self->tags);
    self->isup = False;
    CnvBrowseDestroy(self->dump);

    /*
     * dirty part:
     * here we find out to what part this window belongs to.
     * it may be:
     *  - Attr of Look in App
     *  - Attr of other Attr (inventory)
     */

    if (self == self->app->attr)
	self->app->attr = NULL;
    else {
	for (tmp2 = self->app->attr; tmp2; tmp2 = tmp2->attr)
	    if (self == tmp2->attr) {
		tmp2->attr = NULL;
		break;
	    }
#ifdef DEBUG
	if (!tmp2)
	    debug0 ("Cannot find origin of Attr!!\n");
#endif
    }
    XtFree((char*)self);
}

/*
 * member: store information from edited widget structure
 * to object structure
 */
void AttrApply(Attr self)
{
    String str,var;
    object *ob;
    char buf[BUFSIZ];
    int len, mask,set_all=1;
    size_t i;

    /* check out, that object exist */
    if(!self->op) {
	return;
    }

    for(ob = self->op; ob && set_all; ob = ob->more) {
	/*** variable ***/
	XtVaGetValues(self->iw.var,
		      XtNstring,&var,
		      NULL);
	if(var && *var) {
	    debug1("AttrApply(), %s\n",var);

	    /* This is a pretty gross hack, but somewhat necessary.  Otherwise,
	     * all pieces of a multisquare monster will get the treasure, which
	     * is really not what we want.
	     */
	    if (!strncmp(var, "randomitems", 11)) set_all=0;
	    for (i = 0; i < ALLOWED_VARIABLES; i++)
		if (!strncmp (allowed_variables[i], var,
			      strlen(allowed_variables[i]))) {
		    if (set_variable(ob,var) == -1) {
			sprintf(buf,"%s: no such variable",var);
			CnvNotify(buf,"Continue",NULL);
		    }
		    break;
		}
	    if (i >= ALLOWED_VARIABLES) {
		sprintf(buf,"%s: cannot set variable",var);
		CnvNotify(buf,"Continue",NULL);
	    }
	}
	/*** name ***/
	XtVaGetValues(self->iw.name,
		      XtNstring,&str,
		      NULL);
	if(ob->name) free_string(ob->name);
	ob->name = add_string(str);

	/*** message ***/
	XtVaGetValues(self->iw.msg,
		      XtNstring,&str,
		      NULL);

	if(self->op->msg) free_string(self->op->msg);
	if((len = strlen(str))) {
	    if(str[len-1] != '\n') str[len-1] = '\n'; /*** kludge ***/
	    self->op->msg = add_string(str);
	} else {
	    self->op->msg = NULL;
	}

	/* set individual attribute value */
	for (i = 0, mask = 1; self->desc[i].label; i++, mask <<= 1) {
	    if(self->flags & mask) {
		if (self->desc[i].type == TypeString) {
		    XtVaGetValues(self->tags[i].value, XtNstring, &str, NULL);
		    self->desc[i].putValue (ob, str, (XtPointer)self->client);
		} else if (self->desc[i].type == TypeToggle) {
		    Boolean tmp;
		    XtVaGetValues(self->tags[i].value, XtNstate, &tmp, NULL);
		    *str = tmp ? ~0 : 0;
		    self->desc[i].putValue (ob, str, (XtPointer)self->client);
		}
	    }
	}
    } /* for all parts of the object */

    /*** clear variables ***/
    XtVaSetValues(self->iw.var,XtNstring,NULL,NULL);

    /*** update ***/
    AppUpdate (self->app);

    self->modified = True;
    /*self->client->modified = True;*/
    EditModified(self->client);
}


/*** end of Attr.c ***/
