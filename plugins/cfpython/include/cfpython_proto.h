/* cfpython.c */
void initContextStack(void);
void pushContext(CFPContext *context);
CFPContext *popContext(void);
void freeContext(CFPContext *context);
int initPlugin(const char *iversion, f_plug_api gethooksptr);
void *getPluginProperty(int *type, ...);
int runPluginCommand(object *op, char *params);
int postInitPlugin(void);
void *globalEventListener(int *type, ...);
void *eventListener(int *type, ...);
int closePlugin(void);
/* cfpython_archetype.c */
PyObject *Crossfire_Archetype_wrap(archetype *what);
/* cfpython_object.c */
PyObject *Crossfire_Object_wrap(object *what);
/* cfpython_party.c */
PyObject *Crossfire_Party_wrap(partylist *what);
/* cfpython_region.c */
PyObject *Crossfire_Region_wrap(region *what);
/* cfpython_map.c */
PyObject *Crossfire_Map_wrap(mapstruct *what);
