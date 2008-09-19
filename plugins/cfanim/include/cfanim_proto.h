/* cfanim.c */
int get_boolean(char *strg, int *bl);
int is_animated_player(object *pl);
CF_PLUGIN int initPlugin(const char *iversion, f_plug_api gethooksptr);
CF_PLUGIN void *getPluginProperty(int *type, ...);
CF_PLUGIN anim_move_result runPluginCommand(object *op, char *params);
CF_PLUGIN int postInitPlugin(void);
CF_PLUGIN void *cfanim_globalEventListener(int *type, ...);
CF_PLUGIN void *eventListener(int *type, ...);
CF_PLUGIN int closePlugin(void);
