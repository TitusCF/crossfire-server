/* anim.c */
extern void free_all_anim(void);
extern void init_anim(void);
extern int find_animation(char *name);
extern void animate_object(object *op, int dir);
/* arch.c */
extern archetype *find_archetype_by_object_name(char *name);
extern object *get_archetype_by_skill_name(char *skill, int type);
extern archetype *get_archetype_by_type_subtype(int type, int subtype);
extern object *get_archetype_by_object_name(char *name);
extern object *find_best_weapon_used_match(object *pl, char *params);
extern int item_matched_string(object *pl, object *op, char *name);
extern void init_archetypes(void);
extern void arch_info(object *op);
extern void clear_archetable(void);
extern void init_archetable(void);
extern void dump_arch(archetype *at);
extern void dump_all_archetypes(void);
extern void free_all_archs(void);
extern archetype *get_archetype_struct(void);
extern void first_arch_pass(FILE *fp);
extern void second_arch_pass(FILE *fp);
extern void check_generators(void);
extern void load_archetypes(void);
extern object *arch_to_object(archetype *at);
extern object *create_singularity(char *name);
extern object *get_archetype(char *name);
extern unsigned long hasharch(char *str, int tablesize);
extern archetype *find_archetype(char *name);
extern void add_arch(archetype *at);
extern archetype *type_to_archetype(int type);
extern object *clone_arch(int type);
extern object *ObjectCreateArch(archetype *at);
/* button.c */
extern void push_button(object *op);
extern void update_button(object *op);
extern void update_buttons(mapstruct *m);
extern void use_trigger(object *op);
extern void animate_turning(object *op);
extern int check_altar_sacrifice(object *altar, object *sacrifice);
extern int operate_altar(object *altar, object **sacrifice);
extern void trigger_move(object *op, int state);
extern int check_trigger(object *op, object *cause);
extern void add_button_link(object *button, mapstruct *map, int connected);
extern void remove_button_link(object *op);
extern objectlink *get_button_links(object *button);
extern int get_button_value(object *button);
extern void do_mood_floor(object *op, object *op2);
extern object *check_inv_recursive(object *op, object *trig);
extern void check_inv(object *op, object *trig);
extern void verify_button_links(mapstruct *map);
/* exp.c */
extern int new_exp(object *ob);
extern int has_ability(object *ob);
extern void init_experience(void);
extern void dump_experience(void);
/* friend.c */
extern void add_friendly_object(object *op);
extern void remove_friendly_object(object *op);
extern void dump_friendly_objects(void);
extern void clean_friendly_list(void);
/* glue.c */
extern void init_function_pointers(void);
extern void set_emergency_save(type_func_int addr);
extern void set_clean_tmp_files(type_func_void addr);
extern void set_fix_auto_apply(type_func_map addr);
extern void set_remove_friendly_object(type_func_ob addr);
extern void set_process_active_maps(type_func_void addr);
extern void set_update_buttons(type_func_map addr);
extern void set_draw_info(type_func_int_int_ob_cchar addr);
extern void set_move_apply(type_move_apply_func addr);
extern void set_monster_check_apply(type_func_ob_ob addr);
extern void set_init_blocksview_players(type_func_void addr);
extern void set_info_map(type_func_int_map_char addr);
extern void set_move_teleporter(type_func_ob addr);
extern void set_move_firewall(type_func_ob addr);
extern void set_trap_adjust(type_func_ob_int addr);
extern void set_move_creator(type_func_ob addr);
extern void set_move_duplicator(type_func_ob addr);
extern void set_esrv_send_item(type_func_ob_ob addr);
extern void set_esrv_update_item(type_func_int_ob_ob addr);
extern void set_esrv_del_item(type_func_player_int addr);
extern void set_set_darkness_map(type_func_map addr);
extern void set_dragon_gain_func(type_func_dragon_gain addr);
extern void set_weather_effect_func(type_func_char addr);
extern void fatal(int err);
extern void dummy_function_int(int i);
extern void dummy_function_int_int(int i, int j);
extern void dummy_function_player_int(player *p, int j);
extern void dummy_function(void);
extern void dummy_function_map(mapstruct *m);
extern void dummy_function_ob(object *ob);
extern void dummy_function_ob2(object *ob, object *ob2);
extern int dummy_function_ob2int(object *ob, object *ob2);
extern void dummy_function_ob_int(object *ob, int i);
extern void dummy_function_txtnr(char *txt, int nr);
extern void dummy_draw_info(int a, int b, object *ob, const char *txt);
extern void dummy_function_mapstr(int a, mapstruct *map, char *str);
extern void dummy_function_int_ob_ob(int n, object *ob, object *ob2);
extern void dummy_move_apply_func(object *ob, object *ob2, object *ob3);
extern void dummy_function_dragongain(object *ob, int a1, int a2);
extern void dummy_function_char(char *filename);
/* holy.c */
extern void init_gods(void);
extern void add_god_to_list(archetype *god_arch);
extern int baptize_altar(object *op);
extern godlink *get_rand_god(void);
extern object *pntr_to_god_obj(godlink *godlnk);
extern void free_all_god(void);
extern void dump_gods(void);
/* info.c */
extern void dump_abilities(void);
extern void print_monsters(void);
extern void bitstostring(long bits, int num, char *str);
/* image.c */
extern char find_color(char *name);
extern int ReadBmapNames(void);
extern int FindFace(char *name, int error);
extern char *FindFaceName(int facenbr, char *error);
extern int ReadSmooth(void);
extern int FindSmooth(uint16 face, uint16 *smoothed);
extern void free_all_images(void);
/* init.c */
extern void init_library(void);
extern void init_environ(void);
extern void init_globals(void);
extern void init_objects(void);
extern void init_defaults(void);
extern void init_dynamic(void);
extern void write_todclock(void);
extern void init_clocks(void);
extern void init_attackmess(void);
/* item.c */
extern int get_power_from_ench(int ench);
extern int calc_item_power(object *op, int flag);
extern char *describe_resistance(object *op, int newline);
extern char *query_weight(object *op);
extern char *get_levelnumber(int i);
extern char *get_number(int i);
extern char *ring_desc(object *op);
extern char *query_short_name(object *op);
extern char *query_name(object *op);
extern char *query_base_name(object *op, int plural);
extern char *describe_item(object *op, object *owner);
extern int is_magical(object *op);
extern int need_identify(object *op);
extern void identify(object *op);
/* links.c */
extern objectlink *get_objectlink(void);
extern oblinkpt *get_objectlinkpt(void);
extern void free_objectlink(objectlink *ol);
extern void free_objectlinkpt(oblinkpt *obp);
/* living.c */
extern void set_attr_value(living *stats, int attr, signed char value);
extern void change_attr_value(living *stats, int attr, signed char value);
extern signed char get_attr_value(living *stats, int attr);
extern void check_stat_bounds(living *stats);
extern int change_abil(object *op, object *tmp);
extern void drain_stat(object *op);
extern void drain_specific_stat(object *op, int deplete_stats);
extern void change_luck(object *op, int value);
extern void remove_statbonus(object *op);
extern void add_statbonus(object *op);
extern void fix_player(object *op);
extern int allowed_class(object *op);
extern void set_dragon_name(object *pl, object *abil, object *skin);
extern void dragon_level_gain(object *who);
extern object *give_skill_by_name(object *op, char *skill_name);
extern void player_lvl_adj(object *who, object *op);
extern sint64 level_exp(int level, double expmul);
extern void calc_perm_exp(object *op);
extern sint64 check_exp_loss(object *op, sint64 exp);
extern sint64 check_exp_adjust(object *op, sint64 exp);
extern void change_exp(object *op, int exp, char *skill_name, int flag);
extern void apply_death_exp_penalty(object *op);
extern int did_make_save(object *op, int level, int bonus);
/* logger.c */
extern void LOG(LogLevel logLevel, char *format, ...);
/* los.c */
extern void set_block(int x, int y, int bx, int by);
extern void init_block(void);
extern void clear_los(object *op);
extern void expand_sight(object *op);
extern int has_carried_lights(object *op);
extern void expand_lighted_sight(object *op);
extern void blinded_sight(object *op);
extern void update_los(object *op);
extern void update_all_map_los(mapstruct *map);
extern void update_all_los(mapstruct *map, int x, int y);
extern void print_los(object *op);
extern void make_sure_seen(object *op);
extern void make_sure_not_seen(object *op);
/* map.c */
extern mapstruct *has_been_loaded(char *name);
extern char *create_pathname(char *name);
extern char *create_overlay_pathname(char *name);
extern int check_path(char *name, int prepend_dir);
extern void dump_map(mapstruct *m);
extern void dump_all_maps(void);
extern int get_map_flags(mapstruct *oldmap, mapstruct **newmap, sint16 x, sint16 y, sint16 *nx, sint16 *ny);
extern int blocked_link(object *ob, int x, int y);
extern int arch_blocked(archetype *at, mapstruct *m, int x, int y);
extern void fix_container(object *container);
extern void load_objects(mapstruct *m, FILE *fp, int mapflags);
extern void save_objects(mapstruct *m, FILE *fp, FILE *fp2, int flag);
extern mapstruct *get_linked_map(void);
extern void allocate_map(mapstruct *m);
extern mapstruct *get_empty_map(int sizex, int sizey);
extern mapstruct *load_original_map(char *filename, int flags);
extern mapstruct *load_overlay_map(char *filename, mapstruct *m);
extern int new_save_map(mapstruct *m, int flag);
extern void clean_object(object *op);
extern void free_all_objects(mapstruct *m);
extern void free_map(mapstruct *m, int flag);
extern void delete_map(mapstruct *m);
extern mapstruct *ready_map_name(char *name, int flags);
extern int calculate_difficulty(mapstruct *m);
extern void clean_tmp_map(mapstruct *m);
extern void free_all_maps(void);
extern int change_map_light(mapstruct *m, int change);
extern void update_position(mapstruct *m, int x, int y);
extern void set_map_reset_time(mapstruct *map);
extern int out_of_map(mapstruct *m, int x, int y);
extern mapstruct *get_map_from_coord(mapstruct *m, int *x, int *y);
extern void get_rangevector(object *op1, object *op2, rv_vector *retval, int flags);
extern void get_rangevector_from_mapcoord(mapstruct *m, int x, int y, object *op2, rv_vector *retval, int flags);
extern int on_same_map(object *op1, object *op2);
/* object.c */
extern signed long sum_weight(object *op);
extern object *is_player_inv(object *op);
extern void dump_object2(object *op);
extern void dump_object(object *op);
extern void dump_me(object *op, char *outstr);
extern void dump_all_objects(void);
extern object *get_nearest_part(object *op, object *pl);
extern object *find_object(int i);
extern object *find_object_name(char *str);
extern void free_all_object_data(void);
extern object *get_owner(object *op);
extern void clear_owner(object *op);
extern void set_owner(object *op, object *owner);
extern void copy_owner(object *op, object *clone);
extern void reset_object(object *op);
extern void clear_object(object *op);
extern void copy_object(object *op2, object *op);
extern void expand_objects(void);
extern object *get_object(void);
extern void update_turn_face(object *op);
extern void update_ob_speed(object *op);
extern void remove_from_active_list(object *op);
extern void update_object(object *op, int action);
extern void free_object(object *ob);
extern int count_free(void);
extern int count_used(void);
extern int count_active(void);
extern void sub_weight(object *op, signed long weight);
extern void remove_ob(object *op);
extern object *merge_ob(object *op, object *top);
extern object *insert_ob_in_map_at(object *op, mapstruct *m, object *originator, int flag, int x, int y);
extern object *insert_ob_in_map(object *op, mapstruct *m, object *originator, int flag);
extern void replace_insert_ob_in_map(char *arch_string, object *op);
extern object *get_split_ob(object *orig_ob, int nr);
extern object *decrease_ob_nr(object *op, int i);
extern void add_weight(object *op, signed long weight);
extern object *insert_ob_in_ob(object *op, object *where);
extern int check_walk_on(object *op, object *originator);
extern object *present_arch(archetype *at, mapstruct *m, int x, int y);
extern object *present(unsigned char type, mapstruct *m, int x, int y);
extern object *present_in_ob(unsigned char type, object *op);
extern object *present_in_ob_by_name(int type, char *str, object *op);
extern object *present_arch_in_ob(archetype *at, object *op);
extern void flag_inv(object *op, int flag);
extern void unflag_inv(object *op, int flag);
extern void set_cheat(object *op);
extern int find_free_spot(archetype *at, mapstruct *m, int x, int y, int start, int stop);
extern int find_first_free_spot(archetype *at, mapstruct *m, int x, int y);
extern void get_search_arr(int *search_arr);
extern int find_dir(mapstruct *m, int x, int y, object *exclude);
extern int distance(object *ob1, object *ob2);
extern int find_dir_2(int x, int y);
extern int absdir(int d);
extern int dirdiff(int dir1, int dir2);
extern int can_see_monsterP(mapstruct *m, int x, int y, int dir);
extern int can_pick(object *who, object *item);
extern object *ObjectCreateClone(object *asrc);
extern int was_destroyed(object *op, tag_t old_tag);
extern object *load_object_str(char *obstr);
extern object *find_obj_by_type_subtype(object *who, int type, int subtype);
/* porting.c */
extern char *tempnam_local(char *dir, char *pfx);
extern void remove_directory(const char *path);
extern char *strdup_local(char *str);
extern long strtol_local(register char *str, char **ptr, register int base);
extern char *strcasestr_local(const char *s, char *find);
extern char *strerror_local(int errnum);
extern int isqrt(int n);
extern char *ltostr10(signed long n);
extern char *doubletostr10(double v);
extern void save_long(char *buf, char *name, long n);
extern void save_long_long(char *buf, char *name, sint64 n);
extern FILE *open_and_uncompress(char *name, int flag, int *compressed);
extern void close_and_delete(FILE *fp, int compressed);
extern void make_path_to_file(char *filename);
/* player.c */
extern void free_player(player *pl);
extern object *find_skill(object *op, int skillnr);
extern int atnr_is_dragon_enabled(int attacknr);
extern int is_dragon_pl(object *op);
/* re-cmp.c */
extern char *re_cmp(char *str, char *regexp);
/* readable.c */
extern int nstrtok(char *buf1, char *buf2);
extern char *strtoktolin(char *buf1, char *buf2);
extern int book_overflow(char *buf1, char *buf2, int booksize);
extern void init_readable(void);
extern void change_book(object *book, int msgtype);
extern object *get_random_mon(int level);
extern char *mon_desc(object *mon);
extern object *get_next_mon(object *tmp);
extern char *mon_info_msg(int level, int booksize);
extern char *artifact_msg(int level, int booksize);
extern char *spellpath_msg(int level, int booksize);
extern void make_formula_book(object *book, int level);
extern char *msgfile_msg(int level, int booksize);
extern char *god_info_msg(int level, int booksize);
extern void tailor_readable_ob(object *book, int msg_type);
extern void free_all_readable(void);
extern void write_book_archive(void);
/* recipe.c */
extern recipelist *get_formulalist(int i);
extern void init_formulae(void);
extern void check_formulae(void);
extern void dump_alchemy(void);
extern archetype *find_treasure_by_name(treasure *t, char *name, int depth);
extern long find_ingred_cost(char *name);
extern void dump_alchemy_costs(void);
extern char *ingred_name(char *name);
extern int strtoint(char *buf);
extern artifact *locate_recipe_artifact(recipe *rp);
extern int numb_ingred(char *buf);
extern recipelist *get_random_recipelist(void);
extern recipe *get_random_recipe(recipelist *rpl);
extern void free_all_recipes(void);
/* shstr.c */
extern void init_hash_table(void);
extern char *add_string(const char *str);
extern char *add_refcount(char *str);
extern int query_refcount(const char *str);
extern char *find_string(const char *str);
extern void free_string(char *str);
extern void ss_dump_statistics(void);
extern char *ss_dump_table(int what);
extern int buf_overflow(char *buf1, char *buf2, int bufsize);
/* time.c */
extern void reset_sleep(void);
extern void log_time(long process_utime);
extern int enough_elapsed_time(void);
extern void sleep_delta(void);
extern void set_max_time(long t);
extern void get_tod(timeofday_t *tod);
extern void print_tod(object *op);
extern void time_info(object *op);
extern long seconds(void);
/* treasure.c */
extern void init_archetype_pointers(void);
extern void load_treasures(void);
extern treasurelist *find_treasurelist(char *name);
extern void create_all_treasures(treasure *t, object *op, int flag, int difficulty, int tries);
extern void create_one_treasure(treasurelist *tl, object *op, int flag, int difficulty, int tries);
extern void create_treasure(treasurelist *t, object *op, int flag, int difficulty, int tries);
extern object *generate_treasure(treasurelist *t, int difficulty);
extern int level_for_item(object *op, int difficulty, int retmult);
extern int magic_from_difficulty(int difficulty);
extern void set_abs_magic(object *op, int magic);
extern void set_ring_bonus(object *op, int bonus);
extern int get_magic(int diff);
extern void fix_generated_item(object *op, object *creator, int difficulty, int max_magic, int flags);
extern artifactlist *find_artifactlist(int type);
extern void dump_artifacts(void);
extern void dump_monster_treasure_rec(char *name, treasure *t, int depth);
extern void dump_monster_treasure(char *name);
extern void init_artifacts(void);
extern void add_abilities(object *op, object *change);
extern void give_artifact_abilities(object *op, object *artifct);
extern void generate_artifact(object *op, int difficulty);
extern void fix_flesh_item(object *item, object *donor);
extern int special_potion(object *op);
extern void free_treasurestruct(treasure *t);
extern void free_charlinks(linked_char *lc);
extern void free_artifact(artifact *at);
extern void free_artifactlist(artifactlist *al);
extern void free_all_treasures(void);
/* utils.c */
extern int random_roll(int min, int max, object *op, int goodbad);
extern int die_roll(int num, int size, object *op, int goodbad);
extern int rndm(int min, int max);
extern void decay_objects(mapstruct *m);
extern materialtype_t *name_to_material(char *name);
extern void transmute_materialname(object *op, object *change);
extern void set_materialname(object *op, int difficulty, materialtype_t *nmt);
/* loader.c */
extern int lex_load(object *op, int map_flags);
extern void yyrestart(FILE *input_file);
extern void yy_load_buffer_state(void);
extern int yyerror(char *s);
extern int load_object(FILE *fp, object *op, int bufstate, int map_flags);
extern int set_variable(object *op, char *buf);
extern void save_double(char *buf, char *name, double v);
extern void init_vars(void);
extern char *get_ob_diff(object *op, object *op2);
extern void save_object(FILE *fp, object *op, int flag);
extern void insert_event(object *op, int etype, char *ehook, char *eplug, char *eoptions);
extern event *find_event(object *op, int etype);
