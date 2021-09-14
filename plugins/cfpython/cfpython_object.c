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
#include <hashtable.h>

#define EXISTCHECK(ob) { \
    if (!ob || !ob->obj || (object_was_destroyed(ob->obj, ob->obj->count))) { \
        PyErr_SetString(PyExc_ReferenceError, "Crossfire object no longer exists"); \
        return NULL; \
    } }

/**
 * This is meant to be used for parameters where you don't know if the type of
 * the object is correct. It should NOT be used for the self pointer, since that
 * will always be a compatible type.
 */
#define TYPEEXISTCHECK(ob) { \
    if (!ob || !PyObject_TypeCheck((PyObject*)ob, &Crossfire_ObjectType) || !ob->obj || (object_was_destroyed(ob->obj, ob->obj->count))) { \
        PyErr_SetString(PyExc_ReferenceError, "Not a Crossfire object or Crossfire object no longer exists"); \
        return NULL; \
    } }

#define EXISTCHECK_INT(ob) { \
    if (!ob || !ob->obj || (object_was_destroyed(ob->obj, ob->obj->count))) { \
        PyErr_SetString(PyExc_ReferenceError, "Crossfire object no longer exists"); \
        return -1; \
    } }

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
    (void)closure;
    EXISTCHECK(whoptr);
    return Py_BuildValue("s", cf_player_get_title(whoptr->obj, title, MAX_NAME));
}

static int Player_SetTitle(Crossfire_Object *whoptr, PyObject *value, void *closure) {
    char *val;
    (void)closure;

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
    (void)closure;
    EXISTCHECK(whoptr);
    return Py_BuildValue("s", cf_player_get_ip(whoptr->obj));
}

static PyObject *Player_GetMarkedItem(Crossfire_Player *whoptr, void *closure) {
    (void)closure;
    EXISTCHECK(whoptr);
    return Crossfire_Object_wrap(cf_player_get_marked_item(whoptr->obj));
}

static int Player_SetMarkedItem(Crossfire_Player *whoptr, PyObject *value, void *closure) {
    Crossfire_Object *ob;
    (void)closure;

    EXISTCHECK_INT(whoptr);
    if (value == Py_None)
        cf_player_set_marked_item(whoptr->obj, NULL);
    else if (!PyArg_Parse(value, "O!", &Crossfire_ObjectType, &ob))
        return -1;
    else
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

static PyObject *Player_GiveKnowledge(Crossfire_Player *who, PyObject *args) {
    const char *knowledge;

    EXISTCHECK(who);
    if (!PyArg_ParseTuple(args, "s", &knowledge))
        return NULL;

    cf_player_knowledge_give(who->obj, knowledge);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *Player_GetParty(Crossfire_Player *whoptr, void *closure) {
    (void)closure;
    EXISTCHECK(whoptr);
    return Crossfire_Party_wrap(cf_player_get_party(whoptr->obj));
}

static int Player_SetParty(Crossfire_Player *whoptr, PyObject *value, void *closure) {
    Crossfire_Party *ob;
    (void)closure;

    EXISTCHECK_INT(whoptr);
    if (!PyArg_Parse(value, "O!", &Crossfire_PartyType, &ob))
        return -1;
    cf_player_set_party(whoptr->obj, ob->party);
    return 0;
}

static PyObject *Crossfire_Player_CanPay(Crossfire_Player *who, PyObject *args) {
    (void)args;
    EXISTCHECK(who);
    return Py_BuildValue("i", cf_player_can_pay(who->obj));
}

static PyObject *Player_GetBedMap(Crossfire_Player *whoptr, void *closure) {
    char bed[200];
    (void)closure;

    EXISTCHECK(whoptr);
    return Py_BuildValue("s", cf_object_get_string_property(whoptr->obj, CFAPI_PLAYER_PROP_BED_MAP, bed, sizeof(bed)));
}

static int Player_SetBedMap(Crossfire_Player *whoptr, PyObject *value, void *closure) {
    char *location;
    (void)closure;

    EXISTCHECK_INT(whoptr);
    if (!PyArg_Parse(value, "s", &location))
        return -1;
    cf_object_set_string_property(whoptr->obj, CFAPI_PLAYER_PROP_BED_MAP, location);
    return 0;
}

static PyObject *Player_GetBedX(Crossfire_Player *whoptr, void *closure) {
    (void)closure;
    EXISTCHECK(whoptr);
    return Py_BuildValue("i", cf_object_get_int_property(whoptr->obj, CFAPI_PLAYER_PROP_BED_X));
}

static int Player_SetBedX(Crossfire_Player *whoptr, PyObject *value, void *closure) {
    int x;
    (void)closure;

    EXISTCHECK_INT(whoptr);
    if (!PyArg_Parse(value, "i", &x))
        return -1;
    cf_object_set_int_property(whoptr->obj, CFAPI_PLAYER_PROP_BED_X, x);
    return 0;
}

static PyObject *Player_GetBedY(Crossfire_Player *whoptr, void *closure) {
    (void)closure;
    EXISTCHECK(whoptr);
    return Py_BuildValue("i", cf_object_get_int_property(whoptr->obj, CFAPI_PLAYER_PROP_BED_Y));
}

static int Player_SetBedY(Crossfire_Player *whoptr, PyObject *value, void *closure) {
    int y;
    (void)closure;

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
static PyObject *Object_GetStringProperty(Crossfire_Object *whoptr, void *closure) {
    char buf[MAX_BUF];
    (void)closure;
    EXISTCHECK(whoptr);
    return Py_BuildValue("s", cf_object_get_string_property(whoptr->obj, (int)(intptr_t)closure, buf, sizeof(buf)));
}

static PyObject *Object_GetSStringProperty(Crossfire_Object *whoptr, void *closure) {
    (void)closure;
    EXISTCHECK(whoptr);
    return Py_BuildValue("s", cf_object_get_sstring_property(whoptr->obj, (int)(intptr_t)closure));
}

static PyObject *Object_GetIntProperty(Crossfire_Object *whoptr, void *closure) {
    (void)closure;
    EXISTCHECK(whoptr);
    return Py_BuildValue("i", cf_object_get_int_property(whoptr->obj, (int)(intptr_t)closure));
}

static PyObject *Object_GetFloatProperty(Crossfire_Object *whoptr, void *closure) {
    (void)closure;
    EXISTCHECK(whoptr);
    return Py_BuildValue("f", cf_object_get_float_property(whoptr->obj, (int)(intptr_t)closure));
}

static PyObject *Object_GetFlagProperty(Crossfire_Object *whoptr, void *closure) {
    (void)closure;
    EXISTCHECK(whoptr);
    return Py_BuildValue("i", cf_object_get_flag(whoptr->obj, (int)(intptr_t)closure));
}

static PyObject *Object_GetObjectProperty(Crossfire_Object *whoptr, void *closure) {
    object *op;
    (void)closure;

    EXISTCHECK(whoptr);
    op = cf_object_get_object_property(whoptr->obj, (int)(intptr_t)closure);
    return Crossfire_Object_wrap(op);
}

static PyObject *Object_GetName(Crossfire_Object *whoptr, void *closure) {
    char name[200];
    (void)closure;

    EXISTCHECK(whoptr);
    return Py_BuildValue("s", cf_query_name(whoptr->obj, name, sizeof(name)));
}

static PyObject *Object_GetNamePl(Crossfire_Object *whoptr, void *closure) {
    (void)closure;
    EXISTCHECK(whoptr);
    return Py_BuildValue("s", (char *)cf_query_name_pl(whoptr->obj));
}

static PyObject *Object_GetMap(Crossfire_Object *whoptr, void *closure) {
    mapstruct *m;
    (void)closure;

    EXISTCHECK(whoptr);
    m = cf_object_get_map_property(whoptr->obj, CFAPI_OBJECT_PROP_MAP);
    return Crossfire_Map_wrap(m);
}

static PyObject *Object_GetExp(Crossfire_Object *whoptr, void *closure) {
    (void)closure;
    EXISTCHECK(whoptr);
    return Py_BuildValue("L", cf_object_get_int64_property(whoptr->obj, CFAPI_OBJECT_PROP_EXP));
}

static PyObject *Object_GetTotalExp(Crossfire_Object *whoptr, void *closure) {
    (void)closure;
    EXISTCHECK(whoptr);
    return Py_BuildValue("L", cf_object_get_int64_property(whoptr->obj, CFAPI_OBJECT_PROP_TOTAL_EXP));
}

static PyObject *Object_GetExpMul(Crossfire_Object *whoptr, void *closure) {
    (void)closure;
    EXISTCHECK(whoptr);
    return Py_BuildValue("d", cf_object_get_double_property(whoptr->obj, CFAPI_OBJECT_PROP_EXP_MULTIPLIER));
}

static PyObject *Object_GetPickable(Crossfire_Object *whoptr, void *closure) {
    (void)closure;
    EXISTCHECK(whoptr);
    return Py_BuildValue("i", !cf_object_get_flag(whoptr->obj, FLAG_NO_PICK));
}

static PyObject *Object_GetMoney(Crossfire_Object *whoptr, void *closure) {
    (void)closure;
    EXISTCHECK(whoptr);
    return Py_BuildValue("i", cf_object_query_money(whoptr->obj));
}

static PyObject *Object_GetValue(Crossfire_Object *whoptr, void *closure) {
    (void)closure;
    EXISTCHECK(whoptr);
    return Py_BuildValue("l", cf_object_get_long_property(whoptr->obj, CFAPI_OBJECT_PROP_VALUE));
}

static PyObject *Object_GetArchetype(Crossfire_Object *whoptr, void *closure) {
    (void)closure;
    EXISTCHECK(whoptr);
    return Crossfire_Archetype_wrap(cf_object_get_archetype_property(whoptr->obj, CFAPI_OBJECT_PROP_ARCHETYPE));
}

static PyObject *Object_GetOtherArchetype(Crossfire_Object *whoptr, void *closure) {
    (void)closure;
    EXISTCHECK(whoptr);
    return Crossfire_Archetype_wrap(cf_object_get_archetype_property(whoptr->obj, CFAPI_OBJECT_PROP_OTHER_ARCH));
}

static PyObject *Object_GetExists(Crossfire_Object *whoptr, void *closure) {
    (void)closure;
    if (!object_was_destroyed(whoptr->obj, whoptr->obj->count)) {
        Py_INCREF(Py_True);
        return Py_True;
    } else {
        Py_INCREF(Py_False);
        return Py_False;
    }
}

static PyObject *Object_GetMoveType(Crossfire_Object *whoptr, void *closure) {
    (void)closure;
    EXISTCHECK(whoptr);
    return Py_BuildValue("i", cf_object_get_movetype_property(whoptr->obj, CFAPI_OBJECT_PROP_MOVE_TYPE));
}

static PyObject *Object_GetMoveBlock(Crossfire_Object *whoptr, void *closure) {
    (void)closure;
    EXISTCHECK(whoptr);
    return Py_BuildValue("i", cf_object_get_movetype_property(whoptr->obj, CFAPI_OBJECT_PROP_MOVE_BLOCK));
}

static PyObject *Object_GetMoveAllow(Crossfire_Object *whoptr, void *closure) {
    (void)closure;
    EXISTCHECK(whoptr);
    return Py_BuildValue("i", cf_object_get_movetype_property(whoptr->obj, CFAPI_OBJECT_PROP_MOVE_ALLOW));
}

static PyObject *Object_GetMoveOn(Crossfire_Object *whoptr, void *closure) {
    (void)closure;
    EXISTCHECK(whoptr);
    return Py_BuildValue("i", cf_object_get_movetype_property(whoptr->obj, CFAPI_OBJECT_PROP_MOVE_ON));
}

static PyObject *Object_GetMoveOff(Crossfire_Object *whoptr, void *closure) {
    (void)closure;
    EXISTCHECK(whoptr);
    return Py_BuildValue("i", cf_object_get_movetype_property(whoptr->obj, CFAPI_OBJECT_PROP_MOVE_OFF));
}

static PyObject *Object_GetMoveSlow(Crossfire_Object *whoptr, void *closure) {
    (void)closure;
    EXISTCHECK(whoptr);
    return Py_BuildValue("i", cf_object_get_movetype_property(whoptr->obj, CFAPI_OBJECT_PROP_MOVE_SLOW));
}

static PyObject *Object_GetMaterial(Crossfire_Object *whoptr, void *closure) {
    (void)closure;
    EXISTCHECK(whoptr);
    return Py_BuildValue("{s:s,s:i}", "Name", cf_object_get_sstring_property(whoptr->obj, CFAPI_OBJECT_PROP_MATERIAL_NAME), "Number", cf_object_get_int_property(whoptr->obj, CFAPI_OBJECT_PROP_MATERIAL));
}

/** Setters */
static int Object_SetStringProperty(Crossfire_Object *whoptr, PyObject *value, void *closure) {
    char *val;

    EXISTCHECK_INT(whoptr);
    if (value == NULL) {
        PyErr_SetString(PyExc_TypeError, "Cannot delete the attribute");
        return -1;
    }
    if (!CF_IS_PYSTR(value)) {
        PyErr_SetString(PyExc_TypeError, "The attribute must be a string");
        return -1;
    }
    if (!PyArg_Parse(value, "s", &val))
        return -1;

    cf_object_set_string_property(whoptr->obj, (int)(intptr_t)closure, val);
    return 0;
}

static int Object_SetSStringProperty(Crossfire_Object *whoptr, PyObject *value, void *closure) {
    char *val;

    EXISTCHECK_INT(whoptr);
    if (value == NULL) {
        PyErr_SetString(PyExc_TypeError, "Cannot delete attribute");
        return -1;
    }
    if (!CF_IS_PYSTR(value)) {
        PyErr_SetString(PyExc_TypeError, "The attribute must be a string");
        return -1;
    }
    if (!PyArg_Parse(value, "s", &val))
        return -1;

    cf_object_set_string_property(whoptr->obj, (int)(intptr_t)closure, val);
    return 0;
}

static int Object_SetIntProperty(Crossfire_Object *whoptr, PyObject *value, void *closure) {
    int val;

    EXISTCHECK_INT(whoptr);
    if (!PyArg_Parse(value, "i", &val))
        return -1;

    cf_object_set_int_property(whoptr->obj, (int)(intptr_t)closure, val);
    return 0;
}

static int Object_SetFloatProperty(Crossfire_Object *whoptr, PyObject *value, void *closure) {
    float val;

    EXISTCHECK_INT(whoptr);
    if (!PyArg_Parse(value, "f", &val))
        return -1;

    cf_object_set_float_property(whoptr->obj, (int)(intptr_t)closure, val);
    return 0;
}

static int Object_SetFlagProperty(Crossfire_Object *whoptr, PyObject *value, void *closure) {
    int val;

    EXISTCHECK_INT(whoptr);
    if (!PyArg_Parse(value, "i", &val))
        return -1;

    cf_object_set_flag(whoptr->obj, (int)(intptr_t)closure, val);
    return 0;
}

static int Object_SetName(Crossfire_Object *whoptr, PyObject *value, void *closure) {
    char *val;
    (void)closure;

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
    (void)closure;

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

static int Object_SetPickable(Crossfire_Object *whoptr, PyObject *value, void *closure) {
    int val;
    (void)closure;

    EXISTCHECK_INT(whoptr);
    if (!PyArg_Parse(value, "i", &val))
        return -1;

    cf_object_set_flag(whoptr->obj, FLAG_NO_PICK, !val);
    return 0;
}

static int Object_SetMap(Crossfire_Object *whoptr, PyObject *value, void *closure) {
    Crossfire_Map *val;
    (void)closure;

    EXISTCHECK_INT(whoptr);
    if (!PyArg_Parse(value, "O!", &Crossfire_MapType, &val))
        return -1;

    cf_object_change_map(whoptr->obj, val->map, NULL, 0, -1, -1);
    return 0;
}

static int Object_SetQuantity(Crossfire_Object *whoptr, PyObject *value, void *closure) {
    int val;
    (void)closure;

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

static int Object_SetFace(Crossfire_Object *whoptr, PyObject *value, void *closure) {
    char *face;
    (void)closure;

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
    char *anim;
    (void)closure;

    EXISTCHECK_INT(whoptr);
    if (!PyArg_Parse(value, "s", &anim))
        return -1;

    if (!cf_object_set_animation(whoptr->obj, anim)) {
        PyErr_SetString(PyExc_TypeError, "Unknown animation.");
        return -1;
    }

    return 0;
}

static int Object_SetValue(Crossfire_Object *whoptr, PyObject *value, void *closure) {
    long val;
    (void)closure;

    EXISTCHECK_INT(whoptr);
    if (!PyArg_Parse(value, "l", &val))
        return -1;

    cf_object_set_long_property(whoptr->obj, CFAPI_OBJECT_PROP_VALUE, val);
    return 0;
}

static int Object_SetOwner(Crossfire_Object *whoptr, PyObject *value, void *closure) {
    Crossfire_Object *ob;
    (void)closure;

    EXISTCHECK_INT(whoptr);
    if (!PyArg_Parse(value, "O!", &Crossfire_ObjectType, &ob))
        return -1;
    cf_object_set_object_property(whoptr->obj, CFAPI_OBJECT_PROP_OWNER, ob->obj);
    return 0;
}

static int Object_SetEnemy(Crossfire_Object *whoptr, PyObject *value, void *closure) {
    Crossfire_Object *ob;
    (void)closure;

    EXISTCHECK_INT(whoptr);
    if (!PyArg_Parse(value, "O!", &Crossfire_ObjectType, &ob))
        return -1;
    cf_object_set_object_property(whoptr->obj, CFAPI_OBJECT_PROP_ENEMY, ob->obj);
    return 0;
}

static int Object_SetExp(Crossfire_Object *whoptr, PyObject *value, void *closure) {
    int64_t val;
    (void)closure;

    EXISTCHECK_INT(whoptr);
    if (!PyArg_Parse(value, "L", &val))
        return -1;

    cf_object_set_int64_property(whoptr->obj, CFAPI_OBJECT_PROP_EXP, val);
    return 0;
}

static int Object_SetMoveType(Crossfire_Object *whoptr, PyObject *value, void *closure) {
    MoveType move;
    (void)closure;

    EXISTCHECK_INT(whoptr);
    if (!PyArg_Parse(value, "B", &move))
        return -1;
    cf_object_set_movetype_property(whoptr->obj, CFAPI_OBJECT_PROP_MOVE_TYPE, (MoveType)(move & MOVE_ALL));
    return 0;
}

static int Object_SetMoveBlock(Crossfire_Object *whoptr, PyObject *value, void *closure) {
    MoveType move;
    (void)closure;

    EXISTCHECK_INT(whoptr);
    if (!PyArg_Parse(value, "B", &move))
        return -1;
    cf_object_set_movetype_property(whoptr->obj, CFAPI_OBJECT_PROP_MOVE_BLOCK, move);
    return 0;
}

static int Object_SetMoveAllow(Crossfire_Object *whoptr, PyObject *value, void *closure) {
    MoveType move;
    (void)closure;

    EXISTCHECK_INT(whoptr);
    if (!PyArg_Parse(value, "B", &move))
        return -1;
    cf_object_set_movetype_property(whoptr->obj, CFAPI_OBJECT_PROP_MOVE_ALLOW, move);
    return 0;
}

static int Object_SetMoveOn(Crossfire_Object *whoptr, PyObject *value, void *closure) {
    MoveType move;
    (void)closure;

    EXISTCHECK_INT(whoptr);
    if (!PyArg_Parse(value, "B", &move))
        return -1;
    cf_object_set_movetype_property(whoptr->obj, CFAPI_OBJECT_PROP_MOVE_ON, move);
    return 0;
}

static int Object_SetMoveOff(Crossfire_Object *whoptr, PyObject *value, void *closure) {
    MoveType move;
    (void)closure;

    EXISTCHECK_INT(whoptr);
    if (!PyArg_Parse(value, "B", &move))
        return -1;
    cf_object_set_movetype_property(whoptr->obj, CFAPI_OBJECT_PROP_MOVE_OFF, move);
    return 0;
}

static int Object_SetMoveSlow(Crossfire_Object *whoptr, PyObject *value, void *closure) {
    MoveType move;
    (void)closure;

    EXISTCHECK_INT(whoptr);
    if (!PyArg_Parse(value, "B", &move))
        return -1;
    cf_object_set_movetype_property(whoptr->obj, CFAPI_OBJECT_PROP_MOVE_SLOW, move);
    return 0;
}

/* Methods. */

static PyObject *Crossfire_Object_Remove(Crossfire_Object *who, PyObject *args) {
    (void)args;
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
    (void)args;
    cf_fix_object(who->obj);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *Crossfire_Object_Take(Crossfire_Object *who, PyObject *args) {
    /* Note that this function uses the METH_O calling convention. */
    Crossfire_Object *whoptr = (Crossfire_Object*)args;

    EXISTCHECK(who);
    TYPEEXISTCHECK(whoptr);

    cf_object_pickup(who->obj, whoptr->obj);
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
    char *message, buf[2048];

    EXISTCHECK(who);
    if (!PyArg_ParseTuple(args, "s", &message))
        return NULL;

    /* compatibility */
    if (current_context->talk == NULL) {
        cf_object_say(who->obj, message);
        Py_INCREF(Py_None);
        return Py_None;
    }

    if (current_context->talk->npc_msg_count == MAX_NPC) {
        PyErr_SetString(PyExc_ValueError, "too many NPCs");
        return NULL;
    }

    if (strlen(message) >= sizeof(buf) - 1)
        cf_log(llevError, "warning, too long message in npcSay, will be truncated");
    /** @todo fix by wrapping monster_format_say() (or the whole talk structure methods) */
    snprintf(buf, sizeof(buf), "%s says: %s", who->obj->name, message);

    current_context->talk->npc_msgs[current_context->talk->npc_msg_count] = cf_add_string(buf);
    current_context->talk->npc_msg_count++;

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
    (void)args;

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

    cf_log_plain(llevError, "CastAbility is deprecated and will be removed, use 'Cast'.\n");
    cf_object_cast_spell(who->obj, who->obj, dir, pspell->obj, str);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *Crossfire_Object_PayAmount(Crossfire_Object *who, PyObject *args) {
    uint64_t to_pay;
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
    if (!PyArg_ParseTuple(args, "sz|i", &keyname, &value, &add_key))
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
    int64_t exp;
    const char *skill = NULL;
    int arg = 0;

    if (!PyArg_ParseTuple(args, "L|si", &exp, &skill, &arg))
        return NULL;
    EXISTCHECK(who);
    cf_object_change_exp(who->obj, exp, skill, arg);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *Crossfire_Object_PermExp(Crossfire_Object *who, PyObject *args) {
    (void)args;
    EXISTCHECK(who);
    return Py_BuildValue("L", cf_object_perm_exp(who->obj));
}

static PyObject *Crossfire_Object_Move(Crossfire_Object *who, PyObject *args) {
    int dir;

    if (!PyArg_ParseTuple(args, "i", &dir))
        return NULL;
    EXISTCHECK(who);
    return Py_BuildValue("i", cf_object_move(who->obj, dir, who->obj));
}

static PyObject *Crossfire_Object_MoveTo(Crossfire_Object *who, PyObject *args) {
    int x,y;

    if (!PyArg_ParseTuple(args, "ii", &x, &y))
        return NULL;
    EXISTCHECK(who);
    return Py_BuildValue("i", cf_object_move_to(who->obj, x, y));
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

static PyObject *Crossfire_Object_Arrest(Crossfire_Object *who, PyObject *args) {
    (void)args;
    EXISTCHECK(who);
    return Py_BuildValue("i", cf_player_arrest(who->obj));
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

/* Python binding */
static PyGetSetDef Object_getseters[] = {
    { "Name",           (getter)Object_GetName,         (setter)Object_SetName, NULL, NULL },
    { "NamePl",         (getter)Object_GetNamePl,       (setter)Object_SetNamePl, NULL, NULL },
    { "Title",          (getter)Object_GetSStringProperty,  (setter)Object_SetStringProperty, NULL, (void*)CFAPI_OBJECT_PROP_TITLE },
    { "Race",           (getter)Object_GetSStringProperty,  (setter)Object_SetStringProperty, NULL, (void*)CFAPI_OBJECT_PROP_RACE },
    { "Skill",          (getter)Object_GetSStringProperty,  (setter)Object_SetStringProperty, NULL, (void*)CFAPI_OBJECT_PROP_SKILL },
    { "Map",            (getter)Object_GetMap,          (setter)Object_SetMap, NULL, NULL },
    { "Cha",            (getter)Object_GetIntProperty,  (setter)Object_SetIntProperty, NULL, (void*)CFAPI_OBJECT_PROP_CHA },
    { "Con",            (getter)Object_GetIntProperty,  (setter)Object_SetIntProperty, NULL, (void*)CFAPI_OBJECT_PROP_CON },
    { "Dex",            (getter)Object_GetIntProperty,  (setter)Object_SetIntProperty, NULL, (void*)CFAPI_OBJECT_PROP_DEX },
    { "Int",            (getter)Object_GetIntProperty,  (setter)Object_SetIntProperty, NULL, (void*)CFAPI_OBJECT_PROP_INT },
    { "Pow",            (getter)Object_GetIntProperty,  (setter)Object_SetIntProperty, NULL, (void*)CFAPI_OBJECT_PROP_POW },
    { "Str",            (getter)Object_GetIntProperty,  (setter)Object_SetIntProperty, NULL, (void*)CFAPI_OBJECT_PROP_STR },
    { "Wis",            (getter)Object_GetIntProperty,  (setter)Object_SetIntProperty, NULL, (void*)CFAPI_OBJECT_PROP_WIS },
    { "HP",             (getter)Object_GetIntProperty,  (setter)Object_SetIntProperty, NULL, (void*)CFAPI_OBJECT_PROP_HP },
    { "MaxHP",          (getter)Object_GetIntProperty,  (setter)Object_SetIntProperty, NULL, (void*)CFAPI_OBJECT_PROP_MAXHP },
    { "SP",             (getter)Object_GetIntProperty,  (setter)Object_SetIntProperty, NULL, (void*)CFAPI_OBJECT_PROP_SP },
    { "MaxSP",          (getter)Object_GetIntProperty,  (setter)Object_SetIntProperty, NULL, (void*)CFAPI_OBJECT_PROP_MAXSP },
    { "Grace",          (getter)Object_GetIntProperty,  (setter)Object_SetIntProperty, NULL, (void*)CFAPI_OBJECT_PROP_GP },
    { "MaxGrace",       (getter)Object_GetIntProperty,  (setter)Object_SetIntProperty, NULL, (void*)CFAPI_OBJECT_PROP_MAXGP },
    { "Food",           (getter)Object_GetIntProperty,  (setter)Object_SetIntProperty, NULL, (void*)CFAPI_OBJECT_PROP_FP },
    { "AC",             (getter)Object_GetIntProperty,  (setter)Object_SetIntProperty, NULL, (void*)CFAPI_OBJECT_PROP_AC },
    { "WC",             (getter)Object_GetIntProperty,  (setter)Object_SetIntProperty, NULL, (void*)CFAPI_OBJECT_PROP_WC },
    { "Dam",            (getter)Object_GetIntProperty,  (setter)Object_SetIntProperty, NULL, (void*)CFAPI_OBJECT_PROP_DAM },
    { "Luck",           (getter)Object_GetIntProperty,  NULL, NULL, (void*)CFAPI_OBJECT_PROP_LUCK },
    { "Exp",            (getter)Object_GetExp,          (setter)Object_SetExp, NULL, NULL },
    { "ExpMul",         (getter)Object_GetExpMul,       NULL, NULL, NULL },
    { "TotalExp",       (getter)Object_GetTotalExp,     NULL, NULL, NULL },
    { "Message",        (getter)Object_GetSStringProperty,  (setter)Object_SetStringProperty, NULL, (void*)CFAPI_OBJECT_PROP_MESSAGE },
    { "Slaying",        (getter)Object_GetSStringProperty,  (setter)Object_SetStringProperty, NULL, (void*)CFAPI_OBJECT_PROP_SLAYING },
    { "Cursed",         (getter)Object_GetFlagProperty,     (setter)Object_SetFlagProperty, NULL, (void*)FLAG_CURSED },
    { "Damned",         (getter)Object_GetFlagProperty,       (setter)Object_SetFlagProperty, NULL, (void*)FLAG_DAMNED },
    { "Weight",         (getter)Object_GetIntProperty,       (setter)Object_SetIntProperty, NULL, (void*)CFAPI_OBJECT_PROP_WEIGHT },
    { "WeightLimit",    (getter)Object_GetIntProperty,  (setter)Object_SetIntProperty, NULL, (void*)CFAPI_OBJECT_PROP_WEIGHT_LIMIT },
    { "Above",          (getter)Object_GetObjectProperty,        NULL, NULL, (void*)CFAPI_OBJECT_PROP_OB_ABOVE },
    { "Below",          (getter)Object_GetObjectProperty,        NULL, NULL, (void*)CFAPI_OBJECT_PROP_OB_BELOW },
    { "Inventory",      (getter)Object_GetObjectProperty,    NULL, NULL, (void*)CFAPI_OBJECT_PROP_INVENTORY },
    { "X",              (getter)Object_GetIntProperty,            NULL, NULL, (void*)CFAPI_OBJECT_PROP_X },
    { "Y",              (getter)Object_GetIntProperty,            NULL, NULL, (void*)CFAPI_OBJECT_PROP_Y },
    { "Direction",      (getter)Object_GetIntProperty,    (setter)Object_SetIntProperty, NULL, (void*)CFAPI_OBJECT_PROP_DIRECTION },
    { "Facing",         (getter)Object_GetIntProperty,       (setter)Object_SetIntProperty, NULL, (void*)CFAPI_OBJECT_PROP_FACING },
    { "Unaggressive",   (getter)Object_GetFlagProperty, (setter)Object_SetFlagProperty, NULL, (void*)FLAG_UNAGGRESSIVE },
    { "God",            (getter)Object_GetSStringProperty,          (setter)Object_SetFlagProperty, NULL, (void*)CFAPI_OBJECT_PROP_GOD },
    { "Pickable",       (getter)Object_GetPickable,     (setter)Object_SetPickable, NULL, NULL },
    { "Quantity",       (getter)Object_GetIntProperty,     (setter)Object_SetQuantity, NULL, (void*)CFAPI_OBJECT_PROP_NROF },
    { "Invisible",      (getter)Object_GetIntProperty,    (setter)Object_SetIntProperty, NULL, (void*)CFAPI_OBJECT_PROP_INVISIBLE },
    { "Speed",          (getter)Object_GetFloatProperty,        (setter)Object_SetFloatProperty, NULL, (void*)CFAPI_OBJECT_PROP_SPEED },
    { "SpeedLeft",      (getter)Object_GetFloatProperty,    (setter)Object_SetFloatProperty, NULL, (void*)CFAPI_OBJECT_PROP_SPEED_LEFT },
    { "LastSP",         (getter)Object_GetIntProperty,       (setter)Object_SetIntProperty, NULL, (void*)CFAPI_OBJECT_PROP_LAST_SP },
    { "LastGrace",      (getter)Object_GetIntProperty,    (setter)Object_SetIntProperty, NULL, (void*)CFAPI_OBJECT_PROP_LAST_GRACE },
    { "LastEat",        (getter)Object_GetIntProperty,      (setter)Object_SetIntProperty, NULL, (void*)CFAPI_OBJECT_PROP_LAST_EAT },
    { "Level",          (getter)Object_GetIntProperty,        (setter)Object_SetIntProperty, NULL, (void*)CFAPI_OBJECT_PROP_LEVEL },
    { "Face",           (getter)Object_GetSStringProperty,   (setter)Object_SetFace, NULL, (void*)CFAPI_OBJECT_PROP_FACE },
    { "Anim",           (getter)Object_GetSStringProperty,   (setter)Object_SetAnim, NULL, (void*)CFAPI_OBJECT_PROP_ANIMATION },
    { "AnimSpeed",      (getter)Object_GetIntProperty,    (setter)Object_SetIntProperty, NULL, (void*)CFAPI_OBJECT_PROP_ANIM_SPEED },
    { "AttackType",     (getter)Object_GetIntProperty,   (setter)Object_SetIntProperty, NULL, (void*)CFAPI_OBJECT_PROP_ATTACK_TYPE },
    { "BeenApplied",    (getter)Object_GetFlagProperty,  (setter)Object_SetFlagProperty, NULL, (void*)FLAG_BEEN_APPLIED },
    { "Identified",     (getter)Object_GetFlagProperty,   (setter)Object_SetFlagProperty, NULL, (void*)FLAG_IDENTIFIED },
    { "Alive",          (getter)Object_GetFlagProperty,        (setter)Object_SetFlagProperty, NULL, (void*)FLAG_ALIVE },
    { "DungeonMaster",  (getter)Object_GetFlagProperty,         (setter)Object_SetFlagProperty, NULL, (void*)FLAG_WIZ },
    { "WasDungeonMaster", (getter)Object_GetFlagProperty,      (setter)Object_SetFlagProperty, NULL, (void*)FLAG_WAS_WIZ },
    { "Applied",        (getter)Object_GetFlagProperty,      (setter)Object_SetFlagProperty, NULL, (void*)FLAG_APPLIED },
    { "Unpaid",         (getter)Object_GetFlagProperty,       (setter)Object_SetFlagProperty, NULL, (void*)FLAG_UNPAID },
    { "Monster",        (getter)Object_GetFlagProperty,      NULL, NULL, (void*)FLAG_MONSTER },
    { "Friendly",       (getter)Object_GetFlagProperty,     (setter)Object_SetFlagProperty, NULL, (void*)FLAG_FRIENDLY },
    { "Generator",      (getter)Object_GetFlagProperty,    NULL, NULL, (void*)FLAG_GENERATOR },
    { "Thrown",         (getter)Object_GetFlagProperty,       NULL, NULL, (void*)FLAG_IS_THROWN },
    { "CanSeeInvisible", (getter)Object_GetFlagProperty, (setter)Object_SetFlagProperty, NULL, (void*)FLAG_SEE_INVISIBLE },
    { "Rollable",       (getter)Object_GetFlagProperty,     (setter)Object_SetFlagProperty, NULL, (void*)FLAG_CAN_ROLL },
    { "Turnable",       (getter)Object_GetFlagProperty,     (setter)Object_SetFlagProperty, NULL, (void*)FLAG_IS_TURNABLE },
    { "UsedUp",         (getter)Object_GetFlagProperty,       (setter)Object_SetFlagProperty, NULL, (void*)FLAG_IS_USED_UP },
    { "Splitting",      (getter)Object_GetFlagProperty,    NULL, NULL, (void*)FLAG_SPLITTING },
    { "Blind",          (getter)Object_GetFlagProperty,        (setter)Object_SetFlagProperty, NULL, (void*)FLAG_BLIND },
    { "CanUseSkill",    (getter)Object_GetFlagProperty,  NULL, NULL, (void*)FLAG_CAN_USE_SKILL },
    { "KnownCursed",    (getter)Object_GetFlagProperty,  (setter)Object_SetFlagProperty, NULL, (void*)FLAG_KNOWN_CURSED },
    { "Stealthy",       (getter)Object_GetFlagProperty,     (setter)Object_SetFlagProperty, NULL, (void*)FLAG_STEALTH },
    { "Confused",       (getter)Object_GetFlagProperty,     (setter)Object_SetFlagProperty, NULL, (void*)FLAG_CONFUSED },
    { "Sleeping",       (getter)Object_GetFlagProperty,     (setter)Object_SetFlagProperty, NULL, (void*)FLAG_SLEEP },
    { "Lifesaver",      (getter)Object_GetFlagProperty,    (setter)Object_SetFlagProperty, NULL, (void*)FLAG_LIFESAVE },
    { "Floor",          (getter)Object_GetFlagProperty,        NULL, NULL, (void*)FLAG_IS_FLOOR },
    { "HasXRays",       (getter)Object_GetFlagProperty,     (setter)Object_SetFlagProperty, NULL, (void*)FLAG_XRAYS },
    { "CanUseRing",     (getter)Object_GetFlagProperty,   NULL, NULL, (void*)FLAG_USE_RING },
    { "CanUseBow",      (getter)Object_GetFlagProperty,    NULL, NULL, (void*)FLAG_USE_BOW },
    { "CanUseWand",     (getter)Object_GetFlagProperty,   NULL, NULL, (void*)FLAG_USE_RANGE },
    { "CanSeeInDark",   (getter)Object_GetFlagProperty, (setter)Object_SetFlagProperty, NULL, (void*)FLAG_SEE_IN_DARK },
    { "KnownMagical",   (getter)Object_GetFlagProperty, (setter)Object_SetFlagProperty, NULL, (void*)FLAG_KNOWN_MAGICAL },
    { "CanUseWeapon",   (getter)Object_GetFlagProperty, NULL, NULL, (void*)FLAG_USE_WEAPON },
    { "CanUseArmour",   (getter)Object_GetFlagProperty, NULL, NULL, (void*)FLAG_USE_ARMOUR },
    { "CanUseScroll",   (getter)Object_GetFlagProperty, NULL, NULL, (void*)FLAG_USE_SCROLL },
    { "CanCastSpell",   (getter)Object_GetFlagProperty, NULL, NULL, (void*)FLAG_CAST_SPELL },
    { "ReflectSpells",  (getter)Object_GetFlagProperty, (setter)Object_SetFlagProperty, NULL, (void*)FLAG_REFL_SPELL },
    { "ReflectMissiles", (getter)Object_GetFlagProperty, (setter)Object_SetFlagProperty, NULL, (void*)FLAG_REFL_MISSILE },
    { "Unique",         (getter)Object_GetFlagProperty,       (setter)Object_SetFlagProperty, NULL, (void*)FLAG_UNIQUE },
    { "RunAway",        (getter)Object_GetFlagProperty,      (setter)Object_SetFlagProperty, NULL, (void*)FLAG_RUN_AWAY },
    { "Scared",         (getter)Object_GetFlagProperty,       (setter)Object_SetFlagProperty, NULL, (void*)FLAG_SCARED },
    { "Undead",         (getter)Object_GetFlagProperty,       (setter)Object_SetFlagProperty, NULL, (void*)FLAG_UNDEAD },
    { "BlocksView",     (getter)Object_GetFlagProperty,   (setter)Object_SetFlagProperty, NULL, (void*)FLAG_BLOCKSVIEW },
    { "HitBack",        (getter)Object_GetFlagProperty,      (setter)Object_SetFlagProperty, NULL, (void*)FLAG_HITBACK },
    { "StandStill",     (getter)Object_GetFlagProperty,   (setter)Object_SetFlagProperty, NULL, (void*)FLAG_STAND_STILL },
    { "OnlyAttack",     (getter)Object_GetFlagProperty,   (setter)Object_SetFlagProperty, NULL, (void*)FLAG_ONLY_ATTACK },
    { "MakeInvisible",  (getter)Object_GetFlagProperty, (setter)Object_SetFlagProperty, NULL, (void*)FLAG_MAKE_INVIS },
    { "Money",          (getter)Object_GetMoney,        NULL, NULL, NULL },
    { "Type",           (getter)Object_GetIntProperty,         NULL, NULL, (void*)CFAPI_OBJECT_PROP_TYPE },
    { "Subtype",        (getter)Object_GetIntProperty,      NULL, NULL, (void*)CFAPI_OBJECT_PROP_SUBTYPE },
    { "Value",          (getter)Object_GetValue,        (setter)Object_SetValue, NULL, NULL },
    { "ArchName",       (getter)Object_GetSStringProperty,  NULL, NULL, (void*)CFAPI_OBJECT_PROP_ARCH_NAME },
    { "Archetype",      (getter)Object_GetArchetype,    NULL, NULL, NULL },
    { "OtherArchetype", (getter)Object_GetOtherArchetype,NULL, NULL, NULL },
    { "Exists",         (getter)Object_GetExists,       NULL, NULL, NULL },
    { "NoSave",         (getter)Object_GetFlagProperty,       (setter)Object_SetFlagProperty, NULL, (void*)FLAG_NO_SAVE },
    { "Env",            (getter)Object_GetObjectProperty,          NULL, NULL, (void*)CFAPI_OBJECT_PROP_ENVIRONMENT },
    { "MoveType",       (getter)Object_GetMoveType,     (setter)Object_SetMoveType, NULL, NULL },
    { "MoveBlock",      (getter)Object_GetMoveBlock,    (setter)Object_SetMoveBlock, NULL, NULL },
    { "MoveAllow",      (getter)Object_GetMoveAllow,    (setter)Object_SetMoveAllow, NULL, NULL },
    { "MoveOn",         (getter)Object_GetMoveOn,       (setter)Object_SetMoveOn, NULL, NULL },
    { "MoveOff",        (getter)Object_GetMoveOff,      (setter)Object_SetMoveOff, NULL, NULL },
    { "MoveSlow",       (getter)Object_GetMoveSlow,     (setter)Object_SetMoveSlow, NULL, NULL },
    { "MoveSlowPenalty", (getter)Object_GetFloatProperty, NULL, NULL, (void*)CFAPI_OBJECT_PROP_MOVE_SLOW_PENALTY },
    { "Owner",          (getter)Object_GetObjectProperty,        (setter)Object_SetOwner, NULL, (void*)CFAPI_OBJECT_PROP_OWNER },
    { "Enemy",          (getter)Object_GetObjectProperty,        (setter)Object_SetEnemy, NULL, (void*)CFAPI_OBJECT_PROP_ENEMY },
    { "Count",          (getter)Object_GetIntProperty,  NULL, NULL, (void*)CFAPI_OBJECT_PROP_COUNT },
    { "GodGiven",       (getter)Object_GetFlagProperty,     (setter)Object_SetFlagProperty, NULL, (void*)FLAG_STARTEQUIP },
    { "IsPet",          (getter)Object_GetIntProperty,  (setter)Object_SetIntProperty, NULL, (void*)CFAPI_OBJECT_PROP_FRIENDLY },
    { "AttackMovement", (getter)Object_GetIntProperty,  (setter)Object_SetIntProperty, NULL, (void*)CFAPI_OBJECT_PROP_ATTACK_MOVEMENT },
    { "Duration",       (getter)Object_GetIntProperty,  (setter)Object_SetIntProperty, NULL, (void*)CFAPI_OBJECT_PROP_DURATION },
    { "GlowRadius",     (getter)Object_GetIntProperty,  (setter)Object_SetIntProperty, NULL, (void*)CFAPI_OBJECT_PROP_GLOW_RADIUS },
    { "Animated",       (getter)Object_GetFlagProperty,     (setter)Object_SetFlagProperty, NULL, (void*)FLAG_ANIMATE },
    { "NoDamage",       (getter)Object_GetFlagProperty,     (setter)Object_SetFlagProperty, NULL, (void*)FLAG_NO_DAMAGE },
    { "RandomMovement", (getter)Object_GetFlagProperty, (setter)Object_SetFlagProperty, NULL, (void*)FLAG_RANDOM_MOVE },
    { "Material",       (getter)Object_GetMaterial,     NULL, NULL, NULL },
    { "Container",      (getter)Object_GetObjectProperty,    NULL, NULL, (void*)CFAPI_OBJECT_PROP_CONTAINER },
    { "ItemPower",      (getter)Object_GetIntProperty,  (setter)Object_SetIntProperty, NULL, (void*)CFAPI_OBJECT_PROP_ITEM_POWER },
    { NULL, NULL, NULL, NULL, NULL }
};

static PyMethodDef ObjectMethods[] = {
    { "Remove",         (PyCFunction)Crossfire_Object_Remove,       METH_NOARGS,  NULL },
    { "Apply",          (PyCFunction)Crossfire_Object_Apply,        METH_VARARGS, NULL },
    { "Drop",           (PyCFunction)Crossfire_Object_Drop,         METH_O,       NULL },
    { "Clone",           (PyCFunction)Crossfire_Object_Clone,       METH_VARARGS, NULL },
    { "Split",           (PyCFunction)Crossfire_Object_Split,       METH_VARARGS, NULL },
    { "Fix",            (PyCFunction)Crossfire_Object_Fix,          METH_NOARGS,  NULL },
    { "Say",            (PyCFunction)Crossfire_Object_Say,          METH_VARARGS, NULL },
    { "Speak",          (PyCFunction)Crossfire_Object_Say,          METH_VARARGS, NULL },
    { "Take",           (PyCFunction)Crossfire_Object_Take,         METH_O,       NULL },
    { "Teleport",       (PyCFunction)Crossfire_Object_Teleport,     METH_VARARGS, NULL },
    { "Reposition",     (PyCFunction)Crossfire_Object_Reposition,   METH_VARARGS, NULL },
    { "QueryName",      (PyCFunction)Crossfire_Object_QueryName,    METH_NOARGS,  NULL },
    { "GetResist",      (PyCFunction)Crossfire_Object_GetResist,    METH_VARARGS, NULL },
    { "SetResist",      (PyCFunction)Crossfire_Object_SetResist,    METH_VARARGS, NULL },
    { "ActivateRune",   (PyCFunction)Crossfire_Object_ActivateRune, METH_O,       NULL },
    { "CheckTrigger",   (PyCFunction)Crossfire_Object_CheckTrigger, METH_O,       NULL },
    { "Cast",           (PyCFunction)Crossfire_Object_Cast,         METH_VARARGS, NULL },
    { "LearnSpell",     (PyCFunction)Crossfire_Object_LearnSpell,   METH_O,       NULL },
    { "ForgetSpell",    (PyCFunction)Crossfire_Object_ForgetSpell,  METH_O,       NULL },
    { "KnowSpell",      (PyCFunction)Crossfire_Object_KnowSpell,    METH_VARARGS, NULL },
    { "CastAbility",    (PyCFunction)Crossfire_Object_CastAbility,  METH_VARARGS, NULL },
    { "PayAmount",      (PyCFunction)Crossfire_Object_PayAmount,    METH_VARARGS, NULL },
    { "Pay",            (PyCFunction)Crossfire_Object_Pay,          METH_O,       NULL },
    { "CheckInventory", (PyCFunction)Crossfire_Object_CheckInventory, METH_VARARGS, NULL },
    { "CheckArchInventory", (PyCFunction)Crossfire_Object_CheckArchInventory, METH_VARARGS, NULL },
    { "OutOfMap",       (PyCFunction)Crossfire_Object_GetOutOfMap,  METH_VARARGS, NULL },
    { "CreateObject",   (PyCFunction)Crossfire_Object_CreateInside, METH_VARARGS, NULL },
    { "InsertInto",     (PyCFunction)Crossfire_Object_InsertInto,   METH_O,       NULL },
    { "ReadKey",        (PyCFunction)Crossfire_Object_ReadKey,      METH_VARARGS, NULL },
    { "WriteKey",       (PyCFunction)Crossfire_Object_WriteKey,     METH_VARARGS, NULL },
    { "CreateTimer",    (PyCFunction)Crossfire_Object_CreateTimer,  METH_VARARGS, NULL },
    { "AddExp",         (PyCFunction)Crossfire_Object_AddExp,       METH_VARARGS, NULL },
    { "Move",           (PyCFunction)Crossfire_Object_Move,         METH_VARARGS, NULL },
    { "MoveTo",         (PyCFunction)Crossfire_Object_MoveTo,       METH_VARARGS, NULL },
    { "ChangeAbil",     (PyCFunction)Crossfire_Object_ChangeAbil,   METH_O,       NULL },
    { "Event",          (PyCFunction)Crossfire_Object_Event,        METH_VARARGS, NULL },
    { "RemoveDepletion",(PyCFunction)Crossfire_Object_RemoveDepletion,    METH_VARARGS, NULL },
    { "Arrest",         (PyCFunction)Crossfire_Object_Arrest,    METH_VARARGS, NULL },
    { "PermExp",        (PyCFunction)Crossfire_Object_PermExp,	  METH_NOARGS, NULL },
    { NULL, NULL, 0, NULL }
};

static PyNumberMethods ObjectConvert = {
    NULL,            /* binaryfunc nb_add; */        /* __add__ */
    NULL,            /* binaryfunc nb_subtract; */   /* __sub__ */
    NULL,            /* binaryfunc nb_multiply; */   /* __mul__ */
    NULL,            /* binaryfunc nb_remainder; */  /* __mod__ */
    NULL,            /* binaryfunc nb_divmod; */     /* __divmod__ */
    NULL,            /* ternaryfunc nb_power; */     /* __pow__ */
    NULL,            /* unaryfunc nb_negative; */    /* __neg__ */
    NULL,            /* unaryfunc nb_positive; */    /* __pos__ */
    NULL,            /* unaryfunc nb_absolute; */    /* __abs__ */
    NULL,            /* inquiry nb_bool; */          /* __bool__ */
    NULL,            /* unaryfunc nb_invert; */      /* __invert__ */
    NULL,            /* binaryfunc nb_lshift; */     /* __lshift__ */
    NULL,            /* binaryfunc nb_rshift; */     /* __rshift__ */
    NULL,            /* binaryfunc nb_and; */        /* __and__ */
    NULL,            /* binaryfunc nb_xor; */        /* __xor__ */
    NULL,            /* binaryfunc nb_or; */         /* __or__ */
    /* This is not a typo. For Py3k it should be Crossfire_Object_Long
     * and NOT Crossfire_Object_Int.
     */
    Crossfire_Object_Long, /* unaryfunc nb_int; */    /* __int__ */
    NULL,                  /* void *nb_reserved; */
    NULL,            /* unaryfunc nb_float; */       /* __float__ */
    NULL,            /* binaryfunc nb_inplace_add; */
    NULL,            /* binaryfunc nb_inplace_subtract; */
    NULL,            /* binaryfunc nb_inplace_multiply; */
    NULL,            /* binaryfunc nb_inplace_remainder; */
    NULL,            /* ternaryfunc nb_inplace_power; */
    NULL,            /* binaryfunc nb_inplace_lshift; */
    NULL,            /* binaryfunc nb_inplace_rshift; */
    NULL,            /* binaryfunc nb_inplace_and; */
    NULL,            /* binaryfunc nb_inplace_xor; */
    NULL,            /* binaryfunc nb_inplace_or; */

    NULL,            /* binaryfunc nb_floor_divide; */
    NULL,            /* binaryfunc nb_true_divide; */
    NULL,            /* binaryfunc nb_inplace_floor_divide; */
    NULL,            /* binaryfunc nb_inplace_true_divide; */
    NULL             /* unaryfunc nb_index; */
};

static PyObject *Crossfire_Object_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
    Crossfire_Object *self;
    (void)args;
    (void)kwds;

    self = (Crossfire_Object *)type->tp_alloc(type, 0);
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

static PyObject *Crossfire_Player_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
    Crossfire_Player *self;
    (void)args;
    (void)kwds;

    self = (Crossfire_Player *)type->tp_alloc(type, 0);
    if (self) {
        self->obj = NULL;
        self->count = 0;
    }

    return (PyObject *)self;
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

static PyObject *Player_GetObjectProperty(Crossfire_Player *whoptr, void *closure) {
    EXISTCHECK(whoptr);
    object *ob = cf_object_get_object_property(whoptr->obj, (int)(intptr_t)closure);
    return Crossfire_Object_wrap(ob);
}

/* Our actual Python ObjectType */
CF_PYTHON_OBJECT(Object,
                 Crossfire_Object_dealloc,
                 &ObjectConvert,
                 PyObject_HashNotImplemented,
                 Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
                 "Crossfire objects",
                 (richcmpfunc) Crossfire_Object_RichCompare,
                 ObjectMethods,
                 Object_getseters,
                 NULL,
                 Crossfire_Object_new
                 );

static PyGetSetDef Player_getseters[] = {
    { "Title",         (getter)Player_GetTitle,         (setter)Player_SetTitle, NULL, NULL },
    { "IP",            (getter)Player_GetIP,            NULL, NULL, NULL },
    { "MarkedItem",    (getter)Player_GetMarkedItem,    (setter)Player_SetMarkedItem, NULL, NULL },
    { "Party",         (getter)Player_GetParty,         (setter)Player_SetParty,      NULL, NULL },
    { "BedMap",        (getter)Player_GetBedMap,        (setter)Player_SetBedMap, NULL, NULL },
    { "BedX",          (getter)Player_GetBedX,          (setter)Player_SetBedX, NULL, NULL },
    { "BedY",          (getter)Player_GetBedY,          (setter)Player_SetBedY, NULL, NULL },
    { "Transport",     (getter)Player_GetObjectProperty,NULL, NULL, (void*)CFAPI_PLAYER_PROP_TRANSPORT},
    { NULL, NULL, NULL, NULL, NULL }
};

static PyMethodDef PlayerMethods[] = {
    { "Message",        (PyCFunction)Crossfire_Player_Message, METH_VARARGS, NULL },
    { "Write",          (PyCFunction)Crossfire_Player_Message, METH_VARARGS, NULL },
    { "CanPay",         (PyCFunction)Crossfire_Player_CanPay,  METH_NOARGS,  NULL },
    { "QuestStart",     (PyCFunction)Player_QuestStart,        METH_VARARGS,  NULL },
    { "QuestGetState",  (PyCFunction)Player_QuestGetState,     METH_VARARGS,  NULL },
    { "QuestSetState",  (PyCFunction)Player_QuestSetState,     METH_VARARGS,  NULL },
    { "QuestWasCompleted",  (PyCFunction)Player_QuestWasCompleted, METH_VARARGS,  NULL },
    { "KnowledgeKnown",  (PyCFunction)Player_KnowledgeKnown, METH_VARARGS,  NULL },
    { "GiveKnowledge",  (PyCFunction)Player_GiveKnowledge, METH_VARARGS,  NULL },
    { NULL, NULL, 0, NULL }
};

/* Our actual Python ObjectPlayerType */
CF_PYTHON_OBJECT(Player,
                 Crossfire_Player_dealloc,
                 NULL,
                 NULL,
                 Py_TPFLAGS_DEFAULT,
                 "Crossfire player",
                 NULL,
                 PlayerMethods,
                 Player_getseters,
                 &Crossfire_ObjectType,
                 Crossfire_Player_new
                 );

/**
 * Python initialized.
 **/
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
