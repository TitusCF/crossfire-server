/* cfpython.c */
CF_PLUGIN void * getPluginProperty(int *type, ...);
CF_PLUGIN int postInitPlugin(void);
CF_PLUGIN void *globalEventListener(int *type, ...);
CF_PLUGIN void *eventListener(int *type, ...);
CF_PLUGIN int closePlugin(void);
CF_PLUGIN int runPluginCommand(object* op, char* params);
PyObject *Crossfire_Object_wrap(object *what);
