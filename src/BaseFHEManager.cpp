//
// Created by ubuntu on 10/14/24.
//

#include "BaseFHEManager.h"
#include "sealcrypto.h"
SpatialFHE::BaseFHEManager::BaseFHEManager() {
    this->crypto = nullptr;
}

SpatialFHE::BaseFHEManager::BaseFHEManager(
    const std::string &publicKeyPath,
    const std::string &secretKeyPath,
    const std::string &paramsString,
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

SpatialFHE::PlainText SpatialFHE::BaseFHEManager::encode(double d) const {
    return crypto->Encode(d);
}

SpatialFHE::PlainText SpatialFHE::BaseFHEManager::encode(long l) const {
    return crypto->Encode(l);
}

SpatialFHE::PlainText SpatialFHE::BaseFHEManager::encode(std::string const &str) const {
    return crypto->Encode(str);
}

SpatialFHE::PlainText SpatialFHE::BaseFHEManager::encode(std::vector<double> const &vec) const {
    return crypto->Encode(vec);
}

SpatialFHE::CipherText SpatialFHE::BaseFHEManager::encrypt(PlainText const &pt) const {
    return crypto->Encrypt(pt);
}

SpatialFHE::CipherText SpatialFHE::BaseFHEManager::encodeAndEncrypt(double d) const {
    return crypto->Encrypt(encode(d));
}

SpatialFHE::CipherText SpatialFHE::BaseFHEManager::encodeAndEncrypt(long l) const {
    return crypto->Encrypt(encode(l));
}

void SpatialFHE::BaseFHEManager::decode(std::vector<double> &vec, PlainText const &pt) const {
    return crypto->Decode(vec, pt);
}

void SpatialFHE::BaseFHEManager::decode(std::vector<long> &vec, PlainText const &pt) const {
    return crypto->Decode(vec, pt);
}

SpatialFHE::PlainText SpatialFHE::BaseFHEManager::decrypt(CipherText const &ct) const {
    return crypto->Decrypt(ct);
}

void SpatialFHE::BaseFHEManager::decodeAndDecrypt(std::vector<double> &vec, CipherText const &ct) const {
    return crypto->Decode(vec, decrypt(ct));
}

void SpatialFHE::BaseFHEManager::decodeAndDecrypt(std::vector<long> &vec, CipherText const &ct) const {
    return crypto->Decode(vec, decrypt(ct));
}

SpatialFHE::CipherText SpatialFHE::BaseFHEManager::add(CipherText const &ct1, CipherText const &ct2) const {
    return crypto->Add(ct1, ct2);
}