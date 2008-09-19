/* cflogger.c */
int initPlugin(const char *iversion, f_plug_api gethooksptr);
void *getPluginProperty(int *type, ...);
int runPluginCommand(object *op, char *params);
void *eventListener(int *type, ...);
void *cflogger_globalEventListener(int *type, ...);
int postInitPlugin(void);
int closePlugin(void);
