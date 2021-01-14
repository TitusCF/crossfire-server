#ifndef ARCHETYPES_H
#define ARCHETYPES_H

#include "AssetsCollection.h"

extern "C" {
#include "global.h"
}

#include <functional>

/**
 * All archetypes in the game.
 */
class Archetypes : public AssetsCollection<archetype> {

public:
    /** Function prototype to be called when an archetype changes. */
    typedef std::function<void(archetype*, archetype*)> updateListener;

    /**
     * Standard constructor.
     */
    Archetypes() {
        empty_archetype = get("empty_archetype");
    }

    /**
     * Set the listener to be called when an archetype is updated.
     * @param fct listener, may be an empty std::function.
     */
    void setReplaceListener(updateListener fct) {
        m_updateListener = fct;
    }

    /**
     * Remvove the listener for archetypes changes.
     */
    void clearReplaceListener() {
        m_updateListener = updateListener();
    }

    /**
     * Retrieve an archetype given the name that appears
     * during the game (for example, "writing pen" instead of "stylus").
     * @param name
     * the name we're searching for (ex: "writing pen")
     * @return
     * the archetype found or NULL if nothing was found
     */
    archetype *findByObjectName(const char *name);

    /**
     * Retrieve an archetype by type and name that appears during
     * the game. It is basically the same as findByObjectName()
     * except that it considers only items of the given type.
     * @param type
     * item type we're searching
     * @param name
     * the name we're searching for (ex: "writing pen")
     * @return
     * found archetype, NULL if none matched.
     */
    archetype *findByObjectTypeName(int type, const char *name);

    /**
     * Retrieve an archetype by skill name and type.
     * @param skill
     * skill to search for. Must not be NULL.
     * @param type
     * item type to search for. -1 means that it doesn't matter.
     * @return
     * matching archetype, or NULL if none found.
     */
    archetype *findBySkillNameAndType(const char *skill, int type);

    /**
     * Retrieve an archetype by type and subtype.
     * This returns the first archetype
     * that matches both the type and subtype.  type and subtype
     * can be -1 to say ignore, but in this case, the match it does
     * may not be very useful.  This function is most useful when
     * subtypes are known to be unique for a particular type
     * (eg, skills).
     * @param type
     * object type to search for. -1 means any
     * @param subtype
     * object subtype to search for. -1 means any
     * @return
     * matching archetype, or NULL if none found.
     */
    archetype *findByTypeSubtype(int type, int subtype);

    /**
     * Update if needed item based on the updated archetype, then recurse on
     * item->below and item->inv.
     * Updating implies getting the difference with the obsolete archetype's clone,
     * then applying it to the updated version of the archetype.
     * Inventory items of the updated version are not taken into account.
     * @param item what to update from.
     * @param updated updated archetype.
     */
    static void recursive_update(object *item, archetype *updated);

protected:
    virtual archetype *create(const std::string& name);
    virtual void destroy(archetype *item);
    virtual void replace(archetype *existing, archetype *update);

    updateListener m_updateListener;
};

#endif /* ARCHETYPES_H */

