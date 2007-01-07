/*****************************************************************************/
/* Newspaper plugin version 1.0 alpha.                                       */
/* Contact:                                      */
/*****************************************************************************/
/* That code is placed under the GNU General Public Licence (GPL)            */
/* (C)2007 by Weeger Nicolas (Feel free to deliver your complaints)          */
/*****************************************************************************/
/*  CrossFire, A Multiplayer game for X-windows                              */
/*                                                                           */
/*  Copyright (C) 2000 Mark Wedel                                            */
/*  Copyright (C) 1992 Frank Tore Johansen                                   */
/*                                                                           */
/*  This program is free software; you can redistribute it and/or modify     */
/*  it under the terms of the GNU General Public License as published by     */
/*  the Free Software Foundation; either version 2 of the License, or        */
/*  (at your option) any later version.                                      */
/*                                                                           */
/*  This program is distributed in the hope that it will be useful,          */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of           */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            */
/*  GNU General Public License for more details.                             */
/*                                                                           */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program; if not, write to the Free Software              */
/*  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.                */
/*                                                                           */ /*****************************************************************************/

/* First let's include the header file needed                                */

#include <cflogger.h>
//#include <stdarg.h>

#include <sqlite3.h>

f_plug_api gethook;
f_plug_api registerGlobalEvent;
f_plug_api unregisterGlobalEvent;
f_plug_api systemDirectory;
f_plug_api reCmp;

sqlite3* database;
int last_stored_day = -1;

static int check_tables_callback(void *param, int argc, char **argv, char **azColName) {
    int* format = (int*)param;
    *format = atoi( argv[0] );
    return 0;
}

void do_sql(const char* sql) {
    int err;
    char* msg;

    if (!database)
        return;

    err = sqlite3_exec(database, sql, NULL, NULL, &msg);
    if (err != SQLITE_OK) {
        cf_log(llevError, " [%s] error: %d [%s] for sql = %s\n", PLUGIN_NAME, err, msg, sql);
        sqlite3_free(msg);
    }
}

void check_tables() {
    int format;
    int err;
    format = 0;
    err = sqlite3_exec(database, "select param_value from parameters where param_name = 'version';", check_tables_callback, &format, NULL);

    if (format < 1) {
        do_sql("create table living( liv_id integer primary key autoincrement, liv_name text, liv_is_player integer, liv_level integer );");
        do_sql("create table region( reg_id integer primary key autoincrement, reg_name text );");
        do_sql("create table map( map_id integer primary key autoincrement, map_path text, map_reg_id integer );");
        do_sql("create table time( time_real integer, time_ingame text );" );

        do_sql("create table living_event( le_liv_id integer, le_time integer, le_code integer, le_map_id integer );");
        do_sql("create table map_event( me_map_id integer, me_time integer, me_code integer, me_living_id integer );");
        do_sql("create table kill_event( ke_time integer, ke_victim_id integer, ke_victim_level integer, ke_map_id integer , ke_killer_id integer, ke_killer_level integer );");

        do_sql("create table parameters( param_name text, param_value text );");
        do_sql("insert into parameters values( 'version', '1' );");
    }
}

int get_living_id(object* living) {
    char** line;
    char* sql;
    int nrow, ncolumn, id;

    if (living->type == PLAYER)
        sql = sqlite3_mprintf("select liv_id from living where liv_name='%q' and liv_is_player = 1", living->name);
    else
        sql = sqlite3_mprintf("select liv_id from living where liv_name='%q' and liv_is_player = 0 and liv_level = %d", living->name, living->level);
    sqlite3_get_table(database, sql, &line, &nrow, &ncolumn, NULL);

    if (nrow > 0)
        id = atoi( line[ncolumn] );
    else {
        sqlite3_free(sql);
        sql = sqlite3_mprintf("insert into living( liv_name, liv_is_player, liv_level ) values('%q', %d, %d )", living->name, living->type == PLAYER ? 1 : 0, living->level);
        do_sql(sql);
        id = sqlite3_last_insert_rowid(database);
    }
    sqlite3_free(sql);
    sqlite3_free_table(line);
    return id;
}

int get_region_id(region* reg) {
    char** line;
    char* sql;
    int nrow, ncolumn, id;

    if (!reg)
        return 0;

    sql = sqlite3_mprintf("select reg_id from region where reg_name='%q'", reg->name);
    sqlite3_get_table(database, sql, &line, &nrow, &ncolumn, NULL);

    if (nrow > 0)
        id = atoi( line[ncolumn] );
    else {
        sqlite3_free(sql);
        sql = sqlite3_mprintf("insert into region( reg_name ) values( '%q' )", reg->name);
        do_sql(sql);
        id = sqlite3_last_insert_rowid(database);
    }
    sqlite3_free(sql);
    sqlite3_free_table(line);
    return id;
}

int get_map_id(mapstruct* map) {
    char** line;
    char* sql;
    int nrow, ncolumn, id, reg_id;
    char* path = map->path;

    if (strncmp(path, "/random/", 7) == 0)
        path = "/random/";

    reg_id = get_region_id(map->region);
    sql = sqlite3_mprintf("select map_id from map where map_path='%q' and map_reg_id = %d", map->path, reg_id);
    sqlite3_get_table(database, sql, &line, &nrow, &ncolumn, NULL);

    if (nrow > 0)
        id = atoi( line[ncolumn] );
    else {
        sqlite3_free(sql);
        sql = sqlite3_mprintf("insert into map( map_path, map_reg_id ) values( '%q', %d )", map->path, reg_id);
        do_sql(sql);
        id = sqlite3_last_insert_rowid(database);
    }
    sqlite3_free(sql);
    sqlite3_free_table(line);

    return id;
}

int store_time() {
    char** line;
    char* sql;
    int nrow, ncolumn;
    char date[50];
    timeofday_t tod;

    cf_get_time(&tod);

    if (tod.day == last_stored_day)
        return;
    last_stored_day = tod.day;

    snprintf(date, 50, "%10d-%2d-%2d %2d:%2d", tod.year, tod.month, tod.day, tod.hour, tod.minute);

    sql = sqlite3_mprintf("select * from time where time_ingame='%q'", date);
    sqlite3_get_table(database, sql, &line, &nrow, &ncolumn, NULL);
    sqlite3_free(sql);
    sqlite3_free_table(line);
    if (nrow > 0)
        return 0;

    sql = sqlite3_mprintf("insert into time values( %d, '%s' )", time(NULL), date);
    do_sql(sql);
    sqlite3_free(sql);
    return 1;
}

void add_player_event(object* pl, int event_code) {
    int id = get_living_id(pl);
    int map_id = 0;
    char* sql;

    if (pl->map)
        map_id = get_map_id(pl->map);

    sql = sqlite3_mprintf("insert into living_event values( %d, %d, %d, %d )", id, time(NULL), event_code, map_id);
    do_sql(sql);
    sqlite3_free(sql);
}

void add_map_event(mapstruct* map, int event_code, object* pl) {
    int mapid;
    int playerid = 0;
    char* sql;

    if (pl && pl->type == PLAYER)
        playerid = get_living_id(pl);

    mapid = get_map_id(map);
    sql = sqlite3_mprintf("insert into map_event values( %d, %d, %d, %d )", mapid, time(NULL), event_code, playerid);
    do_sql(sql);
    sqlite3_free(sql);
}

void add_death(object* victim, object* killer) {
    int vid, kid, map_id;
    char* sql;
    if (!victim || !killer)
        return;
    if (victim->type != PLAYER && killer->type != PLAYER)
        return;

    vid = get_living_id(victim);
    kid = get_living_id(killer);
    map_id = get_map_id(victim->map);
    sql = sqlite3_mprintf("insert into kill_event values( %d, %d, %d, %d, %d, %d )", time(NULL), vid, victim->level, map_id, kid, killer->level);
    do_sql(sql);
    sqlite3_free(sql);
}

CF_PLUGIN int initPlugin(const char* iversion, f_plug_api gethooksptr)
{
    int rtype = 0;
    int hooktype = 1;

    gethook = gethooksptr;
    cf_init_plugin( gethook );

    cf_log(llevInfo, "%s init\n", PLUGIN_VERSION);

    registerGlobalEvent =   gethook(&rtype,hooktype,"cfapi_system_register_global_event");
    unregisterGlobalEvent = gethook(&rtype,hooktype,"cfapi_system_unregister_global_event");
    systemDirectory       = gethook(&rtype,hooktype,"cfapi_system_directory");
    reCmp                 = gethook(&rtype,hooktype,"cfapi_system_re_cmp");

    return 0;
}

CF_PLUGIN void* getPluginProperty(int* type, ...)
{
    va_list args;
    char* propname;
    int i;
    va_start(args, type);
    propname = va_arg(args, char *);

    if (!strcmp(propname, "Identification"))
    {
        va_end(args);
        return PLUGIN_NAME;
    }
    else if (!strcmp(propname, "FullName"))
    {
        va_end(args);
        return PLUGIN_VERSION;
    }
    return NULL;
}

CF_PLUGIN int runPluginCommand(object* op, char* params)
{
    return -1;
}

void* eventListener(int* type, ...)
{
    static int rv=0;
    return &rv;
}

CF_PLUGIN void* globalEventListener(int* type, ...)
{
    va_list args;
    static int rv=0;
    char* buf;
    player* pl;
    object* op;
    int event_code;
    mapstruct* map;

    va_start(args, type);
    event_code = va_arg(args, int);

    switch(event_code)
    {
        case EVENT_BORN:
        case EVENT_PLAYER_DEATH:
        case EVENT_REMOVE:
        case EVENT_MUZZLE:
        case EVENT_KICK:
            op = va_arg(args, object*);
            add_player_event(op, event_code);
            break;
        case EVENT_LOGIN:
        case EVENT_LOGOUT:
            pl = va_arg(args, player*);
            add_player_event(pl->ob, event_code);
            break;

        case EVENT_MAPENTER:
        case EVENT_MAPLEAVE:
            op = va_arg(args, object*);
            map = va_arg(args, mapstruct*);
            add_map_event(map, event_code, op);
            break;

        case EVENT_MAPLOAD:
        case EVENT_MAPUNLOAD:
        case EVENT_MAPRESET:
            map = va_arg(args, mapstruct*);
            add_map_event(map, event_code, 0);
            break;

        case EVENT_GKILL:
            {
                object* killer;
                op = va_arg(args, object*);
                killer = va_arg(args, object*);
                add_death(op, killer);
            }
            break;

        case EVENT_CLOCK:
            store_time();
            break;
    }
    va_end(args);

    return &rv;
}

CF_PLUGIN int postInitPlugin()
{
    int rtype = 0;
    char path[500];
    const char* dir;
    int i;

    cf_log(llevInfo, "%s post init\n", PLUGIN_VERSION);

    dir = systemDirectory(&i, 4);
    snprintf(path, 500, "%s/cflogger.db", dir);

    if (sqlite3_open(path, &database) != SQLITE_OK) {
        cf_log(llevError, " [%s] database error!\n", PLUGIN_NAME);
        sqlite3_close(database);
        database = NULL;
        return 0;
    }

    check_tables();

    store_time();

    registerGlobalEvent(NULL,EVENT_BORN,PLUGIN_NAME,globalEventListener);
    registerGlobalEvent(NULL,EVENT_REMOVE,PLUGIN_NAME,globalEventListener);
    registerGlobalEvent(NULL,EVENT_GKILL,PLUGIN_NAME,globalEventListener);
    registerGlobalEvent(NULL,EVENT_LOGIN,PLUGIN_NAME,globalEventListener);
    registerGlobalEvent(NULL,EVENT_LOGOUT,PLUGIN_NAME,globalEventListener);

    registerGlobalEvent(NULL,EVENT_PLAYER_DEATH,PLUGIN_NAME,globalEventListener);

    registerGlobalEvent(NULL,EVENT_MAPENTER,PLUGIN_NAME,globalEventListener);
    registerGlobalEvent(NULL,EVENT_MAPLEAVE,PLUGIN_NAME,globalEventListener);
    registerGlobalEvent(NULL,EVENT_MAPRESET,PLUGIN_NAME,globalEventListener);
    registerGlobalEvent(NULL,EVENT_MAPLOAD,PLUGIN_NAME,globalEventListener);
    registerGlobalEvent(NULL,EVENT_MAPUNLOAD,PLUGIN_NAME,globalEventListener);

    registerGlobalEvent(NULL,EVENT_MUZZLE,PLUGIN_NAME,globalEventListener);
    registerGlobalEvent(NULL,EVENT_KICK,PLUGIN_NAME,globalEventListener);

    registerGlobalEvent(NULL,EVENT_CLOCK,PLUGIN_NAME,globalEventListener);

    return 0;
}

int closePlugin()
{
    cf_log(llevInfo, "%s closing.", PLUGIN_VERSION);
    if (database) {
        sqlite3_close(database);
        database = NULL;
    }
    return 0;
}

