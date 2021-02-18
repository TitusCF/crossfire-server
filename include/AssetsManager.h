/*
 * Crossfire -- cooperative multi-player graphical RPG and adventure game
 *
 * Copyright (c) 2020-2021 the Crossfire Development Team
 *
 * Crossfire is free software and comes with ABSOLUTELY NO WARRANTY. You are
 * welcome to redistribute it under certain conditions. For details, please
 * see COPYING and LICENSE.
 *
 * The authors can be reached via e-mail at <crossfire@metalforge.org>.
 */

#ifndef ASSETSMANAGER_H
#define ASSETSMANAGER_H

#include "Archetypes.h"
#include "Animations.h"
#include "Treasures.h"
#include "Faces.h"
#include "Messages.h"
#include "Facesets.h"

/**
 * Represents all assets of the game.
 * It will listen to archetype changes to correctly update artifacts if needed.
 */
class AssetsManager {
public:
    /** Constructor. */
    AssetsManager();
    /** Destructor. */
    virtual ~AssetsManager();

    /**
     * Get faces.
     * @return faces.
     */
    Faces *faces() { return &m_faces; }
    /**
     * Get archetypes.
     * @return archetypes.
     */
    Archetypes *archetypes() { return &m_archetypes; }
    /**
     * Get animations.
     * @return animations.
     */
    AllAnimations *animations() { return &m_animations; }
    /**
     * Get treasures.
     * @return treasures.
     */
    Treasures *treasures() { return &m_treasures; }
    /**
     * Get messages.
     * @return messages.
     */
    Messages *messages() { return &m_messages; }

    /**
     * Get facesets.
     * @return facesets;
     */
    Facesets *facesets() { return &m_facesets; }

    /**
     * Dump through LOG undefined assets (except faces), and return the total number of
     * referenced but not defined assets (except faces).
     * @return count of undefined assets.
     */
    size_t dumpUndefined();

protected:
    Faces m_faces;                          /**< Faces. */
    Archetypes m_archetypes;                /**< Archetypes. */
    Archetypes::updateListener m_listener;  /**< Listener to archetypes changes. */
    AllAnimations m_animations;             /**< Animations. */
    Treasures m_treasures;                  /**< Treasures. */
    Messages m_messages;                    /**< Messages. */
    Facesets m_facesets;                    /**< Cilent image variants. */

    /**
     * Update artifacts if needed when an archetype changes.
     * @param existing archetype being changed.
     * @param update new values of the archetype.
     */
    void archetypeUpdated(archetype *existing, archetype *update);
};

#endif /* ASSETSMANAGER_H */

