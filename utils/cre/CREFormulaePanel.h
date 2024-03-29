#ifndef CREFORMULAEPANEL_H
#define CREFORMULAEPANEL_H

#include <QObject>
#include <QtWidgets>
#include "CREPanel.h"

extern "C" {
#include "global.h"
#include "recipe.h"
}

class CREFormulaePanel : public CRETPanel<const recipe>
{
    Q_OBJECT

    public:
        CREFormulaePanel(QWidget* parent);

        void setItem(const recipe* recipe);

    protected:
        const recipe* myRecipe;
        QComboBox* myTitle;
        QLineEdit* myYield;
        QLineEdit* myChance;
        QLineEdit* myExperience;
        QLineEdit* myDifficulty;
        QComboBox* mySkill;
        QComboBox* myCauldron;
        QCheckBox* myIsTransmute;
        QLineEdit* myIndex;
        QTreeWidget* myArchetypes;
        QTextEdit* myIngredients;
        QPushButton* myValidate;
        QPushButton* myReset;

    protected slots:
        void resetClicked(bool);
        void validateClicked(bool);
};

#endif // CREFORMULAEPANEL_H
