//
// Created by ubuntu on 10/14/24.
//

#ifndef BASEFHEMANAGER_H
#define BASEFHEMANAGER_H
#include "hecrypto.h"

namespace SpatialFHE {
    class BaseFHEManager {
    protected:
        std::shared_ptr<HECrypto> crypto;
        std::string publicKeyPath;

    public:
        // TODO: 后面可以再加上 HELibrary，现在只用 SEAL
        BaseFHEManager();
        BaseFHEManager(std::string publicKeyPath, std::string secretKeyPath, std::string paramsString, bool isInit);
        virtual ~BaseFHEManager() = default;
    };
}

#endif //BASEFHEMANAGER_H
