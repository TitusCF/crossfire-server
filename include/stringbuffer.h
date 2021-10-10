/** @file stringbuffer.h
 *
 * Implements a general string buffer: it builds a string by concatenating. It
 * allocates enough memory to hold the whole string; there is no upper limit
 * for the total string length.
 *
 * Usage is:
 * <code>
 * StringBuffer *sb = stringbuffer_new();
 * stringbuffer_append_string(sb, "abc");
 * stringbuffer_append_string(sb, "def");
 * ... more calls to stringbuffer_append_xxx()
 * char *str = stringbuffer_finish(sb)
 * ... use str
 * free(str);
 * </code>
 *
 * No function ever fails. In case not enough memory is availabl, {@link
 * fatal()} is called.
 */

#ifndef STRING_BUFFER_H
#define STRING_BUFFER_H

#include <ctype.h>

#include "global.h"


/**
 * The string buffer state.
 */
typedef struct StringBuffer StringBuffer;


/**
 * Create a new string buffer.
 *
 * @return The newly allocated string buffer.
 */
StringBuffer *stringbuffer_new(void);

/**
 * Totally delete a string buffer.
 * @param sb String to delete, pointer becomes invalid after the call.
 */
void stringbuffer_delete(StringBuffer *sb);

/**
 * Deallocate the string buffer instance and return the string.
 *
 * The passed string buffer must not be accessed afterwards.
 *
 * @param sb The string buffer to deallocate.
 *
 * @return The result string; to free it, call <code>free()</code> on it.
 */
char *stringbuffer_finish(StringBuffer *sb);

/**
 * Deallocate the string buffer instance and return the string as a shared
 * string.
 *
 * The passed string buffer must not be accessed afterwards.
 *
 * @param sb The string buffer to deallocate.
 *
 * @return The result shared string; to free it, call
 * <code>free_string()</code> on it.
 */
sstring stringbuffer_finish_shared(StringBuffer *sb);

/**
 * Append a string to a string buffer instance.
 *
 * @param sb The string buffer to modify.
 *
 * @param str The string to append.
 */
void stringbuffer_append_string(StringBuffer *sb, const char *str);

/**
 * Append a formatted string to a string buffer instance.
 *
 * @param sb The string buffer to modify.
 *
 * @param format The format string to append.
 */
void stringbuffer_append_printf(StringBuffer *sb, const char *format, ...);

/**
 * Append the contents of a string buffer instance to another string buffer
 * instance.
 *
 * @param sb The string buffer to modify.
 *
 * @param sb2 The string buffer to append; it must be different from sb.
 */
void stringbuffer_append_stringbuffer(StringBuffer *sb, const StringBuffer *sb2);

/**
 * Append the specified content in a multiline block, starting with "start" and ending with "end".
 * "start" and "end" must not end with a newline.
 * @param sb The string buffer to modify.
 * @param start The text to write to signify the start of the contents.
 * @param content The actual content, which may contain multilines. If NULL of empty string, nothing is written.
 * @param end The text to write to signify the end of the contents. If NULL then "end_<start>" will be used.
 */
void stringbuffer_append_multiline_block(StringBuffer *sb, const char *start, const char *content, const char *end);

/**
 * Return the current length of the buffer.
 * @param sb The string buffer to check.
 *
 * @return current length of sb.
 */
size_t stringbuffer_length(StringBuffer *sb);

/**
 * Trim trailing whitespace from a stringbuffer.
 *
 * @param sb
 * The stringbuffer.
 */
void stringbuffer_trim_whitespace(StringBuffer *sb);

#endif
