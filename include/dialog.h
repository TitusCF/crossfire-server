#ifndef DIALOG_H
#define DIALOG_H

#include "global.h"


typedef enum {
    rt_reply,
    rt_question
} reply_type;

typedef struct struct_dialog_reply {
    char *reply;
    char *message;
    reply_type type;
    struct struct_dialog_reply *next;
} struct_dialog_reply;

typedef struct struct_dialog_message {
    char *match;
    char *message;
    struct struct_dialog_reply *replies;
    struct struct_dialog_message *next;
} struct_dialog_message;

typedef struct struct_dialog_information {
    struct struct_dialog_reply *all_replies;
    struct struct_dialog_message *all_messages;
} struct_dialog_information;

/** How many NPC replies maximum to tell the player. */
#define MAX_REPLIES 10
/** How many NPCs maximum will reply to the player. */
#define MAX_NPC     5
/**
 * Structure used to build up dialog information when a player says something.
 * @sa monster_communicate().
 */
typedef struct talk_info {
    struct object *who;                 /**< Player saying something. */
    const char *text;                       /**< What the player actually said. */
    sstring message;                    /**< If not NULL, what the player will be displayed as said. */
    int message_type;                   /**< A reply_type value for message. */
    int replies_count;                  /**< How many items in replies_words and replies. */
    sstring replies_words[MAX_REPLIES]; /**< Available reply words. */
    sstring replies[MAX_REPLIES];       /**< Description for replies_words. */
    int npc_count;                      /**< How many NPCs reacted to the text being said. */
    struct object *npc[MAX_NPC];        /**< The NPCs reacting. */
    sstring npc_msgs[MAX_NPC];          /**< What the NPCs in npc will say. */
} talk_info;


#endif /* DIALOG_H */
