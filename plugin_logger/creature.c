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
typedef struct creature_death{
    char* creaturename;
    long int death_count;
    struct creature_death* next;
    struct creature_death* previous;
} *creature_death_ptr;
int modified_creature_death=0;
struct creature_death* first_creature=NULL;
struct creature_death* last_creature=NULL;
/*
 * Used to reference the last creature we saved
 * Incremented when adding new creature on top of list
 */
#ifndef __CEXTRACT__
/*
 * Begin of private functions (not exported by cextract)
 */
creature_death_ptr find_creature_by_name (char* creaturename){
    creature_death_ptr current=first_creature;
    while (current!=NULL){
        if (current->creaturename==creaturename) return current;
        current=current->next;
    }
    return NULL;
}
creature_death_ptr create_creature (object* creature){
    creature_death_ptr current;
    current=(struct creature_death*)malloc (sizeof (struct creature_death));
    current->death_count=0;
    current->creaturename=hook_add_string(creature->name);
    current->next=NULL;
    current->previous=last_creature;
    //current->clean=1;
    if (last_creature) last_creature->next=current;
    if (!first_creature) first_creature=current;
    last_creature=current;
    modified_creature_death++;
    return first_creature;
}

void freecreature (creature_death_ptr creature)
{
    if (creature->previous)
        creature->previous->next=creature->next;
    else
        first_creature=creature->next;       /*We were the head*/
    if (creature->next)
        creature->next->previous=creature->previous;
    else
        last_creature=creature->previous;    /*We were the back*/
    if (creature->creaturename)
        hook_free_string (creature->creaturename);
    free (creature);
    modified_creature_death--;
}

creature_death_ptr getcreatureAt (int *creaturenumber)
{
    int i=1,j=1;
    creature_death_ptr current=first_creature;
    if (*creaturenumber<1)*creaturenumber=1;
    while (i<*creaturenumber){
        if (current!=NULL){
            j++;
            current=current->next;
        }
        if (current==NULL){/*make it cycle at the end of list*/
            current=first_creature;
            j=1;
        }
        i++;
    }
    if (current==NULL) current=first_creature;
    *creaturenumber=j;
    return current;
}

void save_creature_visits (creature_death_ptr creature){
    char query[MAX_BUF*2];
    char query2[MAX_BUF*2];
    char buffer[MAX_BUF*2];
    int precision;
    long int old_death;
    printf ("flushing %s\n",creature->creaturename);
    sprintf (query,"select count from creature_death where name='%s'",creature->creaturename);
    old_death=database_getint(query);
    gettimestamp (buffer,sizeof (buffer),&precision);
    sprintf (query, "update creature_death set count=%ld where name='%s'",
             old_death+creature->death_count,
             addslashes(creature->creaturename));
    sprintf (query2, "insert into creature_death (count,moment,name,type)\
                      VALUES (%ld,'%s','%s',%d)",
                      old_death+creature->death_count,buffer,
                      addslashes(creature->creaturename),1);
    database_UpdateOrInsert (query,query2);
    creature->death_count=0;
}
#endif
int getcreaturecount(void)
{
    int i=0;
    creature_death_ptr current;
    for (current=first_creature;current;current=current->next)
        i++;
    return i;
}
/*
 * flush_creature_death
 *
 * This function save a certain quantity
 * of creatures (creaturecount) to database
 */
void flush_creature_death (int creaturecount){
    int i;
    creature_death_ptr current;
    creature_death_ptr current2;
    if (creaturecount>modified_creature_death) creaturecount=modified_creature_death; /*Save no more than maximum*/
    if (creaturecount<=0) return;
    current=first_creature;
    i=0;
    for (i=0;i<creaturecount;i++){
        if (current==NULL) current=first_creature;
        save_creature_visits (current);
        current2=current->next;
        freecreature (current); /*Free if necessary*/
        current=current2;
    }
    printf ("\n");
}
void try_flush_creature_death (int ratio){
    int quantity;
    if (modified_creature_death<0) printf ("[crossfire_logger]Warning: negative creature count for logger!!!!!!\n");
    if (modified_creature_death<=0) return; /*Nothing to do (i don't believe it!)*/
    printf ("flushing creatures....\n");
    quantity=(modified_creature_death/ratio);
    if (quantity<=0) flush_creature_death (1);
    else flush_creature_death (quantity);
}

int add_death_creature (object* creature){
    creature_death_ptr currentcreature;
    char *creaturename;
    creaturename=hook_add_string(creature->name);
    printf ("Adding death creature:%s\n",creaturename);
    currentcreature=find_creature_by_name (creaturename);
    if (!currentcreature) currentcreature=create_creature (creature);
    /* Add death to stats */
    currentcreature->death_count++;
    hook_free_string (creaturename);
    return 0;
}