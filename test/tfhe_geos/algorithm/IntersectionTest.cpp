//
// Created by ubuntu on 2/9/25.
//
#include <gtest/gtest.h>
// SpatialFHE
#include <TFHEInt32.h>
#include <tfhe_geos/algorithm/TFHEIntersection.h>
#include <tfhe_geos/geom/TFHECoordinate.h>
#include <tfhe_geos/geom/TFHEGeometry.h>
#include <tfhe_geos/io/WKTReader.h>
// std
#include <memory>
#include <sstream>
#include <string>

using namespace SpatialFHE;

// 定义测试用的结构体
class IntersectionTest : public ::testing::Test {
protected:
    typedef SpatialFHE::geom::TFHEGeometry TFHEGeometry;
    typedef SpatialFHE::geom::TFHECoordinate TFHECoordinate;
    typedef SpatialFHE::algorithm::TFHEIntersection TFHEIntersection;

    SpatialFHE::io::WKTReader reader;
    std::unique_ptr<TFHEGeometry> geom;

    std::unique_ptr<SpatialFHE::TFHEContext> context;

    void SetUp() override {
        context = std::make_unique<SpatialFHE::TFHEContext>();
    }

     void checkIntersectionNull(
        TFHEInt32&& p1x,
        TFHEInt32&& p1y,
        TFHEInt32&& p2x,
        TFHEInt32&& p2y,
        TFHEInt32&& q1x,
        TFHEInt32&& q1y,
        TFHEInt32&& q2x,
        TFHEInt32&& q2y) {
        TFHECoordinate p1(p1x, p1y);
        TFHECoordinate p2(p2x, p2y);
        TFHECoordinate q1(q1x, q1y);
        TFHECoordinate q2(q2x, q2y);
        TFHECoordinate actual = TFHEIntersection::intersection(p1, p2, q1, q2);
        EXPECT_TRUE(actual.isNull()) << "checkIntersectionNull failed";
    }

    void checkIntersection(
        TFHEInt32&& p1x,
        TFHEInt32&& p1y,
        TFHEInt32&& p2x,
        TFHEInt32&& p2y,
        TFHEInt32&& q1x,
        TFHEInt32&& q1y,
        TFHEInt32&& q2x,
        TFHEInt32&& q2y,
        TFHEInt32&& expectedx,
        TFHEInt32&& expectedy) {
        TFHECoordinate p1(p1x, p1y);
        TFHECoordinate p2(p2x, p2y);
        TFHECoordinate q1(q1x, q1y);
        TFHECoordinate q2(q2x, q2y);
        TFHECoordinate expected(expectedx, expectedy);
        TFHECoordinate actual = TFHEIntersection::intersection(p1, p2, q1, q2);
        TFHEInt32 dist = actual.distanceSquared(expected);
        std::cout << "Expected: " << expected.x.decrypt() << ", " << expected.y.decrypt() << std::endl;
        std::cout << "Actual: " << actual.x.decrypt() << ", " << actual.y.decrypt() << "  Dist = " << dist.decrypt()
                  << std::endl;
        EXPECT_TRUE((dist < 1).decrypt()) << "checkIntersection failed";
    }
};

// 定义测试用例
TEST_F(IntersectionTest, TestSimple) {
    checkIntersection(
        TFHEInt32(0),
        TFHEInt32(0),
        TFHEInt32(1000),
        TFHEInt32(1000),
        TFHEInt32(0),
        TFHEInt32(1000),
        TFHEInt32(1000),
        TFHEInt32(0),
        TFHEInt32(500),
        TFHEInt32(500));
}

TEST_F(IntersectionTest, TestCollinear) {
    checkIntersectionNull(
        TFHEInt32(0),
        TFHEInt32(0),
        TFHEInt32(1000),
        TFHEInt32(1000),
        TFHEInt32(2000),
        TFHEInt32(2000),
        TFHEInt32(3000),
        TFHEInt32(3000));
}

TEST_F(IntersectionTest, TestParallel) {
    checkIntersectionNull(
        TFHEInt32(0),
        TFHEInt32(0),
        TFHEInt32(1000),
        TFHEInt32(1000),
        TFHEInt32(1000),
        TFHEInt32(0),
        TFHEInt32(2000), TFHEInt32(1000));
}
