/*
 * Crossfire -- cooperative multi-player graphical RPG and adventure game
 *
 * Copyright (c) 2021 the Crossfire Development Team
 *
 * Crossfire is free software and comes with ABSOLUTELY NO WARRANTY. You are
 * welcome to redistribute it under certain conditions. For details, please
 * see COPYING and LICENSE.
 *
 * The authors can be reached via e-mail at <crossfire@metalforge.org>.
 */

#include "LicenseManager.h"

#include <memory>

LicenseManager *LicenseManager::instance = nullptr;

LicenseManager::LicenseManager() {
}

LicenseManager::~LicenseManager() {
}

LicenseManager *LicenseManager::get() {
    if (LicenseManager::instance == nullptr) {
        LicenseManager::instance = new LicenseManager();
    }
    return LicenseManager::instance;
}

void LicenseManager::readLicense(BufferReader *reader, const char *filename) {
    char *line, *c;
    std::unique_ptr<char, void(*)(void*)> dup(strdup(filename), free);

    c = strrchr(dup.get(), '.');
    if (!c) {
        LOG(llevError, "LicenseManager: invalid filename %s\n", filename);
        return;
    }
    (*c) = '\0';
    c = strrchr(dup.get(), '.');
    if (!c) {
        LOG(llevError, "LicenseManager: invalid filename %s\n", filename);
        return;
    }
    (*c) = '\0';
    c++;

    std::string faceset(c);

    c = strrchr(dup.get(), '/');
    if (!c) {
        LOG(llevError, "LicenseManager: invalid filename %s\n", filename);
        return;
    }
    c++;

    auto &licenses = get()->m_licenses[c];
    auto &item = licenses[faceset];

    while ((line = bufferreader_next_line(reader)) != nullptr) {
        if (line[0] == '\0')
            continue;
        c = strchr(line, ' ');
        if (!c) {
            LOG(llevError, "LicenseManager: malformed line '%s' in %s:%zu\n", line, filename, bufferreader_current_line(reader));
            continue;
        }

        (*c) = '\0';
        c++;

        item.push_back(std::make_pair(line, c));
    }
}

LicenseManager::LicenseItems LicenseManager::getForFace(const std::string &face) {
    auto search = m_licenses.find(licenseNameFromFaceName(face));
    if (search != m_licenses.end()) {
        return search->second;
    }
    return LicenseItems();
}

std::string LicenseManager::licenseNameFromFaceName(const std::string &face) {
    auto dot = face.find('.');
    if (dot == std::string::npos)
    {
        return std::string();
    }
    return face.substr(0, dot);
}
