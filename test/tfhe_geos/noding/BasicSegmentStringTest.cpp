//
// Created by ubuntu on 2/10/25.
//
#include <gtest/gtest.h>
#include <memory>
#include <stdexcept>

// GEOS headers
#include <tfhe_geos/noding/TFHEBasicSegmentString.h>
#include <tfhe_geos/geom/TFHECoordinate.h>
#include <tfhe_geos/geom/TFHECoordinateSequence.h>

// Convenience aliases for unique pointers
using CoordinateSequenceAutoPtr = std::unique_ptr<SpatialFHE::geom::TFHECoordinateSequence>;
using SegmentStringAutoPtr = std::unique_ptr<SpatialFHE::noding::TFHEBasicSegmentString>;
using namespace SpatialFHE;

// Test fixture for BasicSegmentString tests
class BasicSegmentStringTest : public ::testing::Test {
protected:
    // Helper function to create a BasicSegmentString.
    SegmentStringAutoPtr makeSegmentString(SpatialFHE::geom::TFHECoordinateSequence* cs, void* d = nullptr) {
        return std::make_unique<SpatialFHE::noding::TFHEBasicSegmentString>(cs, d);
    }

    std::unique_ptr<SpatialFHE::TFHEContext> context;

    void SetUp() override {
        context = std::make_unique<SpatialFHE::TFHEContext>();
    }
};

// Test 1: Constructor with 2 equal points
TEST_F(BasicSegmentStringTest, ConstructorWith2EqualPoints) {
    // Create an empty CoordinateSequence with initial capacity of 2.
    auto cs = std::make_unique<SpatialFHE::geom::TFHECoordinateSequence>();
    ASSERT_NE(cs.get(), nullptr);

    SpatialFHE::geom::TFHECoordinate c0(TFHEInt32(0), TFHEInt32(0));
    SpatialFHE::geom::TFHECoordinate c1(TFHEInt32(0), TFHEInt32(0));

    cs->add(c0);
    cs->add(c1);

    EXPECT_EQ(cs->size(), 2u);

    // Create SegmentString using the coordinate sequence
    SegmentStringAutoPtr ss = makeSegmentString(cs.get());
    ASSERT_NE(ss.get(), nullptr);

    EXPECT_EQ(ss->size(), 2u);
    EXPECT_EQ(ss->getData(), static_cast<void*>(nullptr));
    EXPECT_EQ(ss->getCoordinates(), cs.get());
    EXPECT_EQ(ss->getCoordinate(0), c0);
    EXPECT_EQ(ss->getCoordinate(1), c1);
    EXPECT_TRUE(ss->isClosed());
}

// Test 2: Constructor with 2 different points
TEST_F(BasicSegmentStringTest, ConstructorWith2DifferentPoints) {
    auto cs = std::make_unique<SpatialFHE::geom::TFHECoordinateSequence>();
    ASSERT_NE(cs.get(), nullptr);

    SpatialFHE::geom::TFHECoordinate c0(TFHEInt32(0), TFHEInt32(0));
    SpatialFHE::geom::TFHECoordinate c1(TFHEInt32(1), TFHEInt32(0));

    cs->add(c0);
    cs->add(c1);

    EXPECT_EQ(cs->size(), 2u);

    SegmentStringAutoPtr ss = makeSegmentString(cs.get());
    ASSERT_NE(ss.get(), nullptr);

    EXPECT_EQ(ss->size(), 2u);
    EXPECT_EQ(ss->getData(), static_cast<void*>(nullptr));
    EXPECT_EQ(ss->getCoordinates(), cs.get());
    EXPECT_EQ(ss->getCoordinate(0), c0);
    EXPECT_EQ(ss->getCoordinate(1), c1);
    EXPECT_FALSE(ss->isClosed());
}

// Test 3: Constructor with 4 different points forming a ring
TEST_F(BasicSegmentStringTest, ConstructorWithRing) {
    auto cs = std::make_unique<SpatialFHE::geom::TFHECoordinateSequence>();
    ASSERT_NE(cs.get(), nullptr);

    SpatialFHE::geom::TFHECoordinate c0(TFHEInt32(0), TFHEInt32(0));
    SpatialFHE::geom::TFHECoordinate c1(TFHEInt32(1), TFHEInt32(0));
    SpatialFHE::geom::TFHECoordinate c2(TFHEInt32(1), TFHEInt32(1));

    cs->add(c0);
    cs->add(c1);
    cs->add(c2);
    cs->add(c0);  // Closing the ring

    EXPECT_EQ(cs->size(), 4u);

    SegmentStringAutoPtr ss = makeSegmentString(cs.get());
    ASSERT_NE(ss.get(), nullptr);

    EXPECT_EQ(ss->size(), 4u);
    EXPECT_EQ(ss->getData(), static_cast<void*>(nullptr));
    EXPECT_EQ(ss->getCoordinates(), cs.get());
    EXPECT_EQ(ss->getCoordinate(0), c0);
    EXPECT_EQ(ss->getCoordinate(1), c1);
    EXPECT_EQ(ss->getCoordinate(2), c2);
    EXPECT_EQ(ss->getCoordinate(3), c0);
    EXPECT_TRUE(ss->isClosed());
}
