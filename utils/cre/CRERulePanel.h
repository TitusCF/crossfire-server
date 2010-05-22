#ifndef _CRERULEPANEL_H
#define	_CRERULEPANEL_H

#include <QTabWidget>

class QListWidget;
class MessageRule;
class QPushButton;
class QLineEdit;
class CREStringListPanel;

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
        CREStringListPanel* myMatches;
        CREStringListPanel* myMessages;

    protected slots:
        void onMatchModified();
        void onMessageModified();
};

#endif	/* _CRERULEPANEL_H */

