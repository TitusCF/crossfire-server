#ifndef _CREPANEL_H
#define _CREPANEL_H

#include <QObject>
#include <QWidget>

class CREPanel : public QWidget
{
    Q_OBJECT

    public:
        CREPanel(QWidget* parent);
        virtual ~CREPanel();

        virtual void commitData();
    private:
};

template<typename T>
class CRETPanel : public CREPanel
{
    public:
        CRETPanel(QWidget* parent) : CREPanel(parent) {};
        virtual void setItem(T* item) = 0;
    private:
};

#endif /* _CREPANEL_H */
