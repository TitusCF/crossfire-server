/*
 * The crossfire logging facility.
 * This plugin is part of a project who's purpose is to track every
 * interesting thing happening in the crossfire server, from conversation
 * between player and their deaths to informations about server crashes.
 * The datas could then be loaded by some php pages to present info on a player,
 * important events of the day, current Xp of all players,
 * crash-o-meter and so on.
 */

/*
 * This code is placed under the GNU General Public Licence (GPL)
 * either version 2 or higher, at your choice. You can get a copy
 * of the GPL at the following address: http://www.gnu.org
 *
 * (C)2001 Tchize. Feel free to log errors.
 * tchize@mailandnews.com
 */

#include <plugin_logging.h>
#include <time.h>
#ifndef __CEXTRACT__
#include <logger_proto.h>
#endif
typedef struct map_time{
    char* mapname;
    char* mappath;
    int players;          /*players curently in map*/
    int clean;            /*If false, contain datas to be saved*/
    long int totaltime;   /*Time will be given in 1 seconds steps*/
    long int entertime;   /*last Time a player entered the map*/
    struct map_time* next;
    struct map_time* previous;
} *map_time_ptr;
int modified_map_time=0;
/*
 * Used to know the current map to save
 * Incremented when we add a map to the top of list
 */
static int mapnumber=0;
struct map_time* first_map_time=NULL;
#ifndef __CEXTRACT__
/*
 * Begin of private functions (not exported by cextract)
 */
map_time_ptr find_map_by_name (char* mappath){
    map_time_ptr current=first_map_time;
    while (current!=NULL){
        if (current->mappath==mappath) return current;
        current=current->next;
    }
    return NULL;
}
map_time_ptr create_map (struct mapdef* map){
    map_time_ptr current;
    current=(struct map_time*)malloc (sizeof (struct map_time));
    current->players=0;
    current->totaltime=0;
    current->entertime=0;
    current->mappath=hook_add_string(map->path);
    current->mapname=hook_add_string(map->name);
    current->next=first_map_time;
    current->previous=NULL;
    current->clean=1;
    if (first_map_time) first_map_time->previous=current;
    first_map_time=current;
    map_alter (map->path);
    mapnumber++;
    return first_map_time;
}
void touchmap (map_time_ptr map)
{
    if (map->clean){
        map->clean=0;
        modified_map_time++;
    }
}

void freemap (map_time_ptr map)
{
    if ((map->players<=0)&&(map->clean)){
        if (map->previous) map->previous->next=map->next;
        else first_map_time=map->next;       /*We were the head*/
        if (map->next) map->next->previous=map->previous;
        if (map->mapname) hook_free_string (map->mapname);
        hook_free_string (map->mappath);
        free (map);
    }
}

void untouchmap (map_time_ptr map)
{
    if (!map->clean){
        map->clean=1;
        modified_map_time--;
    }
}

map_time_ptr getMapAt (int *mapnumber)
{
    int i=1,j=1;
    map_time_ptr current=first_map_time;
    if (*mapnumber<0) *mapnumber=0;
    while (i<*mapnumber){
        if (current!=NULL){
            j++;
            current=current->next;
        }
        if (current==NULL){/*make it cycle at the end of list*/
            current=first_map_time;
            j=1;
        }
        i++;
    }
    if (current==NULL) current=first_map_time;
    *mapnumber=j;
    return current;
}
void save_map_visits (map_time_ptr map){
    char query[MAX_BUF];
    char query2[MAX_BUF];
    char buffer[MAX_BUF];
    int precision;
    int old_visit;
    long int now;
    /*printf ("Flushing %s ",map->mappath);*/
    now=(long int) time(NULL);
    map->totaltime+=(now-map->entertime)*map->players;
    map->entertime=now;
    if ( (strncmp ("/random/",map->mappath,strlen("/random/"))) &&
         (strncmp (LOCALDIR,map->mappath,strlen(LOCALDIR)))    ){
        sprintf (query,"select visittime from map_visits where mappath='%s'",
                 map->mappath);
        old_visit=database_getint(query);
        gettimestamp (buffer,sizeof (buffer),&precision);
        sprintf (query, "update map_visits set visittime=%ld where mappath='%s'",
                 old_visit+map->totaltime,map->mappath);
        sprintf (query2, "insert into map_visits (visittime,period,mappath,mapname)\
                          VALUES (%ld,'%s','%s','%s')",
                          old_visit+map->totaltime,buffer,
                          map->mappath,map->mapname?addslashes(map->mapname):"");
        /*printf ("with visit time: %ld s\n",old_visit+map->totaltime);*/
        database_UpdateOrInsert (query,query2);
    }
    map->totaltime=0;
    untouchmap (map);
}
#endif
/*
 * flush_map_time
 *
 * This function save a certain quantity
 * of maps (mapcount) to database
 */
void flush_map_time (int mapcount){
    int i;
    map_time_ptr current;
    map_time_ptr current2;
    /*printf ("part2(%d)...\n",mapcount);*/
    if (mapcount>modified_map_time) mapcount=modified_map_time; /*Save no more than maximum*/
    if (mapcount<=0) return;
    current=getMapAt (&mapnumber);
    i=0;
    for (;;){
        current2=current->next;
        freemap (current); /*Free if necessary*/
        current=current2;
        if (current==NULL) current=first_map_time;
        /*printf ("\nTrying %s",current->mappath);*/
        if (current->clean) continue;
        /*printf ("not clean");*/
        i++;
        save_map_visits (current);
        if (i>=mapcount) break;
    }
    printf ("\n");
}
void try_flush_map_time (int ratio){
    int quantity;
    if (modified_map_time<0)
        printf ("[crossfire_logger]Warning: negative map count for logger!\n");
    if (modified_map_time<=0) return; /*Nothing to do (i don't believe it!)*/
    /*printf ("flushing....\n");*/
    quantity=(modified_map_time/ratio);
    if (quantity<=0) flush_map_time (1);
    else flush_map_time (quantity);
}

int player_enter_map (struct mapdef* map){
    map_time_ptr currentmap;
    long int now;
    char *mappath;
    mappath=hook_add_string(map->path);
    currentmap=find_map_by_name (mappath);
    if (!currentmap) currentmap=create_map (map);
    now=(long int) time(NULL);
    /* Add cumulative time to stats */
    currentmap->totaltime+=(now-currentmap->entertime)*currentmap->players;
    currentmap->entertime=now;
    currentmap->players++;
    touchmap (currentmap);
    hook_free_string (mappath);
    return 0;
}

int player_leave_map(struct mapdef* map){
    map_time_ptr currentmap;
    long int now;
    char *mappath;
    printf ("In player leave map\n");
    mappath=hook_add_string(map->path);
    currentmap=find_map_by_name (mappath);
    if (currentmap){
        now=(long int) time(NULL);
        /* Add cumulative time to stats */
        currentmap->totaltime+=(now-currentmap->entertime)*currentmap->players;
        currentmap->entertime=now;
        currentmap->players--;
        touchmap (currentmap);
    }else{
        printf ("[crossfire_logger]Warning: players leave a non stated map!\n");
    }
    hook_free_string (mappath);
    return 0;
}

int map_alter (char* mappath){
    char insert[MAX_BUF];
    char update[MAX_BUF];
    char buffer[MAX_BUF];
    int precision;
    printf ("\t[CROSSFIRE LOGGER] ALtering %s\n",mappath);
    if ( (!strncmp ("/random/",mappath,strlen("/random/"))) ||
         (!strncmp (LOCALDIR,mappath,strlen(LOCALDIR)))    )
        return 0;
    gettimestamp (buffer,sizeof (buffer),&precision);
    sprintf (insert,"insert into map_state (path,state,moment)\
                     VALUES ('%s',1,'%s')",mappath,buffer);
    sprintf (update,"update map_state set state=1,moment='%s' where path='%s'",
             buffer,mappath);
    database_UpdateOrInsert (update,insert);
    return 0;
}

int map_reset (char* mappath){
    char insert[MAX_BUF*2];
    char update[MAX_BUF*2];
    char buffer[MAX_BUF*2];
    int precision;
    if ( (strncmp ("/random/",mappath,strlen("/random/"))) ||
         (strncmp (LOCALDIR,mappath,strlen(LOCALDIR)))    )
        return 0;
    gettimestamp (buffer,sizeof (buffer),&precision);
    sprintf (insert,"insert into map_state (path,name,state,moment)\
                     VALUES ('%s','%s',0,'%s')",mappath,mappath,buffer);
    sprintf (update,"update map_state set state=0,moment='%s' where path='%s'",
             buffer,mappath);
    database_UpdateOrInsert (update,insert);
    return 0;
}
