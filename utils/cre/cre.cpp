#include <Qt>
#include <QApplication>
#include <QCoreApplication>

#include <CREMainWindow.h>
#include "CRESettings.h"

#include "CREPixmap.h"

#include "CREFilterDefinition.h"
#include "CREFilterDefinitionManager.h"
#include "CREReportDefinition.h"
#include "CREReportDefinitionManager.h"

int main(int argc, char **argv) {
    QCoreApplication::setOrganizationName("The Legendary Team of Ailesse");
    QCoreApplication::setApplicationName("CRE");
    QApplication app(argc, argv);

    qRegisterMetaTypeStreamOperators<CREFilterDefinition>("CREFilterDefinition");
    qRegisterMetaTypeStreamOperators<CREFilterDefinitionManager>("CREFilterDefinitionManager");
    qRegisterMetaTypeStreamOperators<CREReportDefinition>("CREReportDefinition");
    qRegisterMetaTypeStreamOperators<CREReportDefinitionManager>("CREReportDefinitionManager");

    CREPixmap::init();

    CRESettings settings;
    if (!settings.ensureOptions())
        return -1;

    CREMainWindow win;
    win.show();

    return app.exec();
}

#ifndef DOXYGEN_SHOULD_SKIP_THIS

extern "C"
{

/**
 * Dummy functions to link the library.
 */

void draw_ext_info(int, int, const object *, uint8, uint8, const char *txt) {
    fprintf(logfile, "%s\n", txt);
}

void draw_ext_info_format(
                          int, int, const object *, uint8,
                          uint8,
                          const char *format,
                          ...) {
    va_list ap;
    va_start(ap, format);
    vfprintf(logfile, format, ap);
    va_end(ap);
}

void ext_info_map(int, const mapstruct *, uint8, uint8, const char *str1) {
    fprintf(logfile, "ext_info_map: %s\n", str1);
}

void move_firewall(object *) {
}

void emergency_save(int) {
}

void clean_tmp_files(void) {
}

void esrv_send_item(object *, object *) {
}

void dragon_ability_gain(object *, int, int) {
}

void set_darkness_map(mapstruct *) {
}

object *find_skill_by_number(object *, int) {
    return NULL;
}

void esrv_del_item(player *, int) {
}

void esrv_update_item(int, object *, object *) {
}

void esrv_update_spells(player *) {
}

void monster_check_apply(object *, object *) {
}

void trap_adjust(object *, int) {
}

int execute_event(object *, int , object *, object *, const char *, int) {
    return 0;
}

int execute_global_event(int , ...) {
    return 0;
}

int apply_auto(object *op) {
    int i;
    object* tmp;

    switch(op->type) {
        case SHOP_FLOOR:
            if (!HAS_RANDOM_ITEMS(op))
                return 0;
            do {
                i = 10; /* let's give it 10 tries */
                while ((tmp = generate_treasure(op->randomitems, op->stats.exp ? (int)op->stats.exp : MAX(op->map->difficulty, 5))) == NULL && --i)
                    ;
                if (tmp == NULL)
                    return 0;
                if (QUERY_FLAG(tmp, FLAG_CURSED) || QUERY_FLAG(tmp, FLAG_DAMNED)) {
                    object_free_drop_inventory(tmp);
                    tmp = NULL;
                }
            } while(!tmp);
            SET_FLAG(tmp, FLAG_UNPAID);
            object_insert_in_map_at(tmp, op->map, NULL, 0, op->x, op->y);
            CLEAR_FLAG(op, FLAG_AUTO_APPLY);
            identify(tmp);
            break;

        case TREASURE:
            if (QUERY_FLAG(op,FLAG_IS_A_TEMPLATE))
                return 0;
            while ((op->stats.hp--) > 0)
                create_treasure(op->randomitems, op, op->map ? GT_ENVIRONMENT : 0, op->stats.exp ? (int)op->stats.exp : op->map == NULL ? 14 : op->map->difficulty, 0);

        /* If we generated an object and put it in this object inventory,
            * move it to the parent object as the current object is about
            * to disappear.  An example of this item is the random_ *stuff
            * that is put inside other objects.
        */
            FOR_INV_PREPARE(op, tmp) {
                object_remove(tmp);
                if (op->env)
                    object_insert_in_ob(tmp, op->env);
                else
                    object_free_drop_inventory(tmp);
            } FOR_INV_FINISH();
            object_remove(op);
            object_free_drop_inventory(op);
            break;
    }
    return tmp ? 1 : 0;
}

void apply_auto_fix(mapstruct*)
{
}

void rod_adjust(object *) {
}


}
#endif
