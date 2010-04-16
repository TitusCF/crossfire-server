TEMPLATE = app

INCLUDEPATH += ../../include

SOURCES += cre.cpp \
CREMainWindow.cpp \
CREPixmap.cpp \
 CREUtils.cpp \
 CREAnimationControl.cpp \
 CREAnimationWidget.cpp \
 CREResourcesWindow.cpp \
 CRETreeItem.cpp \
 CRETreeItemAnimation.cpp \
 CREAnimationPanel.cpp \
 CRETreeItemTreasure.cpp \
 CRETreasurePanel.cpp \
 CRETreeItemArtifact.cpp \
 CREArtifactPanel.cpp \
 CREFormulaePanel.cpp \
 CRETreeItemFormulae.cpp \
 CRETreeItemFace.cpp \
 CREFacePanel.cpp \
 CRETreeItemArchetype.cpp \
 CREArchetypePanel.cpp \
 CREMapInformation.cpp \
 CREMapInformationManager.cpp \
 CRESettings.cpp \
 CRESettingsDialog.cpp \
 CREFilterDialog.cpp \
 CREFilter.cpp \
 CREWrapperArchetype.cpp \
 CREWrapperObject.cpp \
 CREWrapperArtifact.cpp \
 CREWrapperFormulae.cpp \
 CREReportDialog.cpp \
 CREReportDisplay.cpp \
 CREFilterDefinition.cpp \
 CREFilterDefinitionManager.cpp \
 CREReportDefinition.cpp \
 CREMapPanel.cpp \
 CRETreeItemMap.cpp \
 CREExperienceWindow.cpp \
 CRERegionPanel.cpp \
 CRETreeItemRegion.cpp \
 CREReportDefinitionManager.cpp
HEADERS += CREMainWindow.h \
CREPixmap.h \
 CREUtils.h \
 CREAnimationControl.h \
 CREAnimationWidget.h \
 CREResourcesWindow.h \
 CRETreeItem.h \
 CRETreeItemAnimation.h \
 CREAnimationPanel.h \
 CRETreeItemTreasure.h \
 CRETreasurePanel.h \
 CRETreeItemArtifact.h \
 CREArtifactPanel.h \
 CREFormulaePanel.h \
 CRETreeItemFormulae.h \
 CRETreeItemFace.h \
 CREFacePanel.h \
 CRETreeItemArchetype.h \
 CREArchetypePanel.h \
 CREMapInformation.h \
 CREMapInformationManager.h \
 CRESettings.h \
 CRESettingsDialog.h \
 CREFilterDialog.h \
 CREFilter.h \
 CREWrapperArchetype.h \
 CREWrapperObject.h \
 CREWrapperArtifact.h \
 CREWrapperFormulae.h \
 CREReportDialog.h \
 CREReportDisplay.h \
 CREFilterDefinition.h \
 CREFilterDefinitionManager.h \
 CREReportDefinition.h \
 CREMapPanel.h \
 CRETreeItemMap.h \
 CREExperienceWindow.h \
 CRERegionPanel.h \
 CRETreeItemRegion.h \
 CREReportDefinitionManager.h
LIBS += ../../common/libcross.a

RESOURCES += cre.qrc

CONFIG -= precompile_header
CONFIG += thread

QT += script
