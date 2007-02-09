/* altar/altar.c */
void init_type_altar(void);
method_ret altar_type_move_on(ob_methods *context, object *trap, object *victim, object *originator);
/* arrow/arrow.c */
void init_type_arrow(void);
method_ret arrow_type_move_on(ob_methods *context, object *trap, object *victim, object *originator);
/* book/book.c */
void init_type_book(void);
method_ret book_type_apply(ob_methods *context, object *op, object *applier, int aflags);
/* button/button.c */
void init_type_button(void);
method_ret button_type_move_on(ob_methods *context, object *trap, object *victim, object *originator);
/* check_inv/check_inv.c */
void init_type_check_inv(void);
method_ret check_inv_type_move_on(ob_methods *context, object *trap, object *victim, object *originator);
/* container/container.c */
void init_type_container(void);
method_ret container_type_move_on(ob_methods *context, object *trap, object *victim, object *originator);
/* converter/converter.c */
void init_type_converter(void);
method_ret converter_type_move_on(ob_methods *context, object *trap, object *victim, object *originator);
/* deep_swamp/deep_swamp.c */
void init_type_deep_swamp(void);
method_ret deep_swamp_type_process(ob_methods *context, object *op);
method_ret deep_swamp_type_move_on(ob_methods *context, object *trap, object *victim, object *originator);
/* director/director.c */
void init_type_director(void);
method_ret director_type_move_on(ob_methods *context, object *trap, object *victim, object *originator);
/* exit/exit.c */
void init_type_exit(void);
method_ret exit_type_move_on(ob_methods *context, object *trap, object *victim, object *originator);
/* hole/hole.c */
void init_type_hole(void);
method_ret hole_type_move_on(ob_methods *context, object *trap, object *victim, object *originator);
/* identify_altar/identify_altar.c */
void init_type_identify_altar(void);
method_ret identify_altar_type_move_on(ob_methods *context, object *altar, object *money, object *originator);
/* item_transformer/item_transformer.c */
void init_type_item_transformer(void);
method_ret item_transformer_type_apply(ob_methods *context, object *op, object *applier, int aflags);
/* pedestal/pedestal.c */
void init_type_pedestal(void);
method_ret pedestal_type_move_on(ob_methods *context, object *trap, object *victim, object *originator);
/* player_mover/player_mover.c */
void init_type_player_mover(void);
method_ret player_mover_type_move_on(ob_methods *context, object *trap, object *victim, object *originator);
/* power_crystal/power_crystal.c */
void init_type_power_crystal(void);
method_ret power_crystal_type_apply(ob_methods *context, object *op, object *applier, int aflags);
/* rune/rune.c */
void init_type_rune(void);
method_ret rune_type_move_on(ob_methods *context, object *trap, object *victim, object *originator);
/* shop_mat/shop_mat.c */
void init_type_shop_mat(void);
method_ret shop_mat_type_move_on(ob_methods *context, object *trap, object *victim, object *originator);
/* sign/sign.c */
void init_type_sign(void);
method_ret sign_type_apply(ob_methods *context, object *op, object *applier, int aflags);
method_ret sign_type_move_on(ob_methods *context, object *trap, object *victim, object *originator);
/* spell_effect/spell_effect.c */
void init_type_spell_effect(void);
method_ret spell_effect_type_move_on(ob_methods *context, object *trap, object *victim, object *originator);
/* spinner/spinner.c */
void init_type_spinner(void);
method_ret spinner_type_move_on(ob_methods *context, object *trap, object *victim, object *originator);
/* thrown_object/thrown_object.c */
void init_type_thrown_object(void);
method_ret thrown_object_type_move_on(ob_methods *context, object *trap, object *victim, object *originator);
/* transport/transport.c */
void init_type_transport(void);
method_ret transport_type_apply(ob_methods *context, object *op, object *applier, int aflags);
method_ret transport_type_process(ob_methods *context, object *op);
/* trap/trap.c */
void init_type_trap(void);
method_ret trap_type_move_on(ob_methods *context, object *trap, object *victim, object *originator);
/* trapdoor/trapdoor.c */
void init_type_trapdoor(void);
method_ret trapdoor_type_move_on(ob_methods *context, object *trap, object *victim, object *originator);
/* trigger/trigger.c */
void init_type_trigger(void);
method_ret trigger_type_apply(ob_methods *context, object *op, object *applier, int aflags);
method_ret trigger_type_process(ob_methods *context, object *op);
/* trigger_altar/trigger_altar.c */
void init_type_trigger_altar(void);
method_ret trigger_altar_type_move_on(ob_methods *context, object *trap, object *victim, object *originator);
/* trigger_button/trigger_button.c */
void init_type_trigger_button(void);
method_ret trigger_button_type_move_on(ob_methods *context, object *trap, object *victim, object *originator);
/* trigger_pedestal/trigger_pedestal.c */
void init_type_trigger_pedestal(void);
method_ret trigger_pedestal_type_move_on(ob_methods *context, object *trap, object *victim, object *originator);
/* common/common_apply.c */
method_ret common_ob_move_on(ob_methods *context, object *trap, object *victim, object *originator);
method_ret common_pre_ob_move_on(object *trap, object *victim, object *originator);
void common_post_ob_move_on(object *trap, object *victim, object *originator);
/* common/describe.c */
void common_ob_describe(const ob_methods *context, const object *op, const object *observer, char* buf, int size);
/* legacy/apply.c */
method_ret legacy_ob_apply(ob_methods *context, object *op, object *applier, int aflags);
/* legacy/legacy_describe.c */
void legacy_ob_describe(const ob_methods *context, const object *op, const object *observer, char* buf, int size);
/* legacy/process.c */
method_ret legacy_ob_process(ob_methods *context, object *op);
