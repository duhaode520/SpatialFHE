//
// Created by ubuntu on 9/19/24.
//

#include "localfsmanager.h"

namespace SpatialFHE {

    LocalFSManager::LocalFSManager(const std::string &path) : FSManager(path) {}

    LocalFSManager::~LocalFSManager() {
        if (input_stream.is_open()) {
            input_stream.close();
        }
        if (output_stream.is_open()) {
            output_stream.close();
        }
    };

    void LocalFSManager::OpenOutputStream() {
        if (!createParentDir(this->path)) {
            return;
        }
        output_stream.open(this->path);
    }

    std::ofstream &LocalFSManager::GetOutputStream() {
        return output_stream;
    }
    void LocalFSManager::CloseOutputStream() {
        output_stream.close();
    }
    void LocalFSManager::OpenInputStream() {
        if (!createParentDir(this->path)) {
            return;
        }
        input_stream.open(this->path);
    }
    std::ifstream &LocalFSManager::GetInputStream() {
        return input_stream;
    }
    void LocalFSManager::CloseInputStream() {
        input_stream.close();
    }
}  // namespace SpatialFHE