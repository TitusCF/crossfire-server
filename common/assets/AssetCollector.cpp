/*
 * Crossfire -- cooperative multi-player graphical RPG and adventure game
 *
 * Copyright (c) 2020 the Crossfire Development Team
 *
 * Crossfire is free software and comes with ABSOLUTELY NO WARRANTY. You are
 * welcome to redistribute it under certain conditions. For details, please
 * see COPYING and LICENSE.
 *
 * The authors can be reached via e-mail at <crossfire@metalforge.org>.
 */

#include "AssetCollector.h"

#ifndef WIN32 /* ---win32 exclude headers */
#include <dirent.h>
#include <sys/stat.h>
#include "../../include/autoconf.h"
#endif

extern "C" {
#include "global.h"
#include "compat.h"
#include "Utils.h"
#include <string.h>
}
#include "microtar.h"

#include "PngLoader.h"
#include <algorithm>

AssetCollector::AssetCollector() {
}

AssetCollector::~AssetCollector() {
    for (auto loader : m_loaders) {
        delete loader;
    }
}

void AssetCollector::processFiles(const std::vector<std::string>& files) {
    for (const auto &file : files) {

        if (Utils::endsWith(file, ".tar")) {
            processTar(file);
        }

        FILE *opened = NULL;

        for (auto loader : m_loaders) {
            if (loader->willProcess(file)) {
                if (!opened) {
                    opened = fopen(file.c_str(), "rb");
                } else {
                    fseek(opened, 0, SEEK_SET);
                }
                loader->processFile(opened, file);
            }
        }

        if (opened) {
            fclose(opened);
        }
    }
}


void AssetCollector::collect(const std::string& directory) {
    DIR *dp;
    struct dirent *d;
    struct stat sb;

    char full_path[PATH_MAX];
    std::vector<std::string> names[2];
    int type;

    dp = opendir(directory.c_str());
    if (dp == NULL) {
        return;
    }

    while ((d = readdir(dp)) != NULL) {

        if (d->d_name[0] == 0 || d->d_name[0] == '.' || strcmp(d->d_name, "dev") == 0 || strcmp(d->d_name, "trashbin") == 0)
            continue;

        snprintf(full_path, sizeof(full_path), "%s/%s", directory.c_str(), d->d_name);
        stat(full_path, &sb);
        type = S_ISDIR(sb.st_mode) ? 0 : 1;
        names[type].push_back(full_path);
    }
    (void)closedir(dp);

    for (type = 0; type < 2; type++) {
        std::sort(names[type].begin(), names[type].end());
    }

    processFiles(names[1]);

    for (const auto &dir : names[0]) {
        collect(dir);
    }
}

void AssetCollector::processTar(const std::string& file) {
    mtar_t tar;
    mtar_header_t h;

    if (mtar_open(&tar, file.c_str(), "r") != MTAR_ESUCCESS) {
        LOG(llevError, "Failed to open tar file %s\n", file.c_str());
        return;
    }

    while ((mtar_read_header(&tar, &h)) != MTAR_ENULLRECORD) {
        for (auto loader : m_loaders) {
            if (loader->willProcess(h.name)) {
                PngLoader *png = dynamic_cast<PngLoader *>(loader);
                if (!png) {
                    continue;
                }

                uint8_t *data = static_cast<uint8_t *>(malloc(h.size));
                if (!data) {
                    LOG(llevError, "Failed to allocate %ul bytes!\n", h.size);
                    fatal(SEE_LAST_ERROR);
                }
                mtar_read_data(&tar, static_cast<void *>(data), h.size);

                png->processData(data, h.size, h.name);
            }
        }
        mtar_next(&tar);
    }

    mtar_close(&tar);
}
