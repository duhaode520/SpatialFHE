//
// Created by ubuntu on 2/10/25.
//
#include <gtest/gtest.h>

#include <memory>
#include <string>

// GEOS headers
#include <tfhe_geos/geom/TFHEGeometry.h>
#include <tfhe_geos/geom/enums.h>
#include <tfhe_geos/io/WKTReader.h>
#include <tfhe_geos/operation/relateng/TFHEAdjacentEdgeLocator.h>

// Using declarations for convenience
using namespace SpatialFHE::geom;
using namespace SpatialFHE::operation::relateng;
using SpatialFHE::TFHEInt32;
using SpatialFHE::io::WKTReader;

class AdjacentEdgeLocatorTest : public ::testing::Test {
protected:
    // Instance of WKTReader to read geometries from WKT strings.
    WKTReader reader;

    std::unique_ptr<SpatialFHE::TFHEContext> context;

    void SetUp() override {
        context = std::make_unique<SpatialFHE::TFHEContext>();
    }

    // Helper function to check the location of a coordinate.
    void checkLocation(const std::string& wkt, int x, int y, Location expectedLoc) {
        std::unique_ptr<TFHEGeometry> geom(reader.read(wkt));
        TFHEAdjacentEdgeLocator ael(geom.get());
        TFHECoordinate c((TFHEInt32(x)), TFHEInt32(y));
        Location loc = ael.locate(&c);
        EXPECT_EQ((int)expectedLoc, (int)loc) << "For coordinate (" << x << ", " << y << ") in geometry: " << wkt;
    }
};

// Test Case 1: testAdjacent2
TEST_F(AdjacentEdgeLocatorTest, TestAdjacent2) {
    checkLocation("POLYGON ((1 9, 5 9, 5 1, 1 1, 1 9))", 5, 5, Location::BOUNDARY);
    checkLocation("POLYGON ((9 9, 9 1, 5 1, 5 9, 9 9))", 5, 5, Location::BOUNDARY);
}

// Test Case 3: testAdjacent6WithFilledHoles
TEST_F(AdjacentEdgeLocatorTest, TestAdjacent6WithHoles) {
    // checkLocation(
    //     "GEOMETRYCOLLECTION (POLYGON ((1 9, 5 9, 6 6, 1 5, 1 9), (2 6, 4 8, 6 6, 2 6)), "
    //     "POLYGON ((2 6, 4 8, 6 6, 2 6)), "
    //     "POLYGON ((9 9, 9 5, 6 6, 5 9, 9 9)), "
    //     "POLYGON ((9 1, 5 1, 6 6, 9 5, 9 1), (7 2, 6 6, 8 3, 7 2)), "
    //     "POLYGON ((7 2, 6 6, 8 3, 7 2)), "
    //     "POLYGON ((1 1, 1 5, 6 6, 5 1, 1 1)))",
    //     6, 6, Location::INTERIOR
    // );

    checkLocation("POLYGON ((1 9, 5 9, 6 6, 1 5, 1 9), (2 6, 4 8, 6 6, 2 6))", 6, 6, Location::BOUNDARY);
    checkLocation("POLYGON ((9 1, 5 1, 6 6, 9 5, 9 1), (7 2, 6 6, 8 3, 7 2))", 6, 6, Location::BOUNDARY);

}


// Test Case 5: testContainedAndAdjacent
TEST_F(AdjacentEdgeLocatorTest, TestContainedAndAdjacent) {
    // std::string wkt = "GEOMETRYCOLLECTION (POLYGON ((1 9, 9 9, 9 1, 1 1, 1 9)), "
    //                   "POLYGON ((9 2, 2 2, 2 8, 9 8, 9 2)))";
    // checkLocation(wkt, 9, 5, Location::BOUNDARY);
    // checkLocation(wkt, 9, 8, Location::BOUNDARY);

    checkLocation("POLYGON ((1 9, 9 9, 9 1, 1 1, 1 9))", 9, 5, Location::BOUNDARY);
    checkLocation("POLYGON ((9 2, 2 2, 2 8, 9 8, 9 2))", 9, 5, Location::BOUNDARY);
    checkLocation("POLYGON ((1 9, 9 9, 9 1, 1 1, 1 9))", 9, 8, Location::BOUNDARY);
    checkLocation("POLYGON ((9 2, 2 2, 2 8, 9 8, 9 2))", 9, 8, Location::BOUNDARY);
}

// Test Case 6: testDisjointCollinear
TEST_F(AdjacentEdgeLocatorTest, TestDisjointCollinear) {
    // checkLocation(
    //     "GEOMETRYCOLLECTION (MULTIPOLYGON (((1 4, 4 4, 4 1, 1 1, 1 4)), "
    //     "((5 4, 8 4, 8 1, 5 1, 5 4))))",
    //     2, 4, Location::BOUNDARY
    // );

    checkLocation("POLYGON ((1 4, 4 4, 4 1, 1 1, 1 4))", 2, 4, Location::BOUNDARY);
    // checkLocation("POLYGON ((5 4, 8 4, 8 1, 5 1, 5 4))", 2, 4, Location::EXTERIOR);
}