/* altar/altar.c */
void init_type_altar(void);
/* arrow/arrow.c */
void init_type_arrow(void);
/* book/book.c */
void init_type_book(void);
/* button/button.c */
void init_type_button(void);
/* check_inv/check_inv.c */
void init_type_check_inv(void);
/* clock/clock.c */
void init_type_clock(void);
/* container/container.c */
void init_type_container(void);
/* converter/converter.c */
void init_type_converter(void);
/* deep_swamp/deep_swamp.c */
void init_type_deep_swamp(void);
/* director/director.c */
void init_type_director(void);
/* exit/exit.c */
void init_type_exit(void);
/* hole/hole.c */
void init_type_hole(void);
/* identify_altar/identify_altar.c */
void init_type_identify_altar(void);
/* lighter/lighter.c */
void init_type_lighter(void);
/* pedestal/pedestal.c */
void init_type_pedestal(void);
/* player_mover/player_mover.c */
void init_type_player_mover(void);
/* power_crystal/power_crystal.c */
void init_type_power_crystal(void);
/* rune/rune.c */
void init_type_rune(void);
/* shop_mat/shop_mat.c */
void init_type_shop_mat(void);
/* sign/sign.c */
void init_type_sign(void);
/* spell_effect/spell_effect.c */
void init_type_spell_effect(void);
/* spinner/spinner.c */
void init_type_spinner(void);
/* thrown_object/thrown_object.c */
void init_type_thrown_object(void);
/* transport/transport.c */
void init_type_transport(void);
/* trap/trap.c */
void init_type_trap(void);
/* trapdoor/trapdoor.c */
void init_type_trapdoor(void);
/* trigger/trigger.c */
void init_type_trigger(void);
/* trigger_altar/trigger_altar.c */
void init_type_trigger_altar(void);
/* trigger_button/trigger_button.c */
void init_type_trigger_button(void);
/* trigger_pedestal/trigger_pedestal.c */
void init_type_trigger_pedestal(void);
/* common/common_apply.c */
method_ret common_ob_move_on(ob_methods *context, object *trap, object *victim, object *originator);
method_ret common_pre_ob_move_on(object *trap, object *victim, object *originator);
void common_post_ob_move_on(object *trap, object *victim, object *originator);
/* common/describe.c */
void common_ob_describe(const ob_methods *context, const object *op, const object *observer, char *buf, int size);
/* common/projectile.c */
void stop_projectile(object *op);
method_ret common_process_projectile(ob_methods *context, object *op);
method_ret common_projectile_move_on(ob_methods *context, object *trap, object *victim, object *originator);
/* legacy/apply.c */
method_ret legacy_ob_apply(ob_methods *context, object *op, object *applier, int aflags);
/* legacy/legacy_describe.c */
void legacy_ob_describe(const ob_methods *context, const object *op, const object *observer, char *buf, int size);
/* legacy/process.c */
method_ret legacy_ob_process(ob_methods *context, object *op);
