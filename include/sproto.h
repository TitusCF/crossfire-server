/* account.c */
void accounts_clear(void);
void accounts_load(void);
void accounts_save(void);
const char *account_exists(const char *account_name);
int account_login(const char *account_name, const char *account_password);
int account_check_string(const char *str);
int account_new(const char *account_name, const char *account_password);
int account_link(const char *account_name, const char *player_name);
int account_remove_player(const char *account_name, const char *player_name);
char **account_get_players_for_account(const char *account_name);
linked_char *account_get_additional_chars(const char *account_name, const Account_Char *chars, int *count);
const char *account_get_account_for_char(const char *charname);
player *account_get_logged_in_player(const char *name);
socket_struct *account_get_logged_in_init_socket(const char *name);
int account_is_logged_in(const char *name);
int account_change_password(const char *account_name, const char *current_password, const char *new_password);
/* account_char.c */
Account_Char *account_char_load(const char *account_name);
void account_char_save(const char *account, Account_Char *chars);
Account_Char *account_char_add(Account_Char *chars, player *pl);
Account_Char *account_char_remove(Account_Char *chars, const char *pl_name);
void account_char_free(Account_Char *chars);
/* alchemy.c */
int use_alchemy(object *op);
/* apply.c */
int transport_can_hold(const object *transport, const object *op, int nrof);
int should_director_abort(const object *op, const object *victim);
void apply_handle_yield(object *tmp);
int set_object_face_main(object *op);
int apply_container(object *op, object *sack, int aflags);
void do_learn_spell(object *op, object *spell, int special_prayer);
void do_forget_spell(object *op, const char *spell);
int apply_manual(object *op, object *tmp, int aflag);
int apply_by_living(object *pl, object *op, int aflag, int quiet);
void apply_by_living_below(object *pl);
int apply_can_apply_object(const object *who, const object *op);
int apply_check_weapon_power(const object *who, int improves);
int apply_special(object *who, object *op, int aflags);
int apply_auto(object *op);
void apply_auto_fix(mapstruct *m);
void scroll_failure(object *op, int failure, int power);
void apply_changes_to_player(object *pl, object *change, int limit_stats);
/* attack.c */
void save_throw_object(object *op, uint32_t type, object *originator);
int slow_living_by(object *op, const int speed_penalty);
int hit_map(object *op, int dir, uint32_t type, int full_hit);
int attack_ob(object *op, object *hitter);
object *hit_with_arrow(object *op, object *victim);
int friendly_fire(object *op, object *hitter);
int hit_player(object *op, int dam, object *hitter, uint32_t type, int full_hit);
void confuse_living(object *op, object *hitter, int dam);
void blind_living(object *op, object *hitter, int dam);
void paralyze_living(object *op, int dam);
/* ban.c */
int checkbanned(const char *login, const char *host);
/* build_map.c */
void apply_builder_remove(object *pl, int dir);
void apply_map_builder(object *pl, int dir);
/* c_chat.c */
void command_say(object *op, const char *params);
void command_me(object *op, const char *params);
void command_cointoss(object *op, const char *params);
void command_orcknuckle(object *op, const char *params);
void command_shout(object *op, const char *params);
void command_chat(object *op, const char *params);
void command_tell(object *op, const char *params);
void command_dmtell(object *op, const char *params);
void command_reply(object *op, const char *params);
void command_nod(object *op, const char *params);
void command_dance(object *op, const char *params);
void command_kiss(object *op, const char *params);
void command_bounce(object *op, const char *params);
void command_smile(object *op, const char *params);
void command_cackle(object *op, const char *params);
void command_laugh(object *op, const char *params);
void command_giggle(object *op, const char *params);
void command_shake(object *op, const char *params);
void command_puke(object *op, const char *params);
void command_growl(object *op, const char *params);
void command_scream(object *op, const char *params);
void command_sigh(object *op, const char *params);
void command_sulk(object *op, const char *params);
void command_hug(object *op, const char *params);
void command_cry(object *op, const char *params);
void command_poke(object *op, const char *params);
void command_accuse(object *op, const char *params);
void command_grin(object *op, const char *params);
void command_bow(object *op, const char *params);
void command_clap(object *op, const char *params);
void command_blush(object *op, const char *params);
void command_burp(object *op, const char *params);
void command_chuckle(object *op, const char *params);
void command_cough(object *op, const char *params);
void command_flip(object *op, const char *params);
void command_frown(object *op, const char *params);
void command_gasp(object *op, const char *params);
void command_glare(object *op, const char *params);
void command_groan(object *op, const char *params);
void command_hiccup(object *op, const char *params);
void command_lick(object *op, const char *params);
void command_pout(object *op, const char *params);
void command_shiver(object *op, const char *params);
void command_shrug(object *op, const char *params);
void command_slap(object *op, const char *params);
void command_smirk(object *op, const char *params);
void command_snap(object *op, const char *params);
void command_sneeze(object *op, const char *params);
void command_snicker(object *op, const char *params);
void command_sniff(object *op, const char *params);
void command_snore(object *op, const char *params);
void command_spit(object *op, const char *params);
void command_strut(object *op, const char *params);
void command_thank(object *op, const char *params);
void command_twiddle(object *op, const char *params);
void command_wave(object *op, const char *params);
void command_whistle(object *op, const char *params);
void command_wink(object *op, const char *params);
void command_yawn(object *op, const char *params);
void command_beg(object *op, const char *params);
void command_bleed(object *op, const char *params);
void command_cringe(object *op, const char *params);
void command_think(object *op, const char *params);
/* c_misc.c */
void command_language(object *op, const char *params);
void command_body(object *op, const char *params);
void command_motd(object *op, const char *params);
void command_rules(object *op, const char *params);
void command_news(object *op, const char *params);
void command_whereabouts(object *op, const char *params);
void list_players(object *op, region *reg, partylist *party);
void command_who(object *op, const char *params);
void command_afk(object *op, const char *params);
void command_malloc(object *op, const char *params);
void command_mapinfo(object *op, const char *params);
void command_whereami(object *op, const char *params);
void command_maps(object *op, const char *params);
void command_strings(object *op, const char *params);
void command_time(object *op, const char *params);
void command_archs(object *op, const char *params);
void command_hiscore(object *op, const char *params);
void command_debug(object *op, const char *params);
void command_wizpass(object *op, const char *params);
void command_wizcast(object *op, const char *params);
void command_dumpallobjects(object *op, const char *params);
void command_dumpfriendlyobjects(object *op, const char *params);
void command_dumpallarchetypes(object *op, const char *params);
void command_ssdumptable(object *op, const char *params);
void command_dumpmap(object *op, const char *params);
void command_dumpallmaps(object *op, const char *params);
void command_printlos(object *op, const char *params);
void command_version(object *op, const char *params);
void command_listen(object *op, const char *params);
void command_statistics(object *pl, const char *params);
void command_fix_me(object *op, const char *params);
void command_players(object *op, const char *params);
void command_applymode(object *op, const char *params);
void command_bowmode(object *op, const char *params);
void command_unarmed_skill(object *op, const char *params);
void command_petmode(object *op, const char *params);
void command_showpets(object *op, const char *params);
void command_usekeys(object *op, const char *params);
void command_resistances(object *op, const char *params);
void command_help(object *op, const char *params);
void command_delete(object *op, const char *params);
void command_quit(object *op, const char *params);
void command_sound(object *op, const char *params);
void receive_player_name(object *op, const char *name);
void receive_player_password(object *op, const char *password);
void command_title(object *op, const char *params);
void command_save(object *op, const char *params);
void command_peaceful(object *op, const char *params);
void command_wimpy(object *op, const char *params);
void command_brace(object *op, const char *params);
void command_kill_pets(object *op, const char *params);
void command_passwd(object *pl, const char *params);
void do_harvest(object *pl, int dir, object *skill);
/* c_move.c */
void command_east(object *op, const char *params);
void command_north(object *op, const char *params);
void command_northeast(object *op, const char *params);
void command_northwest(object *op, const char *params);
void command_south(object *op, const char *params);
void command_southeast(object *op, const char *params);
void command_southwest(object *op, const char *params);
void command_west(object *op, const char *params);
void command_stay(object *op, const char *params);
void command_up(object *op, const char *params);
void command_down(object *op, const char *params);
void do_goto(object *op, const char *name, int x, int y);
/* c_new.c */
void command_run(object *op, const char *params);
void command_run_stop(object *op, const char *params);
void command_fire(object *op, const char *params);
void command_fire_stop(object *op, const char *params);
void command_face(object *op, const char *params);
/* c_object.c */
void command_uskill(object *pl, const char *params);
void command_rskill(object *pl, const char *params);
void command_search(object *op, const char *params);
void command_disarm(object *op, const char *params);
void command_throw(object *op, const char *params);
void command_apply(object *op, const char *params);
int sack_can_hold(const object *pl, const object *sack, const object *op, uint32_t nrof);
void pick_up(object *op, object *alt);
void command_take(object *op, const char *params);
void put_object_in_sack(object *op, object *sack, object *tmp, uint32_t nrof);
object *drop_object(object *op, object *tmp, uint32_t nrof);
void drop(object *op, object *tmp);
void command_dropall(object *op, const char *params);
void command_drop(object *op, const char *params);
void command_empty(object *op, const char *params);
void command_examine(object *op, const char *params);
object *find_marked_object(object *op);
void command_mark(object *op, const char *params);
void examine_monster(object *op, object *tmp, int level);
void examine(object *op, object *tmp);
void inventory(object *op, object *inv);
void command_pickup(object *op, const char *params);
void command_search_items(object *op, const char *params);
void command_rename_item(object *op, const char *params);
void command_lock_item(object *op, const char *params);
void command_use(object *op, const char *params);
/* c_party.c */
void receive_party_password(object *op, const char *password);
void command_gsay(object *op, const char *params);
void command_party(object *op, const char *params);
void command_party_rejoin(object *op, const char *params);
/* c_range.c */
void command_invoke(object *op, const char *params);
void command_cast(object *op, const char *params);
void command_prepare(object *op, const char *params);
void command_cast_spell(object *op, const char *params, char command);
int legal_range(object *op, int r);
void command_rotateshoottype(object *op, const char *params);
/* c_wiz.c */
void command_loadtest(object *op, const char *params);
void command_hide(object *op, const char *params);
void command_setgod(object *op, const char *params);
void command_banish(object *op, const char *params);
void command_kick(object *op, const char *params);
void command_overlay_save(object *op, const char *params);
void command_overlay_reset(object *op, const char *params);
void command_toggle_shout(object *op, const char *params);
void command_shutdown(object *op, const char *params);
void command_goto(object *op, const char *params);
void command_freeze(object *op, const char *params);
int player_arrest(object *who);
void command_arrest(object *op, const char *params);
void command_summon(object *op, const char *params);
void command_teleport(object *op, const char *params);
void command_create(object *op, const char *params);
void command_inventory(object *op, const char *params);
void command_skills(object *op, const char *params);
void command_dump(object *op, const char *params);
void command_mon_aggr(object *op, const char *params);
void command_possess(object *op, const char *params);
void command_patch(object *op, const char *params);
void command_remove(object *op, const char *params);
void command_free(object *op, const char *params);
void command_accountpasswd(object *op, const char *params);
void command_addexp(object *op, const char *params);
void command_speed(object *op, const char *params);
void command_stats(object *op, const char *params);
void command_abil(object *op, const char *params);
void command_reset(object *op, const char *params);
void command_nowiz(object *op, const char *params);
void command_dm(object *op, const char *params);
void command_invisible(object *op, const char *params);
void command_learn_spell(object *op, const char *params);
void command_learn_special_prayer(object *op, const char *params);
void command_forget_spell(object *op, const char *params);
void command_listplugins(object *op, const char *params);
void command_loadplugin(object *op, const char *params);
void command_unloadplugin(object *op, const char *params);
void command_dmhide(object *op, const char *params);
void command_stack_pop(object *op, const char *params);
void command_stack_push(object *op, const char *params);
void command_stack_list(object *op, const char *params);
void command_stack_clear(object *op, const char *params);
void command_diff(object *op, const char *params);
void command_insert_into(object *op, const char *params);
void command_style_map_info(object *op, const char *params);
void command_follow(object *op, const char *params);
void command_purge_quest(object *op, const char *param);
void command_purge_quest_definitions(object *op, const char *param);
void command_dumpabove(object *op, const char *params);
void command_dumpbelow(object *op, const char *params);
void command_settings(object *op, const char *ignored);
/* commands.c */
void commands_init(void);
void command_list(object *pl, bool is_dm);
void command_execute(object *pl, char *command);
command_registration command_register(const char *name, uint8_t type, command_function func, float time);
command_registration command_register_extra(const char *name, const char *extra, uint8_t type, command_function_extra func, float time);
void command_unregister(command_registration command);
void commands_clear();
/* disease.c */
int move_disease(object *disease);
int infect_object(object *victim, object *disease, int force);
void move_symptom(object *symptom);
void check_physically_infect(object *victim, object *hitter);
int cure_disease(object *sufferer, object *caster, sstring skill);
/* hiscore.c */
void hiscore_init(void);
void hiscore_check(object *op, int quiet);
void hiscore_display(object *op, int max, const char *match);
/* gods.c */
const object *find_god(const char *name);
const char *determine_god(object *op);
void pray_at_altar(object *pl, object *altar, object *skill);
int become_follower(object *op, const object *new_god);
archetype *determine_holy_arch(const object *god, const char *type);
int tailor_god_spell(object *spellop, object *caster);
/* init.c */
void init(int argc, char **argv);
void free_server(void);
void add_server_collect_hooks();
void close_modules();
void init_signals();
/* knowledge.c */
void knowledge_give(player *pl, const char *marker, const object *book);
void knowledge_read(player *pl, object *book);
void command_knowledge(object *pl, const char *params);
void free_knowledge(void);
int knowledge_player_knows(const player *pl, const char *knowledge);
void knowledge_item_can_be_used_alchemy(object *op, const object *item);
void knowledge_send_info(socket_struct *ns);
void knowledge_send_known(player *pl);
void knowledge_first_player_save(player *pl);
void knowledge_process_incremental(void);
void knowledge_show_monster_detail(object *op, const char *name);
/* login.c */
void emergency_save(int flag);
void delete_character(const char *name);
int verify_player(const char *name, char *password);
int check_name(player *me, const char *name);
void destroy_object(object *op);
int save_player(object *op, int flag);
void check_login(object *op, const char *password);
/* monster.c */
object *monster_check_enemy(object *npc, rv_vector *rv);
object *monster_find_nearest_living_creature(object *npc);
int monster_compute_path(object *source, object *target, int default_dir);
void monster_do_living(object *op);
int monster_move(object *op);
void monster_check_apply_all(object *monster);
void monster_npc_call_help(object *op);
void monster_check_earthwalls(object *op, mapstruct *m, int x, int y);
void monster_check_doors(object *op, mapstruct *m, int x, int y);
void monster_do_say(const mapstruct *map, const char *message);
void monster_communicate(object *op, const char *txt);
void monster_npc_say(object *npc, const char *cp);
object *monster_find_throw_ob(object *op);
int monster_can_detect_enemy(object *op, object *enemy, rv_vector *rv);
int monster_stand_in_light(object *op);
int monster_can_see_enemy(object *op, object *enemy);
const char *get_reply_text_own(reply_type rt);
/* move.c */
int move_object(object *op, int dir);
int move_ob(object *op, int dir, object *originator);
int transfer_ob(object *op, int x, int y, int randomly, object *originator);
int teleport(object *teleporter, uint8_t tele_type, object *user);
void recursive_roll(object *op, int dir, object *pusher);
int push_ob(object *who, int dir, object *pusher);
int move_to(object *op, int x, int y);
int object_teleport(object *op, mapstruct *map, int x, int y);
/* ob_methods.c */
void init_ob_methods(void);
/* ob_types.c */
void register_all_ob_types(void);
/* party.c */
partylist *party_form(object *op, const char *partyname);
void party_join(object *op, partylist *party);
void party_leave(object *op);
partylist *party_find(const char *partyname);
void party_remove(partylist *party);
partylist *party_get_first(void);
partylist *party_get_next(const partylist *party);
void party_obsolete_parties(void);
const char *party_get_password(const partylist *party);
void party_set_password(partylist *party, const char *password);
int party_confirm_password(const partylist *party, const char *password);
void party_send_message(object *op, const char *message);
const char *party_get_leader(const partylist *party);
/* pets.c */
object *pets_get_enemy(object *pet, rv_vector *rv);
void pets_terminate_all(object *owner);
void pets_remove_all(void);
void pets_follow_owner(object *ob, object *owner);
void pets_move(object *ob);
void pets_move_golem(object *op);
void pets_control_golem(object *op, int dir);
int pets_summon_golem(object *op, object *caster, int dir, object *spob);
int pets_summon_object(object *op, object *caster, object *spell_ob, int dir, const char *stringarg);
int pets_should_arena_attack(object *pet, object *owner, object *target);
/* player.c */
player *find_player(const char *plname);
player *find_player_options(const char *plname, int options, const mapstruct *map);
player *find_player_partial_name(const char *plname);
player *find_player_socket(const socket_struct *ns);
void display_motd(const object *op);
void send_rules(const object *op);
void send_news(const object *op);
int playername_ok(const char *cp);
player *get_player(player *p);
void set_first_map(object *op);
player *add_player(socket_struct *ns, int flags);
object *get_nearest_player(object *mon);
int path_to_player(object *mon, object *pl, unsigned mindiff);
void give_initial_items(object *pl, treasurelist *items);
void get_name(object *op);
void get_password(object *op);
void play_again(object *op);
void receive_play_again(object *op, char key);
void confirm_password(object *op);
int get_party_password(object *op, partylist *party);
int roll_stat(void);
void roll_stats(object *op);
void roll_again(object *op);
void key_roll_stat(object *op, char key);
void key_change_class(object *op, char key);
int check_race_and_class(living *stats, archetype *race, archetype *opclass);
int apply_race_and_class(object *op, archetype *race, archetype *opclass, living *stats);
void key_confirm_quit(object *op, char key);
int check_pick(object *op);
int fire_bow(object *op, object *arrow, int dir, int wc_mod, int16_t sx, int16_t sy);
void fire(object *op, int dir);
object *find_key(object *pl, object *container, object *door);
void move_player_attack(object *op, int dir);
int move_player(object *op, int dir);
int face_player(object *op, int dir);
int handle_newcs_player(object *op);
void remove_unpaid_objects(object *op, object *env, int free_items);
void do_some_living(object *op);
void kill_player(object *op, const object *killer);
void fix_weight(void);
void fix_luck(void);
void cast_dust(object *op, object *throw_ob, int dir);
void make_visible(object *op);
int is_true_undead(object *op);
int hideability(object *ob);
void do_hidden_move(object *op);
int stand_near_hostile(object *who);
int player_can_view(object *pl, object *op);
int op_on_battleground(object *op, int *x, int *y, archetype **trophy);
void dragon_ability_gain(object *who, int atnr, int level);
void player_unready_range_ob(player *pl, object *ob);
void player_set_state(player *pl, uint8_t state);
SockList *player_get_delayed_buffer(player *pl);
/* plugins.c */
int plugins_init_plugin(const char *libfile);
int plugins_remove_plugin(const char *id);
void plugins_display_list(object *op);
void initPlugins(void);
void cleanupPlugins(void);
/* quest.c */
int quest_get_player_state(player *pl, sstring quest_code);
void quest_load_definitions(void);
void quest_start(player *pl, sstring quest_code, int state);
void quest_set_player_state(player *pl, sstring quest_code, int state);
int quest_was_completed(player *pl, sstring quest_code);
void command_quest(object *op, const char *params);
void dump_quests(void);
void free_quest(void);
void free_quest_definitions(void);
void quest_send_initial_states(player *pl);
void quest_first_player_save(player *pl);
/* resurrection.c */
int cast_raise_dead_spell(object *op, object *caster, object *spell, int dir, const char *arg);
void dead_player(object *op);
/* rune.c */
int write_rune(object *op, object *caster, object *spell, int dir, const char *runename);
void spring_trap(object *trap, object *victim);
int dispel_rune(object *op, object *skill, int dir);
int trap_see(object *op, object *trap);
int trap_show(object *trap, object *where);
int trap_disarm(object *disarmer, object *trap, int risk, object *skill);
/* skills.c */
int steal(object *op, int dir, object *skill);
int pick_lock(object *pl, int dir, object *skill);
int hide(object *op, object *skill);
int jump(object *pl, int dir, object *skill);
int detect_curse_on_item(object *pl, object *tmp, object *skill);
int detect_magic_on_item(object *pl, object *tmp, object *skill);
int identify_object_with_skill(object *tmp, object *pl, object *skill, int print_on_success);
int skill_ident(object *pl, object *skill);
int use_oratory(object *pl, int dir, object *skill);
int singing(object *pl, int dir, object *skill);
int find_traps(object *pl, object *skill);
int remove_trap(object *op, object *skill);
int pray(object *pl, object *skill);
void meditate(object *pl, object *skill);
int write_on_item(object *pl, const char *params, object *skill);
int skill_throw(object *op, object *part, int dir, object *skill);
/* skill_util.c */
void init_skills(void);
object *find_skill_by_name(object *who, const char *name);
object *find_applied_skill_by_name(const object* op, const char* name);
object *find_skill_by_number(object *who, int skillno);
int change_skill(object *who, object *new_skill, int flag);
void clear_skill(object *who);
int do_skill(object *op, object *part, object *skill, int dir, const char *string);
int64_t calc_skill_exp(const object *who, const object *op, const object *skill);
int learn_skill(object *pl, object *scroll);
void show_skills(object *op, const char *search);
int use_skill(object *op, const char *string);
void skill_attack(object *tmp, object *pl, int dir, const char *string, object *skill);
/* spell_attack.c */
int fire_bolt(object *op, object *caster, int dir, object *spob);
void explode_bullet(object *op);
void check_bullet(object *op);
void cone_drop(object *op);
int cast_cone(object *op, object *caster, int dir, object *spell);
int create_bomb(object *op, object *caster, int dir, object *spell);
int cast_smite_spell(object *op, object *caster, int dir, object *spell);
int cast_destruction(object *op, object *caster, object *spell_ob);
int cast_curse(object *op, object *caster, object *spell_ob, int dir);
int mood_change(object *op, object *caster, object *spell);
int fire_swarm(object *op, object *caster, object *spell, int dir);
int cast_light(object *op, object *caster, object *spell, int dir);
int cast_cause_disease(object *op, object *caster, object *spell, int dir);
/* spell_effect.c */
void cast_magic_storm(object *op, object *tmp, int lvl);
int recharge(object *op, object *caster, object *spell_ob);
void polymorph(object *op, object *who, int level);
int cast_polymorph(object *op, object *caster, object *spell_ob, int dir);
int cast_create_missile(object *op, object *caster, object *spell, int dir, const char *stringarg);
int cast_create_food(object *op, object *caster, object *spell_ob, int dir, const char *stringarg);
int probe(object *op, object *caster, object *spell_ob, int dir, int level);
int makes_invisible_to(object *pl, object *mon);
int cast_invisible(object *op, object *caster, object *spell_ob);
int cast_earth_to_dust(object *op, object *caster, object *spell_ob);
int cast_word_of_recall(object *op, object *caster, object *spell_ob);
int cast_word_of_penalty(object *op, object *caster, object *spell_ob);
int cast_wonder(object *op, object *caster, int dir, object *spell_ob);
int perceive_self(object *op);
int cast_create_town_portal(object *op, object *caster, object *spell, int dir, bool new_town_portal);
int magic_wall(object *op, object *caster, int dir, object *spell_ob);
int dimension_door(object *op, object *caster, object *spob, int dir);
int cast_heal(object *op, object *caster, object *spell, int dir);
int cast_change_ability(object *op, object *caster, object *spell_ob, int dir, int silent);
int cast_bless(object *op, object *caster, object *spell_ob, int dir);
int alchemy(object *op, object *caster, object *spell_ob);
int remove_curse(object *op, object *caster, object *spell);
int cast_item_curse_or_curse(object *op, object *caster, object *spell_ob);
int cast_identify(object *op, object *caster, object *spell);
int cast_detection(object *op, object *caster, object *spell);
int cast_transfer(object *op, object *caster, object *spell, int dir);
void counterspell(object *op, int dir);
int cast_consecrate(object *op, object *caster, object *spell);
int animate_weapon(object *op, object *caster, object *spell, int dir);
int cast_change_map_lightlevel(object *op, object *caster, object *spell);
int create_aura(object *op, object *caster, object *spell);
int write_mark(object *op, object *spell, const char *msg);
/* spell_util.c */
object *find_random_spell_in_ob(object *ob, const char *skill);
void set_spell_skill(object *op, object *caster, object *spob, object *dest);
void dump_spells(void);
void spell_effect(object *spob, int x, int y, mapstruct *map, object *originator);
int min_casting_level(const object *caster, const object *spell);
int caster_level(const object *caster, const object *spell);
int16_t SP_level_spellpoint_cost(object *caster, object *spell, int flags);
int SP_level_dam_adjust(const object *caster, const object *spob);
int SP_level_duration_adjust(const object *caster, const object *spob);
int SP_level_range_adjust(const object *caster, const object *spob);
int SP_level_wc_adjust(const object *caster, const object *spob);
object *check_spell_known(object *op, const char *name);
object *lookup_spell_by_name(object *op, const char *spname);
int reflwall(mapstruct *m, int x, int y, object *sp_op);
int cast_create_obj(object *op, object *new_op, int dir);
int ok_to_put_more(mapstruct *m, int16_t x, int16_t y, object *op, uint32_t immune_stop);
int fire_arch_from_position(object *op, object *caster, int16_t x, int16_t y, int dir, object *spell);
void regenerate_rod(object *rod);
void drain_rod_charge(object *rod);
void drain_wand_charge(object *wand);
object *find_target_for_friendly_spell(object *op, int dir);
int spell_find_dir(mapstruct *m, int x, int y, object *exclude);
int summon_hostile_monsters(object *op, int n, const char *monstername);
void shuffle_attack(object *op);
void spell_failure(object *op, int failure, int power, object *skill);
int cast_spell(object *op, object *caster, int dir, object *spell_ob, char *stringarg);
void store_spell_expiry(object *spell);
void check_spell_expiry(object *spell);
void rod_adjust(object *rod);
/* swap.c */
void read_map_log(void);
int swap_map(mapstruct *map);
void check_active_maps(void);
int players_on_map(mapstruct *m, int show_all);
void flush_old_maps(void);
/* time.c */
void remove_door(object *op);
void remove_locked_door(object *op);
object *stop_item(object *op);
void fix_stopped_item(object *op, mapstruct *map, object *originator);
object *fix_stopped_arrow(object *op);
int free_no_drop(object *op);
void change_object(object *op);
void move_firewall(object *op);
void move_player_mover(object *op);
int process_object(object *op);
void legacy_remove_force(object *op);
void legacy_animate_trigger(object *op);
void legacy_move_hole(object *op);
unsigned int tick_length(float seconds);
long timespec_diff(struct timespec *end, struct timespec *start);
/* timers.c */
void cftimer_process_timers(void);
int cftimer_create(int id, long delay, object *ob, int mode);
int cftimer_destroy(int id);
int cftimer_find_free_id(void);
void cftimer_init(void);
/* weather.c */
void set_darkness_map(mapstruct *m);
void tick_the_clock(void);
/* server.c */
char const* newhash(char const *password);
bool check_password(const char *typed, const char *crypted);
void enter_player_savebed(object *op);
void set_map_timeout(mapstruct *oldmap);
void enter_exit(object *op, object *exit_ob);
void process_events(void);
void clean_tmp_files(void);
void cleanup(void);
void leave(player *pl, int draw_exit);
int forbid_play(void);
void server_main(int argc, char *argv[]);
/* race.cpp */
#ifdef __cplusplus
extern "C" {
#endif
void load_races(BufferReader *reader, const char *filename);
void finish_races();
void dump_races(void);
void free_races(void);
object *races_get_random_monster(const char *race, int level);
#ifdef __cplusplus
}
#endif


/**
 * Move a player to its stored map level. This function is used to place the
 * player just after logging in. Since the map may no longer exist, dump the
 * player to an alternative 'emergency' location if that's the case.
 */
void enter_player_maplevel(object *op);
