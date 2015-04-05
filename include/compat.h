/**
 * @file
 * Compatibility implementations of useful nonstandard types and functions.
 */

#ifndef _COMPAT_H
#define _COMPAT_H

/* A few compilers refuse to support C99 boolean values. */
#ifdef HAVE_STDBOOL_H
#include <stdbool.h>
#else
#include "compat_stdbool.h"
#endif

/* Maintain backward compatibility with original boolean macros. */
#ifndef TRUE
#define TRUE true
#endif

#ifndef FALSE
#define FALSE false
#endif

/* Missing integer types are filled in by the build system. */
#ifdef HAVE_INTTYPES_H
#include <inttypes.h>
#endif

/* Maintain backward compatibility with format specifiers. */
#ifndef WIN32
#define FMT64   PRId64
#define FMT64U  PRIu64
#endif

#define safe_strncpy strlcpy
#define strcasestr_local strcasestr
#define strdup_local strdup
#define tempnam_local tempnam

#define CALLOC(x, y) calloc(x, y)
#define object_decrease_nrof_by_one(xyz) object_decrease_nrof(xyz, 1)

void safe_strcat(char *dest, const char *orig, size_t *curlen, size_t maxlen);

#ifndef HAVE_STRCASESTR
char *strcasestr(const char *s, const char *find);
#endif

#ifndef HAVE_STRDUP
char *strdup(const char *str);
#endif

#ifndef HAVE_STRLCPY
size_t strlcpy(char *dst, const char *src, size_t size);
#endif

#ifndef HAVE_TEMPNAM
char *tempnam(const char *tmpdir, const char *prefix);
#endif

#endif