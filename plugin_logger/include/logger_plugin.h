#ifndef LOGGER_PLUGIN_H
#define LOGGER_PLUGIN_H

CFParm* PlugProps;
f_plugin PlugHooks[1024];

FILE* logging_file=NULL;
static CFParm GCFP;

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

#endif /* LOGGER_PLUGIN_H */
