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
#ifndef __CEXTRACT__
#include <logger_proto.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>
CFParm* PlugProps;
f_plugin PlugHooks[1024];
/*
 * ODBC Connectivity
 */
#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>


FILE* logging_file=NULL;
static CFParm GCFP;

void log2file (char* text2log)
{
    time_t t = time(NULL);
    char buf[MAX_BUF];
    if (logging_file){
        strftime(buf, sizeof(buf), "[%I:%M %p]", localtime(&t));
        fprintf (logging_file,"%s%s\n",buf,text2log);
    }
}

/******************************************************************************
 * Database management part.
 * These function are responsible of logging datas in a database,
 * connect to the database at startup
 *****************************************************************************/
SQLHENV     V_OD_Env;      // Handle ODBC environment
long        V_OD_erg;      // result of functions
SQLHDBC     V_OD_hdbc;     // Handle connection
char        V_OD_stat[10]; // Status SQL
SQLINTEGER  V_OD_err,V_OD_rowanz,V_OD_id;
SQLSMALLINT V_OD_mlen;
char        V_OD_msg[200],V_OD_buffer[200];
SQLHSTMT    V_OD_hstmt;    // Handle for a statement


/*
 * function connect_database
 *
 * Called at startup, this function gets an handle to the database
 * The DSN (Database Server Name) , username and password are given
 * at compile time or by an init file
 * Note: file logging facilities must be ready
 */
void connect_database (void)
{
    char buf[MAX_BUF];
    // 1. allocate Environment handle and register version
    V_OD_erg=SQLAllocHandle(SQL_HANDLE_ENV,SQL_NULL_HANDLE,&V_OD_Env);
    if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO)){
        sprintf(buf,"[DATABASE]Error AllocHandle");
        log2file (buf);
        printf ("\t[CROSSFIRE LOGGER]%s\n",buf);
        return;
    }

    V_OD_erg=SQLSetEnvAttr(V_OD_Env, SQL_ATTR_ODBC_VERSION,
                           (void*)SQL_OV_ODBC3, 0);
    if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO)){
        sprintf(buf,"[DATABASE]Error SetEnv");
        log2file (buf);
        printf ("\t[CROSSFIRE LOGGER]%s\n",buf);
        SQLFreeHandle(SQL_HANDLE_ENV, V_OD_Env);
        return;
    }
    // 2. allocate connection handle, set timeout
    V_OD_erg = SQLAllocHandle(SQL_HANDLE_DBC, V_OD_Env, &V_OD_hdbc);
    if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO)){
        sprintf(buf,"[DATABASE]Error AllocHDB %ld",V_OD_erg);
        log2file (buf);
        printf ("\t[CROSSFIRE LOGGER]%s\n",buf);
        SQLFreeHandle(SQL_HANDLE_ENV, V_OD_Env);
        return;
    }
    SQLSetConnectAttr(V_OD_hdbc, SQL_LOGIN_TIMEOUT, (SQLPOINTER *)5, 0);
    SQLSetConnectAttr(V_OD_hdbc, SQL_AUTOCOMMIT, (SQLPOINTER *)SQL_TRUE, 0);
    // 3. Connect to the datasource "web"
    V_OD_erg = SQLConnect(V_OD_hdbc, (SQLCHAR*) CROSSFIRE_DSN, SQL_NTS,
                                     (SQLCHAR*) CROSSFIRE_USER, SQL_NTS,
                                     (SQLCHAR*) CROSSFIRE_PASS, SQL_NTS);
    if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO)){
        sprintf(buf,"[DATABASE]Error SQLConnect %ld",V_OD_erg);
        printf ("\t[CROSSFIRE LOGGER]%s\n",buf);
        log2file (buf);
        SQLGetDiagRec(SQL_HANDLE_DBC, V_OD_hdbc,1,
                      V_OD_stat, &V_OD_err,V_OD_msg,100,&V_OD_mlen);
        sprintf(buf,"[DATABASE]%s (%ld)\n",V_OD_msg,V_OD_err);
        printf ("\t[CROSSFIRE LOGGER]%s\n",buf);
        log2file (buf);
        SQLFreeHandle(SQL_HANDLE_ENV, V_OD_Env);
        return;
    }
    printf("\t[CROSSFIRE LOGGER]  Connected !\n");
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
    if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO)){
                printf("\t[CROSSFIRE LOGGER][DATABASE]Error in Alloc Statement Handle %ld\n",V_OD_erg);
                return -1;
    }
    V_OD_erg=SQLExecDirect(V_OD_hstmt,query,SQL_NTS);
    if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO)){
       printf("\t[CROSSFIRE LOGGER][DATABASE]Error in Select %ld\n",V_OD_erg);
       return -1;
    }
    SQLFreeHandle(SQL_HANDLE_STMT,V_OD_hstmt);
    return 0;
}
/*
 * function database_UpdateOrInsert
 *
 * This function tries to update the database executing
 * the update string. If no row is affected it then
 * execute the insert query.
 * This allow to update a row if present and to
 * insert it if not.
 */
int database_UpdateOrInsert (char* update,char* insert)
{
    SQLINTEGER rowcount;
    SQLRETURN state;
    V_OD_erg=SQLAllocHandle(SQL_HANDLE_STMT, V_OD_hdbc, &V_OD_hstmt);
    if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO)){
                printf("\t[CROSSFIRE LOGGER][DATABASE]Error in Alloc Statement Handle %ld\n",V_OD_erg);
                return -1;
    }
    //printf ("[logger]update: %s\n",update);
    V_OD_erg=SQLExecDirect(V_OD_hstmt,update,SQL_NTS);
    if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO)){
       printf("\t[CROSSFIRE LOGGER][DATABASE]Error in Update %ld\n",V_OD_erg);
    }
    state= SQLRowCount (V_OD_hstmt,(&rowcount));
    if (rowcount<1){
        //printf ("[logger]insert: %s\n",insert);
        V_OD_erg=SQLExecDirect(V_OD_hstmt,insert,SQL_NTS);
        if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO)){
            printf("\t[CROSSFIRE LOGGER][DATABASE]Error in Insert %ld\n",V_OD_erg);
            return -1;
        }
    }
    SQLFreeHandle(SQL_HANDLE_STMT,V_OD_hstmt);
    return 0;
}
/*
 * function database_getint
 *
 * this function gets an SQL integer from
 * the execution of int_query
 * and return it
 */
int database_getint (char* int_query){
    SQLINTEGER val;
    SQLINTEGER size;
    V_OD_erg=SQLAllocHandle(SQL_HANDLE_STMT, V_OD_hdbc, &V_OD_hstmt);
    if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO)){
                printf("\t[CROSSFIRE LOGGER][DATABASE]Error in Alloc Statement Handle %ld\n",V_OD_erg);
                return -1;
    }
    //printf ("[logger]select: %s\n",int_query);
    SQLBindCol(V_OD_hstmt,1,SQL_C_SLONG, &val,sizeof (SQLINTEGER),&size);
    V_OD_erg=SQLExecDirect(V_OD_hstmt,int_query,SQL_NTS);
    if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO)){
       printf("\t[CROSSFIRE LOGGER][DATABASE]Error in Select %ld\n",V_OD_erg);
       return -1;
    }
    V_OD_erg=SQLFetch (V_OD_hstmt);
    if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO)){
       return 0;
    }
    SQLFreeHandle(SQL_HANDLE_STMT,V_OD_hstmt);
    return val;
}

#define MAX_STRING_COUNT 10
char* addslashes (char* string)
{
    static char* strings[MAX_STRING_COUNT]={NULL,NULL,NULL,NULL,NULL,
                                            NULL,NULL,NULL,NULL,NULL};
    static char* current;
    static int string_count=0;
    int i;
    if (string==NULL) return NULL;
    current=(char*)malloc (strlen(string)*2+1);
    if (current==NULL) return string;
    if (strings[string_count])
        free (strings[string_count]);
    strings[string_count]=current;
    for (i=0;i<strlen(string);i++){
        switch (string[i]){
            case '\\':
            case '\'':
                *current='\\';
                current++;
            default:
                *current=string[i];
                current++;
                break;
        }
    }
    *current='\0'; /*Null terminated string*/
    current=strings[string_count];
    string_count++;
    string_count%=MAX_STRING_COUNT;
    return current;
}
#undef MAX_STRING_COUNT


/******************************************************************************
 * Events management part.
 * These function are responsible for logging events in a database,
 * using the database interface provided above and some external files.
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
    if (new_sec!=last_sec){
        last_sec=new_sec;
        counter=0;
    } else counter++;
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
    memcpy (levels,&zerolevel,sizeof (player_lvl));
    if (player->type!=PLAYER) return NULL;
    for (op=player->inv;op;op=op->below){
        if (op->type!=EXPERIENCE) continue;
        if (!strcmp (op->name,"agility")){
            levels->agilitylvl=op->level;
            levels->agilityexp=op->stats.exp;
        } else if (!strcmp (op->name,"mental")){
            levels->mentallvl=op->level;
            levels->mentalexp=op->stats.exp;
        } else if (!strcmp (op->name,"physique")){
            levels->physiquelvl=op->level;
            levels->physiqueexp=op->stats.exp;
        } else if (!strcmp (op->name,"magic")){
            levels->magiclvl=op->level;
            levels->magicexp=op->stats.exp;
        } else if (!strcmp (op->name,"wisdom")){
            levels->wisdomlvl=op->level;
            levels->wisdomexp=op->stats.exp;
            levels->god=op->title;
        } else if (!strcmp (op->name,"personality")){
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
 * the resulting screenshoot is what player's client saw
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
    strcpy (key,getuniqid());
    gettimestamp (shootfile,sizeof(shootfile),&precision);
    for (p=shootfile;(*p)!='\0';p++){
        if ((*p)==' ')(*p)='_';
        if ((*p)=='-')(*p)='_';
        if ((*p)==':')(*p)='_';
    }
    sprintf (filename,"%s%s%s%d.sht",web_directory,screenshoot_dir,shootfile,precision);
    sprintf (smallfilename,"%s%d",shootfile,precision);
    /*printf ("Will write screenshoot in %s\n",filename);*/
    screenshoot=fopen (filename,"w");
    if (!screenshoot) return key;
    fprintf (screenshoot,"MAP_SIZE %d,%d\n",op->contr->socket.mapy,op->contr->socket.mapy);
    for (clienty=0;clienty<(op->contr->socket.mapy);clienty++){
        for (clientx=0;clientx<(op->contr->socket.mapx);clientx++){
            if (op->contr->blocked_los[clientx][clienty]>3)
                 /*  face is not visible to client */
                 continue;
            x=op->x-(op->contr->socket.mapx-1)/2+clientx;
            y=op->y-(op->contr->socket.mapy-1)/2+clienty;

	    if (((x>=0) && (y>=0)) && ((x<op->map->width) && (y<op->map->height)))
	    {
                fprintf (screenshoot,"%d,%d %s|%s|%s ",
                    clientx,clienty,
                    GET_MAP_FACE(op->map,x,y,0)?GET_MAP_FACE(op->map,x,y,0)->name:"blank.111",
                    GET_MAP_FACE(op->map,x,y,1)?GET_MAP_FACE(op->map,x,y,1)->name:"blank.111",
                    GET_MAP_FACE(op->map,x,y,2)?GET_MAP_FACE(op->map,x,y,2)->name:"blank.111"
                    );
                for(tmp=get_map_ob(op->map,x,y);tmp!=NULL&&tmp->above!=NULL;
                    tmp=tmp->above);
                for ( ; tmp != NULL; tmp=tmp->below ) {
                    fprintf (screenshoot,"%s|",query_name(tmp));
                    if (QUERY_FLAG(tmp, FLAG_IS_FLOOR))          /* don't continue under the floor */
                        break;
                }
            }
            fprintf (screenshoot,"\n");
        }
    };
    fclose (screenshoot);
    chmod (filename,S_IRUSR| S_IWUSR | S_IRGRP | S_IROTH);
    sprintf (query,"INSERT INTO screenshoot (id,shootfile) VALUES ('%s','%s')",
             addslashes(key),addslashes(smallfilename));
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
    if (id) strcpy (key,id); else strcpy (key,getuniqid());
    sprintf (filename,"%s%s.inv",inventoryshoot_dir,key);
    sprintf (fullfilename,"%s%s",web_directory,filename);
    inventoryshoot=fopen (fullfilename,"w");
    if (!inventoryshoot){
        perror ("Failed to save inventory");
        return "";
    }
    for (what=op->inv;what;what=what->below){
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
                    addslashes(op->name),addslashes(op->contr->title),
                    inventoryfile?inventoryfile:"",op->face->name,
                    levels->agilitylvl, levels->agilityexp, levels->mentallvl, levels->mentalexp,
                    levels->physiquelvl, levels->physiqueexp, levels->magiclvl, levels->magicexp,
                    levels->wisdomlvl, levels->wisdomexp, levels->personalitylvl, levels->personalityexp,
                    levels->generallvl, levels->generalexp, levels->god?addslashes(levels->god):"", key,
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

void insert_message (char* moment, char* player, char* title, char* saywhat, int dialogtype, int precision)
{
    char query[HUGE_BUF];
    sprintf (query,"insert into dialog (moment,player,title, saywhat, dialogtype, counter) VALUES ('%s', '%s', '%s', '%s', %d, %d)",
                    moment,player,title,saywhat,dialogtype,precision);
    database_insert (query);
}

void insert_server_event (int message_type, char* parameter1, char* parameter2, char* moment, int precision)
{
    char query[HUGE_BUF];
    sprintf (query,"insert into server_events (event,parameter1,parameter2,moment,counter)\
                    VALUES (%d,'%s', '%s', '%s', %d)",
                    message_type,parameter1,parameter2,moment,precision);
    database_insert (query);
}
/*
 * function player_shouts (object* op,char* shout_what)
 * This function is used to log the player's dialogs
 *
 */

int player_shouts (object* op, char* shout_what)
{
    char buf[MAX_BUF];
    int precision;
    gettimestamp (buf, sizeof (buf), &precision);
    insert_message (buf,op->name,op->contr?op->contr->title:"",shout_what,DIALOG_SHOUT,precision);
    return 1;
}

int player_birth (object* op)
{
    char buf[MAX_BUF];
    char query[HUGE_BUF];
    char message[MAX_BUF];
    int precision;

    gettimestamp (buf,sizeof(buf),&precision);
    sprintf (query,"insert into birth\
                    (moment, counter, player, race, species, path)\
                    VALUES ('%s', %d, '%s', '%s', '%s', 'Unknown')",
                    buf,precision,addslashes(op->name),addslashes(op->race),
                    op->contr?op->contr->title:"Unknown");
    database_insert (query);
    sprintf (message,"New player created: %s the %s",op->name,op->race);
    insert_message (buf,"SERVER","",message,DIALOG_SERVER_NOTICE,precision);
    log2file (query);
    return 1;
}

int player_changer (object* player, object* changer)
{
    object* walk;
    object* laststat=NULL;
    char message[MAX_BUF];
    char query[HUGE_BUF];
    char buf[MAX_BUF];
    int precision;
    if ((!player) || (player->type!=PLAYER)) return 0;
    for (walk=changer->inv;walk!=NULL;walk=walk->below)
        if (walk->type==CLASS) laststat=walk;
    if (!laststat) return 0;
    sprintf (query,"UPDATE birth SET path='%s' where player='%s'",
                    addslashes(laststat->name),addslashes(player->name));
    database_insert (query);
    gettimestamp (buf,sizeof(buf),&precision);
    sprintf (message,"%s now follows the path: %s ",player->name,laststat->name);
    insert_message (buf,"SERVER","",addslashes(message),DIALOG_SERVER_NOTICE,precision);
    log2file (query);
    return 0;
}

int player_dies (object* op)
{
    char query[HUGE_BUF];
    char buf[MAX_BUF];
    char message[MAX_BUF];
    char *pshoot;
    char *sshoot;
    int precision;
    if (op->type!=PLAYER) return 0;
    if (op->stats.food<0) strcpy (op->contr->killer,"starvation");
    gettimestamp (buf,sizeof(buf),&precision);
    pshoot=takeplayershoot(op,buf,precision);
    /*printf ("\t[CROSSFIRE LOGGER]  %s\n",pshoot);*/
    sshoot=takescreenshoot(op);
    /*printf ("\t[CROSSFIRE LOGGER]  %s\n",sshoot);*/
    sprintf (query,"insert into death\
             (moment, counter, player, killer, playershoot, screenshoot)\
              VALUES ('%s',%d,'%s','%s','%s','%s')",
              buf,precision,addslashes(op->name),addslashes(op->contr->killer),
              pshoot,sshoot);
    database_insert (query);
    sprintf (message,"%s killed %s",op->contr->killer,op->name);
    insert_message (buf,"SERVER","",addslashes(message),DIALOG_SERVER_INFO,precision);
    log2file (query);
    return 0; /*Must die*/
}
int creature_dies (object* op)
{
    if (op->stats.hp<0){
        if (op->type!=PLAYER) add_death_creature(op);
    }
    return 0;
}

int player_login (player* pl, char* host)
{
    char query[HUGE_BUF];
    char buf[MAX_BUF];
    char message[MAX_BUF];
    int precision;
    gettimestamp (buf,sizeof(buf),&precision);
    sprintf (query,"INSERT INTO playerlog (player, logmessage, playerstat, host, moment, counter)\
                                   VALUES ('%s', 'LOGIN', '%s', '%s', '%s', %d)",
                    addslashes(pl->ob->name),
                    saveplayerstats (pl->ob,NULL,NULL,buf,precision,NULL),
/*    Removed by GROS. Why?
                    '', Replaced by empty string*/
                    addslashes(host),buf,precision);
    sprintf (message,"%s has entered the game",pl->ob->name);
    database_insert (query);
    insert_message (buf,"SERVER","",addslashes(message),
                    DIALOG_SERVER_INFO,precision);
    return 1;
}

int player_logout (player* pl, char* host)
{
    char query[HUGE_BUF];
    char buf[MAX_BUF];
    char message[MAX_BUF];
    int precision;
    /*printf ("Going to log out...\n");*/
    gettimestamp (buf,sizeof(buf),&precision);
    sprintf (query,"INSERT INTO playerlog\
                    (player, logmessage, playerstat, host, moment, counter)\
                    VALUES ('%s', 'LOGOUT', '%s', '%s', '%s', %d)",
                    addslashes(pl->ob->name),
                    saveplayerstats (pl->ob,NULL,NULL,buf,precision,NULL),
                    addslashes(host),buf,precision);
    sprintf (message,"%s left the game",pl->ob->name);
    database_insert (query);
    insert_message (buf,"SERVER","",addslashes(message),
                    DIALOG_SERVER_INFO,precision);
    return 1;
}
int event_say (object* talker, object* listener,
               char* saywhat, char* hook, char* options)
{
    if (!hook) return 0;
    if (!strcmp(hook,"ARENA")){
        if (!options) return 0;
        if      (!strcmp (options,"WINNER_COMMENT"))
            return arena_winner_comment (talker,saywhat);
        else if (!strcmp (options,"LOOSER_COMMENT"))
            return arena_looser_comment (talker,saywhat);
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

int update_map_stats (int type,object* op)
{
    if (type==EVENT_MAPENTER) player_enter_map(op->map);
    else return player_leave_map(op->map);
    return 0;
}
void ping_database(void)
{
    int precision;
    char insert[MAX_BUF*2];
    char update[MAX_BUF*2];
    char buffer[MAX_BUF*2];
    gettimestamp (buffer,sizeof (buffer),&precision);
    sprintf (insert,"INSERT INTO server_events\
                     (event,parameter1,parameter2,moment,counter)\
                     VALUES (%d,'','','%s',%d)",
                     SERVER_EVENT_PING,buffer,precision);
    sprintf (update,"UPDATE server_events set moment='%s', counter=%d\
                     where event=%d",
                     buffer,precision,SERVER_EVENT_PING);
    database_UpdateOrInsert(update,insert);
}

void doclock (void){
    static int clock_count=0;
    clock_count=(clock_count+1)%CLOCK_CYCLE;
    if ((clock_count+1)%CLOCK_MAP_USE==0) try_flush_map_time (CLOCK_MAP_RATIO);
    if ((clock_count+3)%CLOCK_PETS==0) try_flush_creature_death (CLOCK_PETS_RATIO);
    if ((clock_count+5)%SERVER_PING_LATENCY==0) ping_database();
    if (clock_count==0) printf ("\t[CROSSFIRE LOGGER]  Tic\n");
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

static void hook_free_memory(CFParm* CFR)
{
    GCFP.Value[0]=CFR;
    PlugHooks[HOOK_FREEMEMORY](&GCFP);
}

char* hook_add_string (char* text){
    CFParm* result;
    char* val;
    GCFP.Value[0]=(void*)text;
    result=(PlugHooks[HOOK_ADDSTRING])(&GCFP);
    val=(char*)result->Value[0];
    hook_free_memory (result);
    return val;
}

char* hook_add_refcount (char* text){
    CFParm* result;
    char* val;
    GCFP.Value[0]=(void*)text;
    result=(PlugHooks[HOOK_ADDREFCOUNT])(&GCFP);
    val=(char*)result->Value[0];
    hook_free_memory (result);
    return val;
}

void hook_free_string (char* text){
    GCFP.Value[0]=(void*)text;
    (PlugHooks[HOOK_FREESTRING])(&GCFP);
    return ;
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
    static int result;
    eventcode = *(int *)(PParm->Value[0]);
    /*May help fixing some events*/
    if (eventcode != 14)
        printf ("\t[Crossfire Logger] Got event %d\n",eventcode);
    result = 0;
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
        case EVENT_MAPENTER:
        case EVENT_MAPLEAVE:
            result=update_map_stats (*(int*)(PParm->Value[0]),(object*) PParm->Value[1]);
            break;
        case EVENT_MAPRESET:
            result=map_reset ((char*) PParm->Value[1]);
            break;
        case EVENT_CLOCK:
            doclock();
            result=0;
            break;
        case EVENT_CRASH:
            printf ("\t[CROSSFIRE LOGGER]  Server crashes ;(\n");
            result=0;
            break;
        /*case EVENT_DEATH:
            if (!strcmp((char*)PParm->Value[10],"PLAYER_DIED"))
                result=player_dies ((object*) PParm->Value[2]);
            break;*/  /*Not needed anymore*/
        case EVENT_GDEATH:
            result=player_dies ((object*) PParm->Value[2]);
            break;
        case EVENT_GKILL:
            result=creature_dies ((object*) PParm->Value[2]);
            break;
        case EVENT_TIME: /* Local event. Need to find quickly the correct handler!! */
            if (1==1){};
            break;
        case EVENT_TRIGGER:
            result=0;
            if (!strcmp ((char*) PParm->Value[9],"ARENA"))
                result=trigger_arena ((object*)PParm->Value[1], (object*)PParm->Value[2], (char*) PParm->Value[10]);
            else if (!strcmp((char*)PParm->Value[9],"PLAYER_CHANGED"))
                result=player_changer ((object*) PParm->Value[2],(object*)PParm->Value[1]);
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
    printf("\tCrossfire logger facility at load...\n");
    connect_database();
    strcpy(buf,log_plugin_ident);
    strcpy(buf2,log_plugin_string);
    GCFP.Value[0] = (void *)(add_string(buf));
    GCFP.Value[1] = (void *)(add_string(buf2));
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
/*****************************************************************************/
/* This function is called to ask various informations to the plugin.        */
/*****************************************************************************/
CFParm* getPluginProperty(CFParm* PParm)
{
    return NULL;
}

void debugF(int* i)
{
        printf("\t[CROSSFIRE LOGGER]  Int is %i\n", *i);
};

/*****************************************************************************/
/* The postinitPlugin function is called by the server when the plugin load  */
/* is complete. It lets the opportunity to the plugin to register some events*/
/*****************************************************************************/
CFParm* postinitPlugin(CFParm* PParm)
{
    int i;
    int precision;
    char insert[MAX_BUF*2];
    char update[MAX_BUF*2];
    char buffer[MAX_BUF*2];
    CFParm* cfpptr;
    struct mapdef* map;
    struct pl* po;

    GCFP.Value[1] = (void *)(add_string(log_plugin_ident));
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
    i = EVENT_LOGOUT;
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
    i = EVENT_MAPRESET;
    GCFP.Value[0]=(void*)(&i);
    (PlugHooks[HOOK_REGISTEREVENT])(&GCFP);
    i = EVENT_CLOCK;
    GCFP.Value[0]=(void*)(&i);
    (PlugHooks[HOOK_REGISTEREVENT])(&GCFP);
    i = EVENT_GDEATH;
    GCFP.Value[0]=(void*)(&i);
    (PlugHooks[HOOK_REGISTEREVENT])(&GCFP);
    i = EVENT_GKILL;
    GCFP.Value[0]=(void*)(&i);
    (PlugHooks[HOOK_REGISTEREVENT])(&GCFP);
    i = EVENT_CRASH;
    GCFP.Value[0]=(void*)(&i);
    (PlugHooks[HOOK_REGISTEREVENT])(&GCFP);
    /* Reset all maps in database*/
    gettimestamp (buffer,sizeof (buffer),&precision);
    sprintf (insert," ");
    sprintf (update,"update map_state set state=0,moment='%s'",buffer);
    database_UpdateOrInsert (update,insert);
    cfpptr=(PlugHooks[HOOK_GETFIRSTMAP])(NULL);
    map=(struct mapdef*)cfpptr->Value[0];
    hook_free_memory (cfpptr);
    if (!map){
        insert_server_event (SERVER_EVENT_STARTUP,"Cold plug","",buffer,precision);
        printf ("\t[CROSSFIRE LOGGER]  Cold plug. Postinit finished.\n");
    }
    else{
        insert_server_event (SERVER_EVENT_STARTUP,"Hot plug","",buffer,precision);
        printf ("\t[CROSSFIRE LOGGER]  We were hot-plugged.\n");
        printf ("\t[CROSSFIRE LOGGER]   Tracking altered maps.\n");
        for (;map;map=map->next){
            if ( (strncmp ("/random/",map->path,strlen("/random/"))) &&
               (strncmp (LOCALDIR,map->path,strlen(LOCALDIR)))    ){

                printf ("\t[CROSSFIRE LOGGER]     Adding map %s to list.\n",
                        map->path);
                sprintf (insert,"insert into map_state (path,state,moment)\
                                 VALUES ('%s',1,'%s')",map->path,buffer);
                sprintf (update,"update map_state set state=1,moment='%s'\
                                 where path='%s'",buffer,map->path);
                database_UpdateOrInsert (update,insert);
            }
        }
        printf ("\t[CROSSFIRE LOGGER]   Tracking players.\n");
        cfpptr=(PlugHooks[HOOK_GETFIRSTPLAYER])(NULL);
        po=(struct pl *)(cfpptr->Value[0]);
        hook_free_memory (cfpptr);
        for (;po;po=po->next){
            printf ("\t[CROSSFIRE LOGGER]     Found %s.\n",po->ob->name);
            player_enter_map (po->ob->map);
            player_login (po,po->socket.host);
        }
    }
    return NULL;
};
