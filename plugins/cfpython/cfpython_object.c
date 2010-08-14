/*****************************************************************************/
/* CFPython - A Python module for Crossfire RPG.                             */
/* Version: 2.0beta8 (also known as "Alexander")                             */
/* Contact: yann.chachkoff@myrealbox.com                                     */
/*****************************************************************************/
/* That code is placed under the GNU General Public Licence (GPL)            */
/* (C)2001-2005 by Chachkoff Yann (Feel free to deliver your complaints)     */
/*****************************************************************************/
/*  CrossFire, A Multiplayer game for X-windows                              */
/*                                                                           */
/*  Copyright (C) 2000 Mark Wedel                                            */
/*  Copyright (C) 1992 Frank Tore Johansen                                   */
/*                                                                           */
/*  This program is free software; you can redistribute it and/or modify     */
/*  it under the terms of the GNU General Public License as published by     */
/*  the Free Software Foundation; either version 2 of the License, or        */
/*  (at your option) any later version.                                      */
/*                                                                           */
/*  This program is distributed in the hope that it will be useful,          */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of           */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            */
/*  GNU General Public License for more details.                             */
/*                                                                           */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program; if not, write to the Free Software              */
/*  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.                */
/*                                                                           */
/*****************************************************************************/

#include <cfpython.h>
#include <cfpython_object_private.h>
#include <hashtable.h>

/* Table for keeping track of which PyObject goes with with Crossfire object */
static ptr_assoc_table object_assoc_table;

/* Helper functions for dealing with object_assoc_table */
void init_object_assoc_table(void) {
    init_ptr_assoc_table(object_assoc_table);
}

static void add_object_assoc(object *key, PyObject *value) {
    add_ptr_assoc(object_assoc_table, key, value);
}

static PyObject *find_assoc_pyobject(object *key) {
    return (PyObject *)find_assoc_value(object_assoc_table, key);
}

static void free_object_assoc(object *key) {
    free_ptr_assoc(object_assoc_table, key);
}

static PyObject *Player_GetTitle(Crossfire_Object *whoptr, void *closure) {
    char title[MAX_NAME];
    EXISTCHECK(whoptr);
    return Py_BuildValue("s", cf_player_get_title(whoptr->obj, title, MAX_NAME));
}

static int Player_SetTitle(Crossfire_Object *whoptr, PyObject *value, void *closure) {
    char *val;

    EXISTCHECK_INT(whoptr);
    if (value == NULL) {
        PyErr_SetString(PyExc_TypeError, "Cannot delete the Title attribute");
        return -1;
    }
    if (!CF_IS_PYSTR(value)) {
        PyErr_SetString(PyExc_TypeError, "The Title attribute must be a string");
        return -1;
    }
    if (!PyArg_Parse(value, "s", &val))
        return -1;

    cf_player_set_title(whoptr->obj, val);
    return 0;
}

static PyObject *Player_GetIP(Crossfire_Player *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Py_BuildValue("s", cf_player_get_ip(whoptr->obj));
}

static PyObject *Player_GetMarkedItem(Crossfire_Player *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Crossfire_Object_wrap(cf_player_get_marked_item(whoptr->obj));
}

static int Player_SetMarkedItem(Crossfire_Player *whoptr, PyObject *value, void *closure) {
    Crossfire_Object *ob;

    EXISTCHECK_INT(whoptr);
    if (!PyArg_Parse(value, "O!", &Crossfire_ObjectType, &ob))
        return -1;
        cf_player_set_marked_item(whoptr->obj, ob->obj);
        return 0;
}

static PyObject *Crossfire_Player_Message(Crossfire_Player *who, PyObject *args) {
    char *message;
    int color  = NDI_UNIQUE|NDI_ORANGE;

    EXISTCHECK(who);
    if (!PyArg_ParseTuple(args, "s|i", &message, &color))
        return NULL;

    cf_player_message(who->obj, message, color);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *Player_KnowledgeKnown(Crossfire_Player *who, PyObject *args) {
    const char *knowledge;

    EXISTCHECK(who);
    if (!PyArg_ParseTuple(args, "s", &knowledge))
        return NULL;

    return Py_BuildValue("i", cf_player_knowledge_has(who->obj, knowledge));
}

static PyObject *Player_GetParty(Crossfire_Player *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Crossfire_Party_wrap(cf_player_get_party(whoptr->obj));
}

static int Player_SetParty(Crossfire_Player *whoptr, PyObject *value, void *closure) {
    Crossfire_Party *ob;

    EXISTCHECK_INT(whoptr);
    if (!PyArg_Parse(value, "O!", &Crossfire_PartyType, &ob))
        return -1;
    cf_player_set_party(whoptr->obj, ob->party);
    return 0;
}

static PyObject *Crossfire_Player_CanPay(Crossfire_Player *who, PyObject *args) {
    EXISTCHECK(who);
    return Py_BuildValue("i", cf_player_can_pay(who->obj));
}

static PyObject *Player_GetBedMap(Crossfire_Player *whoptr, void *closure) {
    char bed[200];

    EXISTCHECK(whoptr);
    return Py_BuildValue("s", cf_object_get_string_property(whoptr->obj, CFAPI_PLAYER_PROP_BED_MAP, bed, sizeof(bed)));
}

static int Player_SetBedMap(Crossfire_Player *whoptr, PyObject *value, void *closure) {
    char *location;

    EXISTCHECK_INT(whoptr);
    if (!PyArg_Parse(value, "s", &location))
        return -1;
    cf_object_set_string_property(whoptr->obj, CFAPI_PLAYER_PROP_BED_MAP, location);
    return 0;
}

static PyObject *Player_GetBedX(Crossfire_Player *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Py_BuildValue("i", cf_object_get_int_property(whoptr->obj, CFAPI_PLAYER_PROP_BED_X));
}

static int Player_SetBedX(Crossfire_Player *whoptr, PyObject *value, void *closure) {
    int x;

    EXISTCHECK_INT(whoptr);
    if (!PyArg_Parse(value, "i", &x))
        return -1;
    cf_object_set_int_property(whoptr->obj, CFAPI_PLAYER_PROP_BED_X, x);
    return 0;
}

static PyObject *Player_GetBedY(Crossfire_Player *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Py_BuildValue("i", cf_object_get_int_property(whoptr->obj, CFAPI_PLAYER_PROP_BED_Y));
}

static int Player_SetBedY(Crossfire_Player *whoptr, PyObject *value, void *closure) {
    int y;

    EXISTCHECK_INT(whoptr);
    if (!PyArg_Parse(value, "i", &y))
        return -1;
    cf_object_set_int_property(whoptr->obj, CFAPI_PLAYER_PROP_BED_Y, y);
    return 0;
}

static PyObject *Player_QuestStart(Crossfire_Player *whoptr, PyObject *args) {
    char *code;
    int state;
    sstring quest_code;

    EXISTCHECK(whoptr);
    if (!PyArg_ParseTuple(args, "si", &code, &state))
        return NULL;

    quest_code = cf_add_string(code);
    cf_quest_start(whoptr->obj, quest_code, state);
    cf_free_string(quest_code);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *Player_QuestGetState(Crossfire_Player *whoptr, PyObject *args) {
    char *code;
    int state;
    sstring quest_code;

    EXISTCHECK(whoptr);
    if (!PyArg_ParseTuple(args, "s", &code))
        return NULL;

    quest_code = cf_add_string(code);
    state = cf_quest_get_player_state(whoptr->obj, quest_code);
    cf_free_string(quest_code);

    return Py_BuildValue("i", state);
}

static PyObject *Player_QuestSetState(Crossfire_Player *whoptr, PyObject *args) {
    char *code;
    int state;
    sstring quest_code;

    EXISTCHECK(whoptr);
    if (!PyArg_ParseTuple(args, "si", &code, &state))
        return NULL;

    quest_code = cf_add_string(code);
    cf_quest_set_player_state(whoptr->obj, quest_code, state);
    cf_free_string(quest_code);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *Player_QuestWasCompleted(Crossfire_Player *whoptr, PyObject *args) {
    char *code;
    int completed;
    sstring quest_code;

    EXISTCHECK(whoptr);
    if (!PyArg_ParseTuple(args, "s", &code))
        return NULL;

    quest_code = cf_add_string(code);
    completed = cf_quest_was_completed(whoptr->obj, quest_code);
    cf_free_string(quest_code);

    return Py_BuildValue("i", completed);
}

/* Object properties. Get and maybe set. */
static PyObject *Object_GetName(Crossfire_Object *whoptr, void *closure) {
    char name[200];

    EXISTCHECK(whoptr);
    return Py_BuildValue("s", cf_query_name(whoptr->obj, name, sizeof(name)));
}

static PyObject *Object_GetNamePl(Crossfire_Object *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Py_BuildValue("s", (char *)cf_query_name_pl(whoptr->obj));
}

static PyObject *Object_GetTitle(Crossfire_Object *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Py_BuildValue("s", cf_object_get_sstring_property(whoptr->obj, CFAPI_OBJECT_PROP_TITLE));
}

static PyObject *Object_GetRace(Crossfire_Object *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Py_BuildValue("s", cf_object_get_sstring_property(whoptr->obj, CFAPI_OBJECT_PROP_RACE));
}

static PyObject *Object_GetMap(Crossfire_Object *whoptr, void *closure) {
    mapstruct *m;

    EXISTCHECK(whoptr);
    m = cf_object_get_map_property(whoptr->obj, CFAPI_OBJECT_PROP_MAP);
    return Crossfire_Map_wrap(m);
}

static PyObject *Object_GetCha(Crossfire_Object *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Py_BuildValue("i", cf_object_get_int_property(whoptr->obj, CFAPI_OBJECT_PROP_CHA));
}

static PyObject *Object_GetCon(Crossfire_Object *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Py_BuildValue("i", cf_object_get_int_property(whoptr->obj, CFAPI_OBJECT_PROP_CON));
}

static PyObject *Object_GetDex(Crossfire_Object *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Py_BuildValue("i", cf_object_get_int_property(whoptr->obj, CFAPI_OBJECT_PROP_DEX));
}

static PyObject *Object_GetInt(Crossfire_Object *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Py_BuildValue("i", cf_object_get_int_property(whoptr->obj, CFAPI_OBJECT_PROP_INT));
}

static PyObject *Object_GetPow(Crossfire_Object *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Py_BuildValue("i", cf_object_get_int_property(whoptr->obj, CFAPI_OBJECT_PROP_POW));
}

static PyObject *Object_GetStr(Crossfire_Object *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Py_BuildValue("i", cf_object_get_int_property(whoptr->obj, CFAPI_OBJECT_PROP_STR));
}

static PyObject *Object_GetWis(Crossfire_Object *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Py_BuildValue("i", cf_object_get_int_property(whoptr->obj, CFAPI_OBJECT_PROP_WIS));
}

static PyObject *Object_GetHP(Crossfire_Object *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Py_BuildValue("i", cf_object_get_int_property(whoptr->obj, CFAPI_OBJECT_PROP_HP));
}

static PyObject *Object_GetMaxHP(Crossfire_Object *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Py_BuildValue("i", cf_object_get_int_property(whoptr->obj, CFAPI_OBJECT_PROP_MAXHP));
}

static PyObject *Object_GetSP(Crossfire_Object *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Py_BuildValue("i", cf_object_get_int_property(whoptr->obj, CFAPI_OBJECT_PROP_SP));
}

static PyObject *Object_GetMaxSP(Crossfire_Object *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Py_BuildValue("i", cf_object_get_int_property(whoptr->obj, CFAPI_OBJECT_PROP_MAXSP));
}

static PyObject *Object_GetGrace(Crossfire_Object *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Py_BuildValue("i", cf_object_get_int_property(whoptr->obj, CFAPI_OBJECT_PROP_GP));
}

static PyObject *Object_GetMaxGrace(Crossfire_Object *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Py_BuildValue("i", cf_object_get_int_property(whoptr->obj, CFAPI_OBJECT_PROP_MAXGP));
}

static PyObject *Object_GetFood(Crossfire_Object *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Py_BuildValue("i", cf_object_get_int_property(whoptr->obj, CFAPI_OBJECT_PROP_FP));
}

static PyObject *Object_GetAC(Crossfire_Object *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Py_BuildValue("i", cf_object_get_int_property(whoptr->obj, CFAPI_OBJECT_PROP_AC));
}

static PyObject *Object_GetWC(Crossfire_Object *whoptr, void *closure) {
    return Py_BuildValue("i", cf_object_get_int_property(whoptr->obj, CFAPI_OBJECT_PROP_WC));
}

static PyObject *Object_GetDam(Crossfire_Object *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Py_BuildValue("i", cf_object_get_int_property(whoptr->obj, CFAPI_OBJECT_PROP_DAM));
}

static PyObject *Object_GetLuck(Crossfire_Object *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Py_BuildValue("i", cf_object_get_int_property(whoptr->obj, CFAPI_OBJECT_PROP_LUCK));
}

static PyObject *Object_GetMessage(Crossfire_Object *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Py_BuildValue("s", cf_object_get_sstring_property(whoptr->obj, CFAPI_OBJECT_PROP_MESSAGE));
}

static PyObject *Object_GetSkill(Crossfire_Object *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Py_BuildValue("s", cf_object_get_sstring_property(whoptr->obj, CFAPI_OBJECT_PROP_SKILL));
}

static PyObject *Object_GetExp(Crossfire_Object *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Py_BuildValue("L", cf_object_get_int64_property(whoptr->obj, CFAPI_OBJECT_PROP_EXP));
}

static PyObject *Object_GetPermExp(Crossfire_Object *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Py_BuildValue("L", cf_object_get_int64_property(whoptr->obj, CFAPI_OBJECT_PROP_PERM_EXP));
}

static PyObject *Object_GetExpMul(Crossfire_Object *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Py_BuildValue("d", cf_object_get_double_property(whoptr->obj, CFAPI_OBJECT_PROP_EXP_MULTIPLIER));
}

static PyObject *Object_GetSlaying(Crossfire_Object *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Py_BuildValue("s", cf_object_get_sstring_property(whoptr->obj, CFAPI_OBJECT_PROP_SLAYING));
}

static PyObject *Object_GetCursed(Crossfire_Object *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Py_BuildValue("i", cf_object_get_flag(whoptr->obj, FLAG_CURSED));
}

static PyObject *Object_GetDamned(Crossfire_Object *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Py_BuildValue("i", cf_object_get_flag(whoptr->obj, FLAG_DAMNED));
}

static PyObject *Object_GetWeight(Crossfire_Object *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Py_BuildValue("i", cf_object_get_int_property(whoptr->obj, CFAPI_OBJECT_PROP_WEIGHT));
}

static PyObject *Object_GetWeightLimit(Crossfire_Object *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Py_BuildValue("i", cf_object_get_int_property(whoptr->obj, CFAPI_OBJECT_PROP_WEIGHT_LIMIT));
}

static PyObject *Object_GetAbove(Crossfire_Object *whoptr, void *closure) {
    object *op;

    EXISTCHECK(whoptr);
    op = cf_object_get_object_property(whoptr->obj, CFAPI_OBJECT_PROP_OB_ABOVE);
    return Crossfire_Object_wrap(op);
}

static PyObject *Object_GetBelow(Crossfire_Object *whoptr, void *closure) {
    object *op;

    EXISTCHECK(whoptr);
    op = cf_object_get_object_property(whoptr->obj, CFAPI_OBJECT_PROP_OB_BELOW);
    return Crossfire_Object_wrap(op);
}

static PyObject *Object_GetInventory(Crossfire_Object *whoptr, void *closure) {
    object *op;

    EXISTCHECK(whoptr);
    op = cf_object_get_object_property(whoptr->obj, CFAPI_OBJECT_PROP_INVENTORY);
    return Crossfire_Object_wrap(op);
}

static PyObject *Object_GetX(Crossfire_Object *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Py_BuildValue("i", cf_object_get_int_property(whoptr->obj, CFAPI_OBJECT_PROP_X));
}

static PyObject *Object_GetY(Crossfire_Object *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Py_BuildValue("i", cf_object_get_int_property(whoptr->obj, CFAPI_OBJECT_PROP_Y));
}

static PyObject *Object_GetDirection(Crossfire_Object *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Py_BuildValue("i", cf_object_get_int_property(whoptr->obj, CFAPI_OBJECT_PROP_DIRECTION));
}

static PyObject *Object_GetFacing(Crossfire_Object *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Py_BuildValue("i", cf_object_get_int_property(whoptr->obj, CFAPI_OBJECT_PROP_FACING));
}

static PyObject *Object_GetUnaggressive(Crossfire_Object *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Py_BuildValue("i", cf_object_get_flag(whoptr->obj, FLAG_UNAGGRESSIVE));
}

static PyObject *Object_GetGod(Crossfire_Object *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Py_BuildValue("s", cf_object_get_sstring_property(whoptr->obj, CFAPI_OBJECT_PROP_GOD));
}

static PyObject *Object_GetPickable(Crossfire_Object *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Py_BuildValue("i", !cf_object_get_flag(whoptr->obj, FLAG_NO_PICK));
}

static PyObject *Object_GetQuantity(Crossfire_Object *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Py_BuildValue("i", cf_object_get_int_property(whoptr->obj, CFAPI_OBJECT_PROP_NROF));
}

static PyObject *Object_GetInvisible(Crossfire_Object *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Py_BuildValue("i", cf_object_get_int_property(whoptr->obj, CFAPI_OBJECT_PROP_INVISIBLE));
}

static PyObject *Object_GetSpeed(Crossfire_Object *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Py_BuildValue("f", cf_object_get_float_property(whoptr->obj, CFAPI_OBJECT_PROP_SPEED));
}

static PyObject *Object_GetSpeedLeft(Crossfire_Object *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Py_BuildValue("f", cf_object_get_float_property(whoptr->obj, CFAPI_OBJECT_PROP_SPEED_LEFT));
}

static PyObject *Object_GetLastSP(Crossfire_Object *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Py_BuildValue("i", cf_object_get_int_property(whoptr->obj, CFAPI_OBJECT_PROP_LAST_SP));
}

static PyObject *Object_GetLastGrace(Crossfire_Object *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Py_BuildValue("i", cf_object_get_int_property(whoptr->obj, CFAPI_OBJECT_PROP_LAST_GRACE));
}

static PyObject *Object_GetLastEat(Crossfire_Object *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Py_BuildValue("i", cf_object_get_int_property(whoptr->obj, CFAPI_OBJECT_PROP_LAST_EAT));
}

static PyObject *Object_GetLevel(Crossfire_Object *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Py_BuildValue("i", cf_object_get_int_property(whoptr->obj, CFAPI_OBJECT_PROP_LEVEL));
}

static PyObject *Object_GetFace(Crossfire_Object *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    char buf[200];
    return Py_BuildValue("s", cf_object_get_string_property(whoptr->obj, CFAPI_OBJECT_PROP_FACE, buf, sizeof(buf)));
}

static PyObject *Object_GetAnim(Crossfire_Object *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Py_BuildValue("i", cf_object_get_int_property(whoptr->obj, CFAPI_OBJECT_PROP_ANIMATION));
}

static PyObject *Object_GetAnimSpeed(Crossfire_Object *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Py_BuildValue("i", cf_object_get_int_property(whoptr->obj, CFAPI_OBJECT_PROP_ANIM_SPEED));
}

static PyObject *Object_GetAttackType(Crossfire_Object *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Py_BuildValue("i", cf_object_get_int_property(whoptr->obj, CFAPI_OBJECT_PROP_ATTACK_TYPE));
}

static PyObject *Object_GetBeenApplied(Crossfire_Object *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Py_BuildValue("i", cf_object_get_flag(whoptr->obj, FLAG_BEEN_APPLIED));
}

static PyObject *Object_GetIdentified(Crossfire_Object *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Py_BuildValue("i", cf_object_get_flag(whoptr->obj, FLAG_IDENTIFIED));
}

static PyObject *Object_GetAlive(Crossfire_Object *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Py_BuildValue("i", cf_object_get_flag(whoptr->obj, FLAG_ALIVE));
}

static PyObject *Object_GetDM(Crossfire_Object *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Py_BuildValue("i", cf_object_get_flag(whoptr->obj, FLAG_WIZ));
}

static PyObject *Object_GetWasDM(Crossfire_Object *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Py_BuildValue("i", cf_object_get_flag(whoptr->obj, FLAG_WAS_WIZ));
}

static PyObject *Object_GetApplied(Crossfire_Object *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Py_BuildValue("i", cf_object_get_flag(whoptr->obj, FLAG_APPLIED));
}

static PyObject *Object_GetUnpaid(Crossfire_Object *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Py_BuildValue("i", cf_object_get_flag(whoptr->obj, FLAG_UNPAID));
}

static PyObject *Object_GetMonster(Crossfire_Object *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Py_BuildValue("i", cf_object_get_flag(whoptr->obj, FLAG_MONSTER));
}

static PyObject *Object_GetFriendly(Crossfire_Object *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Py_BuildValue("i", cf_object_get_flag(whoptr->obj, FLAG_FRIENDLY));
}

static PyObject *Object_GetGenerator(Crossfire_Object *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Py_BuildValue("i", cf_object_get_flag(whoptr->obj, FLAG_GENERATOR));
}

static PyObject *Object_GetThrown(Crossfire_Object *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Py_BuildValue("i", cf_object_get_flag(whoptr->obj, FLAG_IS_THROWN));
}

static PyObject *Object_GetCanSeeInvisible(Crossfire_Object *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Py_BuildValue("i", cf_object_get_flag(whoptr->obj, FLAG_SEE_INVISIBLE));
}

static PyObject *Object_GetRollable(Crossfire_Object *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Py_BuildValue("i", cf_object_get_flag(whoptr->obj, FLAG_CAN_ROLL));
}

static PyObject *Object_GetTurnable(Crossfire_Object *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Py_BuildValue("i", cf_object_get_flag(whoptr->obj, FLAG_IS_TURNABLE));
}

static PyObject *Object_GetUsedUp(Crossfire_Object *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Py_BuildValue("i", cf_object_get_flag(whoptr->obj, FLAG_IS_USED_UP));
}

static PyObject *Object_GetSplitting(Crossfire_Object *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Py_BuildValue("i", cf_object_get_flag(whoptr->obj, FLAG_SPLITTING));
}

static PyObject *Object_GetBlind(Crossfire_Object *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Py_BuildValue("i", cf_object_get_flag(whoptr->obj, FLAG_BLIND));
}

static PyObject *Object_GetCanUseSkill(Crossfire_Object *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Py_BuildValue("i", cf_object_get_flag(whoptr->obj, FLAG_CAN_USE_SKILL));
}

static PyObject *Object_GetKnownCursed(Crossfire_Object *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Py_BuildValue("i", cf_object_get_flag(whoptr->obj, FLAG_KNOWN_CURSED));
}

static PyObject *Object_GetStealthy(Crossfire_Object *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Py_BuildValue("i", cf_object_get_flag(whoptr->obj, FLAG_STEALTH));
}

static PyObject *Object_GetConfused(Crossfire_Object *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Py_BuildValue("i", cf_object_get_flag(whoptr->obj, FLAG_CONFUSED));
}

static PyObject *Object_GetSleeping(Crossfire_Object *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Py_BuildValue("i", cf_object_get_flag(whoptr->obj, FLAG_SLEEP));
}

static PyObject *Object_GetLifesaver(Crossfire_Object *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Py_BuildValue("i", cf_object_get_flag(whoptr->obj, FLAG_LIFESAVE));
}

static PyObject *Object_GetFloor(Crossfire_Object *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Py_BuildValue("i", cf_object_get_flag(whoptr->obj, FLAG_IS_FLOOR));
}

static PyObject *Object_GetHasXRays(Crossfire_Object *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Py_BuildValue("i", cf_object_get_flag(whoptr->obj, FLAG_XRAYS));
}

static PyObject *Object_GetCanUseRing(Crossfire_Object *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Py_BuildValue("i", cf_object_get_flag(whoptr->obj, FLAG_USE_RING));
}

static PyObject *Object_GetCanUseBow(Crossfire_Object *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Py_BuildValue("i", cf_object_get_flag(whoptr->obj, FLAG_USE_BOW));
}

static PyObject *Object_GetCanUseWand(Crossfire_Object *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Py_BuildValue("i", cf_object_get_flag(whoptr->obj, FLAG_USE_RANGE));
}

static PyObject *Object_GetCanSeeInDark(Crossfire_Object *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Py_BuildValue("i", cf_object_get_flag(whoptr->obj, FLAG_SEE_IN_DARK));
}

static PyObject *Object_GetKnownMagical(Crossfire_Object *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Py_BuildValue("i", cf_object_get_flag(whoptr->obj, FLAG_KNOWN_MAGICAL));
}

static PyObject *Object_GetCanUseWeapon(Crossfire_Object *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Py_BuildValue("i", cf_object_get_flag(whoptr->obj, FLAG_USE_WEAPON));
}

static PyObject *Object_GetCanUseArmour(Crossfire_Object *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Py_BuildValue("i", cf_object_get_flag(whoptr->obj, FLAG_USE_ARMOUR));
}

static PyObject *Object_GetCanUseScroll(Crossfire_Object *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Py_BuildValue("i", cf_object_get_flag(whoptr->obj, FLAG_USE_SCROLL));
}

static PyObject *Object_GetCanCastSpell(Crossfire_Object *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Py_BuildValue("i", cf_object_get_flag(whoptr->obj, FLAG_CAST_SPELL));
}

static PyObject *Object_GetReflectSpells(Crossfire_Object *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Py_BuildValue("i", cf_object_get_flag(whoptr->obj, FLAG_REFL_SPELL));
}

static PyObject *Object_GetReflectMissiles(Crossfire_Object *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Py_BuildValue("i", cf_object_get_flag(whoptr->obj, FLAG_REFL_MISSILE));
}

static PyObject *Object_GetUnique(Crossfire_Object *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Py_BuildValue("i", cf_object_get_flag(whoptr->obj, FLAG_UNIQUE));
}

static PyObject *Object_GetRunAway(Crossfire_Object *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Py_BuildValue("i", cf_object_get_flag(whoptr->obj, FLAG_RUN_AWAY));
}

static PyObject *Object_GetScared(Crossfire_Object *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Py_BuildValue("i", cf_object_get_flag(whoptr->obj, FLAG_SCARED));
}

static PyObject *Object_GetUndead(Crossfire_Object *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Py_BuildValue("i", cf_object_get_flag(whoptr->obj, FLAG_UNDEAD));
}

static PyObject *Object_GetBlocksView(Crossfire_Object *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Py_BuildValue("i", cf_object_get_flag(whoptr->obj, FLAG_BLOCKSVIEW));
}

static PyObject *Object_GetHitBack(Crossfire_Object *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Py_BuildValue("i", cf_object_get_flag(whoptr->obj, FLAG_HITBACK));
}

static PyObject *Object_GetStandStill(Crossfire_Object *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Py_BuildValue("i", cf_object_get_flag(whoptr->obj, FLAG_STAND_STILL));
}

static PyObject *Object_GetOnlyAttack(Crossfire_Object *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Py_BuildValue("i", cf_object_get_flag(whoptr->obj, FLAG_ONLY_ATTACK));
}

static PyObject *Object_GetMakeInvisible(Crossfire_Object *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Py_BuildValue("i", cf_object_get_flag(whoptr->obj, FLAG_MAKE_INVIS));
}

static PyObject *Object_GetMoney(Crossfire_Object *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Py_BuildValue("i", cf_object_query_money(whoptr->obj));
}

static PyObject *Object_GetType(Crossfire_Object *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Py_BuildValue("i", cf_object_get_int_property(whoptr->obj, CFAPI_OBJECT_PROP_TYPE));
}

static PyObject *Object_GetSubtype(Crossfire_Object *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Py_BuildValue("i", cf_object_get_int_property(whoptr->obj, CFAPI_OBJECT_PROP_SUBTYPE));
}

static PyObject *Object_GetValue(Crossfire_Object *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Py_BuildValue("l", cf_object_get_long_property(whoptr->obj, CFAPI_OBJECT_PROP_VALUE));
}

static PyObject *Object_GetArchName(Crossfire_Object *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Py_BuildValue("s", cf_object_get_sstring_property(whoptr->obj, CFAPI_OBJECT_PROP_ARCH_NAME));
}

static PyObject *Object_GetArchetype(Crossfire_Object *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Crossfire_Archetype_wrap(cf_object_get_archetype_property(whoptr->obj, CFAPI_OBJECT_PROP_ARCHETYPE));
}

static PyObject *Object_GetNoSave(Crossfire_Object *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Py_BuildValue("i", cf_object_get_int_property(whoptr->obj, CFAPI_OBJECT_PROP_NO_SAVE));
}

static PyObject *Object_GetExists(Crossfire_Object *whoptr, void *closure) {
    if (!object_was_destroyed(whoptr->obj, whoptr->obj->count)) {
        Py_INCREF(Py_True);
        return Py_True;
    } else {
        Py_INCREF(Py_False);
        return Py_False;
    }
}

static PyObject *Object_GetEnv(Crossfire_Object *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Crossfire_Object_wrap(cf_object_get_object_property(whoptr->obj, CFAPI_OBJECT_PROP_ENVIRONMENT));
}

static PyObject *Object_GetMoveType(Crossfire_Object *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Py_BuildValue("i", cf_object_get_movetype_property(whoptr->obj, CFAPI_OBJECT_PROP_MOVE_TYPE));
}

static PyObject *Object_GetMoveBlock(Crossfire_Object *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Py_BuildValue("i", cf_object_get_movetype_property(whoptr->obj, CFAPI_OBJECT_PROP_MOVE_BLOCK));
}

static PyObject *Object_GetMoveAllow(Crossfire_Object *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Py_BuildValue("i", cf_object_get_movetype_property(whoptr->obj, CFAPI_OBJECT_PROP_MOVE_ALLOW));
}

static PyObject *Object_GetMoveOn(Crossfire_Object *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Py_BuildValue("i", cf_object_get_movetype_property(whoptr->obj, CFAPI_OBJECT_PROP_MOVE_ON));
}

static PyObject *Object_GetMoveOff(Crossfire_Object *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Py_BuildValue("i", cf_object_get_movetype_property(whoptr->obj, CFAPI_OBJECT_PROP_MOVE_OFF));
}

static PyObject *Object_GetMoveSlow(Crossfire_Object *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Py_BuildValue("i", cf_object_get_movetype_property(whoptr->obj, CFAPI_OBJECT_PROP_MOVE_SLOW));
}

static PyObject *Object_GetMoveSlowPenalty(Crossfire_Object *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Py_BuildValue("f", cf_object_get_float_property(whoptr->obj, CFAPI_OBJECT_PROP_MOVE_SLOW_PENALTY));
}

static PyObject *Object_GetOwner(Crossfire_Object *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Crossfire_Object_wrap(cf_object_get_object_property(whoptr->obj, CFAPI_OBJECT_PROP_OWNER));
}

static PyObject *Object_GetEnemy(Crossfire_Object *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Crossfire_Object_wrap(cf_object_get_object_property(whoptr->obj, CFAPI_OBJECT_PROP_ENEMY));
}

static PyObject *Object_GetCount(Crossfire_Object *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Py_BuildValue("i", cf_object_get_int_property(whoptr->obj, CFAPI_OBJECT_PROP_COUNT));
}

static PyObject *Object_GetGodGiven(Crossfire_Object *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Py_BuildValue("i", cf_object_get_flag(whoptr->obj, FLAG_STARTEQUIP));
}

static PyObject *Object_GetNoDamage(Crossfire_Object *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Py_BuildValue("i", cf_object_get_flag(whoptr->obj, FLAG_NO_DAMAGE));
}

static PyObject *Object_GetRandomMovement(Crossfire_Object *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Py_BuildValue("i", cf_object_get_flag(whoptr->obj, FLAG_RANDOM_MOVE));
}

static PyObject *Object_GetIsPet(Crossfire_Object *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Py_BuildValue("i", cf_object_get_int_property(whoptr->obj, CFAPI_OBJECT_PROP_FRIENDLY));
}

static PyObject *Object_GetAttackMovement(Crossfire_Object *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Py_BuildValue("i", cf_object_get_int_property(whoptr->obj, CFAPI_OBJECT_PROP_ATTACK_MOVEMENT));
}

static PyObject *Object_GetDuration(Crossfire_Object *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Py_BuildValue("i", cf_object_get_int_property(whoptr->obj, CFAPI_OBJECT_PROP_DURATION));
}

static PyObject *Object_GetGlowRadius(Crossfire_Object *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Py_BuildValue("i", cf_object_get_int_property(whoptr->obj, CFAPI_OBJECT_PROP_GLOW_RADIUS));
}

static PyObject *Object_GetAnimated(Crossfire_Object *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    return Py_BuildValue("i", cf_object_get_flag(whoptr->obj, FLAG_ANIMATE));
}

/** Setters */
static int Object_SetMessage(Crossfire_Object *whoptr, PyObject *value, void *closure) {
    char *val;

    EXISTCHECK_INT(whoptr);
    if (value == NULL) {
        PyErr_SetString(PyExc_TypeError, "Cannot delete the Message attribute");
        return -1;
    }
    if (!CF_IS_PYSTR(value)) {
        PyErr_SetString(PyExc_TypeError, "The Message attribute must be a string");
        return -1;
    }
    if (!PyArg_Parse(value, "s", &val))
        return -1;

    cf_object_set_string_property(whoptr->obj, CFAPI_OBJECT_PROP_MESSAGE, val);
    return 0;
}

static int Object_SetName(Crossfire_Object *whoptr, PyObject *value, void *closure) {
    char *val;

    EXISTCHECK_INT(whoptr);
    if (value == NULL) {
        PyErr_SetString(PyExc_TypeError, "Cannot delete the Name attribute");
        return -1;
    }
    if (!CF_IS_PYSTR(value)) {
        PyErr_SetString(PyExc_TypeError, "The Name attribute must be a string");
        return -1;
    }
    if (!PyArg_Parse(value, "s", &val))
        return -1;

    cf_object_set_string_property(whoptr->obj, CFAPI_OBJECT_PROP_NAME, val);
    cf_object_set_string_property(whoptr->obj, CFAPI_OBJECT_PROP_NAME_PLURAL, val);
    return 0;
}

static int Object_SetNamePl(Crossfire_Object *whoptr, PyObject *value, void *closure) {
    char *val;

    EXISTCHECK_INT(whoptr);
    if (value == NULL) {
        PyErr_SetString(PyExc_TypeError, "Cannot delete the NamePl attribute");
        return -1;
    }
    if (!CF_IS_PYSTR(value)) {
        PyErr_SetString(PyExc_TypeError, "The NamePl attribute must be a string");
        return -1;
    }
    if (!PyArg_Parse(value, "s", &val))
        return -1;

    cf_object_set_string_property(whoptr->obj, CFAPI_OBJECT_PROP_NAME_PLURAL, val);
    return 0;
}

static int Object_SetTitle(Crossfire_Object *whoptr, PyObject *value, void *closure) {
    char *val;

    EXISTCHECK_INT(whoptr);
    if (value == NULL) {
        PyErr_SetString(PyExc_TypeError, "Cannot delete the Title attribute");
        return -1;
    }
    if (!CF_IS_PYSTR(value)) {
        PyErr_SetString(PyExc_TypeError, "The Title attribute must be a string");
        return -1;
    }
    if (!PyArg_Parse(value, "s", &val))
        return -1;

    cf_object_set_string_property(whoptr->obj, CFAPI_OBJECT_PROP_TITLE, val);
    return 0;
}

static int Object_SetRace(Crossfire_Object *whoptr, PyObject *value, void *closure) {
    char *val;

    EXISTCHECK_INT(whoptr);
    if (value == NULL) {
        PyErr_SetString(PyExc_TypeError, "Cannot delete the Race attribute");
        return -1;
    }
    if (!CF_IS_PYSTR(value)) {
        PyErr_SetString(PyExc_TypeError, "The Race attribute must be a string");
        return -1;
    }
    if (!PyArg_Parse(value, "s", &val))
        return -1;

    cf_object_set_string_property(whoptr->obj, CFAPI_OBJECT_PROP_RACE, val);
    return 0;
}

static int Object_SetMap(Crossfire_Object *whoptr, PyObject *value, void *closure) {
    Crossfire_Map *val;

    EXISTCHECK_INT(whoptr);
    if (!PyArg_Parse(value, "O!", &Crossfire_MapType, &val))
        return -1;

    cf_object_change_map(whoptr->obj, val->map, NULL, 0, -1, -1);
    return 0;
}

static int Object_SetSlaying(Crossfire_Object *whoptr, PyObject *value, void *closure) {
    char *val;

    EXISTCHECK_INT(whoptr);
    if (value == NULL) {
        PyErr_SetString(PyExc_TypeError, "Cannot delete the Slaying attribute");
        return -1;
    }
    if (!CF_IS_PYSTR(value)) {
        PyErr_SetString(PyExc_TypeError, "The Slaying attribute must be a string");
        return -1;
    }
    if (!PyArg_Parse(value, "s", &val))
        return -1;

    cf_object_set_string_property(whoptr->obj, CFAPI_OBJECT_PROP_SLAYING, val);
    return 0;
}

static int Object_SetSkill(Crossfire_Object *whoptr, PyObject *value, void *closure) {
    char *val;

    EXISTCHECK_INT(whoptr);
    if (value == NULL) {
        PyErr_SetString(PyExc_TypeError, "Cannot delete the Skill attribute");
        return -1;
    }
    if (!CF_IS_PYSTR(value)) {
        PyErr_SetString(PyExc_TypeError, "The Skill attribute must be a string");
        return -1;
    }
    if (!PyArg_Parse(value, "s", &val))
        return -1;

    cf_object_set_string_property(whoptr->obj, CFAPI_OBJECT_PROP_SKILL, val);
    return 0;
}

static int Object_SetCursed(Crossfire_Object *whoptr, PyObject *value, void *closure) {
    int val;

    EXISTCHECK_INT(whoptr);
    if (!PyArg_Parse(value, "i", &val))
        return -1;

    cf_object_set_flag(whoptr->obj, FLAG_CURSED, val);
    return 0;
}

static int Object_SetDamned(Crossfire_Object *whoptr, PyObject *value, void *closure) {
    int val;

    EXISTCHECK_INT(whoptr);
    if (!PyArg_Parse(value, "i", &val))
        return -1;

    cf_object_set_flag(whoptr->obj, FLAG_DAMNED, val);
    return 0;
}

static int Object_SetApplied(Crossfire_Object *whoptr, PyObject *value, void *closure) {
    int val;

    EXISTCHECK_INT(whoptr);
    if (!PyArg_Parse(value, "i", &val))
        return -1;

    cf_object_set_flag(whoptr->obj, FLAG_APPLIED, val);
    return 0;
}

static int Object_SetStr(Crossfire_Object *whoptr, PyObject *value, void *closure) {
    int val;

    EXISTCHECK_INT(whoptr);
    if (!PyArg_Parse(value, "i", &val))
        return -1;

    cf_object_set_int_property(whoptr->obj, CFAPI_OBJECT_PROP_STR, val);
/*    cf_fix_object(whoptr->obj);*/
    return 0;
}

static int Object_SetDex(Crossfire_Object *whoptr, PyObject *value, void *closure) {
    int val;

    EXISTCHECK_INT(whoptr);
    if (!PyArg_Parse(value, "i", &val))
        return -1;

    cf_object_set_int_property(whoptr->obj, CFAPI_OBJECT_PROP_DEX, val);
    /*cf_fix_object(whoptr->obj);*/
    return 0;
}

static int Object_SetCon(Crossfire_Object *whoptr, PyObject *value, void *closure) {
    int val;

    EXISTCHECK_INT(whoptr);
    if (!PyArg_Parse(value, "i", &val))
        return -1;

    cf_object_set_int_property(whoptr->obj, CFAPI_OBJECT_PROP_CON, val);
    /*cf_fix_object(whoptr->obj);*/
    return 0;
}

static int Object_SetInt(Crossfire_Object *whoptr, PyObject *value, void *closure) {
    int val;

    EXISTCHECK_INT(whoptr);
    if (!PyArg_Parse(value, "i", &val))
        return -1;

    cf_object_set_int_property(whoptr->obj, CFAPI_OBJECT_PROP_INT, val);
    /*cf_fix_object(whoptr->obj);*/
    return 0;
}

static int Object_SetPow(Crossfire_Object *whoptr, PyObject *value, void *closure) {
    int val;

    EXISTCHECK_INT(whoptr);
    if (!PyArg_Parse(value, "i", &val))
        return -1;

    cf_object_set_int_property(whoptr->obj, CFAPI_OBJECT_PROP_POW, val);
    /*cf_fix_object(whoptr->obj);*/
    return 0;
}

static int Object_SetWis(Crossfire_Object *whoptr, PyObject *value, void *closure) {
    int val;

    EXISTCHECK_INT(whoptr);
    if (!PyArg_Parse(value, "i", &val))
        return -1;

    cf_object_set_int_property(whoptr->obj, CFAPI_OBJECT_PROP_WIS, val);
    /*cf_fix_object(whoptr->obj);*/
    return 0;
}

static int Object_SetCha(Crossfire_Object *whoptr, PyObject *value, void *closure) {
    int val;

    EXISTCHECK_INT(whoptr);
    if (!PyArg_Parse(value, "i", &val))
        return -1;

    cf_object_set_int_property(whoptr->obj, CFAPI_OBJECT_PROP_CHA, val);
    /*cf_fix_object(whoptr->obj);*/
    return 0;
}

static int Object_SetHP(Crossfire_Object *whoptr, PyObject *value, void *closure) {
    int val;

    EXISTCHECK_INT(whoptr);
    if (!PyArg_Parse(value, "i", &val))
        return -1;

    cf_object_set_int_property(whoptr->obj, CFAPI_OBJECT_PROP_HP, val);
    return 0;
}

static int Object_SetMaxHP(Crossfire_Object *whoptr, PyObject *value, void *closure) {
    int val;

    EXISTCHECK_INT(whoptr);
    if (!PyArg_Parse(value, "i", &val))
        return -1;

    cf_object_set_int_property(whoptr->obj, CFAPI_OBJECT_PROP_MAXHP, val);
    return 0;
}

static int Object_SetSP(Crossfire_Object *whoptr, PyObject *value, void *closure) {
    int val;

    EXISTCHECK_INT(whoptr);
    if (!PyArg_Parse(value, "i", &val))
        return -1;

    cf_object_set_int_property(whoptr->obj, CFAPI_OBJECT_PROP_SP, val);
    return 0;
}

static int Object_SetMaxSP(Crossfire_Object *whoptr, PyObject *value, void *closure) {
    int val;

    EXISTCHECK_INT(whoptr);
    if (!PyArg_Parse(value, "i", &val))
        return -1;

    cf_object_set_int_property(whoptr->obj, CFAPI_OBJECT_PROP_MAXSP, val);
    return 0;
}

static int Object_SetGrace(Crossfire_Object *whoptr, PyObject *value, void *closure) {
    int val;

    EXISTCHECK_INT(whoptr);
    if (!PyArg_Parse(value, "i", &val))
        return -1;

    cf_object_set_int_property(whoptr->obj, CFAPI_OBJECT_PROP_GP, val);
    return 0;
}

static int Object_SetMaxGrace(Crossfire_Object *whoptr, PyObject *value, void *closure) {
    int val;

    EXISTCHECK_INT(whoptr);
    if (!PyArg_Parse(value, "i", &val))
        return -1;

    cf_object_set_int_property(whoptr->obj, CFAPI_OBJECT_PROP_MAXGP, val);
    return 0;
}

static int Object_SetAC(Crossfire_Object *whoptr, PyObject *value, void *closure) {
    int val;

    EXISTCHECK_INT(whoptr);
    if (!PyArg_Parse(value, "i", &val))
        return -1;

    cf_object_set_int_property(whoptr->obj, CFAPI_OBJECT_PROP_AC, val);
    return 0;
}

static int Object_SetWC(Crossfire_Object *whoptr, PyObject *value, void *closure) {
    int val;

    EXISTCHECK_INT(whoptr);
    if (!PyArg_Parse(value, "i", &val))
        return -1;

    cf_object_set_int_property(whoptr->obj, CFAPI_OBJECT_PROP_WC, val);
    return 0;
}

static int Object_SetDam(Crossfire_Object *whoptr, PyObject *value, void *closure) {
    int val;

    EXISTCHECK_INT(whoptr);
    if (!PyArg_Parse(value, "i", &val))
        return -1;

    cf_object_set_int_property(whoptr->obj, CFAPI_OBJECT_PROP_DAM, val);
    return 0;
}

static int Object_SetFood(Crossfire_Object *whoptr, PyObject *value, void *closure) {
    int val;

    EXISTCHECK_INT(whoptr);
    if (!PyArg_Parse(value, "i", &val))
        return -1;

    cf_object_set_int_property(whoptr->obj, CFAPI_OBJECT_PROP_FP, val);
    return 0;
}

static int Object_SetWeight(Crossfire_Object *whoptr, PyObject *value, void *closure) {
    int val;

    EXISTCHECK_INT(whoptr);
    if (!PyArg_Parse(value, "i", &val))
        return -1;

    cf_object_set_int_property(whoptr->obj, CFAPI_OBJECT_PROP_WEIGHT, val);
    return 0;
}

static int Object_SetWeightLimit(Crossfire_Object *whoptr, PyObject *value, void *closure) {
    int val;

    EXISTCHECK_INT(whoptr);
    if (!PyArg_Parse(value, "i", &val))
        return -1;

    cf_object_set_int_property(whoptr->obj, CFAPI_OBJECT_PROP_WEIGHT_LIMIT, val);
    return 0;
}

static int Object_SetDirection(Crossfire_Object *whoptr, PyObject *value, void *closure) {
    int val;

    EXISTCHECK_INT(whoptr);
    if (!PyArg_Parse(value, "i", &val))
        return -1;

    cf_object_set_int_property(whoptr->obj, CFAPI_OBJECT_PROP_DIRECTION, val);
    return 0;
}

static int Object_SetFacing(Crossfire_Object *whoptr, PyObject *value, void *closure) {
    int val;

    EXISTCHECK_INT(whoptr);
    if (!PyArg_Parse(value, "i", &val))
        return -1;

    cf_object_set_int_property(whoptr->obj, CFAPI_OBJECT_PROP_FACING, val);
    return 0;
}

static int Object_SetGod(Crossfire_Object *whoptr, PyObject *value, void *closure) {
    char *val;

    EXISTCHECK_INT(whoptr);
    if (!PyArg_Parse(value, "s", &val))
        return -1;

    cf_object_set_string_property(whoptr->obj, CFAPI_OBJECT_PROP_GOD, val);
    return 0;
}

static int Object_SetSpeed(Crossfire_Object *whoptr, PyObject *value, void *closure) {
    float val;

    EXISTCHECK_INT(whoptr);
    if (!PyArg_Parse(value, "f", &val))
        return -1;

    cf_object_set_float_property(whoptr->obj, CFAPI_OBJECT_PROP_SPEED, val);
/*    cf_fix_object(whoptr->obj);*/
    return 0;
}

static int Object_SetSpeedLeft(Crossfire_Object *whoptr, PyObject *value, void *closure) {
    float val;

    EXISTCHECK_INT(whoptr);
    if (!PyArg_Parse(value, "f", &val))
        return -1;

    cf_object_set_float_property(whoptr->obj, CFAPI_OBJECT_PROP_SPEED_LEFT, val);
/*    cf_fix_object(whoptr->obj);*/
    return 0;
}

static int Object_SetQuantity(Crossfire_Object *whoptr, PyObject *value, void *closure) {
    int val;

    EXISTCHECK_INT(whoptr);
    if (!PyArg_Parse(value, "i", &val))
        return -1;

    if (cf_object_set_nrof(whoptr->obj, val) != 0) {
        PyErr_SetString(PyExc_TypeError, "Invalid quantity");
        return -1;
    }

/*    cf_fix_object(whoptr->obj);*/
    return 0;
}

static int Object_SetLastSP(Crossfire_Object *whoptr, PyObject *value, void *closure) {
    int val;

    EXISTCHECK_INT(whoptr);
    if (!PyArg_Parse(value, "i", &val))
        return -1;

    cf_object_set_int_property(whoptr->obj, CFAPI_OBJECT_PROP_LAST_SP, val);
/*    cf_fix_object(whoptr->obj);*/
    return 0;
}

static int Object_SetLastGrace(Crossfire_Object *whoptr, PyObject *value, void *closure) {
    int val;

    EXISTCHECK_INT(whoptr);
    if (!PyArg_Parse(value, "i", &val))
        return -1;

    cf_object_set_int_property(whoptr->obj, CFAPI_OBJECT_PROP_LAST_GRACE, val);
/*    cf_fix_object(whoptr->obj);*/
    return 0;
}

static int Object_SetLastEat(Crossfire_Object *whoptr, PyObject *value, void *closure) {
    int val;

    EXISTCHECK_INT(whoptr);
    if (!PyArg_Parse(value, "i", &val))
        return -1;

    cf_object_set_int_property(whoptr->obj, CFAPI_OBJECT_PROP_LAST_EAT, val);
    return 0;
}

static int Object_SetFace(Crossfire_Object *whoptr, PyObject *value, void *closure) {
    char *face;

    EXISTCHECK_INT(whoptr);
    if (!PyArg_Parse(value, "s", &face))
        return -1;

    if (!cf_object_set_face(whoptr->obj, face)) {
        PyErr_SetString(PyExc_TypeError, "Unknown face.");
        return -1;
    }
    return 0;
}

static int Object_SetAnim(Crossfire_Object *whoptr, PyObject *value, void *closure) {
    char *txt;
    int anim;

    EXISTCHECK_INT(whoptr);
    if (!PyArg_Parse(value, "s", &txt))
        return -1;

    anim = cf_find_animation(txt);
    if (anim == 0) {
        PyErr_SetString(PyExc_TypeError, "Unknown animation.");
        return -1;
    }

    cf_object_set_int_property(whoptr->obj, CFAPI_OBJECT_PROP_ANIMATION, anim);
    return 0;
}

static int Object_SetAnimSpeed(Crossfire_Object *whoptr, PyObject *value, void *closure) {
    int val;

    EXISTCHECK_INT(whoptr);
    if (!PyArg_Parse(value, "i", &val))
        return -1;

    cf_object_set_int_property(whoptr->obj, CFAPI_OBJECT_PROP_ANIM_SPEED, val);
    return 0;
}

static int Object_SetAttackType(Crossfire_Object *whoptr, PyObject *value, void *closure) {
    int val;

    EXISTCHECK_INT(whoptr);
    if (!PyArg_Parse(value, "i", &val))
        return -1;

    cf_object_set_int_property(whoptr->obj, CFAPI_OBJECT_PROP_ATTACK_TYPE, val);
/*    cf_fix_object(whoptr->obj);*/
    return 0;
}

static int Object_SetIdentified(Crossfire_Object *whoptr, PyObject *value, void *closure) {
    int val;

    EXISTCHECK_INT(whoptr);
    if (!PyArg_Parse(value, "i", &val))
        return -1;

    cf_object_set_flag(whoptr->obj, FLAG_IDENTIFIED, val);
    return 0;
}

static int Object_SetUnaggressive(Crossfire_Object *whoptr, PyObject *value, void *closure) {
    int val;

    EXISTCHECK_INT(whoptr);
    if (!PyArg_Parse(value, "i", &val))
        return -1;

    cf_object_set_flag(whoptr->obj, FLAG_UNAGGRESSIVE, val);
    return 0;
}

static int Object_SetPickable(Crossfire_Object *whoptr, PyObject *value, void *closure) {
    int val;

    EXISTCHECK_INT(whoptr);
    if (!PyArg_Parse(value, "i", &val))
        return -1;

    cf_object_set_flag(whoptr->obj, FLAG_NO_PICK, !val);
    return 0;
}

static int Object_SetInvisible(Crossfire_Object *whoptr, PyObject *value, void *closure) {
    int val;

    EXISTCHECK_INT(whoptr);
    if (!PyArg_ParseTuple(value, "i", &val))
        return -1;

    cf_object_set_int_property(whoptr->obj, CFAPI_OBJECT_PROP_INVISIBLE, val);
    return 0;
}

static int Object_SetUnpaid(Crossfire_Object *whoptr, PyObject *value, void *closure) {
    int val;

    EXISTCHECK_INT(whoptr);
    if (!PyArg_Parse(value, "i", &val))
        return -1;

    cf_object_set_flag(whoptr->obj, FLAG_UNPAID, val);
    return 0;
}

static int Object_SetFriendly(Crossfire_Object *whoptr, PyObject *value, void *closure) {
    int val;

    EXISTCHECK_INT(whoptr);
    if (!PyArg_Parse(value, "i", &val))
        return -1;

    cf_object_set_flag(whoptr->obj, FLAG_FRIENDLY, val);
    return 0;
}

static int Object_SetCanSeeInvisible(Crossfire_Object *whoptr, PyObject *value, void *closure) {
    int val;

    EXISTCHECK_INT(whoptr);
    if (!PyArg_Parse(value, "i", &val))
        return -1;

    cf_object_set_flag(whoptr->obj, FLAG_SEE_INVISIBLE, val);
    return 0;
}

static int Object_SetRollable(Crossfire_Object *whoptr, PyObject *value, void *closure) {
    int val;

    EXISTCHECK_INT(whoptr);
    if (!PyArg_Parse(value, "i", &val))
        return -1;

    cf_object_set_flag(whoptr->obj, FLAG_CAN_ROLL, val);
    return 0;
}

static int Object_SetTurnable(Crossfire_Object *whoptr, PyObject *value, void *closure) {
    int val;

    EXISTCHECK_INT(whoptr);
    if (!PyArg_Parse(value, "i", &val))
        return -1;

    cf_object_set_flag(whoptr->obj, FLAG_IS_TURNABLE, val);
    return 0;
}

static int Object_SetUsedUp(Crossfire_Object *whoptr, PyObject *value, void *closure) {
    int val;

    EXISTCHECK_INT(whoptr);
    if (!PyArg_Parse(value, "i", &val))
        return -1;

    cf_object_set_flag(whoptr->obj, FLAG_IS_USED_UP, val);
    return 0;
}

static int Object_SetBlind(Crossfire_Object *whoptr, PyObject *value, void *closure) {
    int val;

    EXISTCHECK_INT(whoptr);
    if (!PyArg_Parse(value, "i", &val))
        return -1;

    cf_object_set_flag(whoptr->obj, FLAG_BLIND, val);
    return 0;
}

static int Object_SetKnownCursed(Crossfire_Object *whoptr, PyObject *value, void *closure) {
    int val;

    EXISTCHECK_INT(whoptr);
    if (!PyArg_Parse(value, "i", &val))
        return -1;

    cf_object_set_flag(whoptr->obj, FLAG_KNOWN_CURSED, val);
    return 0;
}

static int Object_SetStealthy(Crossfire_Object *whoptr, PyObject *value, void *closure) {
    int val;

    EXISTCHECK_INT(whoptr);
    if (!PyArg_Parse(value, "i", &val))
        return -1;

    cf_object_set_flag(whoptr->obj, FLAG_STEALTH, val);
    return 0;
}

static int Object_SetConfused(Crossfire_Object *whoptr, PyObject *value, void *closure) {
    int val;

    EXISTCHECK_INT(whoptr);
    if (!PyArg_Parse(value, "i", &val))
        return -1;

    cf_object_set_flag(whoptr->obj, FLAG_CONFUSED, val);
    return 0;
}

static int Object_SetSleeping(Crossfire_Object *whoptr, PyObject *value, void *closure) {
    int val;

    EXISTCHECK_INT(whoptr);
    if (!PyArg_Parse(value, "i", &val))
        return -1;

    cf_object_set_flag(whoptr->obj, FLAG_SLEEP, val);
    return 0;
}

static int Object_SetLifesaver(Crossfire_Object *whoptr, PyObject *value, void *closure) {
    int val;

    EXISTCHECK_INT(whoptr);
    if (!PyArg_Parse(value, "i", &val))
        return -1;

    cf_object_set_flag(whoptr->obj, FLAG_LIFESAVE, val);
    return 0;
}

static int Object_SetHasXRays(Crossfire_Object *whoptr, PyObject *value, void *closure) {
    int val;

    EXISTCHECK_INT(whoptr);
    if (!PyArg_Parse(value, "i", &val))
        return -1;

    cf_object_set_flag(whoptr->obj, FLAG_XRAYS, val);
    return 0;
}

static int Object_SetCanSeeInDark(Crossfire_Object *whoptr, PyObject *value, void *closure) {
    int val;

    EXISTCHECK_INT(whoptr);
    if (!PyArg_Parse(value, "i", &val))
        return -1;

    cf_object_set_flag(whoptr->obj, FLAG_SEE_IN_DARK, val);
    return 0;
}

static int Object_SetKnownMagical(Crossfire_Object *whoptr, PyObject *value, void *closure) {
    int val;

    EXISTCHECK_INT(whoptr);
    if (!PyArg_Parse(value, "i", &val))
        return -1;

    cf_object_set_flag(whoptr->obj, FLAG_KNOWN_MAGICAL, val);
    return 0;
}

static int Object_SetReflectSpells(Crossfire_Object *whoptr, PyObject *value, void *closure) {
    int val;

    EXISTCHECK_INT(whoptr);
    if (!PyArg_Parse(value, "i", &val))
        return -1;

    cf_object_set_flag(whoptr->obj, FLAG_REFL_SPELL, val);
    return 0;
}

static int Object_SetReflectMissiles(Crossfire_Object *whoptr, PyObject *value, void *closure) {
    int val;

    EXISTCHECK_INT(whoptr);
    if (!PyArg_Parse(value, "i", &val))
        return -1;

    cf_object_set_flag(whoptr->obj, FLAG_REFL_MISSILE, val);
    return 0;
}

static int Object_SetUnique(Crossfire_Object *whoptr, PyObject *value, void *closure) {
    int val;

    EXISTCHECK_INT(whoptr);
    if (!PyArg_Parse(value, "i", &val))
        return -1;

    cf_object_set_flag(whoptr->obj, FLAG_UNIQUE, val);
    return 0;
}

static int Object_SetCanPassThru(Crossfire_Object *whoptr, PyObject *value, void *closure) {
    int val;

    EXISTCHECK_INT(whoptr);
    if (!PyArg_Parse(value, "i", &val))
        return -1;
    /* FIXME */
    /*cf_object_set_flag(whoptr->obj, FLAG_CAN_PASS_THRU, val);*/
    return 0;
}

static int Object_SetRunAway(Crossfire_Object *whoptr, PyObject *value, void *closure) {
    int val;

    EXISTCHECK_INT(whoptr);
    if (!PyArg_Parse(value, "i", &val))
        return -1;

    cf_object_set_flag(whoptr->obj, FLAG_RUN_AWAY, val);
    return 0;
}

static int Object_SetScared(Crossfire_Object *whoptr, PyObject *value, void *closure) {
    int val;

    EXISTCHECK_INT(whoptr);
    if (!PyArg_Parse(value, "i", &val))
        return -1;

    cf_object_set_flag(whoptr->obj, FLAG_SCARED, val);
    return 0;
}

static int Object_SetUndead(Crossfire_Object *whoptr, PyObject *value, void *closure) {
    int val;

    EXISTCHECK_INT(whoptr);
    if (!PyArg_Parse(value, "i", &val))
        return -1;

    cf_object_set_flag(whoptr->obj, FLAG_UNDEAD, val);
    return 0;
}

static int Object_SetBlocksView(Crossfire_Object *whoptr, PyObject *value, void *closure) {
    int val;

    EXISTCHECK_INT(whoptr);
    if (!PyArg_Parse(value, "i", &val))
        return -1;

    cf_object_set_flag(whoptr->obj, FLAG_BLOCKSVIEW, val);
    return 0;
}

static int Object_SetHitBack(Crossfire_Object *whoptr, PyObject *value, void *closure) {
    int val;

    EXISTCHECK_INT(whoptr);
    if (!PyArg_Parse(value, "i", &val))
        return -1;

    cf_object_set_flag(whoptr->obj, FLAG_HITBACK, val);
    return 0;
}

static int Object_SetStandStill(Crossfire_Object *whoptr, PyObject *value, void *closure) {
    int val;

    EXISTCHECK_INT(whoptr);
    if (!PyArg_Parse(value, "i", &val))
        return -1;

    cf_object_set_flag(whoptr->obj, FLAG_STAND_STILL, val);
    return 0;
}

static int Object_SetOnlyAttack(Crossfire_Object *whoptr, PyObject *value, void *closure) {
    int val;

    EXISTCHECK_INT(whoptr);
    if (!PyArg_Parse(value, "i", &val))
        return -1;

    cf_object_set_flag(whoptr->obj, FLAG_ONLY_ATTACK, val);
    return 0;
}

static int Object_SetMakeInvisible(Crossfire_Object *whoptr, PyObject *value, void *closure) {
    int val;

    EXISTCHECK_INT(whoptr);
    if (!PyArg_Parse(value, "i", &val))
        return -1;

    cf_object_set_flag(whoptr->obj, FLAG_MAKE_INVIS, val);
    return 0;
}

static int Object_SetValue(Crossfire_Object *whoptr, PyObject *value, void *closure) {
    long val;

    EXISTCHECK_INT(whoptr);
    if (!PyArg_Parse(value, "l", &val))
        return -1;

    cf_object_set_long_property(whoptr->obj, CFAPI_OBJECT_PROP_VALUE, val);
    return 0;
}

static int Object_SetNoSave(Crossfire_Object *whoptr, PyObject *value, void *closure) {
    long val;

    EXISTCHECK_INT(whoptr);
    if (!PyArg_Parse(value, "i", &val))
        return -1;

    cf_object_set_long_property(whoptr->obj, CFAPI_OBJECT_PROP_NO_SAVE, val);
    return 0;
}

static int Object_SetOwner(Crossfire_Object *whoptr, PyObject *value, void *closure) {
    Crossfire_Object *ob;

    EXISTCHECK_INT(whoptr);
    if (!PyArg_Parse(value, "O!", &Crossfire_ObjectType, &ob))
        return -1;
    cf_object_set_object_property(whoptr->obj, CFAPI_OBJECT_PROP_OWNER, ob->obj);
    return 0;
}

static int Object_SetEnemy(Crossfire_Object *whoptr, PyObject *value, void *closure) {
    Crossfire_Object *ob;

    EXISTCHECK_INT(whoptr);
    if (!PyArg_Parse(value, "O!", &Crossfire_ObjectType, &ob))
        return -1;
    cf_object_set_object_property(whoptr->obj, CFAPI_OBJECT_PROP_ENEMY, ob->obj);
    return 0;
}

static int Object_SetGodGiven(Crossfire_Object *whoptr, PyObject *value, void *closure) {
    int val;

    EXISTCHECK_INT(whoptr);
    if (!PyArg_Parse(value, "i", &val))
        return -1;

    cf_object_set_flag(whoptr->obj, FLAG_STARTEQUIP, val);
    return 0;
}

static int Object_SetNoDamage(Crossfire_Object *whoptr, PyObject *value, void *closure) {
    int val;

    EXISTCHECK_INT(whoptr);
    if (!PyArg_Parse(value, "i", &val))
        return -1;

    cf_object_set_flag(whoptr->obj, FLAG_NO_DAMAGE, val);
    return 0;
}

static int Object_SetRandomMovement(Crossfire_Object *whoptr, PyObject *value, void *closure) {
    int val;

    EXISTCHECK_INT(whoptr);
    if (!PyArg_Parse(value, "i", &val))
        return -1;

    cf_object_set_flag(whoptr->obj, FLAG_RANDOM_MOVE, val);
    return 0;
}

static int Object_SetIsPet(Crossfire_Object *whoptr, PyObject *value, void *closure) {
    int val;

    EXISTCHECK_INT(whoptr);
    if (!PyArg_Parse(value, "i", &val))
        return -1;

    cf_object_set_int_property(whoptr->obj, CFAPI_OBJECT_PROP_FRIENDLY, val);
    return 0;
}

static int Object_SetAttackMovement(Crossfire_Object *whoptr, PyObject *value, void *closure) {
    int val;

    EXISTCHECK_INT(whoptr);
    if (!PyArg_Parse(value, "i", &val))
        return -1;

    cf_object_set_int_property(whoptr->obj, CFAPI_OBJECT_PROP_ATTACK_MOVEMENT, val);
    return 0;
}

static int Object_SetExp(Crossfire_Object *whoptr, PyObject *value, void *closure) {
    sint64 val;

    EXISTCHECK_INT(whoptr);
    if (!PyArg_Parse(value, "L", &val))
        return -1;

    cf_object_set_int64_property(whoptr->obj, CFAPI_OBJECT_PROP_EXP, val);
    return 0;
}

static int Object_SetDuration(Crossfire_Object *whoptr, PyObject *value, void *closure) {
    int val;

    EXISTCHECK_INT(whoptr);
    if (!PyArg_Parse(value, "i", &val))
        return -1;

    cf_object_set_int_property(whoptr->obj, CFAPI_OBJECT_PROP_DURATION, val);
    return 0;
}

static int Object_SetGlowRadius(Crossfire_Object *whoptr, PyObject *value, void *closure) {
    int val;

    EXISTCHECK_INT(whoptr);
    if (!PyArg_Parse(value, "i", &val))
        return -1;

    cf_object_set_int_property(whoptr->obj, CFAPI_OBJECT_PROP_GLOW_RADIUS, val);
    return 0;
}

static int Object_SetAnimated(Crossfire_Object *whoptr, PyObject *value, void *closure) {
    int val;

    EXISTCHECK_INT(whoptr);
    if (!PyArg_Parse(value, "i", &val))
        return -1;
    cf_object_set_flag(whoptr->obj, FLAG_ANIMATE, val);
    return 0;
}

/* Methods. */

static PyObject *Crossfire_Object_Remove(Crossfire_Object *who, PyObject *args) {
    EXISTCHECK(who);

    if ((current_context->who != NULL) && (((Crossfire_Object *)current_context->who)->obj == who->obj))
        current_context->who = NULL;

    if (!cf_object_get_flag(who->obj, FLAG_REMOVED)) {
        cf_object_remove(who->obj);
    }

    cf_object_free_drop_inventory(who->obj);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *Crossfire_Object_Apply(Crossfire_Object *who, PyObject *args) {
    Crossfire_Object *whoptr;
    int flags;

    if (!PyArg_ParseTuple(args, "O!i", &Crossfire_ObjectType, &whoptr, &flags))
        return NULL;
    EXISTCHECK(who);
    EXISTCHECK(whoptr);

    return Py_BuildValue("i", cf_object_apply(whoptr->obj, who->obj, flags));
}

static PyObject *Crossfire_Object_Drop(Crossfire_Object *who, PyObject *args) {
    /* Note that this function uses the METH_O calling convention. */
    Crossfire_Object *whoptr = (Crossfire_Object*)args;

    EXISTCHECK(who);
    TYPEEXISTCHECK(whoptr);

    cf_object_drop(whoptr->obj, who->obj);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *Crossfire_Object_Clone(Crossfire_Object *who, PyObject *args) {
    int clone_type;
    object *clone;

    if (!PyArg_ParseTuple(args, "i", &clone_type))
        return NULL;

    if (clone_type != 0 && clone_type != 1)
    {
        PyErr_SetString(PyExc_ValueError, "Clone type must be 0 (object_create_clone) or 1 (object_copy).");
        return NULL;
    }

    clone = cf_object_clone(who->obj, clone_type);

    if (clone == NULL)
    {
        PyErr_SetString(PyExc_RuntimeError, "Clone failed.");
        return NULL;
    }

    return Crossfire_Object_wrap(clone);
}

static PyObject *Crossfire_Object_Split(Crossfire_Object *who, PyObject *args) {
    int count;
    char err[255];
    object *split;

    err[0] = '\0'; /* Just in case. */

    if (!PyArg_ParseTuple(args, "i", &count))
        return NULL;

    split = cf_object_split(who->obj, count, err, 255);

    if (split == NULL)
    {
        PyErr_SetString(PyExc_ValueError, err);
        return NULL;
    }

    return Crossfire_Object_wrap(split);
}

static PyObject *Crossfire_Object_Fix(Crossfire_Object *who, PyObject *args) {
    cf_fix_object(who->obj);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *Crossfire_Object_Take(Crossfire_Object *who, PyObject *args) {
    /* Note that this function uses the METH_O calling convention. */
    Crossfire_Object *whoptr = (Crossfire_Object*)args;

    EXISTCHECK(who);
    TYPEEXISTCHECK(whoptr);

    cf_object_pickup(whoptr->obj, who->obj);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *Crossfire_Object_Teleport(Crossfire_Object *who, PyObject *args) {
    Crossfire_Map *where;
    int x, y;
    int val;

    EXISTCHECK(who);
    if (!PyArg_ParseTuple(args, "O!ii", &Crossfire_MapType, &where, &x, &y))
        return NULL;

    val = cf_object_teleport(who->obj, where->map, x, y);

    return Py_BuildValue("i", val);
}

static PyObject *Crossfire_Object_ActivateRune(Crossfire_Object *who, PyObject *args) {
    /* Note that this function uses the METH_O calling convention. */
    object *trap;
    object *victim;
    Crossfire_Object *pcause = (Crossfire_Object*)args;

    EXISTCHECK(who);
    TYPEEXISTCHECK(pcause);
    trap = who->obj;
    victim = pcause->obj;
    cf_spring_trap(trap, victim);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *Crossfire_Object_CheckTrigger(Crossfire_Object *who, PyObject *args) {
    /* Note that this function uses the METH_O calling convention. */
    object *trigger;
    object *cause;
    int result;
    Crossfire_Object *pcause = (Crossfire_Object*)args;

    EXISTCHECK(who);
    TYPEEXISTCHECK(pcause);
    trigger = who->obj;
    cause = pcause->obj;
    result = cf_object_check_trigger(trigger, cause);

    return Py_BuildValue("i", result);
}

static PyObject *Crossfire_Object_Say(Crossfire_Object *who, PyObject *args) {
    char *message;

    EXISTCHECK(who);
    if (!PyArg_ParseTuple(args, "s", &message))
        return NULL;
    cf_object_say(who->obj, message);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *Crossfire_Object_Reposition(Crossfire_Object *who, PyObject *args) {
    int x, y;

    EXISTCHECK(who);
    if (!PyArg_ParseTuple(args, "ii", &x, &y))
        return NULL;

    cf_object_transfer(who->obj, x, y, 0, NULL);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *Crossfire_Object_QueryName(Crossfire_Object *who, PyObject *args) {
    char name[200];

    EXISTCHECK(who);
    return Py_BuildValue("s", cf_query_name(who->obj, name, sizeof(name)));
}

static PyObject *Crossfire_Object_GetResist(Crossfire_Object *who, PyObject *args) {
    int resist;

    EXISTCHECK(who);
    if (!PyArg_ParseTuple(args, "i", &resist))
        return NULL;
    if ((resist < 0) || (resist >= NROFATTACKS)) {
        return Py_BuildValue("l", 0);
    }
    return Py_BuildValue("i", cf_object_get_resistance(who->obj, resist));
}

static PyObject *Crossfire_Object_SetResist(Crossfire_Object *who, PyObject *args) {
    int resist, value;

    EXISTCHECK(who);
    if (!PyArg_ParseTuple(args, "ii", &resist, &value))
        return NULL;
    if ((resist >= 0) && (resist < NROFATTACKS))
        cf_object_set_resistance(who->obj, resist, value);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *Crossfire_Object_QueryCost(Crossfire_Object *who, PyObject *args) {
    int flags;
    Crossfire_Object *pcause;

    if (!PyArg_ParseTuple(args, "O!i", &Crossfire_ObjectType, &pcause, &flags))
        return NULL;
    EXISTCHECK(who);
    EXISTCHECK(pcause);
    return Py_BuildValue("i", cf_object_query_cost(who->obj, pcause->obj, flags));
}

static PyObject *Crossfire_Object_Cast(Crossfire_Object *who, PyObject *args) {
    int dir;
    char *op;
    Crossfire_Object *pspell;

    if (!PyArg_ParseTuple(args, "O!is", &Crossfire_ObjectType, &pspell, &dir, &op))
        return NULL;
    EXISTCHECK(who);
    EXISTCHECK(pspell);

    cf_object_cast_spell(who->obj, who->obj, dir, pspell->obj, op);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *Crossfire_Object_LearnSpell(Crossfire_Object *who, PyObject *args) {
    /* Note that this function uses the METH_O calling convention. */
    Crossfire_Object *pspell = (Crossfire_Object*)args;

    EXISTCHECK(who);
    TYPEEXISTCHECK(pspell);

    cf_object_learn_spell(who->obj, pspell->obj, 0);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *Crossfire_Object_ForgetSpell(Crossfire_Object *who, PyObject *args) {
    /* Note that this function uses the METH_O calling convention. */
    Crossfire_Object *pspell = (Crossfire_Object*)args;

    EXISTCHECK(who);
    TYPEEXISTCHECK(pspell);

    cf_object_forget_spell(who->obj, pspell->obj);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *Crossfire_Object_KnowSpell(Crossfire_Object *who, PyObject *args) {
    char *spellname;
    object *op;

    EXISTCHECK(who);
    if (!PyArg_ParseTuple(args, "s", &spellname))
        return NULL;

    op = cf_object_check_for_spell(who->obj, spellname);

    return Crossfire_Object_wrap(op);
}

static PyObject *Crossfire_Object_CastAbility(Crossfire_Object *who, PyObject *args) {
    Crossfire_Object *pspell;
    int dir;
    char *str;

    if (!PyArg_ParseTuple(args, "O!is", &Crossfire_ObjectType, &pspell, &dir, &str))
        return NULL;
    EXISTCHECK(who);
    EXISTCHECK(pspell);

    cf_object_cast_ability(who->obj, who->obj, dir, pspell->obj, str);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *Crossfire_Object_PayAmount(Crossfire_Object *who, PyObject *args) {
    uint64 to_pay;
    int val;

    EXISTCHECK(who);
    if (!PyArg_ParseTuple(args, "L", &to_pay))
        return NULL;

    val = cf_object_pay_amount(who->obj, to_pay);

    return Py_BuildValue("i", val);
}

static PyObject *Crossfire_Object_Pay(Crossfire_Object *who, PyObject *args) {
    /* Note that this function uses the METH_O calling convention. */
    Crossfire_Object *op = (Crossfire_Object*)args;
    int val;

    EXISTCHECK(who);
    TYPEEXISTCHECK(op);

    val = cf_object_pay_item(who->obj, op->obj);

    return Py_BuildValue("i", val);
}

static PyObject *Crossfire_Object_ReadKey(Crossfire_Object *who, PyObject *args) {
    const char *val;
    char *keyname;

    EXISTCHECK(who);
    if (!PyArg_ParseTuple(args, "s", &keyname))
        return NULL;

    val = cf_object_get_key(who->obj, keyname);

    return Py_BuildValue("s", val ? val : "");
}

static PyObject *Crossfire_Object_WriteKey(Crossfire_Object *who, PyObject *args) {
    char *keyname;
    char *value;
    int add_key = 0;

    EXISTCHECK(who);
    if (!PyArg_ParseTuple(args, "ss|i", &keyname, &value, &add_key))
        return NULL;

    return Py_BuildValue("i", cf_object_set_key(who->obj, keyname, value, add_key));
}

static PyObject *Crossfire_Object_CreateTimer(Crossfire_Object *who, PyObject *args) {
    int mode;
    long delay;

    EXISTCHECK(who);
    if (!PyArg_ParseTuple(args, "li", &delay, &mode))
        return NULL;

    return Py_BuildValue("i", cf_timer_create(who->obj, delay, mode));
}

static PyObject *Crossfire_Object_CheckInventory(Crossfire_Object *who, PyObject *args) {
    char *whatstr;
    object *foundob;

    EXISTCHECK(who);
    if (!PyArg_ParseTuple(args, "s", &whatstr))
        return NULL;

    foundob = cf_object_present_archname_inside(who->obj, whatstr);

    return Crossfire_Object_wrap(foundob);
/*  FOR_INV_PREPARE(WHO, tmp) {
        if (!strncmp(PyQueryName(tmp), whatstr, strlen(whatstr))) {
            return Py_BuildValue("l", (long)(tmp));
        }
        if (!strncmp(tmp->name, whatstr, strlen(whatstr))) {
            return Py_BuildValue("l", (long)(tmp));
        }
    } FOR_INV_FINISH();

    return Py_BuildValue("l", (long)0);*/
}

static PyObject *Crossfire_Object_CheckArchInventory(Crossfire_Object *who, PyObject *args) {
    char *whatstr;
    object *tmp;

    EXISTCHECK(who);
    if (!PyArg_ParseTuple(args, "s", &whatstr))
        return NULL;

    tmp = cf_object_find_by_arch_name(who->obj, whatstr);
    return Crossfire_Object_wrap(tmp);
}

static PyObject *Crossfire_Object_GetOutOfMap(Crossfire_Object *who, PyObject *args) {
    int x, y;

    EXISTCHECK(who);
    if (!PyArg_ParseTuple(args, "ii", &x, &y))
        return NULL;

    return Py_BuildValue("i", cf_object_out_of_map(who->obj, x, y));
}

static PyObject *Crossfire_Object_CreateInside(Crossfire_Object *who, PyObject *args) {
    char *txt;
    object *myob;

    EXISTCHECK(who);
    if (!PyArg_ParseTuple(args, "s", &txt))
        return NULL;

    myob = cf_create_object_by_name(txt);
    if (myob)
        myob = cf_object_insert_object(myob, who->obj);

    return Crossfire_Object_wrap(myob);
}

static PyObject *Crossfire_Object_InsertInto(Crossfire_Object *who, PyObject *args) {
    /* Note that this function uses the METH_O calling convention. */
    Crossfire_Object *op = (Crossfire_Object*)args;
    object *myob;

    EXISTCHECK(who);
    TYPEEXISTCHECK(op);

    /* we can only insert removed object, so first remove it
     * from it's current container
     */
    if (!cf_object_get_flag(who->obj, FLAG_REMOVED)) {
        cf_object_remove(who->obj);
    }
    myob = cf_object_insert_in_ob(who->obj, op->obj);

    return Crossfire_Object_wrap(myob);
}

static PyObject *Crossfire_Object_ChangeAbil(Crossfire_Object *who, PyObject *args) {
    /* Note that this function uses the METH_O calling convention. */
    Crossfire_Object *op = (Crossfire_Object*)args;

    EXISTCHECK(who);
    TYPEEXISTCHECK(op);

    return Py_BuildValue("i", cf_object_change_abil(who->obj, op->obj));
}

static PyObject *Crossfire_Object_AddExp(Crossfire_Object *who, PyObject *args) {
    sint64 exp;
    const char *skill = NULL;
    int arg = 0;

    if (!PyArg_ParseTuple(args, "L|si", &exp, &skill, &arg))
        return NULL;
    EXISTCHECK(who);
    cf_object_change_exp(who->obj, exp, skill, arg);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *Crossfire_Object_Move(Crossfire_Object *who, PyObject *args) {
    int dir;

    if (!PyArg_ParseTuple(args, "i", &dir))
        return NULL;
    EXISTCHECK(who);
    return Py_BuildValue("i", cf_object_move(who->obj, dir, who->obj));
}

static PyObject *Crossfire_Object_Event(Crossfire_Object *who, PyObject *args) {
    int fix;
    const char *message = NULL;
    object *op1 = NULL;
    object *op2 = NULL;
    object *op3 = NULL;
    Crossfire_Object *activator = NULL;
    Crossfire_Object *third = NULL;

    if (!PyArg_ParseTuple(args, "O!O!si", &Crossfire_ObjectType, &activator, &Crossfire_ObjectType, &third, &message, &fix))
        return NULL;
    EXISTCHECK(who);
    EXISTCHECK(activator);
    EXISTCHECK(third);
    op1 = who->obj;
    op2 = activator->obj;
    op3 = third->obj;
    return Py_BuildValue("i", cf_object_user_event(op1, op2, op3, message, fix));
}

static PyObject *Crossfire_Object_RemoveDepletion(Crossfire_Object *who, PyObject *args) {
    int level;

    if (!PyArg_ParseTuple(args, "i", &level))
        return NULL;
    EXISTCHECK(who);

    return Py_BuildValue("i", cf_object_remove_depletion(who->obj, level));
}



static int Crossfire_Object_InternalCompare(Crossfire_Object *left, Crossfire_Object *right) {
    EXISTCHECK_INT(left);
    EXISTCHECK_INT(right);
    return (left->obj < right->obj ? -1 : (left->obj == right->obj ? 0 : 1));
}

static PyObject *Crossfire_Object_RichCompare(Crossfire_Object *left, Crossfire_Object *right, int op) {
    int result;
    if (!left
        || !right
        || !PyObject_TypeCheck((PyObject*)left, &Crossfire_ObjectType)
        || !PyObject_TypeCheck((PyObject*)right, &Crossfire_ObjectType)) {
        Py_INCREF(Py_NotImplemented);
        return Py_NotImplemented;
    }
    result = Crossfire_Object_InternalCompare(left, right);
    /* Handle removed objects. */
    if (result == -1 && PyErr_Occurred())
        return NULL;
    /* Based on how Python 3.0 (GPL compatible) implements it for internal types: */
    switch (op) {
        case Py_EQ:
            result = (result == 0);
            break;
        case Py_NE:
            result = (result != 0);
            break;
        case Py_LE:
            result = (result <= 0);
            break;
        case Py_GE:
            result = (result >= 0);
            break;
        case Py_LT:
            result = (result == -1);
            break;
        case Py_GT:
            result = (result == 1);
            break;
    }
    return PyBool_FromLong(result);
}

/* Legacy code: convert to long so that non-object functions work correctly */
static PyObject *Crossfire_Object_Long(PyObject *obj) {
    return Py_BuildValue("l", ((Crossfire_Object *)obj)->obj);
}

#ifndef IS_PY3K
static PyObject *Crossfire_Object_Int(PyObject *obj) {
    return Py_BuildValue("i", ((Crossfire_Object *)obj)->obj);
}
#endif

/**
 * Python initialized.
 **/
static PyObject *Crossfire_Object_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
    Crossfire_Object *self;

    self = (Crossfire_Object *)type->tp_alloc(type, 0);
    if (self) {
        self->obj = NULL;
        self->count = 0;
    }

    return (PyObject *)self;
}

static PyObject *Crossfire_Player_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
    Crossfire_Player *self;

    self = (Crossfire_Player *)type->tp_alloc(type, 0);
    if (self) {
        self->obj = NULL;
        self->count = 0;
    }

    return (PyObject *)self;
}

static void Crossfire_Object_dealloc(PyObject *obj) {
    Crossfire_Object *self;

    self = (Crossfire_Object *)obj;
    if (self) {
        if (self->obj) {
            free_object_assoc(self->obj);
        }
        Py_TYPE(self)->tp_free(obj);
    }
}

static void Crossfire_Player_dealloc(PyObject *obj) {
    Crossfire_Player *self;

    self = (Crossfire_Player *)obj;
    if (self) {
        if (self->obj) {
            free_object_assoc(self->obj);
        }
        Py_TYPE(self)->tp_free(obj);
    }
}

PyObject *Crossfire_Object_wrap(object *what) {
    Crossfire_Object *wrapper;
    Crossfire_Player *plwrap;
    PyObject *pyobj;

    /* return None if no object was to be wrapped */
    if (what == NULL) {
        Py_INCREF(Py_None);
        return Py_None;
    }

    pyobj = find_assoc_pyobject(what);
    if ((!pyobj) || (object_was_destroyed(((Crossfire_Object *)pyobj)->obj, ((Crossfire_Object *)pyobj)->count))) {
        if (what->type == PLAYER) {
            plwrap = PyObject_NEW(Crossfire_Player, &Crossfire_PlayerType);
            if (plwrap != NULL) {
                plwrap->obj = what;
                plwrap->count = what->count;
            }
            pyobj = (PyObject *)plwrap;
        } else {
            wrapper = PyObject_NEW(Crossfire_Object, &Crossfire_ObjectType);
            if (wrapper != NULL) {
                wrapper->obj = what;
                wrapper->count = what->count;
            }
            pyobj = (PyObject *)wrapper;
        }
        add_object_assoc(what, pyobj);
    } else {
        Py_INCREF(pyobj);
    }
    return pyobj;
}
