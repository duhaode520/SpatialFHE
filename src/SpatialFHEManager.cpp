//
// Created by ubuntu on 10/14/24.
//

#include "SpatialFHEManager.h"

#include <utility>
SpatialFHE::SpatialFHEManager::SpatialFHEManager() = default;
SpatialFHE::SpatialFHEManager::SpatialFHEManager(
    std::string publicKeyPath,
    std::string secretKeyPath,
    std::string paramsString,
    bool isInit) : BaseFHEManager(std::move(publicKeyPath), std::move(secretKeyPath), std::move(paramsString), isInit) {

    }
SpatialFHE::SpatialFHEManager::~SpatialFHEManager() = default;

SpatialFHE::CipherMat SpatialFHE::SpatialFHEManager::encryptMat(int width, int height, const std::vector<double> &data){
    std::vector<PlainText> plain_vec = crypto->EncodeMany(data);
    std::vector<CipherText> cipher_vec;
    cipher_vec.reserve(plain_vec.size());
    for(auto &plain : plain_vec){
        cipher_vec.emplace_back(crypto->Encrypt(plain));
    }
    return CipherMat(width, height, cipher_vec);
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

SpatialFHE::CipherMat SpatialFHE::SpatialFHEManager::addMat(CipherMat &a, CipherMat &b) {
    if (a.getWidth() != b.getWidth() || a.getHeight() != b.getHeight()) {
        throw std::invalid_argument("CipherMat size not match");
    }
    if (a.getData().size() != b.getData().size()) {
        throw std::invalid_argument("CipherMat CipherText vector size not match");
    }
    int size = a.getData().size();
    std::vector<CipherText> result;
    result.reserve(size);
    for (int i = 0; i < size; i++) {
        result.emplace_back(crypto->Add(a.getData()[i], b.getData()[i]));
    }
    return CipherMat(a.getWidth(), a.getHeight(), result);
}