//
// Created by ubuntu on 1/7/25.
//

#include "VectorFHEManager.h"

namespace SpatialFHE {

    VectorFHEManager::VectorFHEManager() {
        context = new TFHEContext();
    }

    VectorFHEManager::VectorFHEManager(const std::string &publicKeyPath, const std::string &secretKeyPath, const std::string &server_url, bool isInit) {
        std::string public_key_path = publicKeyPath + ".tfhe";
        std::string client_key_path = secretKeyPath + ".tfhe";
        
        // 从 publicKeyPath 中提取目录部分
        std::string dir;
        size_t last_slash = publicKeyPath.find_last_of("/\\");
        if (last_slash != std::string::npos) {
            dir = publicKeyPath.substr(0, last_slash + 1); // 包含斜杠
        }
        
        std::string server_key_path = dir + "server.tfhe";
        
        // 创建 TFHEContext
        if (isInit) {
            context = new TFHEContext(server_url, public_key_path, client_key_path, server_key_path);
        } else {
            context = new TFHEContext(server_url, public_key_path, server_key_path);
        }
    }

    VectorFHEManager::~VectorFHEManager() {
        delete context;
    }
} // SpatialFHE