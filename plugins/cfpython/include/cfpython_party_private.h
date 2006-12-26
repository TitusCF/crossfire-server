static PyObject* Crossfire_Party_GetName( Crossfire_Party* whoptr, void* closure);
static PyObject* Crossfire_Party_GetPassword( Crossfire_Party* whoptr, void* closure);
static PyObject* Crossfire_Party_GetNext( Crossfire_Party* who, void* closure );
static PyObject* Crossfire_Party_GetPlayers( Crossfire_Party* who, PyObject* args );

static int Crossfire_Party_InternalCompare(Crossfire_Party* left, Crossfire_Party* right);

static PyGetSetDef Party_getseters[] = {
    { "Name",       (getter)Crossfire_Party_GetName,     NULL, NULL, NULL },
    { "Password",   (getter)Crossfire_Party_GetPassword, NULL, NULL, NULL },
    { "Next",       (getter)Crossfire_Party_GetNext,     NULL, NULL, NULL },
    { NULL, NULL, NULL, NULL, NULL }
};

static PyMethodDef PartyMethods[] = {
	{ "GetPlayers",     (PyCFunction)Crossfire_Party_GetPlayers,        METH_VARARGS},
    {NULL, NULL, 0}
};

/* Our actual Python ArchetypeType */
PyTypeObject Crossfire_PartyType = {
            PyObject_HEAD_INIT(NULL)
                    0,                         /* ob_size*/
            "Crossfire.Party",        /* tp_name*/
            sizeof(Crossfire_Party),  /* tp_basicsize*/
            0,                         /* tp_itemsize*/
            0,                         /* tp_dealloc*/
            0,                         /* tp_print*/
            0,                         /* tp_getattr*/
            0,                         /* tp_setattr*/
            (cmpfunc)Crossfire_Party_InternalCompare,                         /* tp_compare*/
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
            "Crossfire parties",       /* tp_doc */
            0,                         /* tp_traverse */
            0,                         /* tp_clear */
            0,                         /* tp_richcompare */
            0,                         /* tp_weaklistoffset */
            0,                         /* tp_iter */
            0,                         /* tp_iternext */
            PartyMethods,              /* tp_methods */
            0,                         /* tp_members */
            Party_getseters,           /* tp_getset */
};
