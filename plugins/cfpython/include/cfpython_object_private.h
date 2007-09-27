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
/*                                                                           */ /*****************************************************************************/
static PyObject* Object_GetName(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetNamePl(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetTitle(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetRace(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetMap(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetCha(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetCon(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetDex(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetInt(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetPow(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetStr(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetWis(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetHP(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetMaxHP(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetSP(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetMaxSP(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetGrace(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetMaxGrace(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetFood(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetAC(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetWC(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetDam(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetLuck(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetMessage(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetSkill(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetExp(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetPermExp(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetExpMul(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetSlaying(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetCursed(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetDamned(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetWeight(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetWeightLimit(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetAbove(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetBelow(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetInventory(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetX(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetY(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetDirection(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetFacing(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetUnaggressive(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetGod(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetPickable(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetQuantity(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetInvisible(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetSpeed(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetSpeedLeft(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetLastSP(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetLastGrace(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetLastEat(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetLevel(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetFace(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetAnim(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetAnimSpeed(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetAttackType(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetBeenApplied(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetIdentified(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetAlive(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetDM(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetWasDM(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetApplied(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetUnpaid(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetMonster(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetFriendly(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetGenerator(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetThrown(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetCanSeeInvisible(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetRollable(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetTurnable(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetUsedUp(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetSplitting(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetBlind(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetCanUseHorn(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetCanUseRod(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetCanUseSkill(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetKnownCursed(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetStealthy(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetConfused(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetSleeping(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetLifesaver(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetFloor(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetHasXRays(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetCanUseRing(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetCanUseBow(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetCanUseWand(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetCanSeeInDark(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetKnownMagical(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetCanUseWeapon(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetCanUseArmour(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetCanUseScroll(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetCanCastSpell(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetReflectSpells(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetReflectMissiles(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetUnique(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetRunAway(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetScared(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetUndead(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetBlocksView(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetHitBack(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetStandStill(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetOnlyAttack(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetMakeInvisible(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetMoney(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetType(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetSubtype(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetValue(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetArchName(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetArchetype(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetNoSave(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetExists(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetEnv(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetMoveType(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetMoveBlock(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetMoveAllow(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetMoveOn(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetMoveOff(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetMoveSlow(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetMoveSlowPenalty(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetOwner(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetEnemy(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetCount(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetGodGiven(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetIsPet(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetAttackMovement(Crossfire_Object* whoptr, void* closure);
static PyObject* Object_GetDuration(Crossfire_Object* whoptr, void* closure);

static int Object_SetMessage(Crossfire_Object* whoptr, PyObject* value, void* closure);
static int Object_SetExp(Crossfire_Object* whoptr, PyObject* value, void* closure);
static int Object_SetName(Crossfire_Object* whoptr, PyObject* value, void* closure);
static int Object_SetNamePl(Crossfire_Object* whoptr, PyObject* value, void* closure);
static int Object_SetTitle(Crossfire_Object* whoptr, PyObject* value, void* closure);
static int Object_SetRace(Crossfire_Object* whoptr, PyObject* value, void* closure);
static int Object_SetMap(Crossfire_Object* whoptr, PyObject* value, void* closure);
static int Object_SetSlaying(Crossfire_Object* whoptr, PyObject* value, void* closure);
static int Object_SetSkill(Crossfire_Object* whoptr, PyObject* value, void* closure);
static int Object_SetCursed(Crossfire_Object* whoptr, PyObject* value, void* closure);
static int Object_SetDamned(Crossfire_Object* whoptr, PyObject* value, void* closure);
static int Object_SetApplied(Crossfire_Object* whoptr, PyObject* value, void* closure);
static int Object_SetStr(Crossfire_Object* whoptr, PyObject* value, void* closure);
static int Object_SetDex(Crossfire_Object* whoptr, PyObject* value, void* closure);
static int Object_SetCon(Crossfire_Object* whoptr, PyObject* value, void* closure);
static int Object_SetInt(Crossfire_Object* whoptr, PyObject* value, void* closure);
static int Object_SetPow(Crossfire_Object* whoptr, PyObject* value, void* closure);
static int Object_SetWis(Crossfire_Object* whoptr, PyObject* value, void* closure);
static int Object_SetCha(Crossfire_Object* whoptr, PyObject* value, void* closure);
static int Object_SetHP(Crossfire_Object* whoptr, PyObject* value, void* closure);
static int Object_SetMaxHP(Crossfire_Object* whoptr, PyObject* value, void* closure);
static int Object_SetSP(Crossfire_Object* whoptr, PyObject* value, void* closure);
static int Object_SetMaxSP(Crossfire_Object* whoptr, PyObject* value, void* closure);
static int Object_SetGrace(Crossfire_Object* whoptr, PyObject* value, void* closure);
static int Object_SetMaxGrace(Crossfire_Object* whoptr, PyObject* value, void* closure);
static int Object_SetDam(Crossfire_Object* whoptr, PyObject* value, void* closure);
static int Object_SetFood(Crossfire_Object* whoptr, PyObject* value, void* closure);
static int Object_SetAC(Crossfire_Object* whoptr, PyObject* value, void* closure);
static int Object_SetWC(Crossfire_Object* whoptr, PyObject* value, void* closure);
static int Object_SetWeight(Crossfire_Object* whoptr, PyObject* value, void* closure);
static int Object_SetWeightLimit(Crossfire_Object* whoptr, PyObject* value, void* closure);
static int Object_SetDirection(Crossfire_Object* whoptr, PyObject* value, void* closure);
static int Object_SetFacing(Crossfire_Object* whoptr, PyObject* value, void* closure);
static int Object_SetGod(Crossfire_Object* whoptr, PyObject* value, void* closure);
static int Object_SetSpeed(Crossfire_Object* whoptr, PyObject* value, void* closure);
static int Object_SetSpeedLeft(Crossfire_Object* whoptr, PyObject* value, void* closure);
static int Object_SetQuantity(Crossfire_Object* whoptr, PyObject* value, void* closure);
static int Object_SetLastSP(Crossfire_Object* whoptr, PyObject* value, void* closure);
static int Object_SetLastGrace(Crossfire_Object* whoptr, PyObject* value, void* closure);
static int Object_SetLastEat(Crossfire_Object* whoptr, PyObject* value, void* closure);
static int Object_SetAttackType(Crossfire_Object* whoptr, PyObject* value, void* closure);
static int Object_SetUnaggressive(Crossfire_Object* whoptr, PyObject* value, void* closure);
static int Object_SetPickable(Crossfire_Object* whoptr, PyObject* value, void* closure);
static int Object_SetInvisible(Crossfire_Object* whoptr, PyObject* value, void* closure);
static int Object_SetUnpaid(Crossfire_Object* whoptr, PyObject* value, void* closure);
static int Object_SetFriendly(Crossfire_Object* whoptr, PyObject* value, void* closure);
static int Object_SetCanSeeInvisible(Crossfire_Object* whoptr, PyObject* value, void* closure);
static int Object_SetRollable(Crossfire_Object* whoptr, PyObject* value, void* closure);
static int Object_SetTurnable(Crossfire_Object* whoptr, PyObject* value, void* closure);
static int Object_SetUsedUp(Crossfire_Object* whoptr, PyObject* value, void* closure);
static int Object_SetBlind(Crossfire_Object* whoptr, PyObject* value, void* closure);
static int Object_SetKnownCursed(Crossfire_Object* whoptr, PyObject* value, void* closure);
static int Object_SetStealthy(Crossfire_Object* whoptr, PyObject* value, void* closure);
static int Object_SetConfused(Crossfire_Object* whoptr, PyObject* value, void* closure);
static int Object_SetSleeping(Crossfire_Object* whoptr, PyObject* value, void* closure);
static int Object_SetLifesaver(Crossfire_Object* whoptr, PyObject* value, void* closure);
static int Object_SetHasXRays(Crossfire_Object* whoptr, PyObject* value, void* closure);
static int Object_SetCanSeeInDark(Crossfire_Object* whoptr, PyObject* value, void* closure);
static int Object_SetKnownMagical(Crossfire_Object* whoptr, PyObject* value, void* closure);
static int Object_SetReflectSpells(Crossfire_Object* whoptr, PyObject* value, void* closure);
static int Object_SetReflectMissiles(Crossfire_Object* whoptr, PyObject* value, void* closure);
static int Object_SetUnique(Crossfire_Object* whoptr, PyObject* value, void* closure);
static int Object_SetRunAway(Crossfire_Object* whoptr, PyObject* value, void* closure);
static int Object_SetScared(Crossfire_Object* whoptr, PyObject* value, void* closure);
static int Object_SetUndead(Crossfire_Object* whoptr, PyObject* value, void* closure);
static int Object_SetBlocksView(Crossfire_Object* whoptr, PyObject* value, void* closure);
static int Object_SetHitBack(Crossfire_Object* whoptr, PyObject* value, void* closure);
static int Object_SetStandStill(Crossfire_Object* whoptr, PyObject* value, void* closure);
static int Object_SetOnlyAttack(Crossfire_Object* whoptr, PyObject* value, void* closure);
static int Object_SetMakeInvisible(Crossfire_Object* whoptr, PyObject* value, void* closure);
static int Object_SetValue(Crossfire_Object* whoptr, PyObject* value, void* closure);
static int Object_SetFace(Crossfire_Object* whoptr, PyObject* value, void* closure);
static int Object_SetAnim(Crossfire_Object* whoptr, PyObject* value, void* closure);
static int Object_SetAnimSpeed(Crossfire_Object* whoptr, PyObject* value, void* closure);
static int Object_SetNoSave(Crossfire_Object* whoptr, PyObject* value, void* closure);
static int Object_SetOwner(Crossfire_Object* whoptr, PyObject* value, void* closure);
static int Object_SetEnemy(Crossfire_Object* whoptr, PyObject* value, void* closure);
static int Object_SetGodGiven(Crossfire_Object* whoptr, PyObject* value, void* closure);
static int Object_SetIsPet(Crossfire_Object* whoptr, PyObject* value, void* closure);
static int Object_SetAttackMovement(Crossfire_Object* whoptr, PyObject* value, void* closure);
static int Object_SetDuration(Crossfire_Object* whoptr, PyObject* value, void* closure);

static PyObject* Crossfire_Object_Remove( Crossfire_Object* who, PyObject* args );
static PyObject* Crossfire_Object_Apply( Crossfire_Object* who, PyObject* args );
static PyObject* Crossfire_Object_Drop( Crossfire_Object* who, PyObject* args );
static PyObject* Crossfire_Object_Fix( Crossfire_Object* who, PyObject* args );
static PyObject* Crossfire_Object_Say( Crossfire_Object* who, PyObject* args );
static PyObject* Crossfire_Object_Pickup( Crossfire_Object* who, PyObject* args );
static PyObject* Crossfire_Object_Take( Crossfire_Object* who, PyObject* args );
static PyObject* Crossfire_Object_Teleport( Crossfire_Object* who, PyObject* args );
static PyObject* Crossfire_Object_Reposition( Crossfire_Object* who, PyObject* args );

static PyObject* Crossfire_Object_QueryName( Crossfire_Object* who, PyObject* args );
static PyObject* Crossfire_Object_GetResist( Crossfire_Object* who, PyObject* args );
static PyObject* Crossfire_Object_SetResist( Crossfire_Object* who, PyObject* args );
static PyObject* Crossfire_Object_ActivateRune( Crossfire_Object* who, PyObject* args );
static PyObject* Crossfire_Object_CheckTrigger( Crossfire_Object* who, PyObject* args );
static PyObject* Crossfire_Object_QueryCost( Crossfire_Object* who, PyObject* args );
static PyObject* Crossfire_Object_Cast( Crossfire_Object* who, PyObject* args );
static PyObject* Crossfire_Object_LearnSpell( Crossfire_Object* who, PyObject* args );
static PyObject* Crossfire_Object_ForgetSpell( Crossfire_Object* who, PyObject* args );
static PyObject* Crossfire_Object_KnowSpell( Crossfire_Object* who, PyObject* args );
static PyObject* Crossfire_Object_CastAbility( Crossfire_Object* who, PyObject* args );
static PyObject* Crossfire_Object_PayAmount( Crossfire_Object* who, PyObject* args );
static PyObject* Crossfire_Object_Pay( Crossfire_Object* who, PyObject* args );
static PyObject* Crossfire_Object_CheckInventory( Crossfire_Object* who, PyObject* args );
static PyObject* Crossfire_Object_CheckArchInventory( Crossfire_Object* who, PyObject* args );
static PyObject* Crossfire_Object_GetOutOfMap(Crossfire_Object* whoptr, PyObject* args);
static PyObject* Crossfire_Object_CreateInside(Crossfire_Object* who, PyObject* args);
static PyObject* Crossfire_Object_InsertInto(Crossfire_Object* who, PyObject* args);
static PyObject* Crossfire_Object_ReadKey(Crossfire_Object* who, PyObject* args);
static PyObject* Crossfire_Object_WriteKey(Crossfire_Object* who, PyObject* args);
static PyObject* Crossfire_Object_CreateTimer(Crossfire_Object* who, PyObject* args);
static PyObject* Crossfire_Object_AddExp(Crossfire_Object* who, PyObject* args);
static PyObject* Crossfire_Object_Move(Crossfire_Object* who, PyObject* args);
static PyObject* Crossfire_Object_ChangeAbil(Crossfire_Object* who, PyObject* args);

static int Crossfire_Object_InternalCompare(Crossfire_Object* left, Crossfire_Object* right);

static PyObject* Crossfire_Object_Long( PyObject* obj );
static PyObject* Crossfire_Object_Int( PyObject* obj );
static PyObject *Crossfire_Object_new(PyTypeObject *type, PyObject *args, PyObject *kwds);

/* Python binding */
static PyGetSetDef Object_getseters[] = {
    { "Name",       (getter)Object_GetName,     (setter)Object_SetName, NULL, NULL },
    { "NamePl",     (getter)Object_GetNamePl,   (setter)Object_SetNamePl, NULL, NULL },
    { "Title",      (getter)Object_GetTitle,    (setter)Object_SetTitle, NULL, NULL },
    { "Race",       (getter)Object_GetRace,     (setter)Object_SetRace, NULL, NULL },
    { "Skill",      (getter)Object_GetSkill,    (setter)Object_SetSkill, NULL, NULL },
    { "Map",        (getter)Object_GetMap,      (setter)Object_SetMap, NULL, NULL },
    { "Cha",        (getter)Object_GetCha,      (setter)Object_SetCha, NULL, NULL },
    { "Con",        (getter)Object_GetCon,      (setter)Object_SetCon, NULL, NULL },
    { "Dex",        (getter)Object_GetDex,      (setter)Object_SetDex, NULL, NULL },
    { "Int",        (getter)Object_GetInt,      (setter)Object_SetInt, NULL, NULL },
    { "Pow",        (getter)Object_GetPow,      (setter)Object_SetPow, NULL, NULL },
    { "Str",        (getter)Object_GetStr,      (setter)Object_SetStr, NULL, NULL },
    { "Wis",        (getter)Object_GetWis,      (setter)Object_SetWis, NULL, NULL },
    { "HP",         (getter)Object_GetHP,       (setter)Object_SetHP, NULL, NULL },
    { "MaxHP",      (getter)Object_GetMaxHP,    (setter)Object_SetMaxHP, NULL, NULL },
    { "SP",         (getter)Object_GetSP,       (setter)Object_SetSP, NULL, NULL },
    { "MaxSP",      (getter)Object_GetMaxSP,    (setter)Object_SetMaxSP, NULL, NULL },
    { "Grace",      (getter)Object_GetGrace,    (setter)Object_SetGrace, NULL, NULL },
    { "MaxGrace",   (getter)Object_GetMaxGrace, (setter)Object_SetMaxGrace, NULL, NULL },
    { "Food",       (getter)Object_GetFood,     (setter)Object_SetFood, NULL, NULL },
    { "AC",         (getter)Object_GetAC,       (setter)Object_SetAC, NULL, NULL },
    { "WC",         (getter)Object_GetWC,       (setter)Object_SetWC, NULL, NULL },
    { "Dam",        (getter)Object_GetDam,      (setter)Object_SetDam, NULL, NULL },
    { "Luck",       (getter)Object_GetLuck,     NULL, NULL, NULL },
    { "Exp",        (getter)Object_GetExp,      (setter)Object_SetExp, NULL, NULL },
    { "ExpMul",     (getter)Object_GetExpMul,   NULL, NULL, NULL },
    { "PermExp",    (getter)Object_GetPermExp,  NULL, NULL, NULL },
    { "Message",    (getter)Object_GetMessage,  (setter)Object_SetMessage, NULL, NULL },
    { "Slaying",    (getter)Object_GetSlaying,  (setter)Object_SetSlaying, NULL, NULL },
    { "Cursed",     (getter)Object_GetCursed,   (setter)Object_SetCursed, NULL, NULL },
    { "Damned",     (getter)Object_GetDamned,   (setter)Object_SetDamned, NULL, NULL },
    { "Weight",     (getter)Object_GetWeight,   (setter)Object_SetWeight, NULL, NULL },
    { "WeightLimit",(getter)Object_GetWeightLimit,(setter)Object_SetWeightLimit, NULL, NULL },
    { "Above",      (getter)Object_GetAbove,    NULL, NULL, NULL },
    { "Below",      (getter)Object_GetBelow,    NULL, NULL, NULL },
    { "Inventory",  (getter)Object_GetInventory,NULL, NULL, NULL },
    { "X",          (getter)Object_GetX,        NULL, NULL, NULL },
    { "Y",          (getter)Object_GetY,        NULL, NULL, NULL },
    { "Direction",  (getter)Object_GetDirection,(setter)Object_SetDirection, NULL, NULL },
    { "Facing",     (getter)Object_GetFacing,   (setter)Object_SetFacing, NULL, NULL },
    { "Unaggressive",(getter)Object_GetUnaggressive, (setter)Object_SetUnaggressive, NULL, NULL },
    { "God",        (getter)Object_GetGod,      (setter)Object_SetGod, NULL, NULL },
    { "Pickable",   (getter)Object_GetPickable, (setter)Object_SetPickable, NULL, NULL },
    { "Quantity",   (getter)Object_GetQuantity, (setter)Object_SetQuantity, NULL, NULL },
    { "Invisible",  (getter)Object_GetInvisible,(setter)Object_SetInvisible, NULL, NULL },
    { "Speed",      (getter)Object_GetSpeed,    (setter)Object_SetSpeed, NULL, NULL },
    { "SpeedLeft",  (getter)Object_GetSpeedLeft,(setter)Object_SetSpeedLeft, NULL, NULL },
    { "LastSP",     (getter)Object_GetLastSP,   (setter)Object_SetLastSP, NULL, NULL },
    { "LastGrace",  (getter)Object_GetLastGrace,(setter)Object_SetLastGrace, NULL, NULL },
    { "LastEat",    (getter)Object_GetLastEat,  (setter)Object_SetLastEat, NULL, NULL },
    { "Level",      (getter)Object_GetLevel,    NULL, NULL, NULL },
    { "Face",       (getter)Object_GetFace,     (setter)Object_SetFace, NULL, NULL },
    { "Anim",       (getter)Object_GetAnim,     (setter)Object_SetAnim, NULL, NULL },
    { "AnimSpeed",  (getter)Object_GetAnimSpeed,(setter)Object_SetAnimSpeed, NULL, NULL },
    { "AttackType", (getter)Object_GetAttackType, (setter)Object_SetAttackType, NULL, NULL },
    { "BeenApplied",(getter)Object_GetBeenApplied,NULL, NULL, NULL },
    { "Identified", (getter)Object_GetIdentified, NULL, NULL, NULL },
    { "Alive",      (getter)Object_GetAlive,    NULL, NULL, NULL },
    { "DungeonMaster",(getter)Object_GetDM,     NULL, NULL, NULL },
    { "WasDungeonMaster",(getter)Object_GetWasDM, NULL, NULL, NULL },
    { "Applied",    (getter)Object_GetApplied,  (setter)Object_SetApplied, NULL, NULL },
    { "Unpaid",     (getter)Object_GetUnpaid,   (setter)Object_SetUnpaid, NULL, NULL },
    { "Monster",    (getter)Object_GetMonster,  NULL, NULL, NULL },
    { "Friendly",   (getter)Object_GetFriendly, (setter)Object_SetFriendly, NULL, NULL },
    { "Generator",  (getter)Object_GetGenerator,NULL, NULL, NULL },
    { "Thrown",     (getter)Object_GetThrown,   NULL, NULL, NULL },
    { "CanSeeInvisible", (getter)Object_GetCanSeeInvisible, (setter)Object_SetCanSeeInvisible, NULL, NULL },
    { "Rollable",   (getter)Object_GetRollable, (setter)Object_SetRollable, NULL, NULL },
    { "Turnable",   (getter)Object_GetTurnable, (setter)Object_SetTurnable, NULL, NULL },
    { "UsedUp",     (getter)Object_GetUsedUp,   (setter)Object_SetUsedUp, NULL, NULL },
    { "Splitting",  (getter)Object_GetSplitting, NULL, NULL, NULL },
    { "Blind",          (getter)Object_GetBlind,        (setter)Object_SetBlind, NULL, NULL },
    { "CanUseHorn",     (getter)Object_GetCanUseHorn,   NULL, NULL, NULL },
    { "CanUseRod",      (getter)Object_GetCanUseRod,    NULL, NULL, NULL },
    { "CanUseSkill",    (getter)Object_GetCanUseSkill,  NULL, NULL, NULL },
    { "KnownCursed",    (getter)Object_GetKnownCursed,  (setter)Object_SetKnownCursed, NULL, NULL },
    { "Stealthy",       (getter)Object_GetStealthy,     (setter)Object_SetStealthy, NULL, NULL },
    { "Confused",       (getter)Object_GetConfused,     (setter)Object_SetConfused, NULL, NULL },
    { "Sleeping",       (getter)Object_GetSleeping,     (setter)Object_SetSleeping, NULL, NULL },
    { "Lifesaver",      (getter)Object_GetLifesaver,    (setter)Object_SetLifesaver, NULL, NULL },
    { "Floor",          (getter)Object_GetFloor,        NULL, NULL, NULL },
    { "HasXRays",       (getter)Object_GetHasXRays,     (setter)Object_SetHasXRays, NULL, NULL },
    { "CanUseRing",     (getter)Object_GetCanUseRing,   NULL, NULL, NULL },
    { "CanUseBow",      (getter)Object_GetCanUseBow,    NULL, NULL, NULL },
    { "CanUseWand",     (getter)Object_GetCanUseWand,   NULL, NULL, NULL },
    { "CanSeeInDark",   (getter)Object_GetCanSeeInDark, (setter)Object_SetCanSeeInDark, NULL, NULL },
    { "KnownMagical",   (getter)Object_GetKnownMagical, (setter)Object_SetKnownMagical, NULL, NULL },
    { "CanUseWeapon",   (getter)Object_GetCanUseWeapon, NULL, NULL, NULL },
    { "CanUseArmour",   (getter)Object_GetCanUseArmour, NULL, NULL, NULL },
    { "CanUseScroll",   (getter)Object_GetCanUseScroll, NULL, NULL, NULL },
    { "CanCastSpell",   (getter)Object_GetCanCastSpell, NULL, NULL, NULL },
    { "ReflectSpells",  (getter)Object_GetReflectSpells,(setter)Object_SetReflectSpells, NULL, NULL },
    { "ReflectMissiles",(getter)Object_GetReflectMissiles,(setter)Object_SetReflectMissiles, NULL, NULL },
    { "Unique",         (getter)Object_GetUnique,       (setter)Object_SetUnique, NULL, NULL },
    { "RunAway",        (getter)Object_GetRunAway,      (setter)Object_SetRunAway, NULL, NULL },
    { "Scared",         (getter)Object_GetScared,       (setter)Object_SetScared, NULL, NULL },
    { "Undead",         (getter)Object_GetUndead,       (setter)Object_SetUndead, NULL, NULL },
    { "BlocksView",     (getter)Object_GetBlocksView,   (setter)Object_SetBlocksView, NULL, NULL },
    { "HitBack",        (getter)Object_GetHitBack,      (setter)Object_SetHitBack, NULL, NULL },
    { "StandStill",     (getter)Object_GetStandStill,   (setter)Object_SetStandStill, NULL, NULL },
    { "OnlyAttack",     (getter)Object_GetOnlyAttack,   (setter)Object_SetOnlyAttack, NULL, NULL },
    { "MakeInvisible",  (getter)Object_GetMakeInvisible,(setter)Object_SetMakeInvisible,NULL, NULL },
    { "Money",          (getter)Object_GetMoney,        NULL ,NULL, NULL },
    { "Type",           (getter)Object_GetType,         NULL ,NULL, NULL },
    { "Subtype",        (getter)Object_GetSubtype,         NULL ,NULL, NULL },
    { "Value",          (getter)Object_GetValue,        (setter)Object_SetValue ,NULL, NULL },
    { "ArchName",       (getter)Object_GetArchName,     NULL ,NULL, NULL },
    { "Archetype",      (getter)Object_GetArchetype,    NULL ,NULL, NULL },
    { "Exists",         (getter)Object_GetExists,       NULL ,NULL, NULL },
    { "NoSave",         (getter)Object_GetNoSave,       (setter)Object_SetNoSave, NULL, NULL },
    { "Env",            (getter)Object_GetEnv,          NULL, NULL, NULL },
    { "MoveType",       (getter)Object_GetMoveType,     NULL, NULL, NULL },
    { "MoveBlock",      (getter)Object_GetMoveBlock,    NULL, NULL, NULL },
    { "MoveAllow",      (getter)Object_GetMoveAllow,    NULL, NULL, NULL },
    { "MoveOn",         (getter)Object_GetMoveOn,       NULL, NULL, NULL },
    { "MoveOff",        (getter)Object_GetMoveOff,      NULL, NULL, NULL },
    { "MoveSlow",       (getter)Object_GetMoveSlow,     NULL, NULL, NULL },
    { "MoveSlowPenalty",(getter)Object_GetMoveSlowPenalty,  NULL, NULL, NULL },
    { "Owner",          (getter)Object_GetOwner,        (setter)Object_SetOwner, NULL, NULL },
    { "Enemy",          (getter)Object_GetEnemy,        (setter)Object_SetEnemy, NULL, NULL },
    { "Count",          (getter)Object_GetCount,        NULL, NULL, NULL },
    { "GodGiven",       (getter)Object_GetGodGiven,     (setter)Object_SetGodGiven, NULL, NULL },
    { "IsPet",          (getter)Object_GetIsPet,     (setter)Object_SetIsPet, NULL, NULL },
    { "AttackMovement", (getter)Object_GetAttackMovement, (setter)Object_SetAttackMovement, NULL, NULL },
    { "Duration",       (getter)Object_GetDuration,     (setter)Object_SetDuration, NULL, NULL },
    { NULL, NULL, NULL, NULL, NULL }
};

static PyMethodDef ObjectMethods[] = {
    { "Remove",         (PyCFunction)Crossfire_Object_Remove,       METH_VARARGS},
    { "Apply",          (PyCFunction)Crossfire_Object_Apply,        METH_VARARGS},
    { "Drop",           (PyCFunction)Crossfire_Object_Drop,         METH_VARARGS},
    { "Fix",            (PyCFunction)Crossfire_Object_Fix,          METH_VARARGS},
    { "Say",            (PyCFunction)Crossfire_Object_Say,          METH_VARARGS},
    { "Speak",          (PyCFunction)Crossfire_Object_Say,          METH_VARARGS},
    { "Take",           (PyCFunction)Crossfire_Object_Take,         METH_VARARGS},
    { "Teleport",       (PyCFunction)Crossfire_Object_Teleport,     METH_VARARGS},
    { "Reposition",     (PyCFunction)Crossfire_Object_Reposition,   METH_VARARGS},
    { "QueryName",      (PyCFunction)Crossfire_Object_QueryName,    METH_VARARGS},
    { "GetResist",      (PyCFunction)Crossfire_Object_GetResist,    METH_VARARGS},
    { "SetResist",      (PyCFunction)Crossfire_Object_SetResist,    METH_VARARGS},
    { "ActivateRune",   (PyCFunction)Crossfire_Object_ActivateRune, METH_VARARGS},
    { "CheckTrigger",   (PyCFunction)Crossfire_Object_CheckTrigger, METH_VARARGS},
    { "QueryCost",      (PyCFunction)Crossfire_Object_QueryCost,    METH_VARARGS},
    { "Cast",           (PyCFunction)Crossfire_Object_Cast,         METH_VARARGS},
    { "LearnSpell",     (PyCFunction)Crossfire_Object_LearnSpell,   METH_VARARGS},
    { "ForgetSpell",    (PyCFunction)Crossfire_Object_ForgetSpell,  METH_VARARGS},
    { "KnowSpell",      (PyCFunction)Crossfire_Object_KnowSpell,    METH_VARARGS},
    { "CastAbility",    (PyCFunction)Crossfire_Object_CastAbility,  METH_VARARGS},
    { "PayAmount",      (PyCFunction)Crossfire_Object_PayAmount,    METH_VARARGS},
    { "Pay",            (PyCFunction)Crossfire_Object_Pay,          METH_VARARGS},
    { "CheckInventory", (PyCFunction)Crossfire_Object_CheckInventory,METH_VARARGS},
    { "CheckArchInventory", (PyCFunction)Crossfire_Object_CheckArchInventory,METH_VARARGS},
    { "OutOfMap",       (PyCFunction)Crossfire_Object_GetOutOfMap,  METH_VARARGS},
    { "CreateObject",   (PyCFunction)Crossfire_Object_CreateInside, METH_VARARGS},
    { "InsertInto",     (PyCFunction)Crossfire_Object_InsertInto,   METH_VARARGS},
    { "ReadKey",        (PyCFunction)Crossfire_Object_ReadKey,      METH_VARARGS},
    { "WriteKey",       (PyCFunction)Crossfire_Object_WriteKey,     METH_VARARGS},
    { "CreateTimer",    (PyCFunction)Crossfire_Object_CreateTimer,  METH_VARARGS},
    { "AddExp",         (PyCFunction)Crossfire_Object_AddExp,       METH_VARARGS},
    { "Move",           (PyCFunction)Crossfire_Object_Move,         METH_VARARGS},
    { "ChangeAbil",     (PyCFunction)Crossfire_Object_ChangeAbil,   METH_VARARGS},
    {NULL, NULL, 0}
};

static PyNumberMethods ObjectConvert[ ] = {
        0,               /* binaryfunc nb_add; */        /* __add__ */
        0,               /* binaryfunc nb_subtract; */   /* __sub__ */
        0,               /* binaryfunc nb_multiply; */   /* __mul__ */
        0,               /* binaryfunc nb_divide; */     /* __div__ */
        0,               /* binaryfunc nb_remainder; */  /* __mod__ */
        0,               /* binaryfunc nb_divmod; */     /* __divmod__ */
        0,               /* ternaryfunc nb_power; */     /* __pow__ */
        0,               /* unaryfunc nb_negative; */    /* __neg__ */
        0,               /* unaryfunc nb_positive; */    /* __pos__ */
        0,               /* unaryfunc nb_absolute; */    /* __abs__ */
        0,               /* inquiry nb_nonzero; */       /* __nonzero__ */
        0,               /* unaryfunc nb_invert; */      /* __invert__ */
        0,               /* binaryfunc nb_lshift; */     /* __lshift__ */
        0,               /* binaryfunc nb_rshift; */     /* __rshift__ */
        0,               /* binaryfunc nb_and; */        /* __and__ */
        0,               /* binaryfunc nb_xor; */        /* __xor__ */
        0,               /* binaryfunc nb_or; */         /* __or__ */
        0,               /* coercion nb_coerce; */       /* __coerce__ */
        Crossfire_Object_Int, /* unaryfunc nb_int; */    /* __int__ */
        Crossfire_Object_Long, /* unaryfunc nb_long; */  /* __long__ */
        0
};

static void Crossfire_Object_dealloc(PyObject *obj);
static PyObject *Crossfire_Object_new(PyTypeObject *type, PyObject *args, PyObject *kwds);

/* Our actual Python ObjectType */
PyTypeObject Crossfire_ObjectType = {
            PyObject_HEAD_INIT(NULL)
                    0,                         /* ob_size*/
            "Crossfire.Object",        /* tp_name*/
            sizeof(Crossfire_Object),  /* tp_basicsize*/
            0,                         /* tp_itemsize*/
            Crossfire_Object_dealloc,  /* tp_dealloc*/
            0,                         /* tp_print*/
            0,                         /* tp_getattr*/
            0,                         /* tp_setattr*/
            (cmpfunc)Crossfire_Object_InternalCompare,                         /* tp_compare*/
            0,                         /* tp_repr*/
            ObjectConvert,             /* tp_as_number*/
            0,                         /* tp_as_sequence*/
            0,                         /* tp_as_mapping*/
            0,                         /* tp_hash */
            0,                         /* tp_call*/
            0,                         /* tp_str*/
            PyObject_GenericGetAttr,   /* tp_getattro*/
            PyObject_GenericSetAttr,   /* tp_setattro*/
            0,                         /* tp_as_buffer*/
            Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,        /* tp_flags*/
            "Crossfire objects",       /* tp_doc */
            0,                         /* tp_traverse */
            0,                         /* tp_clear */
            0,                         /* tp_richcompare */
            0,                         /* tp_weaklistoffset */
            0,                         /* tp_iter */
            0,                         /* tp_iternext */
            ObjectMethods,             /* tp_methods */
            0,                         /* tp_members */
            Object_getseters,          /* tp_getset */
            0,                         /* tp_base */
            0,                         /* tp_dict */
            0,                         /* tp_descr_get */
            0,                         /* tp_descr_set */
            0,                         /* tp_dictoffset */
            0,                         /* tp_init */
            0,                         /* tp_alloc */
            Crossfire_Object_new,      /* tp_new */
};





static PyObject* Player_GetIP(Crossfire_Player* whoptr, void* closure);
static PyObject* Player_GetMarkedItem(Crossfire_Player* whoptr, void* closure);
static int Player_SetMarkedItem(Crossfire_Player* whoptr, PyObject* value, void* closure);
static PyObject* Crossfire_Player_Message( Crossfire_Player* who, PyObject* args );
static PyObject* Player_GetParty(Crossfire_Player* whoptr, void* closure);
static int Player_SetParty(Crossfire_Player* whoptr, PyObject* value, void* closure);
static PyObject* Crossfire_Player_CanPay( Crossfire_Player* who, PyObject* args );
static PyObject* Player_GetBedMap(Crossfire_Player* whoptr, void* closure);
static PyObject* Player_GetBedX(Crossfire_Player* whoptr, void* closure);
static PyObject* Player_GetBedY(Crossfire_Player* whoptr, void* closure);
static int Player_SetBedMap(Crossfire_Player* whoptr, PyObject* value, void* closure);
static int Player_SetBedX(Crossfire_Player* whoptr, PyObject* value, void* closure);
static int Player_SetBedY(Crossfire_Player* whoptr, PyObject* value, void* closure);

static PyGetSetDef Player_getseters[] = {
    { "IP",            (getter)Player_GetIP,            NULL, NULL, NULL },
	{ "MarkedItem",    (getter)Player_GetMarkedItem,    (setter)Player_SetMarkedItem, NULL, NULL },
	{ "Party",         (getter)Player_GetParty,         (setter)Player_SetParty,      NULL, NULL },
    { "BedMap",        (getter)Player_GetBedMap,        (setter)Player_SetBedMap, NULL, NULL },
    { "BedX",          (getter)Player_GetBedX,          (setter)Player_SetBedX, NULL, NULL },
    { "BedY",          (getter)Player_GetBedY,          (setter)Player_SetBedY, NULL, NULL },
    { NULL, NULL, NULL, NULL, NULL }
};

static PyMethodDef PlayerMethods[] = {
    { "Message",          (PyCFunction)Crossfire_Player_Message,        METH_VARARGS},
    { "Write",          (PyCFunction)Crossfire_Player_Message, METH_VARARGS},
    { "CanPay",         (PyCFunction)Crossfire_Player_CanPay, METH_VARARGS},
    {NULL, NULL }
};

static void Crossfire_Player_dealloc(PyObject *obj);
static PyObject *Crossfire_Player_new(PyTypeObject *type, PyObject *args, PyObject *kwds);

/* Our actual Python ObjectPlayerType */
PyTypeObject Crossfire_PlayerType = {
            PyObject_HEAD_INIT(NULL)
                    0,                         /* ob_size*/
            "Crossfire.Player",        /* tp_name*/
            sizeof(Crossfire_Player),  /* tp_basicsize*/
            0,                         /* tp_itemsize*/
            Crossfire_Player_dealloc,  /* tp_dealloc*/
            0,                         /* tp_print*/
            0,                         /* tp_getattr*/
            0,                         /* tp_setattr*/
            0,                         /* tp_compare*/
            0,                         /* tp_repr*/
            0,                         /* tp_as_number*/
            0,                         /* tp_as_sequence*/
            0,                         /* tp_as_mapping*/
            0,                         /* tp_hash */
            0,                         /* tp_call*/
            0,                         /* tp_str*/
            PyObject_GenericGetAttr,   /* tp_getattro*/
            PyObject_GenericSetAttr,   /* tp_setattro*/
            0,                         /* tp_as_buffer*/
            Py_TPFLAGS_DEFAULT,        /* tp_flags*/
            "Crossfire player",        /* tp_doc */
            0,                         /* tp_traverse */
            0,                         /* tp_clear */
            0,                         /* tp_richcompare */
            0,                         /* tp_weaklistoffset */
            0,                         /* tp_iter */
            0,                         /* tp_iternext */
            PlayerMethods,             /* tp_methods */
            0,                         /* tp_members */
            Player_getseters,          /* tp_getset */
            &Crossfire_ObjectType,     /* tp_base */
            0,                         /* tp_dict */
            0,                         /* tp_descr_get */
            0,                         /* tp_descr_set */
            0,                         /* tp_dictoffset */
            0,                         /* tp_init */
            0,                         /* tp_alloc */
            Crossfire_Player_new,      /* tp_new */
};
