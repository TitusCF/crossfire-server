/*****************************************************************************/
/* The crossfire logging facility.                                           */
/* This plugin is part of a projects who's purpose is to track every         */
/* interesting thing happening in the crossfire server, from conversation    */
/* between player and their deaths to informations about server crashes.     */
/* The datas could then be loaded by php scripts to present info on a player,*/
/* important events of the day, current experience of all players,           */
/* crash-o-meter and so on.                                                  */
/*****************************************************************************/
/* This code is placed under the GNU General Public Licence (GPL) either     */
/* version 2 or higher, at your choice. You can get a copy of the GPL at the */
/* following address: http://www.gnu.org                                     */
/*                                                                           */
/* (C)2001 Tchize. Feel free to report any problem.                          */
/* tchize@mailandnews.com                                                    */
/*****************************************************************************/


#include <plugin_logging.h>
#ifndef __CEXTRACT__
#include <logger_proto.h>
#endif

#include <logger_plugin.h>

void log2file (char* text2log)
{
    time_t t = time(NULL);
    char buf[MAX_BUF];
    if (logging_file)
    {
        strftime(buf, sizeof(buf), "[%I:%M %p]", localtime(&t));
        fprintf (logging_file,"%s%s\n",buf,text2log);
    }
}

/*****************************************************************************/
/* function connect_database                                                 */
/*****************************************************************************/
/* Called at startup, this function gets an handle to the database           */
/* The DSN (Database Server Name) , username and password are given          */
/* at compile time or by an init file                                        */
/* Note: file logging facilities must be ready                               */
/*****************************************************************************/
void connect_database (void)
{
    char buf[MAX_BUF];
    // 1. allocate Environment handle and register version
    V_OD_erg=SQLAllocHandle(SQL_HANDLE_ENV,SQL_NULL_HANDLE,&V_OD_Env);
    if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO))
    {
        sprintf(buf,"[DATABASE]Error AllocHandle");
        log2file (buf);
        printf ("%s\n",buf);
        return;
    }

    V_OD_erg=SQLSetEnvAttr(V_OD_Env, SQL_ATTR_ODBC_VERSION,
                           (void*)SQL_OV_ODBC3, 0);
    if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO))
    {
        sprintf(buf,"[DATABASE]Error SetEnv");
        log2file (buf);
        printf ("%s\n",buf);
        SQLFreeHandle(SQL_HANDLE_ENV, V_OD_Env);
        return;
    }
    // 2. allocate connection handle, set timeout
    V_OD_erg = SQLAllocHandle(SQL_HANDLE_DBC, V_OD_Env, &V_OD_hdbc);
    if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO))
    {
        sprintf(buf,"[DATABASE]Error AllocHDB %ld",V_OD_erg);
        log2file (buf);
        printf ("%s\n",buf);
        SQLFreeHandle(SQL_HANDLE_ENV, V_OD_Env);
        return;
    }
    SQLSetConnectAttr(V_OD_hdbc, SQL_LOGIN_TIMEOUT, (SQLPOINTER *)5, 0);
    SQLSetConnectAttr(V_OD_hdbc, SQL_AUTOCOMMIT, (SQLPOINTER *)SQL_TRUE, 0);
    // 3. Connect to the datasource "web"
    V_OD_erg = SQLConnect(V_OD_hdbc, (SQLCHAR*) "PostgreSQL", SQL_NTS,
                                     (SQLCHAR*) "postgres", SQL_NTS,
                                     (SQLCHAR*) "gandalf", SQL_NTS);
    if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO))
    {
        sprintf(buf,"[DATABASE]Error SQLConnect %ld",V_OD_erg);
        printf ("%s\n",buf);
        log2file (buf);
        SQLGetDiagRec(SQL_HANDLE_DBC, V_OD_hdbc,1,
                      V_OD_stat, &V_OD_err,V_OD_msg,100,&V_OD_mlen);
        sprintf(buf,"[DATABASE]%s (%ld)\n",V_OD_msg,V_OD_err);
        printf ("%s\n",buf);
        log2file (buf);
        SQLFreeHandle(SQL_HANDLE_ENV, V_OD_Env);
        return;
    }
    log2file("[DATABASE]Connected");
}
void close_database(void)
{
    SQLDisconnect (V_OD_hdbc);
    SQLFreeHandle(SQL_HANDLE_ENV, V_OD_Env);
}

int database_insert (char * query)
{
    V_OD_erg=SQLAllocHandle(SQL_HANDLE_STMT, V_OD_hdbc, &V_OD_hstmt);
    if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO))
    {
                printf("Error in Alloc Statement Handle %ld\n",V_OD_erg);
                return -1;
    }
    V_OD_erg=SQLExecDirect(V_OD_hstmt,query,SQL_NTS);
    if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO))
    {
       printf("Error in Select %ld\n",V_OD_erg);
       return -1;
    }
    SQLFreeHandle(SQL_HANDLE_STMT,V_OD_hstmt);
    return 0;
}


/******************************************************************************
 * Events management part.
 * These function are responsible for logging events in a database,
 * using the database interface provided above and some external file.
 *****************************************************************************/

int last_sec=0;
int counter=0;
player_lvl zerolevel={1,0,1,0,1,0,1,0,1,0,NULL,1,0,1,0};
              /* level 1 everywhere and experience 0 everywhere*/
/*
 * function gettimestamp
 * This function puts in buffer a string corresponding to the current time
 * with a second precision. the precision pointer is used to give a more
 * unique timestamp. The pair timestamp-buffer is warranted to be unique
 * as long as you don't request more than 2^16 per second.
 */
void gettimestamp (char* buffer, size_t bufsize, int* precision)
{
    int new_sec;
    time_t t;
    t=time(NULL);
    new_sec=(localtime (&t))->tm_sec;
    if (new_sec!=last_sec)
    {
        last_sec=new_sec;
        counter=0;
    }
    else
        counter++;
    strftime (buffer,bufsize,"%Y-%m-%d %H:%M:%S", localtime (&t));
    *precision=counter;
}

/*
 * function getuniqid
 * This function returns a string that is warranted to be uniq
 * as long as the system clock does not get corrupted.
 * the string is maximum 32 chars length and contains
 * only the following characters: [A-Z][a-z][0-9]_
 */
char* getuniqid(void)
{
    char *p;
    int counter;
    char buffer[60];
    static char buffer2[65];
    gettimestamp (buffer,sizeof(buffer),&counter);
    for (p=buffer;(*p)!='\0';p++)
    {
        if ( ((*p)<'a' || ((*p)>'z')) &&
             ((*p)<'A' || ((*p)>'Z')) &&
             ((*p)<'0' || ((*p)>'9')) &&
             ((*p)!='_'))
        *p='_';
    }
    sprintf (buffer2,"%s%d",buffer,counter);
    while (strlen (buffer2) >32)
        /* buffer is too big. let's crypt a bit to reduce...*/
        for (counter=0;counter<(strlen(buffer2)-32);counter++)
          buffer[counter]=((buffer[counter]-'a')^(buffer[counter+32]-'a'))+'a';
        buffer[strlen(buffer2)-32]='\0';
    return buffer2;
}

/*
 * function getexp
 * This function fills the structure pointed by levels
 * with the current Xp and level of player in the differents
 * experience paths.
 * return *levels NULL in case of errors
 */
player_lvl* getexp (object* player, player_lvl* levels)
{
    object* op;
    printf ("In getexp...\n");
    memcpy (levels,&zerolevel,sizeof (player_lvl));
    if (player->type!=PLAYER) return NULL;
    for (op=player->inv;op;op=op->below)
    {
        if (op->type!=EXPERIENCE) continue;
        if (!strcmp (op->name,"agility"))
        {
            levels->agilitylvl=op->level;
            levels->agilityexp=op->stats.exp;
        }
        else if (!strcmp (op->name,"mental"))
        {
            levels->mentallvl=op->level;
            levels->mentalexp=op->stats.exp;
        }
        else if (!strcmp (op->name,"physique"))
        {
            levels->physiquelvl=op->level;
            levels->physiqueexp=op->stats.exp;
        }
        else if (!strcmp (op->name,"magic"))
        {
            levels->magiclvl=op->level;
            levels->magicexp=op->stats.exp;
        }
        else if (!strcmp (op->name,"wisdom"))
        {
            levels->wisdomlvl=op->level;
            levels->wisdomexp=op->stats.exp;
            levels->god=op->title;
        }
        else if (!strcmp (op->name,"personality"))
        {
            levels->personalitylvl=op->level;
            levels->personalityexp=op->stats.exp;
        }
    }
    levels->generallvl=player->level;
    levels->generalexp=player->stats.exp;
    return levels;
}

/*
 * function takescreenshoot
 * This function takes the list of objects
 * visible around the player op and write it
 * to a file in the screenshoot directory.
 * It then return the key to reference it later.
 * Since the los is the los of the player,
 * the resulting screenshoot if what player's client saw
 * and depend on the clientwidth and clientheight!
 */
char* takescreenshoot(object* op)
{
    int x,y;
    int clientx,clienty;
    static char key[33];
    char query[HUGE_BUF];
    char shootfile[MAX_BUF];
    char filename[VERY_BIG_BUF];
    static char smallfilename[MAX_BUF];
    char *p;
    int precision;
    object* tmp;
    FILE * screenshoot;
    printf ("....\n");
    strcpy (key,getuniqid());
    gettimestamp (shootfile,sizeof(shootfile),&precision);
    for (p=shootfile;(*p)!='\0';p++)
    {
        if ((*p)==' ')(*p)='_';
        if ((*p)=='-')(*p)='_';
        if ((*p)==':')(*p)='_';
    }

    sprintf (filename,"%s%s%s%d.sht",WEB_DIR,SCREENSHOT_DIR,shootfile,
        precision);
    sprintf (smallfilename,"%s%d",shootfile,precision);
    printf ("Will write screenshoot in %s\n",filename);

    screenshoot=fopen (filename,"w");
    if (!screenshoot) return key;
    fprintf (screenshoot,"MAP_SIZE %d,%d\n",op->contr->socket.mapy,
        op->contr->socket.mapy);
    for (clienty=0;clienty<(op->contr->socket.mapy);clienty++)
    {
        for (clientx=0;clientx<(op->contr->socket.mapx);clientx++)
        {
            if (op->contr->blocked_los[clientx][clienty]>3)
                 /*  face is not visible to client */
                 continue;
            x=op->x-(op->contr->socket.mapx-1)/2+clientx;
            y=op->y-(op->contr->socket.mapy-1)/2+clienty;


            fprintf (screenshoot,"%d,%d %s|%s|%s ",clientx,clienty,
                GET_MAP_FACE(op->map,x,y,0)?GET_MAP_FACE(op->map,x,y,0)->name:
                    "blank.111",
                GET_MAP_FACE(op->map,x,y,1)?GET_MAP_FACE(op->map,x,y,1)->name:
                    "blank.111",
                GET_MAP_FACE(op->map,x,y,2)?GET_MAP_FACE(op->map,x,y,2)->name:
                    "blank.111"
            );
            for(tmp=get_map_ob(op->map,x,y);tmp!=NULL&&tmp->above!=NULL;
                tmp=tmp->above);
            for ( ; tmp != NULL; tmp=tmp->below )
            {
                fprintf (screenshoot,"%s|",query_name(tmp));
                if (QUERY_FLAG(tmp, FLAG_IS_FLOOR))          /* don't continue under the floor */
                    break;
            }
            fprintf (screenshoot,"\n");
        }
    }
    fclose (screenshoot);
    sprintf (query,"INSERT INTO screenshoot (id,shootfile) VALUES ('%s','%s')",
        key,smallfilename);
    database_insert (query);
    return key;
}

/*
 * function saveinventory
 * This function saves the visible inventory
 * of op to a file. If id is given, it is
 * used as a generator for creating the filename.
 * The return value is the filename, relative to
 * the crossfire web directory. NULL in case of errors.
 * The return value points to an internal buffer which will
 * be overridden at next call!
 */
char* saveinventory (object* op, char* id)
{
    static char key[33];
    char fullfilename[VERY_BIG_BUF];
    static char filename[MAX_BUF];
    FILE *inventoryshoot;
    object *what;

    if (id)
        strcpy (key,id);
    else
        strcpy (key,getuniqid());

    sprintf (filename,"%s%s.inv",INVSHOT_DIR,key);
    sprintf (fullfilename,"%s%s",WEB_DIR,filename);
    inventoryshoot=fopen (fullfilename,"w");

    if (!inventoryshoot)
    {
        perror ("Failed to save inventory");
        return "";
    }
    for (what=op->inv;what;what=what->below)
    {
        if (what->invisible) continue;
        fprintf (inventoryshoot,"[%s%s] %s\n",
                 what->face->name,
                 QUERY_FLAG(what,FLAG_APPLIED)?"|Active":"",
                 query_short_name(what));
    }
    fclose (inventoryshoot);
    return filename;
};

/*
 * function saveplayerstats
 * This function save the player's current stats in the database
 * Input values:
 *   -----------
 *    object* op           The player being saved
 *    player_lvl* levels   The stats of the player.
 *                         NULL if have to guess
 *    char* inventoryfile  The file the inventory has been saved in.
 *                         NULL if not relevant
 *    char* timestamp
 *    int precision        The timestamp used to give time information to stats
 *    char* key            The ident to use for those datas in database.
 *                         NULL if still has to create
 *
 * Return value: key points to an internal buffer if key was not provided at call
 */
char* saveplayerstats (object* op, player_lvl* levels,char* inventoryfile,
                       char* timestamp, int precision, char* key)
{
    char query[HUGE_BUF];
    char key2[33];
    player_lvl lvl;
    if (!key) key=strcpy (key2,getuniqid());
    if (!levels) levels=getexp (op,&lvl);
    if (!levels) return key;
    sprintf (query,"INSERT INTO playershoot (player,title,inventoryfile,face,\
                    agilitylvl, agilityexp, mentallvl, mentalexp,\
                    physiquelvl, physiqueexp, magiclvl, magicexp,\
                    wisdomlvl, wisdomexp, personalitylvl, personalityexp,\
                    generallvl, generalexp, god, id,\
                    moment, counter)\
                    VALUES ('%s', '%s', '%s', '%s',\
                    %d, %d, %d, %d,\
                    %d, %d, %d, %d,\
                    %d, %d, %d, %d,\
                    %d, %d, '%s', '%s',\
                    '%s', %d) ",
                    op->name,op->contr->title,inventoryfile?inventoryfile:"",op->face->name,
                    levels->agilitylvl, levels->agilityexp, levels->mentallvl, levels->mentalexp,
                    levels->physiquelvl, levels->physiqueexp, levels->magiclvl, levels->magicexp,
                    levels->wisdomlvl, levels->wisdomexp, levels->personalitylvl, levels->personalityexp,
                    levels->generallvl, levels->generalexp, levels->god?levels->god:"", key,
                    timestamp, precision);
    database_insert (query);
    return key;
};

/*
 * function takeplayershoot
 * This function takes a shoot of the list of
 * characteristics of a player in the database
 * It also save it's current inventory to a file
 * and returns a string used to references
 * these datas in the database.
 */
char* takeplayershoot(object* op, char* timestamp, int precision)
{
    static char key[33];
    char* filename;
    if (op->type!=PLAYER) return NULL;
    strcpy (key,getuniqid());
    filename=saveinventory(op,key);
    return saveplayerstats(op,NULL,filename,timestamp,precision,key);
}
/*
 * function player_shouts (object* op,char* shout_what)
 * This function is used to log the player's dialogs
 *
 */

int player_shouts (object* op, char* shout_what)
{
    char buf[MAX_BUF];
    char query[HUGE_BUF];
    int precision;
    gettimestamp (buf, sizeof (buf), &precision);
    sprintf (query,"insert into dialog (moment,player,title, saywhat, dialogtype, counter) VALUES ('%s', '%s', '%s', '%s', %d, %d)",
                    buf,op->name,op->contr?op->contr->title:"",shout_what,DIALOG_SHOUT,precision);
    database_insert (query);
    log2file (query);
    return 1;
}

int player_birth (object* op)
{
    char buf[MAX_BUF];
    char query[HUGE_BUF];
    int precision;

    gettimestamp (buf,sizeof(buf),&precision);
    sprintf (query,"insert into birth (moment, counter, player, race, species, path)\
                    VALUES ('%s', %d, '%s', '%s', '%s', 'Unknown')",
                    buf,precision,op->name,op->race, op->contr?op->contr->title:"Unknown");
    database_insert (query);
    log2file (query);
    return 1;
}

int player_changer (object* player, object* changer)
{
    object* walk;
    object* laststat=NULL;
    char query[HUGE_BUF];
    if ((!player) || (player->type!=PLAYER)) return 1;
    for (walk=changer->inv;walk!=NULL;walk=walk->below)
        if (walk->type==CLASS) laststat=walk;
    if (!laststat) return 1;
    sprintf (query,"UPDATE birth SET path='%s' where player='%s'",
                    laststat->name,player->name);
    database_insert (query);
    log2file (query);
    return 1;
}

int player_dies (object* op, object* hitter)
{
    char query[HUGE_BUF];
    char buf[MAX_BUF];
    char *pshoot;
    char *sshoot;
    int precision;
    printf("Ready to die ?\n");
    if (op->type!=PLAYER) return 0;
    if (hitter)  /*This is a buggy case: the player is not dying. Just being hurt!*/
        return 0;
    if (op->stats.food<0) strcpy (op->contr->killer,"starvation");
    gettimestamp (buf,sizeof(buf),&precision);
    pshoot=takeplayershoot(op,buf,precision);
    printf ("%s\n",pshoot);
    sshoot=takescreenshoot(op);
    printf ("%s\n",sshoot);
    printf("We got someone dead !\n");
    sprintf (query,"insert into death (moment, counter, player, killer, playershoot, screenshoot) VALUES ('%s',%d,'%s','%s','%s','%s')",
                    buf,precision,op->name,op->contr->killer,pshoot,sshoot);
    database_insert (query);
    log2file (query);
    return 0; /*Must die*/
}

int player_login (player* pl, char* host)
{
    char query[HUGE_BUF];
    char buf[MAX_BUF];
    int precision;
    gettimestamp (buf,sizeof(buf),&precision);
    sprintf (query,"INSERT INTO playerlog (player, logmessage, playerstat, host, moment, counter)\
                                   VALUES ('%s', 'LOGIN', '', '%s', '%s', %d)",
                    pl->ob->name,
                    host,buf,precision);
    database_insert (query);
    return 1;
}

int player_logout (player* pl, char* host)
{
    char query[HUGE_BUF];
    char buf[MAX_BUF];
    int precision;
    printf ("Going to log out...\n");
    gettimestamp (buf,sizeof(buf),&precision);
    sprintf (query,"INSERT INTO playerlog (player, logmessage, playerstat, host, moment, counter)\
                                   VALUES ('%s', 'LOGOUT', '%s', '%s', '%s', %d)",
                    pl->ob->name,
                    saveplayerstats (pl->ob,NULL,NULL,buf,precision,NULL),
                    host,buf,precision);
    database_insert (query);
    return 1;
}
int event_say (object* talker, object* listener, char* saywhat, char* hook, char* options)
{
    printf ("Got event say!\n");
    if (!hook) return 0;
    if (!strcmp(hook,"ARENA")){
        if (!options) return 0;
        if      (!strcmp (options,"WINNER_COMMENT")) return arena_winner_comment (talker,saywhat);
        else if (!strcmp (options,"LOOSER_COMMENT")) return arena_looser_comment (talker,saywhat);
    }
    return 0;
}

int event_apply (object* who, object* what, char* hook, char* options)
{
    if (!hook) return 0;
    if (!strcmp (hook,"ARENA")){
        if (!options) return 0;
        if      (!strcmp(options,"LOOSER_LEAVE")) return arena_looser_leave (who);
        else if (!strcmp(options,"WINNER_LEAVE")) return arena_looser_leave (who);
    }
    return 0;
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
    static int result;
    eventcode = *(int *)(PParm->Value[0]);
    switch (eventcode){
        case EVENT_BORN:
            result=player_birth ( (object*)PParm->Value[1]);
            break;
        case EVENT_SHOUT:
            result=player_shouts ( (object*)PParm->Value[1],(char*)PParm->Value[2]);
            break;
        case EVENT_LOGIN:
            result=player_login ( (player*)PParm->Value[1], (char*) PParm->Value[2]);
            break;
        case EVENT_LOGOUT:
            result=player_logout ( (player*)PParm->Value[1], (char*) PParm->Value[2]);
            break;
        case EVENT_DEATH:
            if (!strcmp((char*)PParm->Value[10],"PLAYER_DIED"))
                result=player_dies ((object*) PParm->Value[2], (object*)PParm->Value[1]);
            break;
        case EVENT_TIME: /* Local event. Need to find quickly the correct handler!! */
            if (!strcmp((char*)PParm->Value[10],"PLAYER_CHANGED"))
                result=player_changer (((object*) PParm->Value[2])->above,(object*)PParm->Value[2]);
            else if (1==1){};
            break;
        case EVENT_TRIGGER:
            result=0;
            if (!strcmp ((char*) PParm->Value[9],"ARENA"))
                result=trigger_arena ((object*)PParm->Value[1], (object*)PParm->Value[2], (char*) PParm->Value[10]);
            break;
        case EVENT_SAY:
            result=event_say ((object*) PParm->Value[1], (object*) PParm->Value[2],
                       (char*) PParm->Value[4],
                       (char*) PParm->Value[9], (char*) PParm->Value[10]);
            break;
        case EVENT_APPLY:
            result=event_apply ((object*) PParm->Value[1], (object*) PParm->Value[2],
                       (char*) PParm->Value[9], (char*) PParm->Value[10]);
            break;
        case EVENT_GDEATH:
            result=player_dies ((object*) PParm->Value[1], NULL);
            break;
    };
    GCFP.Value[0]=(void*)(&result);
    return &GCFP;
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
    printf("    CFLogger Plugin loading.....");
    strcpy(buf,PLUGIN_ID);
    strcpy(buf2,PLUGIN_NAME);
    GCFP.Value[0] = (void *)(add_string(buf));
    GCFP.Value[1] = (void *)(add_string(buf2));
    logging_file=fopen(LOG_FILE,"a");
    if (logging_file)
    {
        fprintf (logging_file,"\n"); /*Be sure to start logging on a new line*/
        log2file("[LOGGING START]");
    } else perror ("ALERT!! Cannot open logging file");

    connect_database();
    printf("[Done]\n");
    return &GCFP;
};

/*****************************************************************************/
/* Used to do cleanup before killing the plugin.                             */
/*****************************************************************************/
CFParm* removePlugin(CFParm* PParm)
{
    log2file ("[LOGGING STOP]");
    if (logging_file) fclose (logging_file);
    close_database();
    return NULL;
};

void debugF(int* i)
{
        printf("Int is %i\n", *i);
};

/*****************************************************************************/
/* The postinitPlugin function is called by the server when the plugin load  */
/* is complete. It lets the opportunity to the plugin to register some events*/
/*****************************************************************************/
CFParm* postinitPlugin(CFParm* PParm)
{
    int i;
    GCFP.Value[1] = (void *)(add_string(PLUGIN_ID));
    i = EVENT_BORN;
    GCFP.Value[0]=(void*)(&i);
    (PlugHooks[HOOK_REGISTEREVENT])(&GCFP);
    i = EVENT_SHOUT;
    GCFP.Value[0]=(void*)(&i);
    (PlugHooks[HOOK_REGISTEREVENT])(&GCFP);
    i = EVENT_LOGIN;
    GCFP.Value[0]=(void*)(&i);
    (PlugHooks[HOOK_REGISTEREVENT])(&GCFP);
    i = EVENT_LOGOUT;
    GCFP.Value[0]=(void*)(&i);
    (PlugHooks[HOOK_REGISTEREVENT])(&GCFP);
    i = EVENT_MAPENTER;
    GCFP.Value[0]=(void*)(&i);
    (PlugHooks[HOOK_REGISTEREVENT])(&GCFP);
    i = EVENT_MAPLEAVE;
    GCFP.Value[0]=(void*)(&i);
    (PlugHooks[HOOK_REGISTEREVENT])(&GCFP);
    i = EVENT_GKILL;
    GCFP.Value[0]=(void*)(&i);
    (PlugHooks[HOOK_REGISTEREVENT])(&GCFP);
    i = EVENT_GDEATH;
    GCFP.Value[0]=(void*)(&i);
    (PlugHooks[HOOK_REGISTEREVENT])(&GCFP);
    return NULL;
};

CFParm* getPluginProperty(CFParm* PParm)
{
    return NULL;
};
