#include "FSManager.h"
#include "HDFSManager.h"
#include "LocalFSManager.h"

#include <sys/stat.h>

namespace SpatialFHE {
    FSManager::FSManager(const std::string &path) {
        this->path = path;
    }
    std::shared_ptr<FSManager> FSManager::createFSManager(const std::string &path) {
        // 1. parse the path, find if the path start with "hdfs://"
        if (path.find("hdfs://") == 0) {
            return std::make_shared<HDFSManager>(path);
        }

        return std::make_shared<LocalFSManager>(path);
    }

    bool FSManager::createParentDir(const std::string &path) {
        const size_t pos = path.find_last_of('/');
        if (pos != std::string::npos) {
            const std::string dir = path.substr(0, pos);
            if (mkdir(dir.c_str(), 0777) == -1) {
                if (errno != EEXIST) {
                    std::cerr << "Failed to create local directory" << dir << std::endl;
                    return false;
                }
            }
        }
        return true;
    }

} // SpatialFHE