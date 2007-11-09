#include <svnversion.h>  /* Automatically created at build time. FIXME: it's not the case!*/

#ifdef SVN_REV
#define FULL_VERSION VERSION"-r"SVN_REV
#else
#define FULL_VERSION VERSION
#endif
