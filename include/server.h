#ifndef _SERVER_H
#define _SERVER_H

#include <signal.h>
#include <sqlite3.h>

extern volatile sig_atomic_t shutdown_flag;
extern sqlite3* server_db;

void player_map_change_common(object* op, mapstruct* const oldmap,
                              mapstruct* const newmap);
void login_check_shutdown(object* const op);

bool can_follow(object*, player*);

#endif
