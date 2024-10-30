//
// Created by ubuntu on 10/14/24.
//

#include "SpatialFHEManager.h"

SpatialFHE::SpatialFHEManager::SpatialFHEManager() = default;
SpatialFHE::SpatialFHEManager::SpatialFHEManager(
    const std::string& publicKeyPath,
    const std::string& secretKeyPath,
    const std::string& paramsString,
    bool isInit) : BaseFHEManager(publicKeyPath, secretKeyPath, paramsString, isInit) {

    }
SpatialFHE::SpatialFHEManager::~SpatialFHEManager() = default;

SpatialFHE::CipherMat SpatialFHE::SpatialFHEManager::encryptMat(int width, int height, const std::vector<double> &data){
    std::vector<PlainText> plain_vec = crypto->EncodeMany(data);
    std::vector<CipherText> cipher_vec;
    cipher_vec.reserve(plain_vec.size());
    for(auto &plain : plain_vec){
        cipher_vec.emplace_back(crypto->Encrypt(plain));
    }
    return {width, height, cipher_vec};
}

std::vector<double> SpatialFHE::SpatialFHEManager::decryptMat(CipherMat &cipher_mat) {
    int width = cipher_mat.getWidth();
    int height = cipher_mat.getHeight();
    std::vector<PlainText> plain_vec;
    plain_vec.reserve(cipher_mat.getData().size());
    for(auto &cipher : cipher_mat.getData()){
        plain_vec.emplace_back(crypto->Decrypt(cipher));
    }
    // Decode each plaintext and store in a vector
    std::vector<double> result;
    for (auto &plain : plain_vec) {
        std::vector<double> tmp_vector;
        crypto->Decode(tmp_vector, plain);
        result.insert(result.end(), tmp_vector.begin(), tmp_vector.end());
    }

    // cut the result to width * height
    return std::vector<double>(result.begin(), result.begin() + width * height);
}

SpatialFHE::CipherMat SpatialFHE::SpatialFHEManager::addMat(CipherMat const &a, CipherMat const &b) {
    validateSameSize(a, b);
    int size = a.getData().size();
    std::vector<CipherText> result;
    result.reserve(size);
    for (int i = 0; i < size; i++) {
        result.emplace_back(crypto->Add(a.getData()[i], b.getData()[i]));
    }
    return {a.getWidth(), a.getHeight(), result};
}
SpatialFHE::CipherMat SpatialFHE::SpatialFHEManager::addMatPlain(CipherMat const &a, const std::vector<double> &plain) {
    std::vector<PlainText> plain_vec = crypto->EncodeMany(plain);
    int size = a.getData().size();
    std::vector<CipherText> result;
    result.reserve(size);
    for (int i = 0; i < size; i++) {
        result.emplace_back(crypto->AddPlain(a.getData()[i], plain_vec[i]));
    }
    return {a.getWidth(), a.getHeight(), result};
}


SpatialFHE::CipherMat SpatialFHE::SpatialFHEManager::subMat(CipherMat const &a, CipherMat const &b) {
    validateSameSize(a, b);
    int size = a.getData().size();
    std::vector<CipherText> result;
    result.reserve(size);
    for (int i = 0; i < size; i++) {
        result.emplace_back(crypto->Subtract(a.getData()[i], b.getData()[i]));
    }
    return {a.getWidth(), a.getHeight(), result};
}

SpatialFHE::CipherMat SpatialFHE::SpatialFHEManager::multiplyMat(CipherMat const &a, CipherMat const &b) {
    validateSameSize(a, b);
    int size = a.getData().size();
    std::vector<CipherText> result;
    result.reserve(size);
    for (int i = 0; i < size; i++) {
        result.emplace_back(crypto->Multiply(a.getData()[i], b.getData()[i]));
    }
    return {a.getWidth(), a.getHeight(), result};
}

SpatialFHE::CipherMat SpatialFHE::SpatialFHEManager::multiplyMatPlain(
    CipherMat const &a, const std::vector<double> &plain) {
    std::vector<PlainText> plain_vec = crypto->EncodeMany(plain);
    int size = a.getData().size();
    std::vector<CipherText> result;
    result.reserve(size);
    for (int i = 0; i < size; i++) {
        result.emplace_back(crypto->MultiplyPlain(a.getData()[i], plain_vec[i]));
    }
    return {a.getWidth(), a.getHeight(), result};
}

void SpatialFHE::SpatialFHEManager::validateSameSize(SpatialFHE::CipherMat const &a, SpatialFHE::CipherMat const &b) {
    if (a.getWidth() != b.getWidth() || a.getHeight() != b.getHeight()) {
        throw std::invalid_argument("CipherMat size not match");
    }
    if (a.getData().size() != b.getData().size()) {
        throw std::invalid_argument("CipherMat CipherText vector size not match");
    }
}
