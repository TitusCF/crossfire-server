#ifndef _CREMAPPANEL_H
#define	_CREMAPPANEL_H

#include <QObject>
#include <QtGui>

class CREMapInformation;

class CREMapPanel : public QWidget
{
    public:
        CREMapPanel();
        virtual ~CREMapPanel();

        void setMap(CREMapInformation* map);

    protected:
        QLabel* myName;
        QTreeWidget* myExitsTo;
        QTreeWidget* myExitsFrom;
};

#endif	/* _CREMAPPANEL_H */
