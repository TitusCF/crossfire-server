/* book/book.c */
void init_type_book(void);
method_ret book_type_apply(ob_methods *context, object *op, object *applier, int aflags);
/* item_transformer/item_transformer.c */
void init_type_item_transformer(void);
method_ret item_transformer_type_apply(ob_methods *context, object *op, object *applier, int aflags);
/* power_crystal/power_crystal.c */
void init_type_power_crystal(void);
method_ret power_crystal_type_apply(ob_methods *context, object *op, object *applier, int aflags);
/* transport/transport.c */
void init_type_transport(void);
method_ret transport_type_apply(ob_methods *context, object *op, object *applier, int aflags);
method_ret transport_type_process(ob_methods *context, object *op);
/* trigger/trigger.c */
void init_type_trigger(void);
method_ret trigger_type_apply(ob_methods *context, object *op, object *applier, int aflags);
method_ret trigger_type_process(ob_methods *context, object *op);
/* common/describe.c */
const char *common_ob_describe(ob_methods *context, object *op, object *observer);
/* legacy/apply.c */
method_ret legacy_ob_apply(ob_methods *context, object *op, object *applier, int aflags);
/* legacy/legacy_describe.c */
const char *legacy_ob_describe(ob_methods *context, object *op, object *observer);
/* legacy/process.c */
method_ret legacy_ob_process(ob_methods *context, object *op);
