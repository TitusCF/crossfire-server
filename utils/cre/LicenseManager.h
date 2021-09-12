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

/**
 * Contains all information about licenses for faces.
 */
class LicenseManager {
public:

  typedef std::pair<std::string, std::string> LicenseItem;              /**< One line in a license file. */
  typedef std::map<std::string, std::vector<LicenseItem>> LicenseItems; /**< For a single face, map between a faceset's prefix and the license items. */

  /** Standard constructor. */
  LicenseManager();
  /** Standard destructor. */
  virtual ~LicenseManager();

  /**
   * Get license information for a face.
   * @param face face name, with the suffix.
   * @return license information.
   */
  LicenseItems getForFace(const std::string &face);

  /**
   * Get all known license information.
   * @return map between face names without prefix and license information.
   */
  std::map<std::string, LicenseItems> getAll() const { return m_licenses; }

  /**
   * Reset license information, before a reloading.
   */
  void reset() { m_licenses.clear(); }

  /**
   * Process a license file.
   * @param reader source.
   * @param filename full file name.
   */
  static void readLicense(BufferReader *reader, const char *filename);

  /**
   * Get the single instance of the LicenseManager.
   * @return instance.
   */
  static LicenseManager *get();

  /**
   * Get the expected license name from a face name.
   * @param face face name, with the suffix.
   * @return expected license file name, empty if face is invalid.
   */
  static std::string licenseNameFromFaceName(const std::string &face);

private:
  static LicenseManager *instance;                /**< Single instance. */
  std::map<std::string, LicenseItems> m_licenses; /**< License information for all faces. */
};

#endif /* LICENSEMANAGER_H */
