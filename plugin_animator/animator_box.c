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
int get_dir_from_name (char*name)
    {
    if (!strcmp(name,"north")) return 1;
    if (!strcmp(name,"north_east")) return 2;
    if (!strcmp(name,"east")) return 3;
    if (!strcmp(name,"south_east")) return 4;
    if (!strcmp(name,"south")) return 5;
    if (!strcmp(name,"south_west")) return 6;
    if (!strcmp(name,"west")) return 7;
    if (!strcmp(name,"north_west")) return 8;
    return -1;
    }

long int initmovement(char* name,char* parameters,struct CFmovement_struct* move_entity)
{
    int dir;
    dir=get_dir_from_name (name);
    move_entity->parameters=NULL;
    return dir;
}
int runmovement(struct CFanimation_struct* animation, long int id, void* parameters)
{
    object* op=animation->victim;
    CFParm* CFP;
    int dir=id;
    printf (" moving in direction %ld\n",id);
    GCFP.Value[0]=op;
    GCFP.Value[1]=&dir;
    GCFP.Value[2]=op;
    if (op->type==PLAYER) CFP=(PlugHooks[HOOK_MOVEPLAYER])(&GCFP);
    else CFP=(PlugHooks[HOOK_MOVEOBJECT])(&GCFP);
    return 1;
}

long int initfire(char* name,char* parameters,struct CFmovement_struct* move_entity)
{
    int dir;
    dir=get_dir_from_name (&(name[5]));
    move_entity->parameters=NULL;
    return dir;
}
int runfire(struct CFanimation_struct* animation, long int id, void* parameters)
{
    printf ("Firing in direction %ld\n",id);
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
    printf ("Turning in direction %ld\n",id);
    op->facing=dir;
    GCFP.Value[0]=op;
    GCFP.Value[1]=&face;
    (PlugHooks[HOOK_SETANIMATION])(&GCFP);
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
    printf ("Cameraing in direction %ld\n",id);
    return 1;
    if (animation->victim->type==PLAYER)
        hook_scroll_map (animation->victim,id);
    else printf ("Not a player\n");
    return 1;
}

long int initvisible (char* name, char* parameters, struct CFmovement_struct* move_entity)
{
    int result;
    if (get_boolean (parameters,&result)) return result;
    printf ("Error in animation: parameters of invisible must be yes or no\n");
    return -1;
}
int runvisible(struct CFanimation_struct* animation, long int id, void* parameters)
{
    if (id==-1) return 1;
    animation->invisible=id;
    return 1;
}

long int initwizard (char* name, char* parameters, struct CFmovement_struct* move_entity)
{
    int result;
    if (get_boolean (parameters,&result)) return result;
    printf ("Error in animation: parameters of wizard must be yes or no\n");
    return -1;
}
int runwizard(struct CFanimation_struct* animation, long int id, void* parameters)
{
    if (id==-1) return 1;
    animation->wizard=id;
    return 1;
}
long int initsay (char* name, char* parameters, struct CFmovement_struct* move_entity)
{
    if (parameters) move_entity->parameters=strdup_local (parameters);
    else move_entity->parameters=NULL;
    printf ("init say: parameters: %p\n",parameters);
    return 1;
}
int runsay(struct CFanimation_struct* animation, long int id, void* parameters)
{
    GCFP.Value[0]=animation->victim;
    GCFP.Value[1]=parameters;
    if (parameters)
        {
        (PlugHooks[HOOK_COMMUNICATE])(&GCFP);
        free (parameters);
        printf ("Saying something\n");
        }
    else printf ("Error in animation: nothing to say with say function\n");
    return 1;
}
long int initapply (char* name, char* parameters, struct CFmovement_struct* move_entity)
{
    return 1;
}
int runapply(struct CFanimation_struct* animation, long int id, void* parameters)
{
    object* current_container;
    if (animation->victim->type!=PLAYER) return 0;
    current_container=animation->victim->container;
    animation->victim->container=NULL;
    GCFP.Value[0]=animation->victim;
    (PlugHooks[HOOK_APPLYBELOW])(&GCFP);
    animation->victim->container=current_container;
    return 1;
}
long int initapplyobject (char* name, char* parameters, struct CFmovement_struct* move_entity)
{
    move_entity->parameters=parameters?hook_add_string(parameters):NULL;
    return 1;
}
int runapplyobject(struct CFanimation_struct* animation, long int id, void* parameters)
{
    object* current;
    int aflag;
    CFParm* CFP;
    if (!parameters) return 0;
    for (current=animation->victim->below;current;current=current->below)
        if(current->name==parameters) break;
    if (!current)
        for (current=animation->victim->inv;current;current=current->below)
            if(current->name==parameters) break;
    if (!current)
        {
        hook_free_string (parameters);
        return 0;
        }
    aflag=AP_APPLY;
    GCFP.Value[0]=animation->victim;
    GCFP.Value[1]=current;
    GCFP.Value[2]=&aflag;
    CFP=(PlugHooks[HOOK_MANUALAPPLY])(&GCFP);
    hook_free_string (parameters);
    free (CFP);
    return 1;
}

long int initdropobject (char* name, char* parameters, struct CFmovement_struct* move_entity)
{
    move_entity->parameters=parameters?strdup_local(parameters):NULL;
    return 1;
}
int rundropobject(struct CFanimation_struct* animation, long int id, void* parameters)
{
    CFParm* CFP;
    GCFP.Value[0]=animation->victim;
    GCFP.Value[1]=parameters;
    if (!parameters) return 0;
    CFP=(PlugHooks[HOOK_CMDDROP])(&GCFP);
    free (parameters);
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
    if (!current) return 0;
    GCFP.Value[0]=animation->victim;
    GCFP.Value[1]=current;
    (PlugHooks[HOOK_PICKUP])(&GCFP);
    return 1;
}

long int initpickupobject (char* name, char* parameters, struct CFmovement_struct* move_entity)
{
    move_entity->parameters=parameters?hook_add_string(parameters):NULL;
    return 1;
}
int runpickupobject(struct CFanimation_struct* animation, long int id, void* parameters)
{
    object* current;
    if (!parameters) return 0;
    for (current=animation->victim->below;current;current=current->below)
        if(current->name==parameters) break;
    if (current)
        {
        GCFP.Value[0]=animation->victim;
        GCFP.Value[1]=current;
        (PlugHooks[HOOK_PICKUP])(&GCFP);
        }
    hook_free_string (parameters);
    return 1;
}
long int initghosted (char* name, char* parameters, struct CFmovement_struct* move_entity)
{
    int result;
    if (get_boolean (parameters,&result)) return result;
    printf ("Error in animation: parameters of ghosted must be yes or no\n");
    return -1;
}
int runghosted(struct CFanimation_struct* animation, long int id, void* parameters)
{
    CFParm* CFP;
    object* corpse;
    int val;
    if ( (id && animation->ghosted) ||
         (!id && !animation->ghosted) )
        runghosted(animation, !id, parameters);
    if (id) /*Create a ghost/corpse pair*/
        {
        GCFP.Value[0]=animation->victim;
        val=1;
        GCFP.Value[1]=&val;
        CFP=(PlugHooks[HOOK_CLONEOBJECT])(&GCFP);
        corpse=CFP->Value[0];
        free (CFP);
        corpse->x=animation->victim->x;
        corpse->y=animation->victim->y;
        corpse->type=0;
        corpse->contr=NULL;
        GCFP.Value[0]=corpse;
        GCFP.Value[1]=animation->victim->map;
        GCFP.Value[2]=NULL;
        val=0;
        GCFP.Value[3]=&val;
        CFP=(PlugHooks[HOOK_INSERTOBJECTINMAP])(&GCFP);
        free (CFP);
        animation->wizard=1;
        animation->invisible=1;
        animation->corpse=corpse;
        }
    else /*Remove a corpse, make current player visible*/
        {
        animation->wizard=0;
        animation->invisible=0;
        GCFP.Value[0]=animation->corpse;
        (PlugHooks[HOOK_REMOVEOBJECT])(&GCFP);
        (PlugHooks[HOOK_FREEOBJECT])(&GCFP);
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
    printf (".(%s)\n",parameters);
    if (!parameters)
        {
        printf ("Alert: no parameters for teleport\n");
        return 0;
        }
    mapname=strstr (parameters," ");
    printf (".(%s)\n",parameters);
    if (!mapname) return 0;
    *mapname='\0';
    mapx=atoi(parameters);
    mapname++;
    parameters=mapname;
    if (!parameters)
        {
        printf ("Alert: no enough parameters for teleport\n");
        return 0;
        }
    printf (".(%s)\n",parameters);
    mapname=strstr (parameters," ");
    printf (".\n");
    if (!mapname) return 0;
    *mapname='\0';
    mapy=atoi(parameters);
    mapname++;
    if (mapname[0]=='\0') return 0;
    teleport=(teleport_params*)malloc (sizeof(teleport_params));
    teleport->mapname=strdup_local (mapname);
    teleport->mapx=mapx;
    teleport->mapy=mapy;
    move_entity->parameters=teleport;
    return 1;
}
int runteleport(struct CFanimation_struct* animation, long int id, void* parameters)
{
    int val=0;
    teleport_params* teleport=(teleport_params*)parameters;
    if (!parameters) return 0;
    GCFP.Value[0]=animation->victim;
    GCFP.Value[1]=teleport->mapname;
    GCFP.Value[2]=&teleport->mapx;
    GCFP.Value[3]=&teleport->mapy;
    GCFP.Value[4]=&val;
    GCFP.Value[5]=NULL;
    (PlugHooks[HOOK_TELEPORTOBJECT])(&GCFP);
    free (parameters);
    return 1;
}

long int initnotice (char* name, char* parameters, struct CFmovement_struct* move_entity)
{
    move_entity->parameters=parameters?strdup_local(parameters):NULL;
    return 1;
}
int runnotice(struct CFanimation_struct* animation, long int id, void* parameters)
{
    int val;
    int pri;

    val = NDI_NAVY|NDI_UNIQUE;
    pri = 0;

    GCFP.Value[0] = (&val);
    CFGP.Value[1] = (&pri);
    GCFP.Value[2] = (animation->victim);
    GCFP.Value[3] = (parameters);
    (PlugHooks[HOOK_NEWDRAWINFO])(&GCFP);
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

void prepare_commands ()
{
    qsort (animationbox,animationcount,sizeof (CFanimationHook),compareAnims);
    ordered_commands=1;
}

CFanimationHook* get_command (char* command)
{
    CFanimationHook dummy;
    dummy.name=command;
    if (!ordered_commands) prepare_commands();
    return (CFanimationHook*) bsearch (&dummy,
                                       animationbox,
                                       animationcount,
                                       sizeof (CFanimationHook),
                                       compareAnims);
}

CFmovement* parse_animation_block (char* buffer, size_t buffer_size,FILE* fichier, CFanimation* parent)
{
    CFmovement* first=NULL;
    CFmovement* current=NULL;
    CFmovement* next;
    char* time;
    char* name;
    char* parameters;
    int tick;
    CFanimationHook* animationhook;
    if (parent->verbose) printf ("In parse block for %s\n",buffer);
    while (fgets(buffer,buffer_size,fichier))
      {
      if (buffer[0]=='[') break;
      if (buffer[0]=='#') continue;
      buffer[strlen(buffer)-strlen("\n")]='\0';
      while (buffer[strlen(buffer)-1]==' ')
          buffer[strlen(buffer)-1]='\0';
      if (strlen (buffer)<=0) continue;
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
      printf ("\n");
      if (parent->verbose)
          {
          if (!animationhook)
              printf ("%s : Unknown animation command\n",name);
          else
             printf ("Parsed %s -> %p\n",name,animationhook);
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
    printf ("\n");
    return first;
}
