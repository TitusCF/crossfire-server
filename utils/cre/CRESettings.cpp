#include "CRESettings.h"
#include "CRESettingsDialog.h"

#include <QFileInfo>
#include <QDir>

CRESettings::CRESettings()
{
}

bool CRESettings::ensureOptions()
{
    bool need = false;
    if (mapCacheDirectory().isEmpty())
        need = true;
    else
    {
        QFileInfo info(mapCacheDirectory());
        if (!info.exists())
        {
            QDir dir(mapCacheDirectory());
            dir.mkpath(".");
            info.refresh();
        }

        if (!info.exists())
            need = true;
    }

    if (need)
    {
        CRESettingsDialog dlg(this);
        if (dlg.exec() == QDialog::Rejected)
            return false;

        setValue("mapCacheDirectory", dlg.mapCache());
        return ensureOptions();
    }

    if (facesetToDisplay().isEmpty())
        setFacesetToDisplay("base");

    return true;
}

QString CRESettings::mapCacheDirectory() const
{
    return value("mapCacheDirectory").toString();
}

void CRESettings::loadFilters(CREFilterDefinitionManager& filters)
{
    filters.copy(value("filters").value<CREFilterDefinitionManager>());
}

void CRESettings::saveFilters(const CREFilterDefinitionManager& filters)
{
    QVariant val;
    val.setValue(filters);
    setValue("filters", val);
}

void CRESettings::loadReports(CREReportDefinitionManager& reports)
{
    reports.copy(value("reports").value<CREReportDefinitionManager>());
}

void CRESettings::saveReports(const CREReportDefinitionManager& reports)
{
    QVariant val;
    val.setValue(reports);
    setValue("reports", val);
}

QString CRESettings::facesetToDisplay() const
{
    return value("facesetToDisplay").toString();
}

void CRESettings::setFacesetToDisplay(const QString& faceset)
{
    setValue("facesetToDisplay", faceset);
}

bool CRESettings::facesetUseFallback() const
{
    return value("facesetUseFallback").toBool();
}

void CRESettings::setFacesetUseFallback(bool use)
{
    setValue("facesetUseFallback", use);
}
