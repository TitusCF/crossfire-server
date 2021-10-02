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

#include "FaceLoader.h"
#include "Faces.h"
#include "Animations.h"

extern "C" {
#include "global.h"
#include "compat.h"
#include "string.h"
}

FaceLoader::FaceLoader(Faces *faces, AllAnimations *animations)
    : m_faces(faces), m_animations(animations)
{
}

void FaceLoader::loadAnimationBlock(BufferReader *reader, const std::string &full_path, const char *animation_name) {
    char *buf;
    int num_frames = 0, i;
    const Face *faces[MAX_ANIMATIONS];
    Animations *anim;

    anim = static_cast<Animations *>(calloc(1, sizeof(Animations)));
    anim->name = add_string(animation_name);
    anim->faces = NULL;
    anim->facings = 1;
    anim->num_animations = 0;

    while ((buf = bufferreader_next_line(reader)) != NULL) {
        if (*buf == '#')
            continue;
        if (strlen(buf) == 0)
            continue;

        if (!strncmp(buf, "mina", 4)) {
            anim->faces = static_cast<const Face **>(malloc(sizeof(Face*)*num_frames));
            for (i = 0; i < num_frames; i++)
                anim->faces[i] = faces[i];
            anim->num_animations = num_frames;
            if (num_frames <= 1) {
                LOG(llevDebug, "anim: %s has less then two faces in %s\n",
                        anim->name, full_path.c_str());
            }
            if (num_frames % anim->facings) {
                LOG(llevDebug, "anim: %s has %d frames: not a multiple of facings (%d) in %s\n",
                        anim->name, num_frames,
                        anim->facings, full_path.c_str());
            }
            m_animations->define(anim->name, anim);
            return;
        } else if (!strncmp(buf, "facings", 7)) {
            if (!(anim->facings = atoi(buf+7))) {
                LOG(llevDebug, "anim: %s has 0 facings in %s (line %s)\n",
                    anim->name, full_path.c_str(), buf);
                anim->facings = 1;
            }
        } else {
            const Face *face = find_face(buf);
            faces[num_frames++] = face;
        }
    }
}

void FaceLoader::load(BufferReader *buffer, const std::string& filename) {
    char *buf, *cp;
    Face *on_face = NULL;

    while ((buf = bufferreader_next_line(buffer)) != NULL) {
        if (*buf == '#' || *buf == '\0')
            continue;
        if (!strncmp(buf, "end", 3)) {
            if (on_face) {
                m_faces->define(on_face->name, on_face);
                on_face = NULL;
            }
            continue;
        }

        if (!strncmp(buf, "smoothface ", 11)) {
            if (on_face) {
                on_face->smoothface = m_faces->get(buf + 11);
            } else {
                char *space = strchr(buf + 11, ' ');
                if (!space) {
                    LOG(llevError, "Invalid 'smoothface' %s in %s\n", buf, filename.c_str());
                } else {
                    (*space) = '\0';
                    space++;
                    auto original = m_faces->get(buf + 11);
                    auto smoothed = m_faces->get(space);
                    original->smoothface = smoothed;
                }
            }
            continue;
        }

        if (!strncmp(buf, "face ", 5)) {
            cp = buf+5;

            if (on_face) {
                LOG(llevError, "'face' within a 'face' in %s\n", filename.c_str());
                if (on_face->name) {
                    free_string(on_face->name);
                }
                free(on_face);
            }
            on_face = static_cast<Face *>(calloc(1, sizeof(Face)));
            on_face->name = add_string(cp);
            on_face->visibility = 0;
            on_face->magicmap = 0;
            continue;
        }

        if (!strncmp(buf, "animation ", 10)) {
            loadAnimationBlock(buffer, filename, buf + 10);
            continue;
        }

        if (on_face == NULL) {
            LOG(llevError, "faces: got line with no face set in %s: %s\n", filename.c_str(), buf);
        } else if (!strncmp(buf, "visibility", 10)) {
            on_face->visibility = atoi(buf+11);
        } else if (!strncmp(buf, "magicmap", 8)) {
            cp = buf+9;
            on_face->magicmap = find_color(cp) | (on_face->magicmap & FACE_FLOOR);
        } else if (!strncmp(buf, "is_floor", 8)) {
            int value = atoi(buf+9);
            if (value)
                on_face->magicmap |= FACE_FLOOR;
        } else
            LOG(llevDebug, "faces: unknown line in file %s: %s\n", filename.c_str(), buf);
    }

    if (on_face) {
        LOG(llevError, "unfinished face in %s\n", filename.c_str());
        if (on_face->name) {
            free_string(on_face->name);
        }
        free(on_face);
    }
}
