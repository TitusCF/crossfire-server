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
 CREArchetypePanel.cpp
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
 CREArchetypePanel.h
LIBS += ../../common/libcross.a

RESOURCES += cre.qrc

CONFIG -= precompile_header
