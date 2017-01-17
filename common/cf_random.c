#include <stdint.h>
#include <stdlib.h>

#include "define.h"

uint32_t cf_random() {
#if defined(HAVE_SRANDOM)
    return random();
#elif defined(HAVE_SRAND48)
    return lrand48();
#else
    return rand();
#endif
}

void cf_srandom(unsigned long seed) {
#if defined(HAVE_SRANDOM)
    srandom(seed);
#elif defined(HAVE_SRAND48)
    srand48(seed);
#else
    srand(seed);
#endif
}
