#include "global.h"

#ifdef HAVE_GPERFTOOLS_PROFILER_H
#include <gperftools/profiler.h>
#endif

int main(int argc, char *argv[]) {
    if (argc < 3) {
        puts("usage: perf_mapsave PATH N_TIMES");
        return 0;
    }

    settings.debug = llevDebug;
    init_library();
    int flags = 0;
    if (argv[1][0] == '~') {
        flags |= MAP_PLAYER_UNIQUE;
    }
#ifdef HAVE_GPERFTOOLS_PROFILER_H
    ProfilerStart("mapsave.prof");
#endif
    for (int i = 0; i < (int)strtol(argv[2], NULL, 10); i++) {
        mapstruct *m = mapfile_load(argv[1], flags);
        if (m == NULL) {
            break;
        }
        save_map(m, 0);
        free_map(m);
    }
#ifdef HAVE_GPERFTOOLS_PROFILER_H
    ProfilerStop();
#endif
}
