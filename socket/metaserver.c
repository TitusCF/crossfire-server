/*
 * static char *rcsid_metaserver_c =
 *    "$Id$";
 */

/*
    CrossFire, A Multiplayer game for X-windows

    Copyright (C) 2002 Mark Wedel & Crossfire Development Team
    Copyright (C) 1992 Frank Tore Johansen

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

    The authors can be reached via e-mail at crossfire-devel@real-time.com
*/

/**
 * \file
 * \date 2003-12-02
 * Meta-server related functions.
 */

#include <global.h>

#ifndef WIN32 /* ---win32 exclude unix header files */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#endif /* end win32 */

static int metafd=-1;
static struct sockaddr_in sock;

/**
 * Connects to metaserver.
 *
 * Its only called once.  If we are not
 * trying to contact the metaserver of the connection attempt fails, metafd will be
 * set to -1.  We use this instead of messing with the settings.meta_on so that
 * that can be examined to at least see what the user was trying to do.
 */
void metaserver_init(void)
{

#ifdef WIN32 /* ***win32 metaserver_init(): init win32 socket */
	struct hostent *hostbn;
	int temp = 1;
#endif

    if (!settings.meta_on) {
	metafd=-1;
	return;
    }

    if (isdigit(settings.meta_server[0]))
	sock.sin_addr.s_addr = inet_addr(settings.meta_server);
    else {
        struct hostent *hostbn = gethostbyname(settings.meta_server);
	if (hostbn == NULL) {
	    LOG(llevDebug,"metaserver_init: Unable to resolve hostname %s\n", settings.meta_server);
	    return;
	}
	memcpy(&sock.sin_addr, hostbn->h_addr, hostbn->h_length);
    }
#ifdef WIN32 /* ***win32 metaserver_init(): init win32 socket */
    ioctlsocket(metafd, FIONBIO , &temp);
#else
    fcntl(metafd, F_SETFL, O_NONBLOCK);
#endif
    if ((metafd=socket(AF_INET, SOCK_DGRAM, 0))==-1) {
	LOG(llevDebug,"metaserver_init: Unable to create socket, err %d\n", errno);
	return;
    }
    sock.sin_family = AF_INET;
    sock.sin_port = htons(settings.meta_port);

    /* No hostname specified, so lets try to figure one out */
    if (settings.meta_host[0]==0) {
	char hostname[MAX_BUF], domain[MAX_BUF];
	if (gethostname(hostname, MAX_BUF-1)) {
	    LOG(llevDebug,"metaserver_init: gethostname failed - will not report hostname\n");
	    return;
	}

#ifdef WIN32 /* ***win32 metaserver_init(): gethostbyname! */
	hostbn = gethostbyname(hostname);
	if (hostbn != (struct hostent *) NULL) /* quick hack */
		memcpy(domain, hostbn->h_addr, hostbn->h_length);

	if (hostbn == (struct hostent *) NULL) {
#else
	if (getdomainname(domain, MAX_BUF-1)) {
#endif /* win32 */
	    LOG(llevDebug,"metaserver_init: getdomainname failed - will not report hostname\n");
	    return;
	}
	/* Potential overrun here but unlikely to occur */
	sprintf(settings.meta_host,"%s.%s", hostname, domain);
    }
}

/**
 * Updates our info in the metaserver
 */
void metaserver_update(void)
{
    char data[MAX_BUF], num_players=0;
    player *pl;

    if (metafd == -1) return;	/* No valid connection */

    /* We could use socket_info.nconns, but that is not quite as accurate,
     * as connections in the progress of being established, are listening
     * but don't have a player, etc.  The checks below are basically the
     * same as for the who commands with the addition that WIZ, AFK, and BOT
     * players are not counted.
     */
    for (pl=first_player; pl!=NULL; pl=pl->next) {
        if (pl->ob->map == NULL) continue;
        if (pl->hidden) continue;
        if (QUERY_FLAG(pl->ob, FLAG_WIZ)) continue;
        if (QUERY_FLAG(pl->ob, FLAG_AFK)) continue;
        if (pl->state != ST_PLAYING && pl->state != ST_GET_PARTY_PASSWORD) continue;
        if (pl->socket.is_bot) continue;
        num_players++;
    }

    sprintf(data,"%s|%d|%s|%s|%d|%d|%ld", settings.meta_host, num_players, VERSION,
	    settings.meta_comment, cst_tot.ibytes, cst_tot.obytes,
	    (long)time(NULL) - cst_tot.time_start);
    if (sendto(metafd, data, strlen(data), 0, (struct sockaddr *)&sock, sizeof(sock))<0) {
	LOG(llevDebug,"metaserver_update: sendto failed, err = %d\n", errno);
    }
}
