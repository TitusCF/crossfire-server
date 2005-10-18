/* cfpython.c */
void *getPluginProperty(int *type, ...);
int postInitPlugin(void);
void *globalEventListener(int *type, ...);
void *eventListener(int *type, ...);
int closePlugin(void);
int runPluginCommand(object* op, char* params);
PyObject *Crossfire_Object_wrap(object *what);
