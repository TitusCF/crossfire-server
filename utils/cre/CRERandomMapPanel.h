#ifndef CRERANDOMMAPPANEL_H
#define	CRERANDOMMAPPANEL_H

#include "CREPanel.h"

class CRERandomMap;
class QTextEdit;
class QLabel;

class CRERandomMapPanel : public CRETPanel<const CRERandomMap>
{
public:
  CRERandomMapPanel();
  virtual void setItem(const CRERandomMap* map);
private:
  QLabel* mySource;
  QTextEdit* myInformation;
};

#endif	/* CRERANDOMMAPPANEL_H */

