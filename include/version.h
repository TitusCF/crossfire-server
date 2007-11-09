#include <svnversion.h>  /* Automatically created at build time. */

#ifdef SVN_REV
#define FULL_VERSION VERSION"-r"SVN_REV
#else
#define FULL_VERSION VERSION
#endif
