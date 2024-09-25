//
// Created by ubuntu on 9/19/24.
//

#include "localfsmanager.h"

namespace SpatialFHE {

    LocalFSManager::LocalFSManager(const std::string &path) : FSManager(path) {}

    LocalFSManager::~LocalFSManager() = default;

    void LocalFSManager::OpenOutputStream() {
        output_stream.open(this->path);
    }

    std::ofstream &LocalFSManager::GetOutputStream() {
        return output_stream;
    }
    void LocalFSManager::CloseOutputStream() {
        output_stream.close();
    }
    void LocalFSManager::OpenInputStream() {
        input_stream.open(this->path);
    }
    std::ifstream &LocalFSManager::GetInputStream() {
        return input_stream;
    }
    void LocalFSManager::CloseInputStream() {
        input_stream.close();
    }
}  // namespace SpatialFHE