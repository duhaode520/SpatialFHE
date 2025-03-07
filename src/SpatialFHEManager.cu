//
// Created by ubuntu on 10/14/24.
//

#include "SpatialFHEManager.h"

SpatialFHE::CipherMat SpatialFHE::SpatialFHEManager::parallelOp2(
    CipherMat const &a,
    CipherMat const &b,
    std::function<CipherText(CipherText const &, CipherText const &)> func) const {
    validateSameSize(a, b);
    int size = a.getData().size();
    std::vector<CipherText> result(size);
    int numThreads = std::min(size, max_thread_num);

    if (numThreads == 1) {
        for (int i = 0; i < size; i++) {
            result[i] = func(a.getData()[i], b.getData()[i]);
        }
        return {a.getWidth(), a.getHeight(), result};
    }

    std::vector<std::thread> threads(numThreads);
    int chunkSize = size / numThreads;
    for (int i = 0; i < numThreads; i++) {
        threads[i] = std::thread([&, i]() {
            int start = i * chunkSize;
            int end = (i == numThreads - 1) ? size : start + chunkSize;
            for (int j = start; j < end; j++) {
                result[j] = func(a.getData()[j], b.getData()[j]);
            }
        });
    }
    for (auto &t : threads) {
        if (t.joinable())
            t.join();
    }
    return {a.getWidth(), a.getHeight(), result};
}

SpatialFHE::CipherMat SpatialFHE::SpatialFHEManager::parallelOp2(
    CipherMat const &a,
    std::vector<PlainText> const &plainVector,
    std::function<CipherText(CipherText const &, PlainText const &)> func) const {
    if (a.getData().size() != plainVector.size()) {
        throw std::invalid_argument("CipherMat CipherText vector size not match");
    }
    int size = a.getData().size();
    std::vector<CipherText> result(size);
    int numThreads = std::min(size, max_thread_num);

    if (numThreads == 1) {
        for (int i = 0; i < size; i++) {
            result[i] = func(a.getData()[i], plainVector[i]);
        }
        return {a.getWidth(), a.getHeight(), result};
    }

    std::vector<std::thread> threads(numThreads);
    int chunkSize = size / numThreads;
    for (int i = 0; i < numThreads; i++) {
        threads[i] = std::thread([&, i]() {
            int start = i * chunkSize;
            int end = (i == numThreads - 1) ? size : start + chunkSize;
            for (int j = start; j < end; j++) {
                result[j] = func(a.getData()[j], plainVector[j]);
            }
        });
    }
    for (auto &t : threads) {
        if (t.joinable())
            t.join();
    }
    return {a.getWidth(), a.getHeight(), result};
}

SpatialFHE::SpatialFHEManager::SpatialFHEManager() {
    max_thread_num = 8;
}

SpatialFHE::SpatialFHEManager::SpatialFHEManager(
    const std::string &publicKeyPath,
    const std::string &secretKeyPath,
    const std::string &paramsString,
    const std::string &serverURL,
    const HECrypto::HELibrary heLibrary,
    bool isInit,
    int max_thread_num = 8) :
        BaseFHEManager(publicKeyPath, secretKeyPath, paramsString, heLibrary, isInit),
        VectorFHEManager(publicKeyPath, secretKeyPath, serverURL, isInit) {
    this->max_thread_num = max_thread_num;
}

SpatialFHE::SpatialFHEManager::~SpatialFHEManager() = default;

SpatialFHE::CipherMat SpatialFHE::SpatialFHEManager::encryptMat(
    int width,
    int height,
    const std::vector<double> &data) {
    std::vector<PlainText> plain_vec = crypto->EncodeMany(data);
    std::vector<CipherText> cipher_vec;
    if (heLib == HECrypto::HELibrary::Phantom) {
        for (auto &plain : plain_vec) {
            cipher_vec.emplace_back(crypto->Encrypt(plain));
        }
    } else {
        cipher_vec.resize(plain_vec.size());
        int size = plain_vec.size();
        int numThreads = std::min(size, (int)std::thread::hardware_concurrency());

        std::vector<std::thread> threads(numThreads);
        int chunkSize = size / numThreads;

        for (int i = 0; i < numThreads; i++) {
            threads[i] = std::thread([&, i]() {
                int start = i * chunkSize;
                int end = (i == numThreads - 1) ? size : start + chunkSize;
                for (int j = start; j < end; j++) {
                    cipher_vec[j] = crypto->Encrypt(plain_vec[j]);
                }
            });
        }

        for (auto &t : threads) {
            if (t.joinable())
                t.join();
        }
    }

    return {width, height, cipher_vec};
}

std::vector<double> SpatialFHE::SpatialFHEManager::decryptMat(CipherMat &cipher_mat) {
    int width = cipher_mat.getWidth();
    int height = cipher_mat.getHeight();
    int size = cipher_mat.getData().size();

    std::vector<PlainText> plain_vec;
    if (heLib == HECrypto::HELibrary::Phantom) {
        for (auto &cipher : cipher_mat.getData()) {
            plain_vec.emplace_back(crypto->Decrypt(cipher));
        }
    } else {
        plain_vec.resize(size);
        int numThreads = std::min(size, (int)std::thread::hardware_concurrency());
        std::vector<std::thread> threads(numThreads);
        int chunkSize = size / numThreads;

        // 并行解密
        for (int i = 0; i < numThreads; i++) {
            threads[i] = std::thread([&, i]() {
                int start = i * chunkSize;
                int end = (i == numThreads - 1) ? size : start + chunkSize;
                for (int j = start; j < end; j++) {
                    plain_vec[j] = crypto->Decrypt(cipher_mat.getData()[j]);
                }
            });
        }

        for (auto &t : threads) {
            if (t.joinable())
                t.join();
        }
    }

    // 解码后的结果
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
    return parallelOp2(a, b, [this](CipherText const &x, CipherText const &y) { return crypto->Add(x, y); });
}

SpatialFHE::CipherMat SpatialFHE::SpatialFHEManager::addMatPlain(CipherMat const &a, const std::vector<double> &plain) {
    std::vector<PlainText> plain_vec = crypto->EncodeMany(plain);
    return parallelOp2(
        a, plain_vec, [this](CipherText const &x, PlainText const &y) { return crypto->AddPlain(x, y); });
}

SpatialFHE::CipherMat SpatialFHE::SpatialFHEManager::subMat(CipherMat const &a, CipherMat const &b) {
    return parallelOp2(a, b, [this](CipherText const &x, CipherText const &y) { return crypto->Subtract(x, y); });
}

SpatialFHE::CipherMat SpatialFHE::SpatialFHEManager::multiplyMat(CipherMat const &a, CipherMat const &b) {
    return parallelOp2(a, b, [this](CipherText const &x, CipherText const &y) { return crypto->Multiply(x, y); });
}

SpatialFHE::CipherMat SpatialFHE::SpatialFHEManager::multiplyMatPlain(
    CipherMat const &a,
    const std::vector<double> &plain) {
    std::vector<PlainText> plain_vec = crypto->EncodeMany(plain);
    return parallelOp2(
        a, plain_vec, [this](CipherText const &x, PlainText const &y) { return crypto->MultiplyPlain(x, y); });
}

void SpatialFHE::SpatialFHEManager::validateSameSize(SpatialFHE::CipherMat const &a, SpatialFHE::CipherMat const &b) {
    if (a.getWidth() != b.getWidth() || a.getHeight() != b.getHeight()) {
        throw std::invalid_argument("CipherMat size not match");
    }
    if (a.getData().size() != b.getData().size()) {
        throw std::invalid_argument("CipherMat CipherText vector size not match");
    }
}
