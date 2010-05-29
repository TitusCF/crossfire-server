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
static PyObject *Map_GetDifficulty(Crossfire_Map *whoptr, void *closure);
static PyObject *Map_GetPath(Crossfire_Map *whoptr, void *closure);
static PyObject *Map_GetTempName(Crossfire_Map *whoptr, void *closure);
static PyObject *Map_GetName(Crossfire_Map *whoptr, void *closure);
static PyObject *Map_GetResetTime(Crossfire_Map *whoptr, void *closure);
static PyObject *Map_GetResetTimeout(Crossfire_Map *whoptr, void *closure);
static PyObject *Map_GetPlayers(Crossfire_Map *whoptr, void *closure);
static PyObject *Map_GetDarkness(Crossfire_Map *whoptr, void *closure);
static PyObject *Map_GetWidth(Crossfire_Map *whoptr, void *closure);
static PyObject *Map_GetHeight(Crossfire_Map *whoptr, void *closure);
static PyObject *Map_GetEnterX(Crossfire_Map *whoptr, void *closure);
static PyObject *Map_GetEnterY(Crossfire_Map *whoptr, void *closure);
static PyObject *Map_GetMessage(Crossfire_Map *whoptr, void *closure);
static PyObject *Map_GetRegion(Crossfire_Map *whoptr, void *closure);
static PyObject *Map_GetUnique(Crossfire_Map *whoptr, void *closure);

static int Map_SetPath(Crossfire_Map *whoptr, PyObject *value, void *closure);

static PyObject *Map_Message(Crossfire_Map *map, PyObject *args);
static PyObject *Map_GetFirstObjectAt(Crossfire_Map *map, PyObject *args);
static PyObject *Map_CreateObject(Crossfire_Map *map, PyObject *args);
static PyObject *Map_Check(Crossfire_Map *map, PyObject *args);
static PyObject *Map_Next(Crossfire_Map *map, PyObject *args);
static PyObject *Map_Insert(Crossfire_Map *map, PyObject *args);
static PyObject *Map_InsertAround(Crossfire_Map *map, PyObject *args);
static PyObject *Map_ChangeLight(Crossfire_Map *map, PyObject *args);
static PyObject *Map_TriggerConnected(Crossfire_Map *map, PyObject *args);

static int Map_InternalCompare(Crossfire_Map *left, Crossfire_Map *right);
static PyObject *Crossfire_Map_RichCompare(Crossfire_Map *left, Crossfire_Map *right, int op);

static PyObject *Crossfire_Map_Long(PyObject *obj);
#ifndef IS_PY3K
static PyObject *Crossfire_Map_Int(PyObject *obj);
#endif
static void Crossfire_Map_dealloc(PyObject *obj);
static PyObject *Crossfire_Map_new(PyTypeObject *type, PyObject *args, PyObject *kwds);

/* Python binding */
static PyGetSetDef Map_getseters[] = {
    { "Difficulty",      (getter)Map_GetDifficulty,  NULL, NULL, NULL },
    { "Path",            (getter)Map_GetPath,        (setter)Map_SetPath, NULL, NULL },
    { "TempName",        (getter)Map_GetTempName,    NULL, NULL, NULL },
    { "Name",            (getter)Map_GetName,        NULL, NULL, NULL },
    { "ResetTime",       (getter)Map_GetResetTime,   NULL, NULL, NULL },
    { "ResetTimeout",    (getter)Map_GetResetTimeout, NULL, NULL, NULL },
    { "Players",         (getter)Map_GetPlayers,     NULL, NULL, NULL },
    { "Light",           (getter)Map_GetDarkness,    NULL, NULL, NULL },
    { "Darkness",        (getter)Map_GetDarkness,    NULL, NULL, NULL },
    { "Width",           (getter)Map_GetWidth,       NULL, NULL, NULL },
    { "Height",          (getter)Map_GetHeight,      NULL, NULL, NULL },
    { "EnterX",          (getter)Map_GetEnterX,      NULL, NULL, NULL },
    { "EnterY",          (getter)Map_GetEnterY,      NULL, NULL, NULL },
    { "Message",         (getter)Map_GetMessage,     NULL, NULL, NULL },
    { "Region",          (getter)Map_GetRegion,      NULL, NULL, NULL },
    { "Unique",          (getter)Map_GetUnique,      NULL, NULL, NULL },
    {  NULL, NULL, NULL, NULL, NULL }
};

static PyMethodDef MapMethods[] = {
    { "Print",            (PyCFunction)Map_Message,          METH_VARARGS, NULL },
    { "ObjectAt",         (PyCFunction)Map_GetFirstObjectAt, METH_VARARGS, NULL },
    { "CreateObject",     (PyCFunction)Map_CreateObject,     METH_VARARGS, NULL },
    { "Check",            (PyCFunction)Map_Check,            METH_VARARGS, NULL },
    { "Next",             (PyCFunction)Map_Next,             METH_NOARGS,  NULL },
    { "Insert",           (PyCFunction)Map_Insert,           METH_VARARGS, NULL },
    { "InsertAround",     (PyCFunction)Map_InsertAround,     METH_VARARGS, NULL },
    { "ChangeLight",      (PyCFunction)Map_ChangeLight,      METH_VARARGS, NULL },
    { "TriggerConnected", (PyCFunction)Map_TriggerConnected, METH_VARARGS, NULL },
    { NULL, NULL, 0, NULL }
};

static PyNumberMethods MapConvert = {
    NULL,            /* binaryfunc nb_add; */        /* __add__ */
    NULL,            /* binaryfunc nb_subtract; */   /* __sub__ */
    NULL,            /* binaryfunc nb_multiply; */   /* __mul__ */
#ifndef IS_PY3K
    NULL,            /* binaryfunc nb_divide; */     /* __div__ */
#endif
    NULL,            /* binaryfunc nb_remainder; */  /* __mod__ */
    NULL,            /* binaryfunc nb_divmod; */     /* __divmod__ */
    NULL,            /* ternaryfunc nb_power; */     /* __pow__ */
    NULL,            /* unaryfunc nb_negative; */    /* __neg__ */
    NULL,            /* unaryfunc nb_positive; */    /* __pos__ */
    NULL,            /* unaryfunc nb_absolute; */    /* __abs__ */
#ifdef IS_PY3K
    NULL,            /* inquiry nb_bool; */          /* __bool__ */
#else
    NULL,            /* inquiry nb_nonzero; */       /* __nonzero__ */
#endif
    NULL,            /* unaryfunc nb_invert; */      /* __invert__ */
    NULL,            /* binaryfunc nb_lshift; */     /* __lshift__ */
    NULL,            /* binaryfunc nb_rshift; */     /* __rshift__ */
    NULL,            /* binaryfunc nb_and; */        /* __and__ */
    NULL,            /* binaryfunc nb_xor; */        /* __xor__ */
    NULL,            /* binaryfunc nb_or; */         /* __or__ */
#ifndef IS_PY3K
    NULL,            /* coercion nb_coerce; */       /* __coerce__ */
#endif
#ifdef IS_PY3K
    /* This is not a typo. For Py3k it should be Crossfire_Map_Long
     * and NOT Crossfire_Map_Int.
     */
    Crossfire_Map_Long, /* unaryfunc nb_int; */      /* __int__ */
    NULL,               /* void *nb_reserved; */
#else
    Crossfire_Map_Int,  /* unaryfunc nb_int; */      /* __int__ */
    Crossfire_Map_Long, /* unaryfunc nb_long; */     /* __long__ */
#endif
    NULL,            /* unaryfunc nb_float; */       /* __float__ */
#ifndef IS_PY3K
    NULL,            /* unaryfunc nb_oct; */         /* __oct__ */
    NULL,            /* unaryfunc nb_hex; */         /* __hex__ */
#endif
    NULL,            /* binaryfunc nb_inplace_add; */
    NULL,            /* binaryfunc nb_inplace_subtract; */
    NULL,            /* binaryfunc nb_inplace_multiply; */
#ifndef IS_PY3K
    NULL,            /* binaryfunc nb_inplace_divide; */
#endif
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
#if defined(IS_PY25) || defined(IS_PY3K)
    NULL             /* unaryfunc nb_index; */
#endif
};

/* Our actual Python MapType */
PyTypeObject Crossfire_MapType = {
#ifdef IS_PY3K
    /* See http://bugs.python.org/issue4385 */
    PyVarObject_HEAD_INIT(NULL, 0)
#else
    PyObject_HEAD_INIT(NULL)
    0,                         /* ob_size*/
#endif
    "Crossfire.Map",           /* tp_name*/
    sizeof(Crossfire_Map),     /* tp_basicsize*/
    0,                         /* tp_itemsize*/
    Crossfire_Map_dealloc,     /* tp_dealloc*/
    NULL,                      /* tp_print*/
    NULL,                      /* tp_getattr*/
    NULL,                      /* tp_setattr*/
#ifdef IS_PY3K
    NULL,                      /* tp_reserved */
#else
    (cmpfunc)Map_InternalCompare, /* tp_compare*/
#endif
    NULL,                      /* tp_repr*/
    &MapConvert,               /* tp_as_number*/
    NULL,                      /* tp_as_sequence*/
    NULL,                      /* tp_as_mapping*/
    PyObject_HashNotImplemented, /* tp_hash */
    NULL,                      /* tp_call*/
    NULL,                      /* tp_str*/
    PyObject_GenericGetAttr,   /* tp_getattro*/
    PyObject_GenericSetAttr,   /* tp_setattro*/
    NULL,                      /* tp_as_buffer*/
    Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE, /* tp_flags*/
    "Crossfire maps",          /* tp_doc */
    NULL,                      /* tp_traverse */
    NULL,                      /* tp_clear */
    (richcmpfunc)Crossfire_Map_RichCompare, /* tp_richcompare */
    0,                         /* tp_weaklistoffset */
    NULL,                      /* tp_iter */
    NULL,                      /* tp_iternext */
    MapMethods,                /* tp_methods */
    NULL,                      /* tp_members */
    Map_getseters,             /* tp_getset */
    NULL,                      /* tp_base */
    NULL,                      /* tp_dict */
    NULL,                      /* tp_descr_get */
    NULL,                      /* tp_descr_set */
    0,                         /* tp_dictoffset */
    NULL,                      /* tp_init */
    NULL,                      /* tp_alloc */
    Crossfire_Map_new,         /* tp_new */
    NULL,                      /* tp_free */
    NULL,                      /* tp_is_gc */
    NULL,                      /* tp_bases */
    NULL,                      /* tp_mro */
    NULL,                      /* tp_cache */
    NULL,                      /* tp_subclasses */
    NULL,                      /* tp_weaklist */
    NULL,                      /* tp_del */
};
