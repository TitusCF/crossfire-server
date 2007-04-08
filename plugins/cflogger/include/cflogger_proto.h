/* cflogger.c */
void do_sql(const char *sql);
void check_tables(void);
int get_living_id(object *living);
int get_region_id(region *reg);
int get_map_id(mapstruct *map);
int get_time_id(void);
void add_player_event(object *pl, int event_code);
void add_map_event(mapstruct *map, int event_code, object *pl);
void add_death(object *victim, object *killer);
int initPlugin(const char *iversion, f_plug_api gethooksptr);
void *getPluginProperty(int *type, ...);
int runPluginCommand(object *op, char *params);
void *globalEventListener(int *type, ...);
int postInitPlugin(void);
int closePlugin(void);
