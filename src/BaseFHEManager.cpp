//
// Created by ubuntu on 10/14/24.
//

#include "BaseFHEManager.h"
#include "sealcrypto.h"

using namespace  SpatialFHE;

BaseFHEManager::BaseFHEManager() {
    this->crypto = nullptr;
}

BaseFHEManager::BaseFHEManager(
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

PlainText BaseFHEManager::encode(double d) const {
    return crypto->Encode(d);
}

PlainText BaseFHEManager::encode(long l) const {
    return crypto->Encode(l);
}

PlainText BaseFHEManager::encode(std::string const &str) const {
    return crypto->Encode(str);
}

PlainText BaseFHEManager::encode(std::vector<double> const &vec) const {
    return crypto->Encode(vec);
}

CipherText BaseFHEManager::encrypt(PlainText const &pt) const {
    return crypto->Encrypt(pt);
}

CipherText BaseFHEManager::encodeAndEncrypt(double d) const {
    return crypto->Encrypt(encode(d));
}

CipherText BaseFHEManager::encodeAndEncrypt(long l) const {
    return crypto->Encrypt(encode(l));
}

void BaseFHEManager::decode(std::vector<double> &vec, PlainText const &pt) const {
    return crypto->Decode(vec, pt);
}

void BaseFHEManager::decode(std::vector<long> &vec, PlainText const &pt) const {
    return crypto->Decode(vec, pt);
}

PlainText BaseFHEManager::decrypt(CipherText const &ct) const {
    return crypto->Decrypt(ct);
}

void BaseFHEManager::decodeAndDecrypt(std::vector<double> &vec, CipherText const &ct) const {
    return crypto->Decode(vec, decrypt(ct));
}

void BaseFHEManager::decodeAndDecrypt(std::vector<long> &vec, CipherText const &ct) const {
    return crypto->Decode(vec, decrypt(ct));
}

CipherText BaseFHEManager::add(CipherText const &ct1, CipherText const &ct2) const {
    return crypto->Add(ct1, ct2);
}
CipherText BaseFHEManager::addPlain(CipherText const &ct, PlainText const &pt) const {
    return crypto->AddPlain(ct, pt);
}

CipherText BaseFHEManager::sub(CipherText const &ct1, CipherText const &ct2) const {
    return crypto->Subtract(ct1, ct2);
}

CipherText BaseFHEManager::multiply(CipherText const &ct1, CipherText const &ct2) const {
    return crypto->Multiply(ct1, ct2);
}

CipherText BaseFHEManager::multiplyPlain(CipherText const &ct, PlainText const &pt) const {
    return crypto->MultiplyPlain(ct, pt);
}

CipherText BaseFHEManager::square(CipherText const &ct) const {
    return crypto->Square(ct);
}

CipherText BaseFHEManager::buildCipherText(std::string const &str) const {
    return crypto->buildCipherText(str);
}