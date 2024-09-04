#include <gtest/gtest.h>
#include <gtest/gtest-spi.h>
#include <map>
#include "testutils.h"

#define private public
#include "sealcrypto.h"

// Test SealCryptoInit
TEST(SEALCryptoInitTest, Init) {
    SpatialFHE::SEALCrypto sealCrypto;
    ASSERT_NE(nullptr, &sealCrypto);
}

TEST(SEALCryptoInitTest, GenerateKeyPair) {
    // 1. Create a CryptoParams object with valid parameters.
    SpatialFHE::HECrypto::CryptoParams params;

    const std::string json = R"(
        {
            "CoeffModulusBits": [60, 30, 30, 30, 60],
            "SchemeType": "CKKS",
            "PolyModulusDegree": 8192,
            "PlaintextModulus": 0,
            "CoeffModulusPrimes": [0],
            "Scale": 1073741824
        })";
    rapidjson::Document doc; // doc 被销毁后可能导致读出来的value为空, extremly tricky
    doc.Parse(json.c_str());
    for (auto& m : doc.GetObject()) {
        params[m.name.GetString()] = m.value;
    }


    // 2. Create a SEALCrypto object.
    SpatialFHE::SEALCrypto crypto;

    // 3. Call GenerateKeyPair on the SEALCrypto object with the CryptoParams object and two valid file paths.
    ASSERT_NO_THROW(crypto.GenerateKeyPair(params, "publicKey.txt", "secretKey.txt"));
    // crypto.GenerateKeyPair(params, "publicKey.txt", "secretKey.txt");

    // 5. Verify that the sealParams, sealContext, secretKey, publicKey, encryptor, decryptor, and evaluator member variables were correctly set.
    ASSERT_EQ(crypto.sealParams->poly_modulus_degree(), 8192);
    ASSERT_EQ(crypto.sealParams->scheme(), seal::scheme_type::ckks);
    ASSERT_EQ(crypto.params->plainModulus, 0);
    ASSERT_NE(nullptr, crypto.sealContext);
    ASSERT_NE(nullptr, &crypto.secretKey);
    ASSERT_NE(nullptr, &crypto.publicKey);
    ASSERT_NE(nullptr, &crypto.encryptor);
    ASSERT_NE(nullptr, &crypto.decryptor);
    ASSERT_NE(nullptr, &crypto.evaluator);

    // 6. If context.using_keyswitching() is true, verify that the relinKeys and galoisKeys member variables were correctly set.
    if (crypto.sealContext->using_keyswitching()) {
        ASSERT_NE(nullptr, &crypto.relinKeys);
        ASSERT_NE(nullptr, &crypto.galoisKeys);
    }
    
    // 7. Verify that the encoder member variable was correctly set based on the schemeType.
    // TODO: Add assertions to verify that the encoder member variable was correctly set.
    if (crypto.params->schemeType == SpatialFHE::HECrypto::HEScheme::CKKS) {
        ASSERT_NE(nullptr, &crypto.ckksEncoder);
    }
    if (crypto.params->schemeType == SpatialFHE::HECrypto::HEScheme::BFV) {
        ASSERT_NE(nullptr, &crypto.batchEncoder);
    }
}


#undef private