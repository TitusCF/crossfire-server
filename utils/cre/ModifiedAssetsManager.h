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

#ifndef MODIFIEDASSETSMANAGER_H
#define MODIFIEDASSETSMANAGER_H

#include <string>
#include <set>
#include "AssetWriter.h"
#include <QFile>

template<class T>
class ModifiedAssetsManager {
public:
    ModifiedAssetsManager(AssetWriter<T> *writer) : myWriter(writer) { }
    virtual ~ModifiedAssetsManager() { delete myWriter; }

    const std::map<std::string, std::set<const T*> >& origins() const { return myOrigins; }

    std::string originOf(const T *asset) const {
        for (auto file = myOrigins.begin(); file != myOrigins.end(); file++) {
            if (file->second.count(asset) > 0) {
                return file->first;
            }
        }
        return std::string();
    }

    void assetDefined(const T *asset, const std::string &filename) {
        for (auto it = myOrigins.begin(); it != myOrigins.end(); it++) {
            it->second.erase(asset);
        }
        myOrigins[filename.c_str()].insert(asset);
    }

    void assetModified(T *asset) { myDirty.insert(asset); }

    void saveModifiedAssets() {
      for (auto a = myDirty.begin(); a != myDirty.end(); a++) {
          for (auto file = myOrigins.begin(); file != myOrigins.end(); file++) {
              if (file->second.count(*a) > 0) {
                  write(file->first, file->second);
              }
          }
      }

      myDirty.clear();
    }

    bool hasPendingChanges() const { return !myDirty.empty(); }

protected:
      void write(const std::string &filename, std::set<const T*> assets) {
          auto buf = stringbuffer_new();
          for (auto asset = assets.begin(); asset != assets.end(); asset++) {
              myWriter->write(*asset, buf);
          }

          size_t length = stringbuffer_length(buf);
          char *data = stringbuffer_finish(buf);

          QFile out(QString(filename.data()));
          out.open(QIODevice::WriteOnly);
          out.write(data, length);
          free(data);
      }

private:
    AssetWriter<T> *myWriter;
    std::map<std::string, std::set<const T*> > myOrigins;
    std::set<T *> myDirty;
};

#endif /* MODIFIEDASSETSMANAGER_H */
