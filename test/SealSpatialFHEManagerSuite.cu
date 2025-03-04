//
// Created by ubuntu on 10/14/24.
//
#include <gtest/gtest.h>
#include <seal/ciphertext.h>
#include <vector>
#include "HECrypto.h"
#include "SpatialFHEManager.h"

#define private public
#undef private

using namespace SpatialFHE;

class SpatialFHEManagerSuite : public ::testing::Test {
public:
    static SpatialFHEManager manager;
    static std::string json;
    constexpr static double ERROR = 1e-4;  // scale factor is 30, so the error is 1e-4 ~ 2^-10
    static std::string PUBKEY_PATH;
    static std::string SECKEY_PATH;
    static void SetUpTestCase() {
        json =
            "{\"CoeffModulusBits\":[60,30,30,30,60],\"SchemeType\":\"CKKS\",\"PolyModulusDegree\":8192,"
            "\"PlaintextModulus\":0,\"CoeffModulusPrimes\":[0],\"ScaleFactor\":30}";
        PUBKEY_PATH = "public.key";
        SECKEY_PATH = "secret.key";
        manager = SpatialFHEManager(PUBKEY_PATH, SECKEY_PATH, json, "",
            HECrypto::HELibrary::SEAL, true,  1);
    }

    static void TearDownTestCase() {}
};

std::string SpatialFHEManagerSuite::json, SpatialFHEManagerSuite::PUBKEY_PATH, SpatialFHEManagerSuite::SECKEY_PATH;
SpatialFHEManager SpatialFHEManagerSuite::manager;

TEST_F(SpatialFHEManagerSuite, EncryptDecryptMat) {
    std::vector<double> data = {0, 0, 0, 0, 0, 0, 0, 0};
    int width = 2, height = 4;
    CipherMat cipher_mat = manager.encryptMat(width, height, data);
    std::vector<double> decrypted = manager.decryptMat(cipher_mat);
    ASSERT_EQ(data.size(), decrypted.size());
    for (int i = 0; i < data.size(); ++i) {
        ASSERT_NEAR(data[i], decrypted[i], ERROR);
    }
    data = {1, 2, 3, 4, 5, 6, 7, 8};
    cipher_mat = manager.encryptMat(width, height, data);
    decrypted = manager.decryptMat(cipher_mat);
    ASSERT_EQ(data.size(), decrypted.size());
    for (int i = 0; i < data.size(); ++i) {
        ASSERT_NEAR(data[i], decrypted[i], ERROR);
    }
}

TEST_F(SpatialFHEManagerSuite, EncDecBigMat) {
    std::vector<double> data;
    int width = 1024, height = 1024;
    data.reserve(width * height);
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            data.emplace_back(i*height + j);
        }
    }
    CipherMat cipher_mat = manager.encryptMat(width, height, data);
    std::vector<double> decrypted = manager.decryptMat(cipher_mat);
    ASSERT_EQ(data.size(), decrypted.size());
    for (int i = 0; i < data.size(); ++i) {
        ASSERT_NEAR(data[i], decrypted[i], ERROR);
    }
}

TEST_F(SpatialFHEManagerSuite, AddMat) {
    std::vector<double> data1 = {1,2,3,4,5,6};
    std::vector<double> data2 = {11,22,33,44,55,66};
    std::vector<double> expected;
    int width = 3, height = 2;
    expected.reserve(data1.size());
    for (int i = 0; i < data1.size(); ++i) {
        expected.emplace_back(data1[i] + data2[i]);
    }

    CipherMat mat1 = manager.encryptMat(width, height, data1);
    CipherMat mat2 = manager.encryptMat(width, height, data2);

    CipherMat result = manager.addMat(mat1, mat2);
    std::vector<double> decrypted = manager.decryptMat(result);
    ASSERT_EQ(expected.size(), decrypted.size());
    for (int i = 0; i < expected.size(); ++i) {
        ASSERT_NEAR(expected[i], decrypted[i], ERROR);
    }
}

TEST_F(SpatialFHEManagerSuite, AddBigMat) {
    int width = 100, height = 200;
    int n = width * height;
    std::vector<double> data1;
    std::vector<double> data2;
    for (int i = 0; i < n; ++i) {
        data1.emplace_back(i+1);
        data2.emplace_back(10*(i+1));
    }
    std::vector<double> expected;
    expected.reserve(n);
    for (int i = 0; i < n; ++i) {
        expected.emplace_back(data1[i] + data2[i]);
    }

    CipherMat mat1 = manager.encryptMat(width, height, data1);
    CipherMat mat2 = manager.encryptMat(width, height, data2);

    CipherMat result = manager.addMat(mat1, mat2);
    std::vector<double> decrypted = manager.decryptMat(result);
    ASSERT_EQ(expected.size(), decrypted.size());
    for (int i = 0; i < expected.size(); ++i) {
        ASSERT_NEAR(expected[i], decrypted[i], ERROR);
    }
}

TEST_F(SpatialFHEManagerSuite, AddBigMatPlain) {
    int width = 100, height = 200;
    int n = width * height;
    std::vector<double> data1;
    std::vector<double> data2;
    for (int i = 0; i < n; ++i) {
        data1.emplace_back(i+1);
        data2.emplace_back(10*(i+ 1));
    }
    std::vector<double> expected;
    expected.reserve(n);
    for (int i = 0; i < n; ++i) {
        expected.emplace_back(data1[i] + data2[i]);
    }

    CipherMat mat1 = manager.encryptMat(width, height, data1);

    CipherMat result = manager.addMatPlain(mat1, data2);
    std::vector<double> decrypted = manager.decryptMat(result);
    ASSERT_EQ(expected.size(), decrypted.size());
    for (int i = 0; i < expected.size(); ++i) {
        ASSERT_NEAR(expected[i], decrypted[i], ERROR);
    }
}


TEST_F(SpatialFHEManagerSuite, MultiplyBigMat) {
    int width = 100, height = 200;
    int n = width * height;
    std::vector<double> data1;
    std::vector<double> data2;
    for (int i = 0; i < n; ++i) {
        data1.emplace_back(i+1);
        data2.emplace_back(10*(i+1));
    }
    std::vector<double> expected;
    expected.reserve(n);
    for (int i = 0; i < n; ++i) {
        expected.emplace_back(data1[i] * data2[i]);
    }

    CipherMat mat1 = manager.encryptMat(width, height, data1);
    CipherMat mat2 = manager.encryptMat(width, height, data2);

    CipherMat result = manager.multiplyMat(mat1, mat2);
    std::vector<double> decrypted = manager.decryptMat(result);
    ASSERT_EQ(expected.size(), decrypted.size());
    for (int i = 0; i < expected.size(); ++i) {
        ASSERT_NEAR(expected[i], decrypted[i], ERROR);
    }
}
