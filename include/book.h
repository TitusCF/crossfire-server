
/* Dec '95 - laid down initial file. Stuff in here is for BOOKs 
 * hack. Information in this file describes fundental parameters 
 * of 'books' - objects with type==BOOK. -b.t.
 */
 
/* Message buf size. If this is changed, keep in mind that big strings
 * may be unreadable by the player as the tail of the message
 * can scroll over the beginning (as of v0.92.2).
 * Note that the book messages are stored in the msg buf,
 * which is limited by 'HUGE_BUF' in the loader.
 */
#ifndef BOOK_H
#define BOOK_H

#define BOOK_BUF        HUGE_BUF-10
 
/* if little books arent getting enough text generated, enlarge this */
 
#define BASE_BOOK_BUF   250
 
/* Book buffer size. We shouldnt let little books/scrolls have
 * more info than big, weighty tomes! So lets base the 'natural'
 * book message buffer size on its weight. But never let a book
 * mesg buffer exceed the max. size (BOOK_BUF) */
 
#define BOOKSIZE(xyz)   BASE_BOOK_BUF+((xyz)->weight/10)>BOOK_BUF? \
                                BOOK_BUF:BASE_BOOK_BUF+((xyz)->weight/10);

typedef struct {
	uint8 message_type;
	uint8 message_subtype;
} readable_message_type;

#endif /* BOOK_H */
