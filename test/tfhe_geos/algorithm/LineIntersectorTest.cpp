//
// Created by ubuntu on 2/10/25.
//

#include <gtest/gtest.h>
#include <tfhe_geos/io/WKTReader.h>
#include <tfhe_geos/algorithm/TFHELineIntersector.h>
#include <tfhe_geos/algorithm/TFHEPointLocation.h>
#include <tfhe_geos/algorithm/TFHEOrientation.h>
#include <tfhe_geos/geom/TFHEGeometryFactory.h>
#include <tfhe_geos/geom/TFHEGeometry.h>
#include <tfhe_geos/geom/TFHELineString.h>
#include <tfhe_geos/geom/TFHECoordinate.h>
#include <tfhe_geos/geom/TFHEPoint.h>
#include <tfhe_geos/geom/TFHECoordinateSequence.h>
#include <sstream>
#include <string>
#include <memory>

// Using declarations
using namespace SpatialFHE::geom;
using SpatialFHE::algorithm::TFHELineIntersector;
using SpatialFHE::algorithm::TFHEPointLocation;
using SpatialFHE::algorithm::TFHEOrientation;
using SpatialFHE::TFHEInt32;

class RobustLineIntersectorTest : public ::testing::Test {
protected:
    // The test fixture instantiates a LineIntersector for all tests.
    TFHELineIntersector i;
    std::unique_ptr<SpatialFHE::TFHEContext> context;

    void SetUp() override {
        context = std::make_unique<SpatialFHE::TFHEContext>();
    }
};

// Test 1 - test2Lines
TEST_F(RobustLineIntersectorTest, Test2Lines) {
    TFHECoordinate p1(TFHEInt32(10), TFHEInt32(10));
    TFHECoordinate p2(TFHEInt32(20), TFHEInt32(20));
    TFHECoordinate q1(TFHEInt32(20), TFHEInt32(10));
    TFHECoordinate q2(TFHEInt32(10), TFHEInt32(20));
    TFHECoordinate expected(TFHEInt32(15), TFHEInt32(15));

    i.computeIntersection(p1, p2, q1, q2);

    // Check that the intersection type corresponds to a single point
    EXPECT_EQ(i.getIntersectionNum(), (unsigned int)TFHELineIntersector::POINT_INTERSECTION);
    EXPECT_EQ(i.getIntersectionNum(), 1UL);
    EXPECT_EQ(i.getIntersection(0), expected);
    EXPECT_TRUE(i.isProper());
    EXPECT_TRUE(i.hasIntersection());
}

// Test 2 - testCollinear1
TEST_F(RobustLineIntersectorTest, TestCollinear1) {
    TFHECoordinate p1(TFHEInt32(10), TFHEInt32(10));
    TFHECoordinate p2(TFHEInt32(20), TFHEInt32(10));
    TFHECoordinate q1(TFHEInt32(22), TFHEInt32(10));
    TFHECoordinate q2(TFHEInt32(30), TFHEInt32(10));

    i.computeIntersection(p1, p2, q1, q2);

    EXPECT_EQ(i.getIntersectionNum(), TFHELineIntersector::NO_INTERSECTION);
    EXPECT_EQ(i.getIntersectionNum(), 0UL);
    EXPECT_FALSE(i.isProper());
    EXPECT_FALSE(i.hasIntersection());
}

// Test 3 - testCollinear2
TEST_F(RobustLineIntersectorTest, TestCollinear2) {
    TFHECoordinate p1(TFHEInt32(10), TFHEInt32(10));
    TFHECoordinate p2(TFHEInt32(20), TFHEInt32(10));
    TFHECoordinate q1(TFHEInt32(20), TFHEInt32(10));
    TFHECoordinate q2(TFHEInt32(30), TFHEInt32(10));

    i.computeIntersection(p1, p2, q1, q2);

    EXPECT_EQ(i.getIntersectionNum(), (unsigned int)TFHELineIntersector::POINT_INTERSECTION);
    EXPECT_EQ(i.getIntersectionNum(), 1UL);
    EXPECT_FALSE(i.isProper());
    EXPECT_TRUE(i.hasIntersection());
}

// Test 4 - testCollinear3
TEST_F(RobustLineIntersectorTest, TestCollinear3) {
    TFHECoordinate p1(TFHEInt32(10), TFHEInt32(10));
    TFHECoordinate p2(TFHEInt32(20), TFHEInt32(10));
    TFHECoordinate q1(TFHEInt32(15), TFHEInt32(10));
    TFHECoordinate q2(TFHEInt32(30), TFHEInt32(10));

    i.computeIntersection(p1, p2, q1, q2);

    EXPECT_EQ(i.getIntersectionNum(), (unsigned int)TFHELineIntersector::COLLINEAR_INTERSECTION);
    EXPECT_EQ(i.getIntersectionNum(), 2UL);
    EXPECT_FALSE(i.isProper());
    EXPECT_TRUE(i.hasIntersection());
}

// Test 5 - testCollinear4
TEST_F(RobustLineIntersectorTest, TestCollinear4) {
    TFHECoordinate p1(TFHEInt32(10), TFHEInt32(10));
    TFHECoordinate p2(TFHEInt32(20), TFHEInt32(10));
    TFHECoordinate q1(TFHEInt32(10), TFHEInt32(10));
    TFHECoordinate q2(TFHEInt32(30), TFHEInt32(10));

    i.computeIntersection(p1, p2, q1, q2);

    EXPECT_EQ(i.getIntersectionNum(), (unsigned int)TFHELineIntersector::COLLINEAR_INTERSECTION);
    EXPECT_EQ(i.getIntersectionNum(), 2UL);
    EXPECT_FALSE(i.isProper());
    EXPECT_TRUE(i.hasIntersection());
}

// Test 6 - testEndpointIntersection
TEST_F(RobustLineIntersectorTest, TestEndpointIntersection) {
    i.computeIntersection(TFHECoordinate(TFHEInt32(100), TFHEInt32(100)), TFHECoordinate(TFHEInt32(10), TFHEInt32(100)),
                          TFHECoordinate(TFHEInt32(100), TFHEInt32(10)), TFHECoordinate(TFHEInt32(100), TFHEInt32(100)));
    EXPECT_TRUE(i.hasIntersection());
    EXPECT_EQ(i.getIntersectionNum(), 1UL);
}

// Test 7 - testEndpointIntersection2
TEST_F(RobustLineIntersectorTest, TestEndpointIntersection2) {
    i.computeIntersection(TFHECoordinate(TFHEInt32(190), TFHEInt32(50)), TFHECoordinate(TFHEInt32(120), TFHEInt32(100)),
                          TFHECoordinate(TFHEInt32(120), TFHEInt32(100)), TFHECoordinate(TFHEInt32(50), TFHEInt32(150)));
    EXPECT_TRUE(i.hasIntersection());
    EXPECT_EQ(i.getIntersectionNum(), 1UL);
    // Although typically the intersection point would be at index 0,
    // we follow the original test which accesses index 1.
    EXPECT_EQ(i.getIntersection(1), TFHECoordinate(TFHEInt32(120), TFHEInt32(100)));
}

// Test 8 - testOverlap
TEST_F(RobustLineIntersectorTest, TestOverlap) {
    i.computeIntersection(TFHECoordinate(TFHEInt32(180), TFHEInt32(200)), TFHECoordinate(TFHEInt32(160), TFHEInt32(180)),
                          TFHECoordinate(TFHEInt32(220), TFHEInt32(240)), TFHECoordinate(TFHEInt32(140), TFHEInt32(160)));
    EXPECT_TRUE(i.hasIntersection());
    EXPECT_EQ(i.getIntersectionNum(), 2UL);
}

// Test 9 - testIsProper1
TEST_F(RobustLineIntersectorTest, TestIsProper1) {
    i.computeIntersection(TFHECoordinate(TFHEInt32(30), TFHEInt32(10)), TFHECoordinate(TFHEInt32(30), TFHEInt32(30)),
                          TFHECoordinate(TFHEInt32(10), TFHEInt32(10)), TFHECoordinate(TFHEInt32(90), TFHEInt32(11)));
    EXPECT_TRUE(i.hasIntersection());
    EXPECT_EQ(i.getIntersectionNum(), 1UL);
    EXPECT_TRUE(i.isProper());
}

// Test 10 - testIsProper2
TEST_F(RobustLineIntersectorTest, TestIsProper2) {
    i.computeIntersection(TFHECoordinate(TFHEInt32(10), TFHEInt32(30)), TFHECoordinate(TFHEInt32(10), TFHEInt32(0)),
                          TFHECoordinate(TFHEInt32(11), TFHEInt32(90)), TFHECoordinate(TFHEInt32(10), TFHEInt32(10)));
    EXPECT_TRUE(i.hasIntersection());
    EXPECT_EQ(i.getIntersectionNum(), 1UL);
    EXPECT_FALSE(i.isProper());
}

// Test 11 - testIsCCW
TEST_F(RobustLineIntersectorTest, TestIsCCW) {
    int index = TFHEOrientation::index(
        TFHECoordinate(TFHEInt32(-1234), TFHEInt32(-4)),
        TFHECoordinate(TFHEInt32(0), TFHEInt32(0)),
        TFHECoordinate(TFHEInt32(45104), TFHEInt32(1234))
    ).decrypt();
    EXPECT_EQ(index, 1);
}

// Test 12 - testIsCCW2
TEST_F(RobustLineIntersectorTest, TestIsCCW2) {
    int index = TFHEOrientation::index(
        TFHECoordinate(TFHEInt32(10), TFHEInt32(10)),
        TFHECoordinate(TFHEInt32(20), TFHEInt32(20)),
        TFHECoordinate(TFHEInt32(0), TFHEInt32(0))
    ).decrypt();
    EXPECT_EQ(index, 0);
}

// Test 13 - testA
TEST_F(RobustLineIntersectorTest, TestA) {
    TFHECoordinate p1(TFHEInt32(-1234), TFHEInt32(-4));
    TFHECoordinate p2(TFHEInt32(45104), TFHEInt32(1234));
    TFHECoordinate q(TFHEInt32(0), TFHEInt32(0));

    auto cs = std::make_unique<TFHECoordinateSequence>();
    cs->add(p1);
    cs->add(p2);

    EXPECT_FALSE(TFHEPointLocation::isOnLine(q, cs.get()).decrypt());
    EXPECT_EQ(TFHEOrientation::index(p1, p2, q).decrypt(), -1);

    TFHEGeometryFactory::Ptr factory = TFHEGeometryFactory::create();
    std::unique_ptr<TFHEGeometry> l(factory->createLineString(std::move(cs)));
    std::unique_ptr<TFHEGeometry> p(factory->createPoint(q));
    EXPECT_FALSE(l->intersects(p.get()).decrypt());
}
