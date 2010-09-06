/* toolkit_common.c */
void cctk_setlog(char *logfile);
void cctk_setdatadir(char *datadir);
void cctk_setconfdir(char *confdir);
void cctk_init_std_archetypes(void);
object *cctk_create_game_object(const char *archname);
void cctk_set_object_strings(object *op, const char *string);
