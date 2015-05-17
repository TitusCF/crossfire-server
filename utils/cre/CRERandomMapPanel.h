#ifndef CRERANDOMMAPPANEL_H
#define	CRERANDOMMAPPANEL_H

#include "CREPanel.h"

class CRERandomMap;
class QTextEdit;
class QLabel;

class CRERandomMapPanel : public CREPanel
{
public:
  CRERandomMapPanel();
  void setRandomMap(const CRERandomMap* map);
private:
  QLabel* mySource;
  QTextEdit* myInformation;
};

#endif	/* CRERANDOMMAPPANEL_H */

