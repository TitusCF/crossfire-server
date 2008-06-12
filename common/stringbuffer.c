/*
    CrossFire, A Multiplayer game for X-windows

    Copyright (C) 2008 Crossfire Development Team

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

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "global.h"
#include "libproto.h"
#include "stringbuffer.h"


struct StringBuffer {
    /**
     * The string buffer. The first {@link #pos} bytes contain the collected
     * string. It's size is at least {@link #size} bytes.
     */
    char *buf;

    /**
     * The current length of {@link #buf}. The invariant <code>pos <
     * size</code> always holds; this means there is always enough room to
     * attach a trailing \0 character.
     */
    size_t pos;

    /**
     * The allocation size of {@link #buf}.
     */
    size_t size;
};


/**
 * Make sure that at least <code>len</code> bytes are available in the passed
 * string buffer.
 *
 * @param sb The string buffer to modify.
 *
 * @param len The number of bytes to allocate.
 */
static void stringbuffer_ensure(StringBuffer *sb, size_t len);


StringBuffer *stringbuffer_new(void) {
    StringBuffer *sb;

    sb = malloc(sizeof(*sb));
    if (sb == NULL) {
        fatal(OUT_OF_MEMORY);
    }

    sb->size = 256;
    sb->buf = malloc(sb->size);
    sb->pos = 0;
    return sb;
}

char *stringbuffer_finish(StringBuffer *sb) {
    char *result;

    sb->buf[sb->pos] = '\0';
    result = sb->buf;
    free(sb);
    return result;
}

sstring stringbuffer_finish_shared(StringBuffer *sb) {
    char *str;
    sstring result;

    str = stringbuffer_finish(sb);
    result = add_string(str);
    free(str);
    return result;
}

void stringbuffer_finish_socklist(StringBuffer* sb, SockList* sl) {
    sl->len = sb->pos;
    sl->buf = stringbuffer_finish(sb);
}

void stringbuffer_append_string(StringBuffer *sb, const char *str) {
    size_t len;

    len = strlen(str);
    stringbuffer_ensure(sb, len+1);
    memcpy(sb->buf+sb->pos, str, len);
    sb->pos += len;
}

void stringbuffer_append_printf(StringBuffer *sb, const char *format, ...) {
    size_t size;

    size = 100;                 /* arbitrary guess */
    for (;;) {
        int n;
        va_list arg;

        stringbuffer_ensure(sb, size);

        va_start(arg, format);
        n = vsnprintf(sb->buf+sb->pos, size, format, arg);
        va_end(arg);

        if (n > -1 && (size_t)n < size) {
            sb->pos += (size_t)n;
            break;
        }

        if (n > -1) {
            size = n+1;         /* precisely what is needed */
        } else {
            size *= 2;          /* twice the old size */
        }
    }
}

void stringbuffer_append_stringbuffer(StringBuffer *sb, const StringBuffer *sb2) {
    stringbuffer_ensure(sb, sb2->pos+1);
    memcpy(sb->buf+sb->pos, sb2->buf, sb2->pos);
    sb->pos += sb2->pos;
}

static void stringbuffer_ensure(StringBuffer *sb, size_t len) {
    char *tmp;
    size_t new_size;

    if (sb->pos+len <= sb->size) {
        return;
    }

    new_size = sb->pos+len+256;
    tmp = realloc(sb->buf, new_size);
    if (tmp == NULL) {
        fatal(OUT_OF_MEMORY);
    }
    sb->buf = tmp;
    sb->size = new_size;
}
