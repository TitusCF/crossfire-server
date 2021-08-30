/**
 * Those are dummy functions defined to resolve all symboles.
 * Added as part of glue cleaning.
 * Ryo 2005-07-15
 **/

#include <global.h>
#include <sproto.h>

void apply_auto_fix(mapstruct *m) {
    (void)m;
}

void draw_ext_info(int flags, int pri, const object *pl, uint8_t type, uint8_t subtype, const char *message) {
    (void)flags;
    (void)pri;
    (void)pl;
    (void)type;
    (void)subtype;
    fprintf(logfile, "%s\n", message);
}

void move_firewall(object *ob) {
    (void)ob;
}

void emergency_save(int x) {
    (void)x;
}

void clean_tmp_files(void) {
}

void esrv_send_item(object *ob, object *obx) {
    (void)ob;
    (void)obx;
}

void dragon_ability_gain(object *ob, int x, int y) {
    (void)ob;
    (void)x;
    (void)y;
}

void set_darkness_map(mapstruct *m) {
    (void)m;
}

object *find_skill_by_number(object *who, int skillno) {
    (void)who;
    (void)skillno;
    return NULL;
}

void esrv_del_item(player *pl, object *ob) {
    (void)pl;
    (void)ob;
}

void esrv_update_spells(player *pl) {
    (void)pl;
}

void rod_adjust(object *rod) {
    (void)rod;
}
