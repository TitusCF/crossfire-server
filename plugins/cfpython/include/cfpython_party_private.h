static PyObject *Crossfire_Party_GetName(Crossfire_Party *whoptr, void *closure);
static PyObject *Crossfire_Party_GetPassword(Crossfire_Party *whoptr, void *closure);
static PyObject *Crossfire_Party_GetNext(Crossfire_Party *who, void *closure);
static PyObject *Crossfire_Party_GetPlayers(Crossfire_Party *who, PyObject *args);

static int Crossfire_Party_InternalCompare(Crossfire_Party *left, Crossfire_Party *right);
static PyObject *Crossfire_Party_RichCompare(Crossfire_Party *left, Crossfire_Party *right, int op);

static PyGetSetDef Party_getseters[] = {
    { "Name",       (getter)Crossfire_Party_GetName,     NULL, NULL, NULL },
    { "Password",   (getter)Crossfire_Party_GetPassword, NULL, NULL, NULL },
    { "Next",       (getter)Crossfire_Party_GetNext,     NULL, NULL, NULL },
    { NULL, NULL, NULL, NULL, NULL }
};

static PyMethodDef PartyMethods[] = {
    { "GetPlayers", (PyCFunction)Crossfire_Party_GetPlayers, METH_NOARGS, NULL },
    { NULL, NULL, 0, NULL }
};

/* Our actual Python ArchetypeType */
PyTypeObject Crossfire_PartyType = {
#ifdef IS_PY3K
    /* See http://bugs.python.org/issue4385 */
    PyVarObject_HEAD_INIT(NULL, 0)
#else
    PyObject_HEAD_INIT(NULL)
    0,                         /* ob_size*/
#endif
    "Crossfire.Party",         /* tp_name*/
    sizeof(Crossfire_Party),   /* tp_basicsize*/
    0,                         /* tp_itemsize*/
    NULL,                      /* tp_dealloc*/
    NULL,                      /* tp_print*/
    NULL,                      /* tp_getattr*/
    NULL,                      /* tp_setattr*/
#ifdef IS_PY3K
    NULL,                      /* tp_reserved */
#else
    (cmpfunc)Crossfire_Party_InternalCompare, /* tp_compare*/
#endif
    NULL,                      /* tp_repr*/
    NULL,                      /* tp_as_number*/
    NULL,                      /* tp_as_sequence*/
    NULL,                      /* tp_as_mapping*/
    PyObject_HashNotImplemented, /* tp_hash */
    NULL,                      /* tp_call*/
    NULL,                      /* tp_str*/
    PyObject_GenericGetAttr,   /* tp_getattro*/
    PyObject_GenericSetAttr,   /* tp_setattro*/
    NULL,                      /* tp_as_buffer*/
    Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE, /* tp_flags*/
    "Crossfire parties",       /* tp_doc */
    NULL,                      /* tp_traverse */
    NULL,                      /* tp_clear */
    (richcmpfunc)Crossfire_Party_RichCompare, /* tp_richcompare */
    0,                         /* tp_weaklistoffset */
    NULL,                      /* tp_iter */
    NULL,                      /* tp_iternext */
    PartyMethods,              /* tp_methods */
    NULL,                      /* tp_members */
    Party_getseters,           /* tp_getset */
    NULL,                      /* tp_base */
    NULL,                      /* tp_dict */
    NULL,                      /* tp_descr_get */
    NULL,                      /* tp_descr_set */
    0,                         /* tp_dictoffset */
    NULL,                      /* tp_init */
    NULL,                      /* tp_alloc */
    NULL,                      /* tp_new */
    NULL,                      /* tp_free */
    NULL,                      /* tp_is_gc */
    NULL,                      /* tp_bases */
    NULL,                      /* tp_mro */
    NULL,                      /* tp_cache */
    NULL,                      /* tp_subclasses */
    NULL,                      /* tp_weaklist */
    NULL,                      /* tp_del */
};
