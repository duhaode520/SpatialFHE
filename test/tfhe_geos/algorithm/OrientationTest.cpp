//
// Created by ubuntu on 2/10/25.
//
#include <gtest/gtest.h>

// geos
#include <tfhe_geos/geom/TFHECoordinate.h>
#include <tfhe_geos/algorithm/TFHEOrientation.h>
#include <TFHEInt32.h>

using namespace SpatialFHE::geom;
using namespace SpatialFHE::algorithm;
using SpatialFHE::TFHEInt32;

// Test fixture class derived from ::testing::Test
class OrientationTest : public ::testing::Test {
protected:
    std::unique_ptr<SpatialFHE::TFHEContext> context;

    void SetUp() override {
        context = std::make_unique<SpatialFHE::TFHEContext>();
    }
    // This helper function checks if the orientation index is consistent for cyclic permutations.
    int checkOrientation(const TFHECoordinate& c1, const TFHECoordinate& c2, const TFHECoordinate& c3) {
        int orient0 = TFHEOrientation::index(c1, c2, c3).decrypt();
        int orient1 = TFHEOrientation::index(c2, c3, c1).decrypt();
        int orient2 = TFHEOrientation::index(c3, c1, c2).decrypt();
        return (orient0 == orient1 && orient0 == orient2) ? 1 : 0;
    }
};

TEST_F(OrientationTest, TestCase2) {
    TFHECoordinate c1(TFHEInt32(21936), TFHEInt32(14084));
    TFHECoordinate c2(TFHEInt32(16890), TFHEInt32(-5713));
    TFHECoordinate c3(TFHEInt32(18680), TFHEInt32(46289));
    EXPECT_EQ(1, checkOrientation(c1, c2, c3)) << "ring orientation test 2 failed";
}

TEST_F(OrientationTest, TestCase3) {
    TFHECoordinate c1(TFHEInt32(27956), TFHEInt32(-18638));
    TFHECoordinate c2(TFHEInt32(-20431), TFHEInt32(13620));
    TFHECoordinate c3(TFHEInt32(0), TFHEInt32(0));
    EXPECT_EQ(1, checkOrientation(c1, c2, c3)) << "ring orientation test 3 failed";
}

TEST_F(OrientationTest, TestCase4) {
    TFHECoordinate c1(TFHEInt32(-262), TFHEInt32(1887));
    TFHECoordinate c2(TFHEInt32(370), TFHEInt32(2907));
    TFHECoordinate c3(TFHEInt32(212), TFHEInt32(2652));
    EXPECT_EQ(1, checkOrientation(c1, c2, c3)) << "ring orientation test 4 failed";
}

TEST_F(OrientationTest, TestCase5) {
    TFHECoordinate c1(TFHEInt32(-59), TFHEInt32(1631));
    TFHECoordinate c2(TFHEInt32(761), TFHEInt32(2507));
    TFHECoordinate c3(TFHEInt32(146), TFHEInt32(185));
    EXPECT_EQ(1, checkOrientation(c1, c2, c3)) << "ring orientation test 5 failed";
}

TEST_F(OrientationTest, TestCase7) {
    TFHECoordinate c1(TFHEInt32(-9575), TFHEInt32(4511));
    TFHECoordinate c2(TFHEInt32(-9295), TFHEInt32(3291));
    TFHECoordinate c3(TFHEInt32(-8945), TFHEInt32(1766));
    EXPECT_EQ(1, checkOrientation(c1, c2, c3)) << "ring orientation test 7 failed";
}
