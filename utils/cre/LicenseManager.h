/*
 * Crossfire -- cooperative multi-player graphical RPG and adventure game
 *
 * Copyright (c) 2021 the Crossfire Development Team
 *
 * Crossfire is free software and comes with ABSOLUTELY NO WARRANTY. You are
 * welcome to redistribute it under certain conditions. For details, please
 * see COPYING and LICENSE.
 *
 * The authors can be reached via e-mail at <crossfire@metalforge.org>.
 */

#ifndef LICENSEMANAGER_H
#define LICENSEMANAGER_H

#include <string>
#include <map>
#include <vector>

struct BufferReader;

class LicenseManager {
public:

  typedef std::pair<std::string, std::string> LicenseItem;
  typedef std::map<std::string, std::vector<LicenseItem>> LicenseItems;

  LicenseManager();
  virtual ~LicenseManager();

  LicenseItems getForFace(const std::string &face);

  static void readLicense(BufferReader *reader, const char *filename);
  static LicenseManager *get();

private:

  static LicenseManager *instance;
  std::map<std::string, LicenseItems> m_licenses;
};

#endif /* LICENSEMANAGER_H */
