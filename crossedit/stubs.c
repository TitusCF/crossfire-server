/**
 * Those are dummy functions defined to resolve all symboles.
 * Added as part of glue cleaning.
 * Ryo 2005-07-15
 **/
#include <global.h>

void fix_auto_apply(mapstruct * m)
{
}

void new_draw_info(int a, int b, const object *ob, const char *txt)
    {
    fprintf(logfile, "%s\n", txt);
    }
void new_info_map(int color, const mapstruct *map, const char *str)
    {
    fprintf(logfile, "new_info_map: %s\n", str);
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
 
void clean_tmp_files( )
    {
    }
 
void esrv_send_item( object* ob, object* obx )
    {
    }
 
void dragon_ability_gain( object* ob, int x, int y )
    {
    }
 
void weather_effect( const char* c )
    {
    }
 
void set_darkness_map( mapstruct* m)
    {
    }
 
void move_apply( object* ob, object* obt, object* obx )
    {
    }
 
object* find_skill_by_number( object* ob, int x )
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
