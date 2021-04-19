#include "bufferreader.h"
/* anim.c */
extern Animations *find_animation(const char *name);
extern Animations *try_find_animation(const char *name);
extern void animate_object(object *op, int dir);
extern void apply_anim_suffix(object *who, const char *suffix);
extern void dump_animations(void);
extern void animation_load_block(FILE *file, const char *full_path, const char *animation_name);
/* arch.c */
#ifdef __cplusplus
extern "C" {
#endif
archetype *find_archetype_by_object_name(const char *name);
archetype *find_archetype_by_object_type_name(int type, const char *name);
archetype *get_archetype_by_skill_name(const char *skill, int type);
archetype *get_archetype_by_type_subtype(int type, int subtype);
object *create_archetype_by_object_name(const char *name);
void dump_arch(archetype *at, StringBuffer *sb);
void dump_all_archetypes(void);
void free_arch(archetype *at);
archetype *get_archetype_struct(void);
object *arch_to_object(archetype *at);
object *create_singularity(const char *name);
object *create_archetype(const char *name);
archetype *try_find_archetype(const char *name);
archetype *find_archetype(const char *name);
object *object_create_arch(archetype *at);
archetype *get_next_archetype(archetype *current);
void first_arch_pass(FILE *fp, const char *filename);
#ifdef __cplusplus
}
#endif
/* arch_types_valid.c */
extern int is_type_valid(uint8_t type);
/* artifact.c */
extern void free_all_artifacts(void);
extern void generate_artifact(object *op, int difficulty);
extern void give_artifact_abilities(object *op, const object *artifact);
extern int legal_artifact_combination(const object *op, const artifact *art);
extern void add_abilities(object *op, const object *change);
extern void init_artifacts(BufferReader *reader, const char *filename);
extern const artifactlist *find_artifactlist(int type);
extern const artifact *find_artifact(const object *op, const char *name);
extern void dump_artifacts(void);
extern unsigned artifact_get_face(const artifact *art);
/* button.c */
extern void trigger_connected(objectlink *ol, object *cause, const int state);
extern void push_button(object *op);
extern void update_button(object *op);
extern void update_buttons(mapstruct *m);
extern void use_trigger(object *op);
extern void animate_turning(object *op);
extern int check_altar_sacrifice(const object *altar, const object *sacrifice, int remove_others, int *toremove);
extern int operate_altar(object *altar, object **sacrifice);
extern int check_trigger(object *op, object *cause);
extern void add_button_link(object *button, mapstruct *map, int connected);
extern void remove_button_link(object *op);
extern int get_button_value(const object *button);
extern object *check_inv_recursive(object *op, const object *trig);
extern void check_inv(object *op, object *trig);
extern void verify_button_links(const mapstruct *map);
/* dialog.c */
extern void free_dialog_information(object *op);
extern int get_dialog_message(object *op, const char *text, struct_dialog_message **message, struct_dialog_reply **reply);
/* exp.c */
extern int64_t new_exp(const object *ob);
extern int has_ability(const object *ob);
extern void init_experience(void);
extern void dump_experience(void);
extern void free_experience(void);
/* friend.c */
extern void add_friendly_object(object *op);
extern void remove_friendly_object(object *op);
extern void dump_friendly_objects(void);
extern void clean_friendly_list(void);
extern int is_friendly(const object *op);
/* glue.c */
extern void fatal(enum fatal_error err) __attribute__ ((noreturn));
/* holy.c */
extern void init_gods(void);
extern godlink *get_rand_god(void);
extern const object *pntr_to_god_obj(godlink *godlnk);
extern int describe_god(const object *god, int what, StringBuffer *buf, size_t maxlen);
extern void free_all_god(void);
extern void dump_gods(void);
/* info.c */
extern void dump_abilities(void);
extern void print_monsters(void);
/* image.c */
extern int find_smooth(const Face *face, const Face **smoothed);
extern int is_valid_faceset(int fsn);
extern int get_face_fallback(int faceset, int imageno);
extern size_t get_faces_count(void);
extern const Face *get_face_by_id(uint16_t id);
extern int load_face_file(FILE *file, const char *full_path);
extern int load_png_file(FILE *file, const char *full_path);
extern void load_image_info(FILE *file, const char *filename);
extern const Face *find_face(const char *name);
extern const Face *try_find_face(const char *name, const Face *error);
extern uint8_t find_color(const char *name);
extern const char *get_colorname(uint8_t index);
extern void dump_faces(void);
extern int get_bitmap_checksum();
/* init.c */
extern void init_library(void);
extern void init_globals(void);
extern void free_globals(void);
extern void init_objects(void);
extern void write_todclock(void);
extern void init_attackmess(BufferReader *reader, const char *filename);
/* item.c */
extern int get_power_from_ench(int ench);
extern int calc_item_power(const object *op);
extern const typedata *get_typedata(int itemtype);
extern const typedata *get_typedata_by_name(const char *name);
extern StringBuffer *describe_resistance(const object *op, int newline, int use_media_tags, StringBuffer *buf);
extern void query_weight(const object *op, char *buf, size_t size);
extern void get_levelnumber(int i, char *buf, size_t size);
extern void query_short_name(const object *op, char *buf, size_t size);
extern void query_name(const object *op, char *buf, size_t size);
extern void query_base_name(const object *op, int plural, char *buf, size_t size);
extern StringBuffer *describe_monster(const object *op, int use_media_tags, StringBuffer *buf);
extern StringBuffer *describe_item(const object *op, const object *owner, int use_media_tags, StringBuffer *buf);
extern int is_magical(const object *op);
extern int is_identifiable_type(const object *op);
extern int is_identified(const object *op);
extern void object_give_identified_properties(object *op);
extern object *identify(object *op);
/* languages.c */
extern const char *i18n(const object *who, const char *code);
extern int i18n_find_language_by_code(const char *code);
extern int i18n_get_language_by_code(const char *code);
extern sstring i18n_get_language_code(int language);
extern void i18n_list_languages(object *who);
extern void i18n_init(void);
extern void i18n_free(void);
/* links.c */
extern objectlink *get_objectlink(void);
extern oblinkpt *get_objectlinkpt(void);
extern void free_objectlink(objectlink *ol);
extern void free_objectlinkpt(oblinkpt *obp);
/* living.c */
extern void set_attr_value(living *stats, int attr, int8_t value);
extern void change_attr_value(living *stats, int attr, int8_t value);
extern int8_t get_attr_value(const living *stats, int attr);
extern void check_stat_bounds(living *stats, int8_t min_stat, int8_t max_stat);
extern int change_abil(object *op, object *tmp);
extern void drain_stat(object *op);
extern void drain_specific_stat(object *op, int deplete_stats);
extern int remove_depletion(object *op, int level);
extern void change_luck(object *op, int value);
extern void remove_statbonus(object *op);
extern void add_statbonus(object *op);
extern void fix_object(object *op);
extern int allowed_class(const object *op);
extern void set_dragon_name(object *pl, const object *abil, const object *skin);
extern object *give_skill_by_name(object *op, const char *skill_name);
extern void player_lvl_adj(object *who, object *op);
extern int64_t level_exp(int level, double expmul);
extern int exp_level(int64_t exp);
extern void calc_perm_exp(object *op);
extern int64_t check_exp_loss(const object *op, int64_t exp);
extern int64_t check_exp_adjust(const object *op, int64_t exp);
extern void change_exp(object *op, int64_t exp, const char *skill_name, int flag);
extern void apply_death_exp_penalty(object *op);
extern int did_make_save(const object *op, int level, int bonus);
extern void share_exp(object *op, int64_t exp, const char *skill, int flag);
extern int get_cha_bonus(int stat);
extern int get_dex_bonus(int stat);
extern int get_thaco_bonus(int stat);
extern uint32_t get_weight_limit(int stat);
extern int get_learn_spell(int stat);
extern int get_cleric_chance(int stat);
extern int get_turn_bonus(int stat);
extern int get_dam_bonus(int stat);
extern float get_speed_bonus(int stat);
extern int get_fear_bonus(int stat);
extern void init_stats(int reload);
/* logger.c */
extern void LOG(LogLevel logLevel, const char *format, ...);
/* los.c */
extern void init_block(void);
extern void clear_los(player *pl);
extern int has_carried_lights(const object *op);
extern void update_los(object *op);
extern void update_all_map_los(mapstruct *map);
extern void update_all_los(const mapstruct *map, int x, int y);
extern void print_los(object *op);
extern void make_sure_seen(const object *op);
extern void make_sure_not_seen(const object *op);
/* map.c */
extern mapstruct *has_been_loaded(const char *name);
extern char *create_pathname(const char *name, char *buf, size_t size);
extern void create_overlay_pathname(const char *name, char *buf, size_t size);
extern void create_template_pathname(const char *name, char *buf, size_t size);
extern int check_path(const char *name, int prepend_dir);
extern void dump_map(const mapstruct *m);
extern void dump_all_maps(void);
extern int get_map_flags(mapstruct *oldmap, mapstruct **newmap, int16_t x, int16_t y, int16_t *nx, int16_t *ny);
extern int blocked_link(object *ob, mapstruct *m, int16_t sx, int16_t sy);
extern int ob_blocked(const object *ob, mapstruct *m, int16_t x, int16_t y);
extern mapstruct *get_linked_map(void);
extern mapstruct *get_empty_map(int sizex, int sizey);
extern mapstruct *mapfile_load(const char *map, int flags);
extern int save_map(mapstruct *m, int flag);
extern void clean_object(object *op);
extern void free_map(mapstruct *m);
extern void delete_map(mapstruct *m);
extern mapstruct *ready_map_name(const char *name, int flags);
extern int calculate_difficulty(mapstruct *m);
extern void clean_tmp_map(mapstruct *m);
extern void free_all_maps(void);
extern int change_map_light(mapstruct *m, int change);
extern void update_position(mapstruct *m, int x, int y);
extern void set_map_reset_time(mapstruct *map);
extern int out_of_map(mapstruct *m, int x, int y);
extern mapstruct *get_map_from_coord(mapstruct *m, int16_t *x, int16_t *y);
extern int get_rangevector(object *op1, const object *op2, rv_vector *retval, int flags);
extern int get_rangevector_from_mapcoord(const mapstruct *m, int x, int y, const object *op2, rv_vector *retval, int flags);
extern int on_same_map(const object *op1, const object *op2);
extern object *map_find_by_flag(mapstruct *map, int x, int y, int flag);
extern void map_remove_unique_files(const mapstruct *map);
extern const char *map_get_path(const object *item);
/* ob_methods.c */
extern method_ret ob_apply(object *op, object *applier, int aflags);
extern method_ret ob_process(object *op);
extern char *ob_describe(const object *op, const object *observer, int use_media_tags, char *buf, size_t size);
extern method_ret ob_move_on(object *op, object *victim, object *originator);
extern method_ret ob_trigger(object *op, object *cause, int state);
/* ob_types.c */
extern void init_ob_method_struct(ob_methods *methods, ob_methods *fallback);
extern void init_ob_types(ob_methods *base_type);
extern void register_apply(int ob_type, apply_func method);
extern void register_process(int ob_type, process_func method);
extern void register_describe(int ob_type, describe_func method);
extern void register_move_on(int ob_type, move_on_func method);
extern void register_trigger(int ob_type, trigger_func method);
/* object.c */
extern int object_can_merge(object *ob1, object *ob2);
extern signed long object_sum_weight(object *op);
extern object *object_get_env_recursive(object *op);
extern object *object_get_player_container(object *op);
extern void object_dump(const object *op, StringBuffer *sb);
extern void object_dump_all(void);
extern object *object_find_by_tag_global(tag_t i);
extern object *object_find_by_name_global(const char *str);
extern void object_free_all_data(void);
extern object *object_get_owner(object *op);
extern void object_clear_owner(object *op);
extern void object_set_owner(object *op, object *owner);
extern void object_copy_owner(object *op, object *clone);
extern void object_set_enemy(object *op, object *enemy);
extern void object_reset(object *op);
extern void object_free_key_values(object *op);
extern void object_clear(object *op);
extern void object_copy(const object *src_ob, object *dest_ob);
extern void object_copy_with_inv(const object *src_ob, object *dest_ob);
extern object *object_new(void);
extern void object_update_turn_face(object *op);
extern void object_update_speed(object *op);
extern void object_remove_from_active_list(object *op);
extern void object_update(object *op, int action);
extern void object_free_drop_inventory(object *ob);
extern void object_free_inventory(object *ob);
extern void object_free(object *ob, int flags);
extern int object_count_free(void);
extern int object_count_used(void);
extern int object_count_active(void);
extern void object_sub_weight(object *op, signed long weight);
extern void object_remove(object *op);
extern object *object_merge(object *op, object *top);
extern object *object_insert_in_map_at(object *op, mapstruct *m, object *originator, int flag, int x, int y);
extern void object_merge_spell(object *op, int16_t x, int16_t y);
extern object *object_insert_in_map(object *op, mapstruct *m, object *originator, int flag);
extern void object_replace_insert_in_map(const char *arch_string, object *op);
extern object *object_split(object *orig_ob, uint32_t nr, char *err, size_t size);
extern object *object_decrease_nrof(object *op, uint32_t i);
extern void object_add_weight(object *op, signed long weight);
extern object *object_insert_in_ob(object *op, object *where);
extern int object_check_move_on(object *op, object *originator);
extern object *map_find_by_archetype(mapstruct *m, int x, int y, const archetype *at);
extern object *map_find_by_type(mapstruct *m, int x, int y, uint8_t type);
extern object *object_present_in_ob(uint8_t type, const object *op);
extern object *object_present_in_ob_by_name(int type, const char *str, const object *op);
extern object *arch_present_in_ob(const archetype *at, const object *op);
extern void object_set_flag_inv(object *op, int flag);
extern void object_unset_flag_inv(object *op, int flag);
extern void object_set_cheat(object *op);
extern int object_find_multi_free_spot_around(const object *ob, const object *gen, int16_t *hx, int16_t *hy);
extern int object_find_multi_free_spot_within_radius(const object *ob, const object *gen, int *hx, int *hy);
extern int object_find_free_spot(const object *ob, mapstruct *m, int x, int y, int start, int stop);
extern int object_find_first_free_spot(const object *ob, mapstruct *m, int x, int y);
extern void get_search_arr(int *search_arr);
extern int map_find_dir(mapstruct *m, int x, int y, object *exclude);
extern int object_distance(const object *ob1, const object *ob2);
extern int find_dir_2(int x, int y);
extern int absdir(int d);
extern int dirdiff(int dir1, int dir2);
extern int can_see_monsterP(mapstruct *m, int x, int y, int dir);
extern int object_can_pick(const object *who, const object *item);
extern object *object_create_clone(object *asrc);
extern object *object_find_by_name(const object *who, const char *name);
extern object *object_find_by_type(const object *who, int type);
extern object *object_find_by_type_without_flags(const object *who, int type, int *flags, int num_flags);
extern object *object_find_by_type2(const object *who, int type1, int type2);
extern object *object_find_by_tag(const object *who, tag_t tag);
extern object *object_find_by_type_applied(const object *who, int type);
extern object *object_find_by_type_and_name(const object *who, int type, const char *name);
extern object *object_find_by_type_and_race(const object *who, int type, const char *race);
extern object *object_find_by_type_and_slaying(const object *who, int type, const char *slaying);
extern object *object_find_by_type_and_skill(const object *who, int type, const char *skill);
extern object *object_find_by_flag(const object *who, int flag);
extern object *object_find_by_flag_applied(const object *who, int flag);
extern object *object_find_by_arch_name(const object *who, const char *name);
extern object *object_find_by_type_and_arch_name(const object *who, int type, const char *name);
extern object *object_find_by_type_subtype(const object *who, int type, int subtype);
extern key_value *object_get_key_value(const object *ob, const char *key);
extern const char *object_get_value(const object *op, const char *const key);
extern int object_set_value(object *op, const char *key, const char *value, int add_key);
extern int object_matches_string(object *pl, object *op, const char *name);
extern void object_fix_multipart(object *tmp);
extern void object_get_multi_size(const object *ob, int *sx, int *sy, int *hx, int *hy);
extern void object_insert_to_free_spot_or_free(object *op, mapstruct *map, int x, int y, int start, int stop, object *originator);
extern void object_set_msg(object *op, const char *msg);
extern void get_ob_diff(StringBuffer *sb, const object *op, const object *op2);
extern int save_object(FILE *fp, object *op, int flag);
extern void object_handle_death_animation(object *op);
extern int object_matches_pickup_mode(const object *item, int mode);
/* path.c */
extern char *path_combine(const char *src, const char *dst, char *path, size_t size);
extern void path_normalize(char *path);
extern char *path_combine_and_normalize(const char *src, const char *dst, char *path, size_t size);
/* porting.c */
extern FILE *tempnam_secure(const char *dir, const char *pfx, char **filename);
extern void remove_directory(const char *path);
extern int isqrt(int n);
extern void make_path_to_file(const char *filename);
/* player.c */
extern void clear_player(player *pl);
extern void free_player(player *pl);
extern int atnr_is_dragon_enabled(int attacknr);
extern int is_dragon_pl(const object *op);
extern client_spell *get_client_spell_state(player *pl, object *spell);
extern int is_wraith_pl(object *op);
extern int is_old_wraith_pl(object *op);
extern void player_set_dragon_title(struct pl *pl, int level, const char *attack, int skin_resist);
extern void player_get_title(const struct pl *pl, char *buf, size_t bufsize);
extern int player_has_own_title(const struct pl *pl);
extern const char *player_get_own_title(const struct pl *pl);
extern void player_set_own_title(struct pl *pl, const char *title);
extern void link_player_skills(object *op);
/* re-cmp.c */
extern const char *re_cmp(const char *str, const char *regexp);
/* readable.c */
extern int nstrtok(const char *buf1, const char *buf2);
extern char *strtoktolin(const char *buf1, const char *buf2, char *retbuf, size_t size);
extern int book_overflow(const char *buf1, const char *buf2, size_t booksize);
extern void init_readable(void);
extern object *get_random_mon(int level);
extern void tailor_readable_ob(object *book, int msg_type);
extern void free_all_readable(void);
extern void write_book_archive(void);
extern const readable_message_type *get_readable_message_type(object *readable);
extern const GeneralMessage *get_message_from_identifier(const char *identifier);
extern sstring get_message_title(const GeneralMessage *message);
extern sstring get_message_body(const GeneralMessage *message);
extern const Face *get_message_face(const GeneralMessage *message);
extern void init_msgfile(FILE *file, const char *filename);
/* recipe.c */
extern recipelist *get_formulalist(int i);
extern void init_formulae(BufferReader *reader, const char *filename);
extern void dump_alchemy(void);
extern archetype *find_treasure_by_name(const treasure *t, const char *name, int depth);
extern void dump_alchemy_costs(void);
extern int strtoint(const char *buf);
extern const artifact *locate_recipe_artifact(const recipe *rp, size_t idx);
extern recipe *get_random_recipe(recipelist *rpl);
extern void free_all_recipes(void);
extern recipe *find_recipe_for_tool(const char *tool, recipe *from);
extern void check_formulae(void);
/* region.c */
extern region *get_region_by_name(const char *region_name);
extern region *get_region_by_map(mapstruct *m);
extern const char *get_name_of_region_for_map(const mapstruct *m);
extern region *get_region_from_string(const char *name);
extern int region_is_child_of_region(const region *child, const region *r);
extern const char *get_region_longname(const region *r);
extern const char *get_region_msg(const region *r);
extern object *get_jail_exit(object *op);
extern int init_regions(void);
extern region *get_region_struct(void);
/* shstr.c */
extern void init_hash_table(void);
extern sstring add_string(const char *str);
extern sstring add_refcount(sstring str);
extern int query_refcount(sstring str);
extern sstring find_string(const char *str);
extern void free_string(sstring str);
extern void ss_dump_statistics(char *buf, size_t size);
extern char *ss_dump_table(int what, char *buf, size_t size);
extern int buf_overflow(const char *buf1, const char *buf2, size_t bufsize);
/* stringbuffer.c */
extern StringBuffer *stringbuffer_new(void);
extern void stringbuffer_delete(StringBuffer *sb);
extern char *stringbuffer_finish(StringBuffer *sb);
extern sstring stringbuffer_finish_shared(StringBuffer *sb);
extern void stringbuffer_append_string(StringBuffer *sb, const char *str);
extern void stringbuffer_append_printf(StringBuffer *sb, const char *format, ...);
extern void stringbuffer_append_stringbuffer(StringBuffer *sb, const StringBuffer *sb2);
extern size_t stringbuffer_length(StringBuffer *sb);
/* time.c */
extern const char *get_periodofday(const int index);
extern const char *get_month_name(const int index);
extern const char *get_weekday(const int index);
extern const char *get_season_name(const int index);
extern void reset_sleep(void);
extern long usec_elapsed(struct timespec first, struct timespec second);
void tick_game_time(void);
long get_sleep_remaining(void);
extern void jump_time(void);
extern void set_max_time(long t);
extern void get_tod(timeofday_t *tod);
extern void time_info(object *op);
extern long seconds(void);
extern const char *time_format_time(const timeofday_t *tod, char *buf, size_t bufsize);
/* treasure.c */
extern void init_archetype_pointers(void);
extern void load_treasures(void);
extern treasurelist *find_treasurelist(const char *name);
extern void create_treasure(treasurelist *t, object *op, int flag, int difficulty, int tries);
extern object *generate_treasure(treasurelist *t, int difficulty);
extern void set_abs_magic(object *op, int magic);
extern void fix_generated_item(object *op, object *creator, int difficulty, int max_magic, int flags);
extern void dump_monster_treasure(const char *name);
/* utils.c */
extern int random_roll(int min, int max, const object *op, int goodbad);
extern int64_t random_roll64(int64_t min, int64_t max, const object *op, int goodbad);
extern int die_roll(int num, int size, const object *op, int goodbad);
extern int rndm(int min, int max);
extern void decay_objects(mapstruct *m);
extern materialtype_t *name_to_material(const char *name);
extern void transmute_materialname(object *op, const object *change);
extern void set_materialname(object *op);
extern const char *strrstr(const char *haystack, const char *needle);
extern void strip_endline(char *buf);
extern void replace(const char *src, const char *key, const char *replacement, char *result, size_t resultsize);
extern void make_list_like(char *input);
extern int get_random_dir(void);
extern int get_randomized_dir(int dir);
extern int adjust_dir(int dir, int destination_dir);
extern void replace_unprintable_chars(char *buf);
extern size_t split_string(char *str, char *array[], size_t array_size, char sep);
extern StringBuffer *describe_spellpath_attenuation(const char *attenuation, int value, StringBuffer *buf);
extern StringBuffer *describe_attacktype(const char *attack, int value, StringBuffer *buf);
/* loader.c */
extern void yyrestart(FILE *input_file);
extern void yypop_buffer_state(void);
extern int yyget_lineno(void);
extern FILE *yyget_in(void);
extern FILE *yyget_out(void);
extern char *yyget_text(void);
extern void yyset_lineno(int line_number);
extern void yyset_in(FILE *in_str);
extern void yyset_out(FILE *out_str);
extern int yyget_debug(void);
extern void yyset_debug(int bdebug);
extern int yylex_destroy(void);
extern void yyfree(void *ptr);
extern int load_object(FILE *fp, object *op, int bufstate, int map_flags);
extern int load_object_from_reader(BufferReader *reader, object *op, int map_flags);
extern int set_variable(object *op, const char *buf);
extern void free_loader(void);
