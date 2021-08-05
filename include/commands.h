/**
 * @file
 * Defines and structures related to commands the player can send.
 */

#ifndef COMMANDS_H
#define COMMANDS_H

/**
 * One command function.
 * @param op
 * the player executing the command
 * @param params
 * the command parameters; empty string if no commands are given; leading and
 * trailing spaces have been removed
 */
typedef void (*command_function)(object *op, const char *params);

/** Represents one command. */
typedef struct {
    const char *name;         /**< Command name. */
    command_function func;    /**< Pointer to command function. */
    float time;               /**< How long it takes to execute this command. */
} command_array_struct;

/** Time, in seconds from epoch, of server shutdown. */
extern int cmd_shutdown_time;

#endif /* COMMANDS_H */
