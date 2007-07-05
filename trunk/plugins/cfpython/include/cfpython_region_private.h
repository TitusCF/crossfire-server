static PyObject* Crossfire_Region_GetName( Crossfire_Region* whoptr, void* closure);
static PyObject* Crossfire_Region_GetLongname( Crossfire_Region* whoptr, void* closure);
static PyObject* Crossfire_Region_GetMessage( Crossfire_Region* whoptr, void* closure);
static PyObject* Crossfire_Region_GetNext( Crossfire_Region* who, void* closure );
static PyObject* Crossfire_Region_GetParent( Crossfire_Region* who, PyObject* args );

static int Crossfire_Region_InternalCompare(Crossfire_Region* left, Crossfire_Region* right);

static PyGetSetDef Region_getseters[] = {
    { "Name",       (getter)Crossfire_Region_GetName,     NULL, NULL, NULL },
    { "Longname",   (getter)Crossfire_Region_GetLongname, NULL, NULL, NULL },
    { "Message",    (getter)Crossfire_Region_GetMessage,  NULL, NULL, NULL },
    { "Next",       (getter)Crossfire_Region_GetNext,     NULL, NULL, NULL },
    { NULL, NULL, NULL, NULL, NULL }
};

static PyMethodDef RegionMethods[] = {
	{ "GetParent",      (PyCFunction)Crossfire_Region_GetParent,         METH_VARARGS},
    {NULL, NULL, 0}
};

/* Our actual Python ArchetypeType */
PyTypeObject Crossfire_RegionType = {
            PyObject_HEAD_INIT(NULL)
                    0,                         /* ob_size*/
            "Crossfire.Party",        /* tp_name*/
            sizeof(Crossfire_Region),  /* tp_basicsize*/
            0,                         /* tp_itemsize*/
            0,                         /* tp_dealloc*/
            0,                         /* tp_print*/
            0,                         /* tp_getattr*/
            0,                         /* tp_setattr*/
            (cmpfunc)Crossfire_Region_InternalCompare,                         /* tp_compare*/
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
            "Crossfire regions",       /* tp_doc */
            0,                         /* tp_traverse */
            0,                         /* tp_clear */
            0,                         /* tp_richcompare */
            0,                         /* tp_weaklistoffset */
            0,                         /* tp_iter */
            0,                         /* tp_iternext */
            RegionMethods,             /* tp_methods */
            0,                         /* tp_members */
            Region_getseters,          /* tp_getset */
};
