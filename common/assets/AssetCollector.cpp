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
    m_reader = bufferreader_create();
}

AssetCollector::~AssetCollector() {
    for (auto loader : m_loaders) {
        delete loader;
    }
    bufferreader_destroy(m_reader);
}

void AssetCollector::processFiles(const std::vector<std::string>& files) {
    for (const auto &file : files) {
        for (auto loader : m_loaders) {
            if (loader->willLoad(file)) {
                FILE *opened = fopen(file.c_str(), "rb");
                if (!opened) {
                    LOG(llevError, "unable to open file %s\n", file.c_str());
                    continue;
                }
                bufferreader_init_from_file(m_reader, opened);
                fclose(opened);
                loader->load(m_reader, file);
            }
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

void AssetCollector::load(BufferReader *reader, const std::string &filename) {
    for (AssetLoader *loader : m_loaders) {
        if (loader->willLoad(filename)) {
            loader->load(reader, filename);
        }
    }
}
