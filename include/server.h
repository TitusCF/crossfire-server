#ifndef _SERVER_H
#define _SERVER_H

#include <signal.h>

volatile sig_atomic_t shutdown_flag;

void player_map_change_common(object* op, mapstruct* const oldmap,
                              mapstruct* const newmap);
void login_check_shutdown(object* const op);

#endif
