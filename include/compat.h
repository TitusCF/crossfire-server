/**
 * @file
 * Compatibility implementations of useful nonstandard types and functions.
 */

#ifndef _COMPAT_H
#define _COMPAT_H

// Maintain backward compatibility with original macros.
#define TRUE true
#define FALSE false
#define FMT64   PRId64
#define FMT64U  PRIu64

#define safe_strncpy strlcpy
#define strcasestr_local strcasestr
#define strdup_local strdup

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
