/**
 * Those are dummy functions defined to resolve all symboles.
 * Added as part of glue cleaning.
 * Ryo 2005-07-15
 **/
#include <global.h>
#include <sproto.h>

void apply_auto_fix(mapstruct *m) {
}

void draw_ext_info(int flags, int pri, const object *pl, uint8_t type, uint8_t subtype, const char *txt) {
    fprintf(logfile, "%s\n", txt);
}

void draw_ext_info_format(int flags, int pri, const object *pl, uint8_t type, uint8_t subtype, const char *format, ...) {
    va_list ap;

    va_start(ap, format);
    vfprintf(logfile, format, ap);
    va_end(ap);
}

void ext_info_map(int color, const mapstruct *map, uint8_t type, uint8_t subtype, const char *str1) {
    fprintf(logfile, "ext_info_map: %s\n", str1);
}

void move_firewall(object *ob) {
}

void emergency_save(int x) {
}

void clean_tmp_files(void) {
}

void esrv_send_item(object *ob, object *obx) {
}

void esrv_update_item(int flags, object *pl, object *op) {
}

void dragon_ability_gain(object *ob, int x, int y) {
}

void set_darkness_map(mapstruct *m) {
}

object *find_skill_by_number(object *who, int skillno) {
    return NULL;
}

void esrv_del_item(player *pl, object *ob) {
}

void esrv_update_spells(player *pl) {
}

void rod_adjust(object *rod) {
}
