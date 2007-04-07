/*****************************************************************************/
/* Crossfire Animator v2.0a                                                  */
/* Contacts: yann.chachkoff@myrealbox.com, tchize@myrealbox.com              */
/*****************************************************************************/
/* That code is placed under the GNU General Public Licence (GPL)            */
/*                                                                           */
/* (C) 2001 David Delbecq for the original code version.                     */
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
/*                                                                           */ 
/*****************************************************************************/

/* First let's include the header file needed                                */

#include <cfanim.h>
#include <stdarg.h>

f_plug_api gethook;
f_plug_api registerGlobalEvent;
f_plug_api unregisterGlobalEvent;
f_plug_api reCmp;

CFPContext* context_stack;
CFPContext* current_context;
CFanimation *first_animation=NULL;

int get_dir_from_name (char*name)
{
    if (!strcmp(name,"north"))      return 1;
    if (!strcmp(name,"north_east")) return 2;
    if (!strcmp(name,"east"))       return 3;
    if (!strcmp(name,"south_east")) return 4;
    if (!strcmp(name,"south"))      return 5;
    if (!strcmp(name,"south_west")) return 6;
    if (!strcmp(name,"west"))       return 7;
    if (!strcmp(name,"north_west")) return 8;
    return -1;
}

long int initmovement(char* name,char* parameters, struct CFmovement_struct* move_entity)
{
    int dir;
    dir=get_dir_from_name (name);
    move_entity->parameters=NULL;
    return dir;
}
int runmovement(struct CFanimation_struct* animation, long int id, void* parameters)
{
    object* op=animation->victim;
    int dir=id;
    cf_log(llevDebug, "CFAnim: Moving in direction %ld\n",id);
    if (op->type==PLAYER)
        cf_player_move(op->contr,dir);
    else
        cf_object_move(op,dir,op);
    return 1;
}

long int initfire(char* name,char* parameters,struct CFmovement_struct* move_entity)
{
    int dir;
    dir=get_dir_from_name(&(name[5]));
    move_entity->parameters=NULL;
    return dir;
}

int runfire(struct CFanimation_struct* animation, long int id, void* parameters)
{
    cf_log(llevDebug, "CFAnim: Firing in direction %ld\n",id);
    return 1;
}

long int initturn(char* name,char* parameters,struct CFmovement_struct* move_entity)
{
    int dir;
    dir=get_dir_from_name (&(name[5]));
    move_entity->parameters=NULL;
    return dir;
}

int runturn(struct CFanimation_struct* animation, long int id, void* parameters)
{
    object* op=animation->victim;
    int dir=id;
    int face;
    cf_log(llevDebug, "CFAnim: Turning in direction %ld\n",id);
    op->facing=dir;
    cf_object_set_int_property(op,CFAPI_OBJECT_ANIMATION,face);
    return 1;
}

long int initcamera(char* name,char* parameters,struct CFmovement_struct* move_entity)
{
    int dir;
    dir=get_dir_from_name (&(name[7]));
    move_entity->parameters=NULL;
    return dir;
}
int runcamera(struct CFanimation_struct* animation, long int id, void* parameters)
{
    cf_log(llevDebug, "CFAnim: Moving the camera in direction %ld\n",id);
    return 1;
    /*if (animation->victim->type==PLAYER)
        hook_scroll_map(animation->victim,id);
    else printf ("CFAnim: Not a player\n");
    return 1;*/
}

long int initvisible (char* name, char* parameters, struct CFmovement_struct* move_entity)
{
    int result;
    if (get_boolean (parameters,&result))
        return result;
    cf_log(llevDebug, "CFAnim: Error in animation - possible values for 'invisible' are 'yes' and 'no'\n");
    return -1;
}
int runvisible(struct CFanimation_struct* animation, long int id, void* parameters)
{
    if (id==-1)
        return 1;
    animation->invisible=id;
    return 1;
}

long int initwizard (char* name, char* parameters, struct CFmovement_struct* move_entity)
{
    int result;
    if (get_boolean (parameters,&result))
        return result;
    cf_log(llevDebug, "CFAnim: Error in animation - possible values for 'wizard' are 'yes' and 'no'\n");
    return -1;
}
int runwizard(struct CFanimation_struct* animation, long int id, void* parameters)
{
    if (id==-1)
        return 1;
    animation->wizard=id;
    return 1;
}
long int initsay (char* name, char* parameters, struct CFmovement_struct* move_entity)
{
    if (parameters)
        move_entity->parameters=cf_strdup_local (parameters);
    else
        move_entity->parameters=NULL;
    cf_log(llevDebug, "CFAnim: init say: parameters: %p\n",parameters);
    return 1;
}
int runsay(struct CFanimation_struct* animation, long int id, void* parameters)
{
    if (parameters)
    {
        cf_object_speak(animation->victim, parameters);
        free (parameters);
    }
    else
        cf_log(llevDebug, "CFAnim: Error in animation: nothing to say with say function\n");
    return 1;
}
long int initapply (char* name, char* parameters, struct CFmovement_struct* move_entity)
{
    return 1;
}
int runapply(struct CFanimation_struct* animation, long int id, void* parameters)
{
    object* current_container;
    if (animation->victim->type!=PLAYER)
        return 0;
    current_container=animation->victim->container;
    animation->victim->container=NULL;
    cf_object_apply_below(animation->victim);
    animation->victim->container=current_container;
    return 1;
}
long int initapplyobject (char* name, char* parameters, struct CFmovement_struct* move_entity)
{
    move_entity->parameters=parameters?cf_add_string(parameters):NULL;
    return 1;
}
int runapplyobject(struct CFanimation_struct* animation, long int id, void* parameters)
{
    object* current;
    int aflag;

    if (!parameters)
        return 0;
    for (current=animation->victim->below;current;current=current->below)
        if(current->name==parameters) break;
    if (!current)
        for (current=animation->victim->inv;current;current=current->below)
            if(current->name==parameters) break;
    if (!current)
    {
        cf_free_string (parameters);
        return 0;
    }
    aflag=AP_APPLY;
    cf_object_apply(animation->victim, current, aflag);
    cf_free_string(parameters);
    return 1;
}

long int initdropobject (char* name, char* parameters, struct CFmovement_struct* move_entity)
{
    move_entity->parameters=parameters?cf_strdup_local(parameters):NULL;
    return 1;
}
int rundropobject(struct CFanimation_struct* animation, long int id, void* parameters)
{
    if (!parameters)
        return 0;
    cf_object_drop(animation->victim, parameters);
    cf_free_string(parameters);
    return 1;
}

long int initpickup (char* name, char* parameters, struct CFmovement_struct* move_entity)
{
    return 1;
}
int runpickup(struct CFanimation_struct* animation, long int id, void* parameters)
{
    object* current;
    current=animation->victim->below;
    if (!current)
        return 0;
    cf_object_pickup(animation->victim, current);
    return 1;
}

long int initpickupobject (char* name, char* parameters, struct CFmovement_struct* move_entity)
{
    move_entity->parameters=parameters?cf_add_string(parameters):NULL;
    return 1;
}
int runpickupobject(struct CFanimation_struct* animation, long int id, void* parameters)
{
    object* current;
    if (!parameters) return 0;
    for (current=animation->victim->below;current;current=current->below)
        if(current->name==parameters)
            break;
    if (current)
        cf_object_pickup(animation->victim, current);
    cf_free_string(parameters);
    return 1;
}
long int initghosted (char* name, char* parameters, struct CFmovement_struct* move_entity)
{
    int result;
    if (get_boolean(parameters,&result))
        return result;
    cf_log(llevDebug, "CFAnim: Error in animation: possible values for 'ghosted' are 'yes' and 'no'\n");
    return -1;
}
int runghosted(struct CFanimation_struct* animation, long int id, void* parameters)
{
    object* corpse;

    if ( (id && animation->ghosted) ||
          (!id && !animation->ghosted) )
        runghosted(animation, !id, parameters);
    if (id) /*Create a ghost/corpse pair*/
    {
        corpse = cf_object_clone(animation->victim,1);
        corpse->x=animation->victim->x;
        corpse->y=animation->victim->y;
        corpse->type=0;
        corpse->contr=NULL;
        cf_map_insert_object_there(animation->victim->map, corpse, NULL, 0);
        animation->wizard=1;
        animation->invisible=1;
        animation->corpse=corpse;
    }
    else /*Remove a corpse, make current player visible*/
    {
        animation->wizard=0;
        animation->invisible=0;
        cf_object_remove(animation->corpse);
        cf_object_free(animation->corpse);
        animation->corpse=NULL;
        animation->victim->invisible=0;
    }
    animation->ghosted=id;
    return 1;
}

typedef struct
{
    char* mapname;
    int mapx;
    int mapy;
}teleport_params;

long int initteleport (char* name, char* parameters, struct CFmovement_struct* move_entity)
{
    char* mapname;
    int mapx;
    int mapy;
    teleport_params* teleport;
    move_entity->parameters=NULL;
    cf_log(llevDebug, ".(%s)\n",parameters);
    if (!parameters)
    {
        cf_log(llevDebug, "CFAnim: Error - no parameters for teleport\n");
        return 0;
    }
    mapname=strstr (parameters," ");
    cf_log(llevDebug, ".(%s)\n",parameters);
    if (!mapname)
        return 0;
    *mapname='\0';
    mapx=atoi(parameters);
    mapname++;
    parameters=mapname;
    if (!parameters)
    {
        cf_log(llevDebug, "CFAnim: Error - not enough parameters for teleport\n");
        return 0;
    }
    cf_log(llevDebug, ".(%s)\n",parameters);
    mapname=strstr (parameters," ");
    cf_log(llevDebug, ".\n");
    if (!mapname)
        return 0;
    *mapname='\0';
    mapy=atoi(parameters);
    mapname++;
    if (mapname[0]=='\0')
        return 0;
    teleport=(teleport_params*)malloc (sizeof(teleport_params));
    teleport->mapname=cf_strdup_local (mapname);
    teleport->mapx=mapx;
    teleport->mapy=mapy;
    move_entity->parameters=teleport;
    return 1;
}
int runteleport(struct CFanimation_struct* animation, long int id, void* parameters)
{
    teleport_params* teleport=(teleport_params*)parameters;
    if (!parameters)
        return 0;
    cf_object_teleport(animation->victim, cf_map_get_map(teleport->mapname, 0),
                       teleport->mapx, teleport->mapy);
    free(parameters);
    return 1;
}

long int initnotice (char* name, char* parameters, struct CFmovement_struct* move_entity)
{
    move_entity->parameters=parameters?cf_strdup_local(parameters):NULL;
    return 1;
}
int runnotice(struct CFanimation_struct* animation, long int id, void* parameters)
{
    int val;

    val = NDI_NAVY|NDI_UNIQUE;

    cf_player_message(animation->victim, parameters, val);
    return 1;
}


CFanimationHook animationbox[]=
{
    {"north",initmovement,runmovement},
    {"north_east",initmovement,runmovement},
    {"east",initmovement,runmovement},
    {"south_east",initmovement,runmovement},
    {"south",initmovement,runmovement},
    {"south_west",initmovement,runmovement},
    {"west",initmovement,runmovement},
    {"north_west",initmovement,runmovement},
    {"fire_north",initfire,runfire},
    {"fire_north_east",initfire,runfire},
    {"fire_east",initfire,runfire},
    {"fire_south_east",initfire,runfire},
    {"fire_south",initfire,runfire},
    {"fire_south_west",initfire,runfire},
    {"fire_west",initfire,runfire},
    {"fire_north_west",initfire,runfire},
    {"turn_north",initturn,runturn},
    {"turn_north_east",initturn,runturn},
    {"turn_east",initturn,runturn},
    {"turn_south_east",initturn,runturn},
    {"turn_south",initturn,runturn},
    {"turn_south_west",initturn,runturn},
    {"turn_west",initturn,runturn},
    {"turn_north_west",initturn,runturn},
    {"camera_north",initcamera,runcamera},
    {"camera_north_east",initcamera,runcamera},
    {"camera_east",initcamera,runcamera},
    {"camera_south_east",initcamera,runcamera},
    {"camera_south",initcamera,runcamera},
    {"camera_south_west",initcamera,runcamera},
    {"camera_west",initcamera,runcamera},
    {"camera_north_west",initcamera,runcamera},
    {"invisible",initvisible,runvisible},
    {"wizard",initwizard,runwizard},
    {"say",initsay,runsay},
    {"apply",initapply,runapply},
    {"apply_object",initapplyobject,runapplyobject},
    {"drop_object",initdropobject,rundropobject},
    {"pickup",initpickup,runpickup},
    {"pickup_object",initpickupobject,runpickupobject},
    {"ghosted",initghosted,runghosted},
    {"teleport",initteleport,runteleport},
    {"notice",initnotice,runnotice}
};
int animationcount=sizeof (animationbox) / sizeof (CFanimationHook);
int ordered_commands=0;
static int compareAnims (const void *a, const void *b)
{
    return strcmp ( ((CFanimationHook*)a)->name,((CFanimationHook*)b)->name);
}

void prepare_commands (void)
{
    qsort (animationbox,animationcount,sizeof (CFanimationHook),compareAnims);
    ordered_commands=1;
}

static CFanimationHook *get_command(char *command)
{
    CFanimationHook dummy;
    dummy.name=command;
    if (!ordered_commands)
        prepare_commands();
    return (CFanimationHook*)
            bsearch (&dummy,animationbox,animationcount,
                      sizeof(CFanimationHook), compareAnims);
}

static CFmovement *parse_animation_block(char *buffer, size_t buffer_size, FILE *fichier, CFanimation *parent)
{
    CFmovement* first=NULL;
    CFmovement* current=NULL;
    CFmovement* next;
    char* time;
    char* name;
    char* parameters;
    int tick;
    CFanimationHook* animationhook;
    if (parent->verbose)
        cf_log(llevDebug, "CFAnim: In parse block for %s\n",buffer);
    while (fgets(buffer,buffer_size,fichier))
    {
        if (buffer[0]=='[') break;
        if (buffer[0]=='#') continue;
        buffer[strlen(buffer)-strlen("\n")]='\0';
        while (buffer[strlen(buffer)-1]==' ')
            buffer[strlen(buffer)-1]='\0';
        if (strlen (buffer)<=0)
            continue;
        time=buffer;

        name= strstr (buffer," ");
        if (!name) continue;
        *name='\0';
        name++;
        while (*name==' ') name++;

        tick=atoi(time);
        if (tick<0) continue;

        parameters=strstr (name," ");
        if (parameters) /*Parameters may be nul*/
        {
            *parameters ='\0';
            parameters++;
            while (*parameters==' ') parameters++;
            if (*parameters=='\0') parameters=NULL;
        }
        animationhook= get_command (name);
        cf_log(llevDebug, "\n");
        if (parent->verbose)
        {
            if (!animationhook)
                cf_log(llevDebug, "CFAnim: %s - Unknown animation command\n",name);
            else
                cf_log(llevDebug, "CFAnim: Parsed %s -> %p\n",name,animationhook);
        }
        if (!animationhook)
        {
            if (parent->errors_allowed)
                continue;
            else
                break;
        }
        next= (CFmovement*) malloc (sizeof (CFmovement));
        if (!next) continue;
        next->parent=parent;
        next->tick=tick;
        next->next=NULL;
        if (animationhook->funcinit)
            next->id=animationhook->funcinit (name,parameters,next);
        next->func=animationhook->funcrun;
        if (current) current->next=next;
        else first=next;
        current=next;
    }
    cf_log(llevDebug, "\n");
    return first;
}

/*
 * This function take buffer with a value like "blabla= things"
 * Its arguments are:
 *    buffer: where equality is written
 *    variable: the address of a char pointer. It will
 *              be positionned to where in buffer the
 *              variable name starts. leading spaces
 *              will be converted to \0
 *    value:    the same as above but for the value part
 *    Note that variable and value become pointers to internals of
 *    buffer. If buffer chages, they will whange too!
 */
int equality_split (char* buffer, char**variable, char**value)
{
    if (!strcmp (&buffer[strlen(buffer)-strlen("\n")],"\n"))
        buffer[strlen(buffer)-strlen("\n")]='\0';
    *value=strstr (buffer,"=");
    if (!*value) return 0;
    **value='\0';
    *variable=buffer;
    (*value)++;
    while ((strlen(*variable)>0) && ((*variable)[strlen(*variable)-1]==' '))
        (*variable)[strlen(*variable)-1]='\0';
    while ((strlen(*value)>0) && ((*value)[strlen(*value)-1]==' '))
        (*value)[strlen(*value)-1]='\0';
    while (**value==' ') (*value)++;
    if ((**variable=='\0') || (**value=='\0')) return 0;
    return 1;
}
/*
 * This function gets a string containing
 * [Y/y](es)/[N/n](o), 1/0
 * and set bool according to what's read
 * if return value is true, bool was set successfully
 * else, an error occured and bool was not touched
 */
int get_boolean (char* string,int* bool)
{
    if (!strncmp (string,"y",1))
        *bool=1;
    else if (!strncmp (string,"n",1))
        *bool=0;
    else if (!strncmp (string,"Y",1))
        *bool=1;
    else if (!strncmp (string,"N",1))
        *bool=0;
    else if (!strncmp (string,"1",1))
        *bool=1;
    else if (!strncmp (string,"0",1))
        *bool=0;
    else return 0;
    return 1;
}

int is_animated_player (object* pl)
{
    CFanimation* current;
    for (current=first_animation;current;current++)
        if ((current->victim==pl) && (current->paralyze))
        {
            if (current->verbose)
                cf_log(llevDebug, "CFAnim: Getting a command for a paralyzed player %s.\n",pl->name);
            return 1;
        }
    return 0;
}

/*
 * return a new animation pointer inserted in the list of animations
 */
static CFanimation *create_animation(void)
{
    CFanimation* new;
    CFanimation* current;
    new=(CFanimation*) malloc (sizeof (CFanimation));
    if (!new) return NULL;
    new->name=NULL;
    new->victim=NULL;
    new->nextmovement=NULL;
    new->nextanimation=NULL;
    for (current=first_animation;(current && current->nextanimation);
         current=current->nextanimation);
    if (!current)
        first_animation=new;
    else
        current->nextanimation=new;
    return new;
}

void free_events(object* who)
{
/*if (who->event_hook[current_event])
    {
    hook_free_string (who->event_hook[current_event]);
    who->event_hook[current_event]=NULL;
}
    if (who->event_plugin[current_event])
    {
    hook_free_string (who->event_plugin[current_event]);
    who->event_plugin[current_event]=NULL;
}
    if (who->event_hook[current_event])
    {
    hook_free_string (who->event_options[current_event]);
    who->event_options[current_event]=NULL;
}*/

}

/*
 * Create a new animation object according to file, option and activator (who)
 */
int start_animation (object* who,object* activator,char* file, char* options)
{
    FILE*   fichier;
    char*   name=NULL;
    int     victimtype=0;
    object* victim=NULL;
    int     unique=0;
    int     always_delete=0;
    int     parallel=0;
    int     paralyzed=1;
    int     invisible=0;
    int     wizard=0;
    enum    time_enum timetype;
    int     errors_allowed=0;
    int     verbose=0;
    char*   animationitem;
    char    buffer[HUGE_BUF];
    char*   variable;
    char*   value;
    int     errors_found=0;
    CFanimation* current_anim;
    char    path[1024];

    fichier = fopen(cf_get_maps_directory(file, path, sizeof(path)),"r");
    if (fichier == NULL)
    {
        cf_log(llevDebug, "CFAnim: Unable to open %s\n", path);
        return 0;
    }
    while (fgets(buffer,HUGE_BUF,fichier))
    {
        if (buffer[0]=='[') break;
        if (buffer[0]=='#') continue;
        if (!strcmp(buffer,"\n")) continue;
        errors_found=1;
        cf_log(llevDebug, "CFAnim: '%s' has an invalid syntax.\n",buffer);
    }
    if (feof(fichier))
        return 0;
    if (strncmp (buffer,"[Config]",8))
    {
        cf_log(llevDebug, "CFAnim: Fatal error in %s: [Config] must be the first group defined.\n",file);
        return 0;
    }
    while (fgets(buffer,HUGE_BUF,fichier))
    {
        if (buffer[0]=='[') break;
        if (buffer[0]=='#') continue;
        if (!strcmp(buffer,"\n")) continue;
        if (!equality_split(buffer,&variable,&value))
            errors_found=1;
        else
        {
            if (!strcmp (variable,"name"))
            {
                if (*value=='"') value++;
                if (value[strlen(value)-1] == '"') value[strlen(value)-1]='\0';
                name=cf_strdup_local (value);
            }
            else if (!strcmp (variable,"victimtype"))
            {
                if (!strcmp (value,"player")) victimtype=0;
                else if (!strcmp (value,"object")) victimtype=1;
                else if (!strcmp (value,"any")) victimtype=2;
                else errors_found=1;
            }
            else if (!strcmp (variable,"victim"))
            {
                cf_log(llevDebug, "Setting victim to %s\n",value);
                if (!strcmp (value,"who"))
                    victim=who;
                else if (!strcmp (value,"activator"))
                    victim=activator;
                else if (!strcmp (value,"who_owner"))
                    if (!who)
                    {
                        errors_found=1;
                        cf_log(llevDebug, "Warning: object \"who\" doesn't exist and you victimized it's owner\n");
                    }
                    else
                        victim=who->env;
                else if (!strcmp (value,"activator_owner"))
                    if (!activator)
                    {
                        errors_found=1;
                        cf_log(llevDebug, "Warning: object \"activator\" doesn't exist and you victimized it's owner\n");
                    }
                    else
                        victim=activator->env;
                else
                    errors_found=1;
            }
            else if (!strcmp(variable,"unique"))
            {
                if (!get_boolean(value,&unique))
                    errors_found=1;
            }
            else if (!strcmp(variable,"always_delete"))
            {
                if (!get_boolean(value,&always_delete))
                    errors_found=1;
            }
            else if (!strcmp(variable,"parallel"))
            {
                if (!get_boolean(value,&parallel))
                    errors_found=1;
            }
            else if (!strcmp(variable,"paralyzed"))
            {
                if (!get_boolean(value,&paralyzed))
                    errors_found=1;
            }
            else if (!strcmp(variable,"invisible"))
            {
                if (!get_boolean(value,&invisible))
                    errors_found=1;
            }
            else if (!strcmp(variable,"wizard"))
            {
                if (!get_boolean(value,&wizard))
                    errors_found=1;
            }
            else if (!strcmp(variable,"errors_allowed"))
            {
                if (!get_boolean(value,&errors_allowed))
                    errors_found=1;
            }
            else if (!strcmp(variable,"verbose"))
            {
                if (!get_boolean(value,&verbose))
                    errors_found=1;
            }
            else if (!strcmp(variable,"time_representation"))
            {
                if (!strcmp (value,"second")) timetype=time_second;
                else if (!strcmp (value,"tick")) timetype=time_tick;
                else errors_found=1;
            }
            else if (!strcmp(variable,"animation"))
            {
                animationitem=cf_strdup_local(value);
            }
            else errors_found=1;
        }
    }
    if (buffer[0]=='\0')
    {
        cf_log(llevDebug, "CFAnim: Errors occurred during the parsing of %s\n", path);
        return 0;
    }
    if (!(current_anim=create_animation()))
    {
        cf_log(llevDebug, "CFAnim: Fatal error - Not enough memory.\n");
        return 0;
    }
    if (always_delete)
    {
        /*if (verbose) printf("CFAnim: Freeing event nr. %d for %s.\n",current_event,who->name);*/
        free_events(who);
    }
    if (!victim)
    {
        cf_log(llevDebug,  "CFAnim: Fatal error - victim is NULL");
        return 0;
    }
    if ( ( (victim->type==PLAYER) && (victimtype==1)) ||
            ( (victim->type!=PLAYER) && (victimtype==0))  ||
            ( errors_found && !errors_allowed) )
    {
        if (verbose) cf_log(llevDebug, "CFAnim: No correct victim found or errors found, aborting.\n");
        return 0;
    }
    if (unique && !always_delete)
    {
        /*if (verbose) printf ("CFAnim: Freeing event nr. %d for %s.\n",current_event,who->name);*/
        free_events(who);
    }
    current_anim->name      = name;
    current_anim->victim    = victim;
    current_anim->paralyze  = paralyzed;
    current_anim->invisible = invisible;
    current_anim->wizard    = wizard;
    current_anim->unique    = unique;
    current_anim->ghosted   = 0;
    current_anim->corpse    = NULL;
    current_anim->time_representation=timetype;
    current_anim->verbose   = verbose;
    current_anim->tick_left = 0;
    current_anim->errors_allowed=errors_allowed;
    while (buffer[0]=='[')
    {
        if (strncmp (&buffer[1],animationitem,strlen(animationitem)))
            while (fgets(buffer,HUGE_BUF,fichier))
                if (buffer[0]=='[') break;
        current_anim->nextmovement=parse_animation_block(buffer,HUGE_BUF,fichier,current_anim);
        if (current_anim->nextmovement) break;
    }
    fclose (fichier);
    return 1;
}
static void animate_one(CFanimation *animation, long int milliseconds)
{
    CFmovement* current;

    if (animation->time_representation==time_second)
        animation->tick_left+=milliseconds;
    else animation->tick_left++;
    if (animation->verbose)
        cf_log(llevDebug, "CFAnim: Ticking %s for %s. Tickleft is %ld\n",
               animation->name,animation->victim->name,animation->tick_left);
    if (animation->invisible)
        animation->victim->invisible=10;
    if (animation->wizard)
    {
        if (animation->verbose)
            cf_log(llevDebug, "CFAnim: Setting wizard flags\n");
        cf_object_set_flag(animation->victim, FLAG_WIZPASS,1);
        cf_object_set_flag(animation->victim, FLAG_WIZCAST,1);
        cf_object_set_flag(animation->victim, FLAG_WIZ,1);

    }
    cf_object_update(animation->victim,UP_OBJ_CHANGE);

    if (animation->nextmovement)
        while ( animation->tick_left> animation->nextmovement->tick)
    {
        animation->tick_left-=animation->nextmovement->tick;
        animation->nextmovement->func (animation,
                                       animation->nextmovement->id,
                                       animation->nextmovement->parameters);
        current=animation->nextmovement;
        animation->nextmovement=animation->nextmovement->next;
        free (current);
        if (!animation->nextmovement) break;
    }
    cf_object_set_flag(animation->victim, FLAG_WIZPASS,0);
    cf_object_set_flag(animation->victim, FLAG_WIZCAST,0);
    cf_object_set_flag(animation->victim, FLAG_WIZ,0);
}

void animate(void)
{
    CFanimation* current;
    CFanimation* next;
    struct timeval now;
    static struct timeval yesterday;
    static int already_passed=0;
    long int delta_milli;
    (void) GETTIMEOFDAY(&now);
    if (!already_passed)
    {
        already_passed=1;
        memcpy (&yesterday,&now,sizeof (struct timeval));
        return;
    }
    delta_milli=(now.tv_sec-yesterday.tv_sec)*1000+(now.tv_usec/1000-yesterday.tv_usec/1000);
    /*printf ("Working for %ld milli seconds\n",delta_milli);*/
    memcpy (&yesterday,&now,sizeof (struct timeval));
    for (current=first_animation;current;current=current->nextanimation)
        animate_one(current,delta_milli);
    current=first_animation;
    while (current)
    {
        if (!current->nextmovement)
        {
            next=current->nextanimation;
            if (first_animation==current)
                first_animation=next;
            if (current->name)
                free (current->name);
            free (current);
            current=next;
        }
        else
            current=current->nextanimation;
    }
}








void initContextStack(void)
{
    current_context = NULL;
    context_stack = NULL;
}

void pushContext(CFPContext* context)
{
    if (current_context == NULL)
    {
        context_stack = context;
        context->down = NULL;
    }
    else
    {
        context->down = current_context;
    }
    current_context = context;
}

CFPContext* popContext()
{
    CFPContext* oldcontext;
    if (current_context != NULL)
    {
        oldcontext = current_context;
        current_context = current_context->down;
        return oldcontext;
    }
    else
        return NULL;
}

CF_PLUGIN int initPlugin(const char* iversion, f_plug_api gethooksptr)
{
    gethook = gethooksptr;

    cf_init_plugin( gethook );
    cf_log(llevDebug, "CFAnim 2.0a init\n");

    /* Place your initialization code here */
    return 0;
}

CF_PLUGIN void* getPluginProperty(int* type, ...)
{
    va_list args;
    char* propname;

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

CF_PLUGIN int postInitPlugin(void)
{
    int hooktype = 1;
    int rtype = 0;

    cf_log(llevDebug, "CFAnim 2.0a post init\n");
    registerGlobalEvent =   gethook(&rtype,hooktype,"cfapi_system_register_global_event");
    unregisterGlobalEvent = gethook(&rtype,hooktype,"cfapi_system_unregister_global_event");
    reCmp                 = gethook(&rtype,hooktype,"cfapi_system_re_cmp");
    initContextStack();
    /* Pick the global events you want to monitor from this plugin */
    registerGlobalEvent(NULL,EVENT_CLOCK,PLUGIN_NAME,globalEventListener);
    return 0;
}

CF_PLUGIN void* globalEventListener(int* type, ...)
{
    va_list args;
    static int rv=0;
    CFPContext* context;
    char* buf;
    player* pl;
    context = malloc(sizeof(CFPContext));

    va_start(args, type);
    context->event_code = va_arg(args, int);

    context->message[0]=0;

    context->who         = NULL;
    context->activator   = NULL;
    context->third       = NULL;
    context->event       = NULL;
    rv = context->returnvalue = 0;
    switch(context->event_code)
    {
        case EVENT_CRASH:
            cf_log(llevDebug, "Unimplemented for now\n");
            break;
        case EVENT_BORN:
            context->activator = va_arg(args, object*);
            break;
        case EVENT_PLAYER_DEATH:
            context->who = va_arg(args, object*);
            break;
        case EVENT_GKILL:
            context->who = va_arg(args, object*);
            context->activator = va_arg(args, object*);
            break;
        case EVENT_LOGIN:
            pl = va_arg(args, player*);
            context->activator = pl->ob;
            buf = va_arg(args, char*);
            if (buf !=0)
                strcpy(context->message,buf);
            break;
        case EVENT_LOGOUT:
            pl = va_arg(args, player*);
            context->activator = pl->ob;
            buf = va_arg(args, char*);
            if (buf !=0)
                strcpy(context->message,buf);
            break;
        case EVENT_REMOVE:
            context->activator = va_arg(args, object*);
            break;
        case EVENT_SHOUT:
            context->activator = va_arg(args, object*);
            buf = va_arg(args, char*);
            if (buf !=0)
                strcpy(context->message,buf);
            break;
        case EVENT_MUZZLE:
            context->activator = va_arg(args, object*);
            buf = va_arg(args, char*);
            if (buf !=0)
                strcpy(context->message,buf);
            break;
        case EVENT_KICK:
            context->activator = va_arg(args, object*);
            buf = va_arg(args, char*);
            if (buf !=0)
                strcpy(context->message,buf);
            break;
        case EVENT_MAPENTER:
            context->activator = va_arg(args, object*);
            break;
        case EVENT_MAPLEAVE:
            context->activator = va_arg(args, object*);
            break;
        case EVENT_CLOCK:
            break;
        case EVENT_MAPRESET:
            buf = va_arg(args, char*);
            if (buf !=0)
                strcpy(context->message,buf);
            break;
        case EVENT_TELL:
            break;
    }
    va_end(args);
    context->returnvalue = 0;

    pushContext(context);
    /* Put your plugin action(s) here */

    context = popContext();
    rv = context->returnvalue;
    free(context);

    return &rv;
}

CF_PLUGIN void* eventListener(int* type, ...)
{
    static int rv=0;
    va_list args;
    char* buf;
    CFPContext* context;

    context = malloc(sizeof(CFPContext));

    context->message[0]=0;

    va_start(args,type);

    context->who         = va_arg(args, object*);
    /*context->event_code  = va_arg(args,int);*/
    context->activator   = va_arg(args, object*);
    context->third       = va_arg(args, object*);
    buf                  = va_arg(args, char*);
    if (buf !=0)
        strcpy(context->message,buf);
    context->fix         = va_arg(args, int);
    context->event       = va_arg(args, object*);
    context->event_code  = context->event->subtype;
    cf_get_maps_directory(context->event->slaying, context->script, sizeof(context->script));
    strcpy(context->options,context->event->name);
    context->returnvalue = 0;
    va_end(args);

    pushContext(context);
    /* Put your plugin action(s) here */
    cf_log(llevDebug, "CFAnim: %s called animator script %s, options are %s\n",
           context->activator->name,
           context->script,
           context->options);

    context->returnvalue = start_animation(context->who, context->activator,
                                           context->script, context->options);

    context = popContext();
    rv = context->returnvalue;
    free(context);
    cf_log(llevDebug, "Execution complete");
    return &rv;
}

CF_PLUGIN int   closePlugin()
{
    cf_log(llevDebug, "CFAnim 2.0a closing\n");
    return 0;
}

