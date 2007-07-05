static PyObject* Crossfire_Archetype_GetName( Crossfire_Archetype* whoptr, void* closure);
static PyObject* Crossfire_Archetype_GetNext( Crossfire_Archetype* who, void* closure );
static PyObject* Crossfire_Archetype_GetMore( Crossfire_Archetype* who, void* closure );
static PyObject* Crossfire_Archetype_GetHead( Crossfire_Archetype* who, void* closure );
static PyObject* Crossfire_Archetype_GetClone( Crossfire_Archetype* who, void* closure );

static PyObject* Crossfire_Archetype_GetNewObject( Crossfire_Archetype* who, PyObject* args );

static int Crossfire_Archetype_InternalCompare(Crossfire_Archetype* left, Crossfire_Archetype* right);

static PyGetSetDef Archetype_getseters[] = {
    { "Name",       (getter)Crossfire_Archetype_GetName,     NULL, NULL, NULL },
    { "Next",       (getter)Crossfire_Archetype_GetNext,     NULL, NULL, NULL },
    { "More",       (getter)Crossfire_Archetype_GetMore,     NULL, NULL, NULL },
    { "Head",       (getter)Crossfire_Archetype_GetHead,     NULL, NULL, NULL },
    { "Clone",      (getter)Crossfire_Archetype_GetClone,    NULL, NULL, NULL },
    { NULL, NULL, NULL, NULL, NULL }
};

static PyMethodDef ArchetypeMethods[] = {
    { "NewObject",         (PyCFunction)Crossfire_Archetype_GetNewObject,       METH_VARARGS},
    {NULL, NULL, 0}
};

/* Our actual Python ArchetypeType */
PyTypeObject Crossfire_ArchetypeType = {
            PyObject_HEAD_INIT(NULL)
                    0,                         /* ob_size*/
            "Crossfire.Archetype",        /* tp_name*/
            sizeof(Crossfire_Archetype),  /* tp_basicsize*/
            0,                         /* tp_itemsize*/
            0,                         /* tp_dealloc*/
            0,                         /* tp_print*/
            0,                         /* tp_getattr*/
            0,                         /* tp_setattr*/
            (cmpfunc)Crossfire_Archetype_InternalCompare,                         /* tp_compare*/
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
            Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,        /* tp_flags*/
            "Crossfire archetypes",       /* tp_doc */
            0,                         /* tp_traverse */
            0,                         /* tp_clear */
            0,                         /* tp_richcompare */
            0,                         /* tp_weaklistoffset */
            0,                         /* tp_iter */
            0,                         /* tp_iternext */
            ArchetypeMethods,          /* tp_methods */
            0,                         /* tp_members */
            Archetype_getseters,       /* tp_getset */
};
