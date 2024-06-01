#include <gtest/gtest.h>
#include "sealcrypto.h"

// Test SealCryptoInit
TEST(SealCryptoInit, IsNotNull) {
    SpatialFHE::SEALCrypto sealCrypto;
    ASSERT_NE(nullptr, &sealCrypto);
}