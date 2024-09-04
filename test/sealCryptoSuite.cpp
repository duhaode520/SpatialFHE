#include <gtest/gtest.h>
#include <seal/ciphertext.h>
#include <vector>
#include "ciphertext.h"

#define private public
#include "sealcrypto.h"
#undef private

class SealCryptoCKKSSuite : public ::testing::Test {
protected:
	static SpatialFHE::SEALCrypto crypto;
	constexpr static const double ERROR = 1e-4; // scale factor is 30, so the error is 1e-4 ~ 2^-10
    static void SetUpTestSuite() {

		SpatialFHE::HECrypto::CryptoParams params;

		const std::string json = R"(
			{
				"CoeffModulusBits": [60, 40, 40, 40, 60],
				"SchemeType": "CKKS",
				"PolyModulusDegree": 16384,
				"PlaintextModulus": 0,
				"CoeffModulusPrimes": [0],
				"ScaleFactor": 30
			})";
		rapidjson::Document doc; // doc 被销毁后可能导致读出来的value为空, extremly tricky
		doc.Parse(json.c_str());
		for (auto& m : doc.GetObject()) {
			params[m.name.GetString()] = m.value;
		}
		crypto.GenerateKeyPair(params, "publicKey.txt", "secretKey.txt");
	}

    static void TearDownTestSuite() {
	}
};
SpatialFHE::SEALCrypto SealCryptoCKKSSuite::crypto;

TEST_F(SealCryptoCKKSSuite, EncodeDecode) {
	SpatialFHE::PlainText pt = crypto.Encode(1.0);
	std::vector<double> result;
	crypto.Decode(result, pt);
	ASSERT_EQ(1.0, result[0]);
}

TEST_F(SealCryptoCKKSSuite, BatchEncodeDecode) {
	std::vector<double> vec = {1.0, 2.0, 3.0};
	std::vector<SpatialFHE::PlainText> pts = crypto.EncodeMany(vec);
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
	std::vector<SpatialFHE::PlainText> pts = crypto.EncodeMany(vec);
	ASSERT_EQ(pts.size(), 2);
	for (int i = 0; i < pts.size(); i++) {
		std::vector<double> result;
		crypto.Decode(result, pts[i]);
		for (int j = 0; j < 4096; j++) {
			ASSERT_NEAR(result[j], vec[j + i * 4096], ERROR);
		}
	}
}

TEST_F(SealCryptoCKKSSuite, EncryptDecrypt) {
	SpatialFHE::PlainText pt = crypto.Encode(1.0);
	SpatialFHE::CipherText ct = crypto.Encrypt(pt);
	SpatialFHE::PlainText res_pt = crypto.Decrypt(ct);
	std::vector<double> result;
	crypto.Decode(result, res_pt);
	ASSERT_NEAR(1.0, result[0], ERROR);
}

TEST_F(SealCryptoCKKSSuite, Add) {
	SpatialFHE::PlainText pt1 = crypto.Encode(1.0);
	SpatialFHE::PlainText pt2 = crypto.Encode(2.0);
	SpatialFHE::CipherText ct1 = crypto.Encrypt(pt1);
	SpatialFHE::CipherText ct2 = crypto.Encrypt(pt2);
	SpatialFHE::CipherText ct3 = crypto.Add(ct1, ct2);
	SpatialFHE::PlainText res_pt = crypto.Decrypt(ct3);
	std::vector<double> result;
	crypto.Decode(result, res_pt);
	ASSERT_NEAR(3.0, result[0], ERROR);
}

TEST_F(SealCryptoCKKSSuite, AddInside) {
	SpatialFHE::PlainText pt1 = crypto.Encode(1.0);
	SpatialFHE::PlainText pt2 = crypto.Encode(2.0);
	SpatialFHE::CipherText ct1 = crypto.Encrypt(pt1);
	SpatialFHE::CipherText ct2 = crypto.Encrypt(pt2);
	seal::Ciphertext seal_ct1 = std::get<seal::Ciphertext>(ct1.getData());
	seal::Ciphertext seal_ct2 = std::get<seal::Ciphertext>(ct2.getData());
	seal::Ciphertext seal_ct3 = seal::Ciphertext();
	crypto._add(seal_ct3, seal_ct1, seal_ct2);
	std::cout << "ct3 scale: " << seal_ct3.scale() << std::endl;
	SpatialFHE::CipherText ct3 = SpatialFHE::CipherText(seal_ct3);

	SpatialFHE::PlainText res_pt = crypto.Decrypt(ct3);
	std::vector<double> result;
	crypto.Decode(result, res_pt);
	ASSERT_NEAR(3.0, result[0], ERROR);
}

TEST_F(SealCryptoCKKSSuite, AddPlain) {
	SpatialFHE::PlainText pt1 = crypto.Encode(1.0);
	SpatialFHE::PlainText pt2 = crypto.Encode(2.0);
	SpatialFHE::CipherText ct1 = crypto.Encrypt(pt1);
	SpatialFHE::CipherText ct3 = crypto.AddPlain(ct1, pt2);
	SpatialFHE::PlainText res_pt = crypto.Decrypt(ct3);
	std::vector<double> result;
	crypto.Decode(result, res_pt);
	ASSERT_NEAR(3.0, result[0], ERROR);
}

TEST_F(SealCryptoCKKSSuite, Sub) {
	SpatialFHE::PlainText pt1 = crypto.Encode(1.0);
	SpatialFHE::PlainText pt2 = crypto.Encode(2.0);
	SpatialFHE::CipherText ct1 = crypto.Encrypt(pt1);
	SpatialFHE::CipherText ct2 = crypto.Encrypt(pt2);
	SpatialFHE::CipherText ct3 = crypto.Subtract(ct1, ct2);
	SpatialFHE::PlainText res_pt = crypto.Decrypt(ct3);
	std::vector<double> result;
	crypto.Decode(result, res_pt);
	ASSERT_NEAR(-1.0, result[0], ERROR);
}

TEST_F(SealCryptoCKKSSuite, Multiply) {
	SpatialFHE::PlainText pt1 = crypto.Encode(1.0);
	SpatialFHE::PlainText pt2 = crypto.Encode(2.0);
	SpatialFHE::CipherText ct1 = crypto.Encrypt(pt1);
	SpatialFHE::CipherText ct2 = crypto.Encrypt(pt2);
	SpatialFHE::CipherText ct3 = crypto.Multiply(ct1, ct2);
	SpatialFHE::PlainText res_pt = crypto.Decrypt(ct3);
	std::vector<double> result;
	crypto.Decode(result, res_pt);
	ASSERT_NEAR(2.0, result[0], ERROR);
}

TEST_F(SealCryptoCKKSSuite, MultiplyPlain) {
	SpatialFHE::PlainText pt1 = crypto.Encode(1.0);
	SpatialFHE::PlainText pt2 = crypto.Encode(2.0);
	SpatialFHE::CipherText ct1 = crypto.Encrypt(pt1);
	SpatialFHE::CipherText ct3 = crypto.MultiplyPlain(ct1, pt2);
	SpatialFHE::PlainText res_pt = crypto.Decrypt(ct3);
	std::vector<double> result;
	crypto.Decode(result, res_pt);
	ASSERT_NEAR(2.0, result[0], ERROR);
}

TEST_F(SealCryptoCKKSSuite, Square) {
	SpatialFHE::PlainText pt1 = crypto.Encode(2.0);
	SpatialFHE::CipherText ct1 = crypto.Encrypt(pt1);
	SpatialFHE::CipherText ct2 = crypto.Square(ct1);
	SpatialFHE::PlainText res_pt = crypto.Decrypt(ct2);
	std::vector<double> result;
	crypto.Decode(result, res_pt);
	ASSERT_NEAR(4.0, result[0], ERROR);
}

// power is not supported in CKKS
// TEST_F(SealCryptoCKKSSuite, Power) {
// 	SpatialFHE::PlainText pt1 = crypto.Encode(2.0);
// 	SpatialFHE::CipherText ct1 = crypto.Encrypt(pt1);
// 	SpatialFHE::CipherText ct2 = crypto.Power(ct1, 3);
// 	SpatialFHE::PlainText res_pt = crypto.Decrypt(ct2);
// 	std::vector<double> result;
// 	crypto.Decode(result, res_pt);
// 	ASSERT_NEAR(8.0, result[0], ERROR);
// }

// TEST_F(SealCryptoCKKSSuite, Shift) {
	//FIXME: shift is not working
// 	SpatialFHE::PlainText pt1 = crypto.Encode(2.0);
// 	SpatialFHE::CipherText ct1 = crypto.Encrypt(pt1);
// 	SpatialFHE::CipherText ct2 = crypto.Shift(ct1, -2);
// 	SpatialFHE::PlainText res_pt = crypto.Decrypt(ct2);
// 	std::vector<double> result;
// 	crypto.Decode(result, res_pt);
// 	ASSERT_NEAR(4.0, result[0], ERROR);
// }

TEST_F(SealCryptoCKKSSuite, And) {
	SpatialFHE::PlainText pt1 = crypto.Encode(1.0);
	SpatialFHE::PlainText pt2 = crypto.Encode(2.0);
	SpatialFHE::CipherText ct1 = crypto.Encrypt(pt1);
	SpatialFHE::CipherText ct2 = crypto.Encrypt(pt2);
	SpatialFHE::CipherText ct3 = crypto.And(ct1, ct2);
	SpatialFHE::PlainText res_pt = crypto.Decrypt(ct3);
	std::vector<double> result;
	crypto.Decode(result, res_pt);
	ASSERT_NEAR(0.0, result[0], ERROR);
}

TEST_F(SealCryptoCKKSSuite, Or) {
	SpatialFHE::PlainText pt1 = crypto.Encode(1.0);
	SpatialFHE::PlainText pt2 = crypto.Encode(0.0);
	SpatialFHE::CipherText ct1 = crypto.Encrypt(pt1);
	SpatialFHE::CipherText ct2 = crypto.Encrypt(pt2);
	SpatialFHE::CipherText ct3 = crypto.Or(ct1, ct2);
	SpatialFHE::PlainText res_pt = crypto.Decrypt(ct3);
	std::vector<double> result;
	crypto.Decode(result, res_pt);
	std::cout << result.size();
	ASSERT_NEAR(1.0, result[0], ERROR);
}

TEST_F(SealCryptoCKKSSuite, Not) {
	SpatialFHE::PlainText pt1 = crypto.Encode(1.0);
	SpatialFHE::CipherText ct1 = crypto.Encrypt(pt1);
	SpatialFHE::CipherText ct2 = crypto.Not(ct1);
	SpatialFHE::PlainText res_pt = crypto.Decrypt(ct2);
	std::vector<double> result;
	crypto.Decode(result, res_pt);
	ASSERT_NEAR(0.0, result[0], ERROR);
}

TEST_F(SealCryptoCKKSSuite, Xor) {
	SpatialFHE::PlainText pt1 = crypto.Encode(1.0);
	SpatialFHE::PlainText pt2 = crypto.Encode(0.0);
	SpatialFHE::CipherText ct1 = crypto.Encrypt(pt1);
	SpatialFHE::CipherText ct2 = crypto.Encrypt(pt2);
	SpatialFHE::CipherText ct3 = crypto.Xor(ct1, ct2);
	SpatialFHE::PlainText res_pt = crypto.Decrypt(ct3);
	std::vector<double> result;
	crypto.Decode(result, res_pt);
	ASSERT_NEAR(1.0, result[0], ERROR);
}
