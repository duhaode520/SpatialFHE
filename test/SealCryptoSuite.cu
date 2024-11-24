#include <gtest/gtest.h>
#include <seal/ciphertext.h>
#include <vector>
#include "CipherText.h"
#include "HECrypto.h"

#define private public
#include "SealCrypto.h"
#undef private

using namespace SpatialFHE;
class SealCryptoCKKSSuite : public ::testing::Test {
protected:
    static SEALCrypto crypto;
    static std::string json;
    constexpr static const double ERROR = 1e-4;  // scale factor is 30, so the error is 1e-4 ~ 2^-10
    static std::string PUBKEY_PATH;
    static std::string SECKEY_PATH;
    static void SetUpTestSuite() {
        json = "{\"CoeffModulusBits\":[60,30,30,30,60],\"SchemeType\":\"CKKS\",\"PolyModulusDegree\":8192,"
            "\"PlaintextModulus\":0,\"CoeffModulusPrimes\":[0],\"ScaleFactor\":30}";
        // SpatialFHE::HECrypto::CryptoParams params;
        crypto = SEALCrypto(json);
        // const std::string json = R"(
        // 	{
        // 		"CoeffModulusBits": [60, 40, 40, 40, 60],
        // 		"SchemeType": "CKKS",
        // 		"PolyModulusDegree": 16384,
        // 		"PlaintextModulus": 0,
        // 		"CoeffModulusPrimes": [0],
        // 		"ScaleFactor": 30
        // 	})";
        PUBKEY_PATH = "public.key";
        SECKEY_PATH = "secret.key";
        crypto.GenerateKeyPair(PUBKEY_PATH, SECKEY_PATH);
    }

    static void TearDownTestSuite() {}
};
SEALCrypto SealCryptoCKKSSuite::crypto;
std::string SealCryptoCKKSSuite::json, SealCryptoCKKSSuite::PUBKEY_PATH, SealCryptoCKKSSuite::SECKEY_PATH;

TEST_F(SealCryptoCKKSSuite, EncodeDecode) {
    PlainText pt = crypto.Encode(1.0);
    std::vector<double> result;
    crypto.Decode(result, pt);
    ASSERT_EQ(1.0, result[0]);
}

TEST_F(SealCryptoCKKSSuite, BatchEncodeDecode) {
    std::vector<double> vec = {1.0, 2.0, 3.0};
    std::vector<PlainText> pts = crypto.EncodeMany(vec);
    std::vector<double> result;
    if (pts.size() == 1) {
        crypto.Decode(result, pts[0]);
        ASSERT_NEAR(1.0, result[0], ERROR);
        ASSERT_NEAR(2.0, result[1], ERROR);
        ASSERT_NEAR(3.0, result[2], ERROR);
    }
}

TEST_F(SealCryptoCKKSSuite, BigBatchEncodeDecode) {
    std::vector<double> vec(8192, 1.0);
    for (int i = 0; i < 8192; i++) {
        vec[i] = i;
    }
    std::vector<PlainText> pts = crypto.EncodeMany(vec);
    ASSERT_EQ(pts.size(), 2);
    for (int i = 0; i < pts.size(); i++) {
        std::vector<double> result;
        crypto.Decode(result, pts[i]);
        for (int j = 0; j < 8192; j++) {
            ASSERT_NEAR(result[j], vec[j + i * 8192], ERROR);
        }
    }
}

TEST_F(SealCryptoCKKSSuite, EncryptDecrypt) {
    PlainText pt = crypto.Encode(1.0);
    CipherText ct = crypto.Encrypt(pt);
    PlainText res_pt = crypto.Decrypt(ct);
    std::vector<double> result;
    crypto.Decode(result, res_pt);
    ASSERT_NEAR(1.0, result[0], ERROR);
}

TEST_F(SealCryptoCKKSSuite, CipherTextToString) {
    PlainText pt = crypto.Encode(std::vector<double>({1, 2,3,4,5,6}));
    CipherText ct = crypto.Encrypt(pt);
    std::string str = ct.toString();
    CipherText new_ct = crypto.buildCipherText(str);
    PlainText res_pt = crypto.Decrypt(new_ct);
    std::vector<double> result;
    crypto.Decode(result, res_pt);
    ASSERT_NEAR(1.0, result[0], ERROR);
}

TEST_F(SealCryptoCKKSSuite, Add) {
    PlainText pt1 = crypto.Encode(1.0);
    PlainText pt2 = crypto.Encode(2.0);
    CipherText ct1 = crypto.Encrypt(pt1);
    CipherText ct2 = crypto.Encrypt(pt2);
    CipherText ct3 = crypto.Add(ct1, ct2);
    PlainText res_pt = crypto.Decrypt(ct3);
    std::vector<double> result;
    crypto.Decode(result, res_pt);
    ASSERT_NEAR(3.0, result[0], ERROR);
}

TEST_F(SealCryptoCKKSSuite, ReloadAndAdd) {
    SEALCrypto crypto2 = SEALCrypto(json);
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

TEST_F(SealCryptoCKKSSuite, AddInside) {
    PlainText pt1 = crypto.Encode(1.0);
    PlainText pt2 = crypto.Encode(2.0);
    CipherText ct1 = crypto.Encrypt(pt1);
    CipherText ct2 = crypto.Encrypt(pt2);
    seal::Ciphertext seal_ct1 = std::get<seal::Ciphertext>(ct1.getData());
    seal::Ciphertext seal_ct2 = std::get<seal::Ciphertext>(ct2.getData());
    seal::Ciphertext seal_ct3 = seal::Ciphertext();
    crypto._add(seal_ct3, seal_ct1, seal_ct2);
    std::cout << "ct3 scale: " << seal_ct3.scale() << std::endl;
    CipherText ct3 = CipherText(seal_ct3);

    PlainText res_pt = crypto.Decrypt(ct3);
    std::vector<double> result;
    crypto.Decode(result, res_pt);
    ASSERT_NEAR(3.0, result[0], ERROR);
}

TEST_F(SealCryptoCKKSSuite, AddPlain) {
    PlainText pt1 = crypto.Encode(1.0);
    PlainText pt2 = crypto.Encode(2.0);
    CipherText ct1 = crypto.Encrypt(pt1);
    CipherText ct3 = crypto.AddPlain(ct1, pt2);
    PlainText res_pt = crypto.Decrypt(ct3);
    std::vector<double> result;
    crypto.Decode(result, res_pt);
    ASSERT_NEAR(3.0, result[0], ERROR);
}

TEST_F(SealCryptoCKKSSuite, Sub) {
    PlainText pt1 = crypto.Encode(1.0);
    PlainText pt2 = crypto.Encode(2.0);
    CipherText ct1 = crypto.Encrypt(pt1);
    CipherText ct2 = crypto.Encrypt(pt2);
    CipherText ct3 = crypto.Subtract(ct1, ct2);
    PlainText res_pt = crypto.Decrypt(ct3);
    std::vector<double> result;
    crypto.Decode(result, res_pt);
    ASSERT_NEAR(-1.0, result[0], ERROR);
}

TEST_F(SealCryptoCKKSSuite, Multiply) {
    PlainText pt1 = crypto.Encode(1.0);
    PlainText pt2 = crypto.Encode(2.0);
    CipherText ct1 = crypto.Encrypt(pt1);
    CipherText ct2 = crypto.Encrypt(pt2);
    CipherText ct3 = crypto.Multiply(ct1, ct2);
    PlainText res_pt = crypto.Decrypt(ct3);
    std::vector<double> result;
    crypto.Decode(result, res_pt);
    ASSERT_NEAR(2.0, result[0], ERROR);
}

TEST_F(SealCryptoCKKSSuite, MultiplyPlain) {
    PlainText pt1 = crypto.Encode(1.0);
    PlainText pt2 = crypto.Encode(2.0);
    CipherText ct1 = crypto.Encrypt(pt1);
    CipherText ct3 = crypto.MultiplyPlain(ct1, pt2);
    PlainText res_pt = crypto.Decrypt(ct3);
    std::vector<double> result;
    crypto.Decode(result, res_pt);
    ASSERT_NEAR(2.0, result[0], ERROR);
}

TEST_F(SealCryptoCKKSSuite, Square) {
    PlainText pt1 = crypto.Encode(2.0);
    CipherText ct1 = crypto.Encrypt(pt1);
    CipherText ct2 = crypto.Square(ct1);
    PlainText res_pt = crypto.Decrypt(ct2);
    std::vector<double> result;
    crypto.Decode(result, res_pt);
    ASSERT_NEAR(4.0, result[0], ERROR);
}

// power is not supported in CKKS
// TEST_F(SealCryptoCKKSSuite, Power) {
// 	PlainText pt1 = crypto.Encode(2.0);
// 	CipherText ct1 = crypto.Encrypt(pt1);
// 	CipherText ct2 = crypto.Power(ct1, 3);
// 	PlainText res_pt = crypto.Decrypt(ct2);
// 	std::vector<double> result;
// 	crypto.Decode(result, res_pt);
// 	ASSERT_NEAR(8.0, result[0], ERROR);
// }

// TEST_F(SealCryptoCKKSSuite, Shift) {
// FIXME: shift is not working
// 	PlainText pt1 = crypto.Encode(2.0);
// 	CipherText ct1 = crypto.Encrypt(pt1);
// 	CipherText ct2 = crypto.Shift(ct1, -2);
// 	PlainText res_pt = crypto.Decrypt(ct2);
// 	std::vector<double> result;
// 	crypto.Decode(result, res_pt);
// 	ASSERT_NEAR(4.0, result[0], ERROR);
// }

TEST_F(SealCryptoCKKSSuite, And) {
    PlainText pt1 = crypto.Encode(1.0);
    PlainText pt2 = crypto.Encode(2.0);
    CipherText ct1 = crypto.Encrypt(pt1);
    CipherText ct2 = crypto.Encrypt(pt2);
    CipherText ct3 = crypto.And(ct1, ct2);
    PlainText res_pt = crypto.Decrypt(ct3);
    std::vector<double> result;
    crypto.Decode(result, res_pt);
    ASSERT_NEAR(0.0, result[0], ERROR);
}

TEST_F(SealCryptoCKKSSuite, Or) {
    PlainText pt1 = crypto.Encode(1.0);
    PlainText pt2 = crypto.Encode(0.0);
    CipherText ct1 = crypto.Encrypt(pt1);
    CipherText ct2 = crypto.Encrypt(pt2);
    CipherText ct3 = crypto.Or(ct1, ct2);
    PlainText res_pt = crypto.Decrypt(ct3);
    std::vector<double> result;
    crypto.Decode(result, res_pt);
    std::cout << result.size();
    ASSERT_NEAR(1.0, result[0], ERROR);
}

TEST_F(SealCryptoCKKSSuite, Not) {
    PlainText pt1 = crypto.Encode(1.0);
    CipherText ct1 = crypto.Encrypt(pt1);
    CipherText ct2 = crypto.Not(ct1);
    PlainText res_pt = crypto.Decrypt(ct2);
    std::vector<double> result;
    crypto.Decode(result, res_pt);
    ASSERT_NEAR(0.0, result[0], ERROR);
}

TEST_F(SealCryptoCKKSSuite, Xor) {
    PlainText pt1 = crypto.Encode(1.0);
    PlainText pt2 = crypto.Encode(0.0);
    CipherText ct1 = crypto.Encrypt(pt1);
    CipherText ct2 = crypto.Encrypt(pt2);
    CipherText ct3 = crypto.Xor(ct1, ct2);
    SpatialFHE::PlainText res_pt = crypto.Decrypt(ct3);
    std::vector<double> result;
    crypto.Decode(result, res_pt);
    ASSERT_NEAR(1.0, result[0], ERROR);
}
