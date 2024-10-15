//
// Created by ubuntu on 10/14/24.
//

#include "BaseFHEManager.h"
#include "sealcrypto.h"
SpatialFHE::BaseFHEManager::BaseFHEManager() {
    this->crypto = nullptr;
}
SpatialFHE::BaseFHEManager::BaseFHEManager(
    std::string publicKeyPath,
    std::string secretKeyPath,
    std::string paramsString,
    bool isInit) {

    this->crypto = std::make_shared<SEALCrypto>(paramsString);
    if (isInit) {
        crypto->GenerateKeyPair(publicKeyPath, secretKeyPath);
    } else {
        if (secretKeyPath.empty()) {
            crypto->LoadPublicKey(publicKeyPath);
        } else {
            crypto->LoadKeyPair(publicKeyPath, secretKeyPath);
        }
    }
}