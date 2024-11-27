//
// Created by ubuntu on 11/26/24.
//
#include <gtest/gtest.h>
#include <phantom.h>
#include <vector>
#include "CipherText.h"
#include "HECrypto.h"

#define private public
#include "PhantomCrypto.cuh"
#undef private

using namespace SpatialFHE;

class PhantomCryptoCKKSSuite : public testing::Test {
protected:
    static PhantomCrypto* crypto;
    static std::string json;
    constexpr static const double ERROR = 1e-4;  // scale factor is 30, so the error is 1e-4 ~ 2^-10
    static std::string PUBKEY_PATH;
    static std::string SECKEY_PATH;
    static void SetUpTestSuite() {
        cudaSetDevice(2);
        json = R"(
            {
                    "CoeffModulusBits": [60, 40, 40, 40, 40, 40, 40, 40, 60],
                    "SchemeType": "CKKS",
                    "PolyModulusDegree": 16384,
                    "PlaintextModulus": 0,
                    "CoeffModulusPrimes": [0],
                    "ScaleFactor": 40
            })";

        crypto = new PhantomCrypto(json);
        PUBKEY_PATH = "public.key";
        SECKEY_PATH = "secret.key";
        crypto->GenerateKeyPair(PUBKEY_PATH, SECKEY_PATH);
    }

    static void TearDownTestSuite() {
        delete crypto;
    }
};

// CUDA 对象不能成为全局静态变量，析构时会有 cudaErrorCudartUnloading 的问题，要手动控制生命周期
PhantomCrypto* PhantomCryptoCKKSSuite::crypto;
std::string PhantomCryptoCKKSSuite::json, PhantomCryptoCKKSSuite::PUBKEY_PATH, PhantomCryptoCKKSSuite::SECKEY_PATH;

TEST_F(PhantomCryptoCKKSSuite, EncodeDecode) {
    PlainText pt = crypto->Encode(1.0);
    std::vector<double> result;
    crypto->Decode(result, pt);
    ASSERT_EQ(1.0, result[0]);
}

TEST_F(PhantomCryptoCKKSSuite, BatchEncodeDecode) {
    std::vector<double> vec = {1.0, 2.0, 3.0};
    std::vector<PlainText> pts = crypto->EncodeMany(vec);
    std::vector<double> result;
    if (pts.size() == 1) {
        crypto->Decode(result, pts[0]);
        ASSERT_NEAR(1.0, result[0], ERROR);
        ASSERT_NEAR(2.0, result[1], ERROR);
        ASSERT_NEAR(3.0, result[2], ERROR);
    }
}

TEST_F(PhantomCryptoCKKSSuite, BigBatchEncodeDecode) {
    std::vector<double> vec(8192, 1.0);
    for (int i = 0; i < 8192; i++) {
        vec[i] = i;
    }
    std::vector<PlainText> pts = crypto->EncodeMany(vec);
    ASSERT_EQ(pts.size(), 1);
    for (int i = 0; i < pts.size(); i++) {
        std::vector<double> result;
        crypto->Decode(result, pts[i]);
        for (int j = 0; j < 8192; j++) {
            ASSERT_NEAR(result[j], vec[j + i * 8192], ERROR);
        }
    }
}

TEST_F(PhantomCryptoCKKSSuite, EncryptDecrypt) {
    PlainText pt = crypto->Encode(1.0);
    CipherText ct = crypto->Encrypt(pt);
    PlainText res_pt = crypto->Decrypt(ct);
    std::vector<double> result;
    crypto->Decode(result, res_pt);
    ASSERT_NEAR(1.0, result[0], ERROR);
}

TEST_F(PhantomCryptoCKKSSuite, Add) {
    PlainText pt1 = crypto->Encode(1.0);
    PlainText pt2 = crypto->Encode(2.0);
    CipherText ct1 = crypto->Encrypt(pt1);
    CipherText ct2 = crypto->Encrypt(pt2);
    CipherText ct3 = crypto->Add(ct1, ct2);
    PlainText res_pt = crypto->Decrypt(ct3);
    std::vector<double> result;
    crypto->Decode(result, res_pt);
    ASSERT_NEAR(3.0, result[0], ERROR);
}

TEST_F(PhantomCryptoCKKSSuite, ReloadAndAdd) {
    PhantomCrypto crypto2 = PhantomCrypto(json);
    crypto2.LoadKeyPair(PUBKEY_PATH, SECKEY_PATH);
    PlainText pt1 = crypto2.Encode(1.0);
    PlainText pt2 = crypto2.Encode(2.0);
    CipherText ct1 = crypto2.Encrypt(pt1);
    CipherText ct2 = crypto2.Encrypt(pt2);
    CipherText ct3 = crypto2.Add(ct1, ct2);
    PlainText res_pt = crypto2.Decrypt(ct3);
    std::vector<double> result;
    crypto2.Decode(result, res_pt);
    ASSERT_NEAR(3.0, result[0], ERROR);
}

TEST_F(PhantomCryptoCKKSSuite, AddInside) {
    PlainText pt1 = crypto->Encode(1.0);
    PlainText pt2 = crypto->Encode(2.0);
    CipherText ct1 = crypto->Encrypt(pt1);
    CipherText ct2 = crypto->Encrypt(pt2);
    PhantomCiphertext ct_1 = std::get<PhantomCiphertext>(ct1.getData());
    PhantomCiphertext ct_2 = std::get<PhantomCiphertext>(ct2.getData());
    PhantomCiphertext ct = PhantomCiphertext();
    crypto->_add(ct, ct_1, ct_2);
    std::cout << "ct3 scale: " << ct.scale() << std::endl;
    CipherText ct3 = CipherText(ct);

    PlainText res_pt = crypto->Decrypt(ct3);
    std::vector<double> result;
    crypto->Decode(result, res_pt);
    ASSERT_NEAR(3.0, result[0], ERROR);
}

TEST_F(PhantomCryptoCKKSSuite, AddPlain) {
    PlainText pt1 = crypto->Encode(1.0);
    PlainText pt2 = crypto->Encode(2.0);
    CipherText ct1 = crypto->Encrypt(pt1);
    CipherText ct3 = crypto->AddPlain(ct1, pt2);
    PlainText res_pt = crypto->Decrypt(ct3);
    std::vector<double> result;
    crypto->Decode(result, res_pt);
    ASSERT_NEAR(3.0, result[0], ERROR);
}

TEST_F(PhantomCryptoCKKSSuite, Sub) {
    PlainText pt1 = crypto->Encode(1.0);
    PlainText pt2 = crypto->Encode(2.0);
    CipherText ct1 = crypto->Encrypt(pt1);
    CipherText ct2 = crypto->Encrypt(pt2);
    CipherText ct3 = crypto->Subtract(ct1, ct2);
    PlainText res_pt = crypto->Decrypt(ct3);
    std::vector<double> result;
    crypto->Decode(result, res_pt);
    ASSERT_NEAR(-1.0, result[0], ERROR);
}

TEST_F(PhantomCryptoCKKSSuite, Multiply) {
    PlainText pt1 = crypto->Encode(1.0);
    PlainText pt2 = crypto->Encode(2.0);
    CipherText ct1 = crypto->Encrypt(pt1);
    CipherText ct2 = crypto->Encrypt(pt2);
    CipherText ct3 = crypto->Multiply(ct1, ct2);
    PlainText res_pt = crypto->Decrypt(ct3);
    std::vector<double> result;
    crypto->Decode(result, res_pt);
    ASSERT_NEAR(2.0, result[0], ERROR);
}

TEST_F(PhantomCryptoCKKSSuite, MultiplyPlain) {
    PlainText pt1 = crypto->Encode(1.0);
    PlainText pt2 = crypto->Encode(2.0);
    CipherText ct1 = crypto->Encrypt(pt1);
    CipherText ct3 = crypto->MultiplyPlain(ct1, pt2);
    PlainText res_pt = crypto->Decrypt(ct3);
    std::vector<double> result;
    crypto->Decode(result, res_pt);
    ASSERT_NEAR(2.0, result[0], ERROR);
}
