/** @file book.h
 * Describes fundental parameters of 'books' - objects with type==BOOK
 */

#ifndef BOOK_H
#define BOOK_H

/**
 * Maximum message buf size for books. If this is changed, keep in mind that big
 * strings may be unreadable by the player as the tail of the message can scroll
 * over the beginning (as of v0.92.2).
 * @note
 * Note that the book messages are stored in the msg buf,
 * which is limited by 'HUGE_BUF' in the loader.
 */
#define BOOK_BUF        HUGE_BUF-10

/**
 * Defines the base value that BOOKSIZE should return for books with no weight.
 * @note
 * If little books arent getting enough text generated, enlarge this.
 */
#define BASE_BOOK_BUF   700

/**
 * Get the book buffer size for an individual book object. Make heavy books hold
 * more text but never let a book msg buffer exceed the max. size (BOOK_BUF)
 * @param xyz
 * The book object to calculate the buffer size for.
 */
#define BOOKSIZE(xyz)   BASE_BOOK_BUF+((xyz)->weight/10) > BOOK_BUF ? \
                                BOOK_BUF : BASE_BOOK_BUF+((xyz)->weight/10);
/**
 * Struct to store the message_type and message_subtype for signs and books used by the player.
 */
typedef struct {
    uint8_t message_type;     /**< Message type to be sent to the client. */
    uint8_t message_subtype;  /**< Message subtype to be sent to the client. */
} readable_message_type;

/**
 * One general message, from the lib/messages file.
 */
typedef struct GeneralMessage {
    int chance;             /**< Relative chance of the message appearing
                              randomly. If 0 will never appear. */
    sstring identifier;     /**< Message identifier, can be NULL. */
    sstring title;          /**< The message's title, only used for knowledge. */
    sstring message;        /**< The message's body. */
    sstring quest_code;     /**< Optional quest code and state this message will start. */
    const Face *face;   /**< Face the message displays at in the knowledge dialog, NULL if no face defined. */
    struct GeneralMessage *next;   /**< Next message in the list. */
} GeneralMessage;

#endif /* BOOK_H */
