static PyObject *Crossfire_Region_GetName(Crossfire_Region *whoptr, void *closure);
static PyObject *Crossfire_Region_GetLongname(Crossfire_Region *whoptr, void *closure);
static PyObject *Crossfire_Region_GetMessage(Crossfire_Region *whoptr, void *closure);
static PyObject *Crossfire_Region_GetNext(Crossfire_Region *who, void *closure);
static PyObject *Crossfire_Region_GetParent(Crossfire_Region *who, PyObject *args);
static PyObject *Crossfire_Region_GetJailX(Crossfire_Region *who, void *closure);
static PyObject *Crossfire_Region_GetJailY(Crossfire_Region *who, void *closure);
static PyObject *Crossfire_Region_GetJailPath(Crossfire_Region *who, void *closure);

static int Crossfire_Region_InternalCompare(Crossfire_Region *left, Crossfire_Region *right);
static PyObject *Crossfire_Region_RichCompare(Crossfire_Region *left, Crossfire_Region *right, int op);

static PyGetSetDef Region_getseters[] = {
    { "Name",       (getter)Crossfire_Region_GetName,     NULL, NULL, NULL },
    { "Longname",   (getter)Crossfire_Region_GetLongname, NULL, NULL, NULL },
    { "Message",    (getter)Crossfire_Region_GetMessage,  NULL, NULL, NULL },
    { "Next",       (getter)Crossfire_Region_GetNext,     NULL, NULL, NULL },
    { "JailX",      (getter)Crossfire_Region_GetJailX,    NULL, NULL, NULL },
    { "JailY",      (getter)Crossfire_Region_GetJailY,    NULL, NULL, NULL },
    { "JailPath",   (getter)Crossfire_Region_GetJailPath, NULL, NULL, NULL },
    { NULL, NULL, NULL, NULL, NULL }
};

static PyMethodDef RegionMethods[] = {
    { "GetParent", (PyCFunction)Crossfire_Region_GetParent, METH_NOARGS, NULL },
    { NULL, NULL, 0, NULL }
};

/* Our actual Python ArchetypeType */
PyTypeObject Crossfire_RegionType = {
#ifdef IS_PY3K
    /* See http://bugs.python.org/issue4385 */
    PyVarObject_HEAD_INIT(NULL, 0)
#else
    PyObject_HEAD_INIT(NULL)
    0,                         /* ob_size*/
#endif
    "Crossfire.Region",        /* tp_name*/
    sizeof(Crossfire_Region),  /* tp_basicsize*/
    0,                         /* tp_itemsize*/
    NULL,                      /* tp_dealloc*/
    NULL,                      /* tp_print*/
    NULL,                      /* tp_getattr*/
    NULL,                      /* tp_setattr*/
#ifdef IS_PY3K
    NULL,                      /* tp_reserved */
#else
    (cmpfunc)Crossfire_Region_InternalCompare, /* tp_compare*/
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
    Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE,        /* tp_flags*/
    "Crossfire regions",       /* tp_doc */
    NULL,                      /* tp_traverse */
    NULL,                      /* tp_clear */
    (richcmpfunc)Crossfire_Region_RichCompare, /* tp_richcompare */
    0,                         /* tp_weaklistoffset */
    NULL,                      /* tp_iter */
    NULL,                      /* tp_iternext */
    RegionMethods,             /* tp_methods */
    NULL,                      /* tp_members */
    Region_getseters,          /* tp_getset */
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
