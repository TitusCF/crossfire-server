/*****************************************************************************/
/* Animator Plugin                                                           */
/*****************************************************************************/
/* The purpose of this plugin is to provide an animation/movie system for    */
/* Crossfire. Read anim files.txt to see the syntax of animation files.      */
/*****************************************************************************/
/* Contact: tchize@mailandnews.com                                           */
/*****************************************************************************/
/* That code is placed under the GNU General Public Licence (GPL)            */
/* (C)2001 by tchize (Delbecq David)                                         */
/*****************************************************************************/

/* First let's include the header file needed                                */
#include <plugin_animator.h>
#include <animator_box.h>
static CFParm GCFP;
static CFParm GCFP0;
static CFParm GCFP1;
static CFParm GCFP2;
int StackPosition=0;
object* StackActivator[MAX_RECURSIVE_CALL];
object* StackWho[MAX_RECURSIVE_CALL];
object* StackOther[MAX_RECURSIVE_CALL];
char* StackText[MAX_RECURSIVE_CALL];
int StackParm1[MAX_RECURSIVE_CALL];
int StackParm2[MAX_RECURSIVE_CALL];
int StackParm3[MAX_RECURSIVE_CALL];
int StackParm4[MAX_RECURSIVE_CALL];
int StackReturn[MAX_RECURSIVE_CALL];
int current_event= EVENT_NONE;
CFanimation *first_animation=NULL;

int stubfunc (object* op, char* params)
{
    return 1;
}

int is_animated_player (object* pl)
{
    CFanimation* current;
    for (current=first_animation;current;current++)
        if ((current->victim==pl) && (current->paralyze))
            {
            if (current->verbose) printf ("    Catching a command for paralyzed player %s.\n",pl->name);
            return 1;
            }
    return 0;
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
/*
 * return a new animation pointer inserted in the list of animations
 */
CFanimation* create_animation ()
{
   CFanimation* new;
   CFanimation* current;
   new=(CFanimation*) malloc (sizeof (CFanimation));
   if (!new) return NULL;
   new->name=NULL;
   new->victim=NULL;
   new->nextmovement=NULL;
   new->nextanimation=NULL;
   for (current=first_animation;(current && current->nextanimation);current=current->nextanimation);
   if (!current) first_animation=new;
   else current->nextanimation=new;
   return new;
}
/*
 * Create a new animation object according to file, option and activator (who)
 */
int start_animation (object* who,object* activator,char* file, char* options)
{
    FILE* fichier;
    char* name=NULL;
    int victimtype=0;
    object* victim=NULL;
    int unique=0;
    int always_delete=0;
    int parallel=0;
    int paralyzed=1;
    int invisible=0;
    int wizard=0;
    enum time_enum timetype;
    int errors_allowed=0;
    int verbose=0;
    char* animationitem;
    char buffer[HUGE_BUF];
    char* variable;
    char* value;
    int errors_found=0;
    CFanimation* current_anim;
    fichier=fopen (create_pathname(file),"r");
    while (fgets (buffer,HUGE_BUF,fichier))
        {
        if (buffer[0]=='[') break;
        if (buffer[0]=='#') continue;
        if (!strcmp(buffer,"\n")) continue;
        errors_found=1;
        printf ("error: %s not in a group.\n",buffer);
        }
    if (feof (fichier)) return 0;
    if (strncmp (buffer,"[Config]",8))
        {
        printf ("Fatal error in %s: [Config] must be the first definition group: %s\n",file,buffer);
        return 0;
        }
    while (fgets (buffer,HUGE_BUF,fichier))
        {
        if (buffer[0]=='[') break;
        if (buffer[0]=='#') continue;
        if (!strcmp(buffer,"\n")) continue;
        if (!equality_split (buffer,&variable,&value))
            errors_found=1;
        else
            {
            if (!strcmp (variable,"name"))
                {
                if (*value=='"') value++;
                if (value[strlen(value)-1] == '"') value[strlen(value)-1]='\0';
                name=strdup_local (value);
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
                printf ("Setting victim to %s\n",value);
                if (!strcmp (value,"who")) victim=who;
                else if (!strcmp (value,"activator")) victim=activator;
                else if (!strcmp (value,"who_owner"))
                    if (!who)
                      {
                      errors_found=1;
                      printf ("Warning: object \"who\" doesn't exist and you victimized it's owner\n");
                      }
                    else victim=who->env;
                else if (!strcmp (value,"activator_owner"))
                    if (!activator)
                      {
                      errors_found=1;
                      printf ("Warning: object \"activator\" doesn't exist and you victimized it's owner\n");
                      }
                    else victim=activator->env;
                else errors_found=1;
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
                animationitem=strdup_local(value);
                }
            else errors_found=1;
            }
        }
    if (buffer[0]=='\0')
        {
        printf ("Errors occured during parsing of animation file\n");
        return 0;
        }
    if (!(current_anim=create_animation()))
        {
        printf ("Emergency! No more memory available!!\n");
        return 0;
        }
    if (always_delete)
        {
        if (verbose) printf ("Freeing event N° %d for %s.\n",current_event,who->name);
        if (who->event_hook[current_event])
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
            }
        }
    if (!victim)
        {
        printf ("Fatal error: victim is NULL");
        return 0;
        }
    if ( ( (victim->type==PLAYER) && (victimtype==1)) ||
         ( (victim->type!=PLAYER) && (victimtype==0))  ||
         ( errors_found && !errors_allowed) )
        {
        if (verbose) printf ("No correct victim found or errors found. forgetting\n");
        return 0;
        }

    if (unique && !always_delete)
        {
        if (verbose) printf ("Freeing event N° %d for %s.\n",current_event,who->name);
        if (who->event_hook[current_event])
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
            }
        }
    current_anim->name=name;
    current_anim->victim=victim;
    current_anim->paralyze=paralyzed;
    current_anim->invisible=invisible;
    current_anim->wizard=wizard;
    current_anim->unique=unique;
    current_anim->ghosted=0;
    current_anim->corpse=NULL;
    current_anim->time_representation=timetype;
    current_anim->verbose=verbose;
    current_anim->tick_left=0;
    current_anim->errors_allowed=errors_allowed;
    while (buffer[0]=='[')
        {
        if (strncmp (&buffer[1],animationitem,strlen(animationitem)))
            while (fgets(buffer,HUGE_BUF,fichier))
                if (buffer[0]=='[') break;
        current_anim->nextmovement=parse_animation_block (buffer,HUGE_BUF,fichier,current_anim);
        if (current_anim->nextmovement) break;
        }
    fclose (fichier);
    return 1;
}
void animate_one (CFanimation* animation, long int milliseconds)
{
    CFmovement* current;
    int val;
    if (animation->time_representation==time_second) animation->tick_left+=milliseconds;
    else animation->tick_left++;
    if (animation->verbose)
        printf ("Ticking %s for %s. Tickleft is %d\n",animation->name,animation->victim->name,animation->tick_left);
    if (animation->invisible) animation->victim->invisible=10;
    if (animation->wizard)
        {
        if (animation->verbose)
            printf ("Setting wizard flags\n");
        SET_FLAG (animation->victim,FLAG_WIZPASS);
        SET_FLAG (animation->victim,FLAG_WIZ);
        }
    GCFP.Value[0]=animation->victim;
    val=UP_OBJ_CHANGE;
    GCFP.Value[1]=&val;
    (PlugHooks[HOOK_UPDATEOBJECT])(&GCFP);
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
    CLEAR_FLAG (animation->victim,FLAG_WIZPASS);
    CLEAR_FLAG (animation->victim,FLAG_WIZ);
}

void animate()
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
        animate_one (current,delta_milli);
    current=first_animation;
    while (current)
        {
        if (!current->nextmovement)
            {
            next=current->nextanimation;
            if (first_animation==current) first_animation=next;
            if (current->name) free (current->name);
            free (current);
            current=next;
            }
        else
            current=current->nextanimation;
        }
}

/*****************************************************************************/
/* The Plugin Management Part.                                               */
/* Most of the functions below should exist in any CF plugin. They are used  */
/* to glue the plugin to the server core. All functions follow the same      */
/* declaration scheme (taking a CFParm* arg, returning a CFParm) to make the */
/* plugin interface as general as possible. And since the loading of modules */
/* isn't time-critical, it is never a problem. It could also make using      */
/* programming languages other than C to write plugins a little easier, but  */
/* this has yet to be proven.                                                */
/*****************************************************************************/
char* hook_add_string (char* text){
    CFParm* result;
    char* val;
    GCFP.Value[0]=(void*)text;
    result=(PlugHooks[HOOK_ADDSTRING])(&GCFP);
    val=(char*)result->Value[0];
    free (result);
    return val;
}

char* hook_add_refcount (char* text){
    CFParm* result;
    char* val;
    GCFP.Value[0]=(void*)text;
    result=(PlugHooks[HOOK_ADDREFCOUNT])(&GCFP);
    val=(char*)result->Value[0];
    free (result);
    return val;
}

void hook_free_string (char* text){
    GCFP.Value[0]=(void*)text;
    (PlugHooks[HOOK_FREESTRING])(&GCFP);
    return ;
}

int hook_move_player (object* player, int dir)
{
    CFParm* CFP;
    int val;
    if (player->type!=PLAYER) return 0;
    GCFP.Value[0]=player;
    GCFP.Value[1]=&dir;
    CFP=(PlugHooks[HOOK_MOVEPLAYER])(&GCFP);
    val=*(int*)CFP->Value[0];
    free (CFP);
    return val;
}

int hook_move_ob (object* what, int dir, object* originator)
{
    CFParm* CFP;
    int val;
    GCFP.Value[0]=what;
    GCFP.Value[1]=&dir;
    GCFP.Value[2]=originator;
    CFP=(PlugHooks[HOOK_MOVEOBJECT])(&GCFP);
    val=*(int*)CFP->Value[0];
    free (CFP);
    return val;
}
void hook_scroll_map (object* player, int dir)
{
    if (player->type!=PLAYER) return;
    GCFP.Value[0]=player;
    GCFP.Value[1]=&dir;
    /*(PlugHooks[HOOK_SCROLLMAP])(&GCFP);*/
}
/*****************************************************************************/
/* Called whenever a Hook Function needs to be connected to the plugin.      */
/*****************************************************************************/
CFParm* registerHook(CFParm* PParm)
{
    int Pos;
    f_plugin Hook;
    Pos = *(int*)(PParm->Value[0]);
    Hook=(f_plugin)(PParm->Value[1]);
    PlugHooks[Pos]=Hook;
    return NULL;
};

/*****************************************************************************/
/* Called whenever an event is triggered, both Local and Global ones.        */
/*****************************************************************************/
/* Two types of events exist in CF:                                          */
/* - Local events: They are triggered by a single action on a single object. */
/*                 Nearly any object can trigger a local event               */
/*                 To warn the plugin of a local event, the map-maker needs  */
/*                 to use the event... tags in the objects of their maps.    */
/* - Global events: Those are triggered by actions concerning CF as a whole. */
/*                 Those events may or may not be triggered by a particular  */
/*                 object; they can't be specified by event... tags in maps. */
/*                 The plugin should register itself for all global events it*/
/*                 wants to be aware of.                                     */
/* Why those two types ? Local Events are made to manage interactions between*/
/* objects, for example to create complex scenarios. Global Events are made  */
/* to allow logging facilities and server management. Global Events tends to */
/* require more CPU time than Local Events, and are sometimes difficult to   */
/* bind to any specific object.                                              */
/*****************************************************************************/
CFParm* triggerEvent(CFParm* PParm)
{
    int eventcode;
    int result;
    eventcode = *(int *)(PParm->Value[0]);
    current_event= eventcode;
    switch(eventcode)
    {
        case EVENT_NONE:
            printf("PLUGIN - Warning - EVENT_NONE requested\n");
            break;
        case EVENT_ATTACK:
        case EVENT_APPLY:
        case EVENT_DEATH:
        case EVENT_DROP:
        case EVENT_PICKUP:
        case EVENT_SAY:
        case EVENT_STOP:
        case EVENT_TELL:
        case EVENT_TIME:
        case EVENT_THROW:
        case EVENT_TRIGGER:
        case EVENT_CLOSE:
            result = HandleEvent(PParm);
            break;
        case EVENT_BORN:
        case EVENT_CRASH:
        case EVENT_LOGIN:
        case EVENT_LOGOUT:
        case EVENT_REMOVE:
        case EVENT_SHOUT:
        case EVENT_MAPENTER:
        case EVENT_MAPLEAVE:
        case EVENT_CLOCK:
        case EVENT_MAPRESET:
            result = HandleGlobalEvent(PParm);
            break;
    };
    GCFP.Value[0] = (void *)(&result);
    return &GCFP;
};

/*****************************************************************************/
/* Handles standard local events.                                            */
/*****************************************************************************/
int HandleGlobalEvent(CFParm* PParm)
{
    if (StackPosition == MAX_RECURSIVE_CALL)
    {
        printf("Can't execute script - No space left of stack\n");
        return 0;
    };

    StackPosition++;

    switch(*(int *)(PParm->Value[0]))
    {
        case EVENT_CRASH:
            printf("Unimplemented for now\n");
            break;
        case EVENT_BORN:
            StackActivator[StackPosition] = (object *)(PParm->Value[1]);
            break;
        case EVENT_LOGIN:
            StackActivator[StackPosition] = ((player *)(PParm->Value[1]))->ob;
            StackWho[StackPosition] = ((player *)(PParm->Value[1]))->ob;
            StackText[StackPosition] = (char *)(PParm->Value[2]);
            break;
        case EVENT_LOGOUT:
            StackActivator[StackPosition] = ((player *)(PParm->Value[1]))->ob;
            StackWho[StackPosition] = ((player *)(PParm->Value[1]))->ob;
            StackText[StackPosition] = (char *)(PParm->Value[2]);
            break;
        case EVENT_REMOVE:
            StackActivator[StackPosition] = (object *)(PParm->Value[1]);
            break;
        case EVENT_SHOUT:
            StackActivator[StackPosition] = (object *)(PParm->Value[1]);
            StackText[StackPosition] = (char *)(PParm->Value[2]);
            break;
        case EVENT_MAPENTER:
            StackActivator[StackPosition] = (object *)(PParm->Value[1]);
            break;
        case EVENT_MAPLEAVE:
            StackActivator[StackPosition] = (object *)(PParm->Value[1]);
            break;
        case EVENT_CLOCK:
            animate();
            break;
        case EVENT_MAPRESET:
            StackText[StackPosition] = (char *)(PParm->Value[1]);/* Map name/path */
            break;
    };
    StackPosition--;
    return 0;
};

/*****************************************************************************/
/* Handles standard local events.                                            */
/*****************************************************************************/
int HandleEvent(CFParm* PParm)
{
  /*  if (StackPosition == MAX_RECURSIVE_CALL)
    {
        printf("PLUGIN - Can't execute script - No space left of stack\n");
        return 0;
    };
    StackPosition++;
    StackActivator[StackPosition]   = (object *)(PParm->Value[1]);
    StackWho[StackPosition]         = (object *)(PParm->Value[2]);
    StackOther[StackPosition]       = (object *)(PParm->Value[3]);
    StackText[StackPosition]        = (char *)(PParm->Value[4]);
    StackParm1[StackPosition]       = *(int *)(PParm->Value[5]);
    StackParm2[StackPosition]       = *(int *)(PParm->Value[6]);
    StackParm3[StackPosition]       = *(int *)(PParm->Value[7]);
    StackParm4[StackPosition]       = *(int *)(PParm->Value[8]);
    StackReturn[StackPosition]      = 0;

    if (StackParm4[StackPosition] == SCRIPT_FIX_ALL)
    {
        if (StackOther[StackPosition] != NULL)
            fix_player(StackOther[StackPosition]);
        if (StackWho[StackPosition] != NULL)
            fix_player(StackWho[StackPosition]);
        if (StackActivator[StackPosition] != NULL)
            fix_player(StackActivator[StackPosition]);
    }
    else if (StackParm4[StackPosition] == SCRIPT_FIX_ACTIVATOR)
    {
        fix_player(StackActivator[StackPosition]);
    };
    StackPosition--;
    return StackReturn[StackPosition];*/
    printf ("printf %s called animator->%p with file %s and options %s\n",
            PParm->Value[2]?((object *)(PParm->Value[2]))->name:"",
            PParm->Value[1],
            (char *)(PParm->Value[9]),
            (char *)(PParm->Value[10]));
    return start_animation ((object *)(PParm->Value[2]),
                            (object *)(PParm->Value[1]),
                            (char *)(PParm->Value[9]),
                            (char *)(PParm->Value[10]));
};

/*****************************************************************************/
/* Plugin initialization.                                                    */
/*****************************************************************************/
/* It is required that:                                                      */
/* - The first returned value of the CFParm structure is the "internal" name */
/*   of the plugin, used by objects to identify it.                          */
/* - The second returned value is the name "in clear" of the plugin, used for*/
/*   information purposes.                                                   */
/*****************************************************************************/
CFParm* initPlugin(CFParm* PParm)
{
    char buf[MAX_BUF];
    char buf2[MAX_BUF];
    strcpy(buf,"Animator");
    strcpy(buf2,"Animator Plugin V0.1");
    GCFP.Value[0] = (void *)(buf);
    GCFP.Value[1] = (void *)(buf2);
    return &GCFP;
};

/*****************************************************************************/
/* Used to do cleanup before killing the plugin.                             */
/*****************************************************************************/
CFParm* removePlugin(CFParm* PParm)
{
        return NULL;
};

/*****************************************************************************/
/* This function is called to ask various informations to the plugin.        */
/*****************************************************************************/
CFParm* getPluginProperty(CFParm* PParm)
{
    double dblval = 0.0;
    static float val = 1.0;
    int i;
    if (PParm!=NULL)
    {
        if(!strcmp((char *)(PParm->Value[0]),"command?"))
        {
        if (is_animated_player ((object*)PParm->Value[2]))
            {
            GCFP.Value[0]=NULL;
            GCFP.Value[1]=stubfunc;
            GCFP.Value[2]=&val;
            return &GCFP;
            }
        }
        else
        {
            printf("PLUGIN - Unknown property tag: %s\n",(char *)(PParm->Value[0]));
        };
    };
    return NULL;
};

/*****************************************************************************/
/* The postinitPlugin function is called by the server when the plugin load  */
/* is complete. It lets the opportunity to the plugin to register some events*/
/*****************************************************************************/
CFParm* postinitPlugin(CFParm* PParm)
{
    int i;

    /* We can now register some global events if we want */
    /* We'll only register the global-only events :      */
    /* BORN, CRASH, LOGIN, LOGOUT, REMOVE, and SHOUT.    */
    /* The events APPLY, ATTACK, DEATH, DROP, PICKUP, SAY*/
    /* STOP, TELL, TIME, THROW and TRIGGER are already   */
    /* handled on a per-object basis and I simply don't  */
    /* see how useful they could be for the Python stuff.*/
    /* Registering them as local would be probably useful*/
    /* for extended logging facilities.                  */
    GCFP.Value[1] = (void *)(add_string("Animator"));
    i = EVENT_CLOCK;
    GCFP.Value[0] = (void *)(&i);
    (PlugHooks[HOOK_REGISTEREVENT])(&GCFP);
    return NULL;
};
