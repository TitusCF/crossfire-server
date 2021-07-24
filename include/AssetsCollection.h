/*
 * Crossfire -- cooperative multi-player graphical RPG and adventure game
 *
 * Copyright (c) 2020 the Crossfire Development Team
 *
 * Crossfire is free software and comes with ABSOLUTELY NO WARRANTY. You are
 * welcome to redistribute it under certain conditions. For details, please
 * see COPYING and LICENSE.
 *
 * The authors can be reached via e-mail at <crossfire@metalforge.org>.
 */

#ifndef ASSETSCOLLECTION_H
#define ASSETSCOLLECTION_H

#include <string>
#include <map>
#include <set>
#include <algorithm>
#include <functional>

/**
 * Collection of assets identified by a unique name.
 * An asset is whetever is wanted, no assumption are done on the class.
 *
 * A pointer to an asset returned by this class is deleted only when the NamedAssets instance is destroyed.
 * The contents of the asset itself may change though.
 *
 * If an asset is searched for and created on the fly, it is marked as "undefined" until actually defined
 * through a call to define().
 *
 * Assets are owned by this instance, and deleted when clear() is called.
 * Because of how C++ destructors work, clear() unfortunately can't be called automatically
 * (virtual function).
 *
 * Actual implementations must provide some pure virtual functions.
 */
template<class T, class Key = std::string>
class AssetsCollection {

    public:

        /**
         * Destroy all assets.
         */
        void clear() {
            for (auto asset : m_assets) {
                destroy(asset.second);
            }
            m_assets.empty();
        }

        /**
         * Get the number of assets.
         * @return number.
         */
        size_t count() const {
            return m_assets.size();
        }

        /**
         * Get a named asset.
         * @param name asset name.
         * @return asset pointer guaranteed to live as long as this instance, never nullptr.
         */
        T *get(const Key& name) {
            auto asset = m_assets.find(name);
            if (asset != m_assets.end()) {
                return asset->second;
            }

            T *add = create(name);
            m_assets[name] = add;
            m_undefined.insert(name);
            added(add);
            return add;
        }

        /**
         * Get a named asset if it exists.
         * @param name asset name.
         * @return asset pointer guaranteed to live as long as this instance if the asset
         * exists, nullptr else.
         */
        T *find(const Key& name) {
            auto asset = m_assets.find(name);
            return asset != m_assets.end() ? asset->second : nullptr;
        }

        /**
         * Define an asset, erasing an existing one.
         * @param name unique asset name.
         * @param asset new version of the asset. This pointer may become invalid
         * and must not be used after the call.
         * @return asset with this name, either asset or the previous existing asset.
         */
        T *define(const Key& name, T *asset) {
            m_undefined.erase(name);
            auto existing = m_assets.find(name);
            if (existing != m_assets.end()) {
                replace(existing->second, asset);
                return existing->second;
            }

            m_assets[name] = asset;
            added(asset);
            return asset;
        }

        /**
         * Allow browsing assets in a list-like manner.
         * @note this function is not efficient, and should not be used in C++ code.
         * @param current asset to get the next of, nullptr to get the first asset.
         * @return next asset, nullptr if no assets is defined or 'current' was the last one.
         */
        T *next(T *current) {
            if (m_assets.empty()) {
                return nullptr;
            }
            if (current == nullptr) {
                return m_assets.begin()->second;
            }

            auto c = std::find_if(m_assets.begin(), m_assets.end(), [&current](auto& item) { return item.second == current; });
            if (c == m_assets.end() || (++c) == m_assets.end()) {
                return nullptr;
            }
            return c->second;
        }

        /**
         * Apply a function to each asset.
         * @param op function to apply.
         */
        void each(std::function<void(T *)> op) {
            std::for_each(m_assets.begin(), m_assets.end(), [&op] (auto item) { op(item.second); });
        }

        /**
         * Find the first asset matching some condition.
         * @param op function which must return true if the asset matches, false else.
         * @return nullptr if no asset matches, else the first matching asset.
         */
        T *first(std::function<bool(const T *)> op) {
            auto found = std::find_if(m_assets.begin(), m_assets.end(), [&op] (auto item) { return op(item.second); });
            return found == m_assets.end() ? nullptr : found->second;
        }

        /**
         * Return the list of undefined assets, that is assets requested through
         * get() but not defined through define().
         * @return list of undefined names.
         */
        const std::set<Key>& undefined() const { return m_undefined; }

    protected:
        std::map<Key, T*> m_assets; /**< Known assets. */
        std::set<Key> m_undefined;  /**< List of undefined assets. */

        /**
         * Create a new asset.
         * @param name asset unique name.
         * @return asset.
         */
        virtual T *create(const Key& name) = 0;
        /**
         * Destroy an asset.
         * @param item asset to destroy.
         */
        virtual void destroy(T *item) = 0;
        /**
         * Replace an asset by an updated version.
         * @param existing asset to be updated.
         * @param update new version of the asset, which must be destroyed.
         */
        virtual void replace(T *existing, T *update) = 0;

        /**
         * An asset was either referenced (but undefined) or defined.
         */
        virtual void added(T *) { }
};

#endif /* ASSETSCOLLECTION_H */
