static PyObject *Crossfire_Archetype_GetName(Crossfire_Archetype *whoptr, void *closure);
static PyObject *Crossfire_Archetype_GetNext(Crossfire_Archetype *who, void *closure);
static PyObject *Crossfire_Archetype_GetMore(Crossfire_Archetype *who, void *closure);
static PyObject *Crossfire_Archetype_GetHead(Crossfire_Archetype *who, void *closure);
static PyObject *Crossfire_Archetype_GetClone(Crossfire_Archetype *who, void *closure);
static PyObject *Crossfire_Archetype_GetNewObject(Crossfire_Archetype *who, PyObject *args);
static int Crossfire_Archetype_InternalCompare(Crossfire_Archetype *left, Crossfire_Archetype *right);
static PyObject *Crossfire_Archetype_RichCompare(Crossfire_Archetype *left, Crossfire_Archetype *right, int op);

static PyGetSetDef Archetype_getseters[] = {
    { "Name",       (getter)Crossfire_Archetype_GetName,     NULL, NULL, NULL },
    { "Next",       (getter)Crossfire_Archetype_GetNext,     NULL, NULL, NULL },
    { "More",       (getter)Crossfire_Archetype_GetMore,     NULL, NULL, NULL },
    { "Head",       (getter)Crossfire_Archetype_GetHead,     NULL, NULL, NULL },
    { "Clone",      (getter)Crossfire_Archetype_GetClone,    NULL, NULL, NULL },
    { NULL, NULL, NULL, NULL, NULL }
};

static PyMethodDef ArchetypeMethods[] = {
    { "NewObject", (PyCFunction)Crossfire_Archetype_GetNewObject, METH_NOARGS, NULL },
    { NULL, NULL, 0, NULL }
};

/* Our actual Python ArchetypeType */
PyTypeObject Crossfire_ArchetypeType = {
#ifdef IS_PY3K
    /* See http://bugs.python.org/issue4385 */
    PyVarObject_HEAD_INIT(NULL, 0)
#else
    PyObject_HEAD_INIT(NULL)
    0,                         /* ob_size*/
#endif
    "Crossfire.Archetype",     /* tp_name*/
    sizeof(Crossfire_Archetype), /* tp_basicsize*/
    0,                         /* tp_itemsize*/
    NULL,                      /* tp_dealloc*/
    NULL,                      /* tp_print*/
    NULL,                      /* tp_getattr*/
    NULL,                      /* tp_setattr*/
#ifdef IS_PY3K
    NULL,                      /* tp_reserved */
#else
    (cmpfunc)Crossfire_Archetype_InternalCompare, /* tp_compare*/
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
    "Crossfire archetypes",    /* tp_doc */
    NULL,                      /* tp_traverse */
    NULL,                      /* tp_clear */
    (richcmpfunc)Crossfire_Archetype_RichCompare, /* tp_richcompare */
    0,                         /* tp_weaklistoffset */
    NULL,                      /* tp_iter */
    NULL,                      /* tp_iternext */
    ArchetypeMethods,          /* tp_methods */
    NULL,                      /* tp_members */
    Archetype_getseters,       /* tp_getset */
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
