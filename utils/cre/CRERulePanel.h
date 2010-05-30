#ifndef _CRERULEPANEL_H
#define	_CRERULEPANEL_H

#include <QTabWidget>

class QListWidget;
class MessageRule;
class QPushButton;
class QLineEdit;
class CREStringListPanel;
class CREPrePostPanel;
class CREReplyPanel;
class MessageManager;

class CRERulePanel : public QTabWidget
{
    Q_OBJECT

    public:
        CRERulePanel(const MessageManager* manager, QWidget* parent);
        virtual ~CRERulePanel();

        void setMessageRule(MessageRule* rule);

    signals:
        void currentRuleModified();

    protected:
        MessageRule* myRule;
        CREStringListPanel* myMatches;
        CREPrePostPanel* myPre;
        CREStringListPanel* myMessages;
        CREPrePostPanel* myPost;
        CREReplyPanel* myReplies;
        QLineEdit* myInclude;

    protected slots:
        void onMatchModified();
        void onPreModified();
        void onMessageModified();
        void onPostModified();
        void onRepliesModified();
        void onIncludeModified(const QString& text);
};

#endif	/* _CRERULEPANEL_H */

