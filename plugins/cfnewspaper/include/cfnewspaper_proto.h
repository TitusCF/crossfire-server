/* cfnewspaper.c */
int initPlugin(const char *iversion, f_plug_api gethooksptr);
void *getPluginProperty(int *type, ...);
int runPluginCommand(object *op, char *params);
void *cfnewspaper_globalEventListener(int *type, ...);
int postInitPlugin(void);
void *eventListener(int *type, ...);
int closePlugin(void);
