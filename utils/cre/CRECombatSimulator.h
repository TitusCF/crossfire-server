#ifndef CRECOMBATSIMULATOR_H
#define	CRECOMBATSIMULATOR_H

#include <QObject>
#include <QDialog>

class QComboBox;
class QSpinBox;

class CRECombatSimulator : public QDialog
{
    Q_OBJECT

    public:
        CRECombatSimulator();
        virtual ~CRECombatSimulator();

    protected slots:
        void fight();

    private:
        void fight(const archetype* first, const archetype* second);

        QComboBox* myFirst;
        QComboBox* mySecond;
        QSpinBox* myCombats;
        QSpinBox* myMaxRounds;
        QLabel* myResultLabel;
        QLabel* myResult;

        int myFirstVictories;
        int myFirstMinHp;
        int myFirstMaxHp;
        int mySecondVictories;
        int mySecondMinHp;
        int mySecondMaxHp;
};

#endif	/* CRECOMBATSIMULATOR_H */

