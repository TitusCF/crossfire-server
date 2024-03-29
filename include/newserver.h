/**
 * @file
 * Defines various structures and values that are used for the
 * new client server communication method.  Values defined here are only
 * used on the server side code.  For shared client/server values, see
 * @ref newclient.h
*/

#ifndef NEWSERVER_H
#define NEWSERVER_H

/**
 * The lower bound for the number of objects to send for the 'look' window
 * (container or ground view).
 */
#define MIN_NUM_LOOK_OBJECTS 3 /* 3=prev group, one object, next group */

/**
 * Default value for the number of objects to send for the 'look' window
 * (container or ground view).
 */
#define DEFAULT_NUM_LOOK_OBJECTS 50

/**
 * The upper bound for the number of objects to send for the 'look' window
 * (container or ground view).
 */
#define MAX_NUM_LOOK_OBJECTS 100

/** One map cell, as sent to the client. */
struct map_cell_struct {
    uint16_t faces[MAP_LAYERS];   /**< Face numbers. */
    int darkness;               /**< Cell's darkness. */
};

/**
 * This basically defines the largest size an
 * archetype may be - it is used for allocation of
 * some structures, as well as determining how far
 * we should look for the heads of big images.
 */
#define MAX_HEAD_OFFSET 8

#define MAX_CLIENT_X (MAP_CLIENT_X+MAX_HEAD_OFFSET)
#define MAX_CLIENT_Y (MAP_CLIENT_Y+MAX_HEAD_OFFSET)

/** One map for a player. */
struct Map {
    struct map_cell_struct cells[MAX_CLIENT_X][MAX_CLIENT_Y];
};

/** True max is 16383 given current map compaction method. */
#define MAXANIMNUM  2000

/** Contains the last range/title information sent to client. */
struct statsinfo {
    char *range, *title, *god;
};


/**
 * What state a socket is in.
 */
enum Sock_Status {
    Ns_Avail,
    Ns_Add,
    Ns_Dead
};

/**
 * Contains parameters for socket() and bind() for listening sockets.
 * This struct contains most fields of "struct addrinfo", but
 * older systems do not know struct addrinfo yet.
 */
struct listen_info {
    int family;
    int socktype;
    int protocol;
    socklen_t addrlen;
    struct sockaddr *addr;
};

/** How many times we are allowed to give the wrong password before being kicked. */
#define MAX_PASSWORD_FAILURES 5

/**
 * Socket structure, represents a client-server connection.
 */
typedef struct socket_struct {
    enum Sock_Status status;
    int         fd;
    struct listen_info  *listen;
    struct Map  lastmap;
    int8_t       map_scroll_x, map_scroll_y;
    size_t      faces_sent_len;         /**< This is the number of elements allocated in faces_sent[]. */
    uint8_t       *faces_sent;            /**< This is a bitmap on sent face status. */
    uint8_t       anims_sent[MAXANIMNUM]; /**< What animations we sent. */
    struct statsinfo stats;
    SockList    inbuf;                  /**< If we get an incomplete packet, this is used to hold the data. */
    char        *host;                  /**< Which host it is connected from (ip address). */
    uint8_t       password_fails;         /**< How many times the player has failed to give the right password. */
    uint32_t      facecache:1;            /**< If true, client is caching images. */
    uint32_t      darkness:1;             /**< True if client wants darkness information. */
    uint32_t      update_look:1;          /**< If true, we need to send the look window. */
    uint32_t      update_inventory:1;     /**< If true, we need to send the inventory list. */
    uint32_t      tick:1;                 /**< Client wishes to get tick commands. */
    uint32_t      is_bot:1;               /**< Client shouldn't be reported to metaserver. */
    uint32_t      want_pickup:1;          /**< Client wants pickup information when logging in. */
    uint32_t      extended_stats:1;       /**< Client wants base and maximum statistics information. */
    uint32_t      monitor_spells;         /**< Client wishes to be informed when their spell list changes. */
    uint32_t      sound;                  /**< Client sound mode. */
    bool          heartbeat;              /**< Client will send hearbeats. */
    uint32_t      cs_version, sc_version; /**< Versions of the client. */
    uint16_t      look_position;          /**< Start of drawing of look window. */
    uint16_t      container_position;     /**< Start of container contents to send to client. */
    uint8_t       mapx, mapy;             /**< How large a map the client wants. */
    uint8_t       faceset;                /**< Set the client is using, default 0. */

    /* Below are flags for extedend infos to pass to client
     * with S->C mapextended command (note: this comment seems incorrect?) */
    int8_t       sounds_this_tick;       /**< Number of sounds sent this tick. */
    uint8_t       num_look_objects;       /**< The maximum number of objects to show on the ground view;
                                             this number includes the prev/next group fake items.
                                             Can be set through "num_look_objects" setup option;
                                             defaults to DEFAULT_NUM_LOOK_OBJECTS. */
    char    *account_name;              /**< Name of the account logged in on this socket */
    Account_Char    *account_chars;     /**< Detailed information on characters on this account */
    uint8_t login_method;   /**< Login method this client is using */
    uint16_t notifications; /**< Notifications this client wants to get. */
    uint32_t last_tick;     /**< Number of ticks since last communication. */
} socket_struct;

/**
 * Bitmask for the faces_sent[] array - what
 * portion of the face have we sent?
 */
#define NS_FACESENT_FACE        0x1
#define NS_FACESENT_SMOOTH      0x2

/** Holds some system-related information. */
typedef struct Socket_Info {
    struct timeval timeout;     /**< Timeout for select. */
    int     max_filedescriptor; /**< max filedescriptor on the system. */
    int     allocated_sockets;  /**< Number of allocated items in ::init_sockets. */
} Socket_Info;

extern Socket_Info socket_info;

#define VERSION_CS 1023    /**< Version >= 1023 understand setup cmd */
#define VERSION_SC 1029
#define VERSION_INFO "Crossfire Server"

#endif /* NEWSERVER_H */
