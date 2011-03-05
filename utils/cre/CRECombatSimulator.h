#ifndef CRECOMBATSIMULATOR_H
#define	CRECOMBATSIMULATOR_H

#include <QObject>
#include <QDialog>

class QComboBox;

class CRECombatSimulator : public QDialog
{
    Q_OBJECT

    public:
        CRECombatSimulator();
        virtual ~CRECombatSimulator();

    protected slots:
        void fight();

    private:
        QComboBox* myFirst;
        QComboBox* mySecond;
        QLabel* myResultLabel;
        QLabel* myResult;
};

#endif	/* CRECOMBATSIMULATOR_H */

