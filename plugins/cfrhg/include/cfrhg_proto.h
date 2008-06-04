/* cfrhg.c */
void *globalEventListener(int *type, ...);
void *eventListener(int *type, ...);
int initPlugin(const char *iversion, f_plug_api gethooksptr);
void *getPluginProperty(int *type, ...);
int runPluginCommand(object *op, char *params);
int postInitPlugin(void);
int closePlugin(void);
