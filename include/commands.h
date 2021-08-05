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

/**
 * One command function, with a custom parameter specified at registration time.
 * @param op
 * the player executing the command
 * @param params
 * the command parameters; empty string if no commands are given; leading and
 * trailing spaces have been removed
 * @param extra
 * extra parameter as specified at registration, with NULL changed to empty string
 */
typedef void (*command_function_extra)(object *op, const char *params, const char *extra);

/** Identifier when registering a command. */
typedef uint64_t command_registration;

/** Standard commands. */
#define COMMAND_TYPE_NORMAL         0
/** Communication commands. */
#define COMMAND_TYPE_COMMUNICATION  1
/** Wizard-only commands. */
#define COMMAND_TYPE_WIZARD         2

/** Time, in seconds from epoch, of server shutdown. */
extern int cmd_shutdown_time;

#endif /* COMMANDS_H */
