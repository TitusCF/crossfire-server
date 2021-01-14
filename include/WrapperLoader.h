#ifndef WRAPPERLOADER_H
#define WRAPPERLOADER_H

#include "AssetLoader.h"
#include "Utils.h"
#include <functional>
#include <string>

/**
 * Loader calling a function for files matching a specific name.
 */
class WrapperLoader : public AssetLoader {
public:
    /** Wrapper to the function to call for each file. */
    typedef std::function<void(FILE *, const char *)> wrapped;

    /**
     * Standard constructor.
     * @param name filename to match, should probably start with a '/'.
     * @param fct function to call for matching files.
     */
    WrapperLoader(const std::string& name, wrapped fct) : m_name(name), m_function(fct) {
    };

    virtual bool willProcess(const std::string &filename) {
        return Utils::endsWith(filename, m_name);
    }

    virtual void processFile(FILE *file, const std::string &filename) {
        m_function(file, filename.c_str());
    }

private:
    std::string m_name; /**< File name. */
    wrapped m_function; /**< Function to call. */
};

#endif /* WRAPPERLOADER_H */

