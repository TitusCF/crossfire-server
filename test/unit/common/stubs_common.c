/**
 * Those are dummy functions defined to resolve all symboles.
 * Added as part of glue cleaning.
 * Ryo 2005-07-15
 **/
#include <global.h>

void fix_auto_apply(mapstruct * m)
{
}

void draw_ext_info(int flags, int pri, const object *pl, uint8 type, uint8 subtype, const char *txt, const char *txt2)
    {
    fprintf(logfile, "%s\n", txt);
    }

void draw_ext_info_format(
        int flags, int pri, const object *pl, uint8 type,
        uint8 subtype,
        const char* new_format,
        const char* old_format,
        ...)
{
    va_list ap;
    va_start(ap, old_format);
    vfprintf(logfile, old_format, ap);
    va_end(ap);
}

void ext_info_map(int color, const mapstruct *map, uint8 type, uint8 subtype, const char *str1, const char *str2)
{
    fprintf(logfile, "ext_info_map: %s\n", str2);
}

void move_teleporter( object* ob)
    {
    }

void move_firewall( object* ob)
    {
    }

void move_duplicator( object* ob)
    {
    }

void move_marker( object* ob)
    {
    }

void move_creator( object* ob)
    {
    }

void emergency_save( int x )
    {
    }

void clean_tmp_files( void )
    {
    }

void esrv_send_item( object* ob, object* obx )
    {
    }

void esrv_update_item(int flags, object *pl, object *op)
{
}

void dragon_ability_gain( object* ob, int x, int y )
    {
    }

void weather_effect(mapstruct *m)
    {
    }

void set_darkness_map( mapstruct* m)
    {
    }

int ob_move_on(object* op, object* victim, object* originator)
    {
    }

object* find_skill_by_number(object *who, int skillno)
    {
    return NULL;
    }

void esrv_del_item(player *pl, int tag)
    {
    }

void esrv_update_spells(player *pl)
    {
    }

void monster_check_apply( object* ob, object* obt )
    {
    }

void trap_adjust( object* ob, int x )
    {
    }

int execute_event(object* op, int eventcode, object* activator, object* third, const char* message, int fix)
    {
    return 0;
    }

int execute_global_event(int eventcode, ...)
    {
    return 0;
    }
