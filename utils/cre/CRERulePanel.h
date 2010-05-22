#ifndef _CRERULEPANEL_H
#define	_CRERULEPANEL_H

#include <QTabWidget>

class QListWidget;
class MessageRule;
class QPushButton;
class QLineEdit;

class CRERulePanel : public QTabWidget
{
    Q_OBJECT

    public:
        CRERulePanel(QWidget* parent);
        virtual ~CRERulePanel();

        void setMessageRule(MessageRule* rule);

    signals:
        void currentRuleModified();

    protected:
        MessageRule* myRule;
        int myCurrentMatch;
        QListWidget* myMatches;
        QLineEdit* myMatch;

        void commitMatch();
    protected slots:
        void onAddMatch(bool);
        void onDeleteMatch(bool);
        void onCurrentMatchChanged(int currentRow);
};

#endif	/* _CRERULEPANEL_H */

