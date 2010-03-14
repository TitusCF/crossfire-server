#ifndef CLASS_CRE_SETTINGS_H
#define CLASS_CRE_SETTINGS_H

#include <QSettings>

class CRESettings : protected QSettings
{
    public:
        CRESettings();

        bool ensureOptions();

        QString mapCacheDirectory() const;
};

#endif // CLASS_CRE_SETTINGS_H
