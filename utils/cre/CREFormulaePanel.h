#ifndef CREFORMULAEPANEL_H
#define CREFORMULAEPANEL_H

#include <QObject>
#include <QtGui>

extern "C" {
#include "global.h"
#include "recipe.h"
}

class CREFormulaePanel : public QWidget
{
    Q_OBJECT

    public:
        CREFormulaePanel();

        void setRecipe(const recipe* recipe);

    protected:
        const recipe* myRecipe;
        QComboBox* myTitle;
        QLineEdit* myYield;
        QLineEdit* myChance;
        QLineEdit* myExperience;
        QLineEdit* myDifficulty;
        QComboBox* mySkill;
        QComboBox* myCauldron;
        QTreeWidget* myArchetypes;
        QTextEdit* myIngredients;
        QPushButton* myValidate;
        QPushButton* myReset;

    protected slots:
        void resetClicked(bool);
        void validateClicked(bool);
};

#endif // CREFORMULAEPANEL_H
