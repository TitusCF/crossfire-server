#ifndef CLASS_CRE_SETTINGS_H
#define CLASS_CRE_SETTINGS_H

#include <QSettings>

#include "CREFilterDefinitionManager.h"

class CRESettings : protected QSettings
{
    public:
        CRESettings();

        bool ensureOptions();

        QString mapCacheDirectory() const;

        void loadFilters(CREFilterDefinitionManager& filters);
        void saveFilters(const CREFilterDefinitionManager& filters);
};

#endif // CLASS_CRE_SETTINGS_H
