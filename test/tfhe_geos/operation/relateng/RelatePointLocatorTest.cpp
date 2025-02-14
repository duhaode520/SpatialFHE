//
// Created by ubuntu on 2/12/25.
//
#include <gtest/gtest.h>

#include <memory>
#include <string>

// geos
#include <tfhe_geos/geom/TFHECoordinate.h>
#include <tfhe_geos/geom/TFHEGeometry.h>
#include <tfhe_geos/io/WKTReader.h>
#include <tfhe_geos/operation/relateng/DimensionLocation.h>
#include <tfhe_geos/operation/relateng/TFHERelatePointLocator.h>

using namespace SpatialFHE::geom;
using SpatialFHE::io::WKTReader;
using namespace SpatialFHE::operation::relateng;
using SpatialFHE::TFHEInt32;

class RelatePointLocatorTest : public ::testing::Test {
protected:
    // WKTReader instance for reading geometries.
    WKTReader reader;

    std::unique_ptr<SpatialFHE::TFHEContext> context;

    void SetUp() override {
        context = std::make_unique<SpatialFHE::TFHEContext>();
    }

    // Checks the location with dimension information.
    void checkDimLocation(const std::string &wkt, int i, int j, int expected) {
        std::unique_ptr<TFHEGeometry> geom(reader.read(wkt));
        TFHERelatePointLocator locator(geom.get());
        // Use CoordinateXY constructed with the given coordinates.
        TFHECoordinate coord((TFHEInt32(i)), TFHEInt32(j));
        int actual = locator.locateWithDim(&coord);
        ASSERT_EQ(expected, actual) << "For coordinate (" << i << ", " << j << ") in geometry: " << wkt;
    }

    // Checks the location at a node.
    void checkNodeLocation(const std::string &wkt, int i, int j, Location expected) {
        std::unique_ptr<TFHEGeometry> geom(reader.read(wkt));
        TFHERelatePointLocator locator(geom.get());
        TFHECoordinate coord((TFHEInt32(i)), TFHEInt32(j));
        Location actual = locator.locateNode(&coord, nullptr);
        ASSERT_EQ(expected, actual) << "For node (" << i << ", " << j << ") in geometry: " << wkt;
    }

    // Checks the location for a line end with dimension information.
    void checkLineEndDimLocation(const std::string &wkt, int x, int y, int expectedDimLoc) {
        std::unique_ptr<TFHEGeometry> geom(reader.read(wkt));
        TFHERelatePointLocator locator(geom.get());
        TFHECoordinate coord((TFHEInt32(x)), TFHEInt32(y));
        int actual = locator.locateLineEndWithDim(&coord);
        ASSERT_EQ(expectedDimLoc, actual) << "For line end (" << x << ", " << y << ") in geometry: " << wkt;
    }
};

// Test using a single POINT geometry.
TEST_F(RelatePointLocatorTest, TestPoint) {
    std::string wkt = "POINT (1 1)";
    // For a point geometry, the point itself is interior.
    checkDimLocation(wkt, 1, 1, DimensionLocation::POINT_INTERIOR);
    // A point not coinciding with the geometry is exterior.
    checkDimLocation(wkt, 0, 1, DimensionLocation::EXTERIOR);
}

// Test using a single LINESTRING geometry.
TEST_F(RelatePointLocatorTest, TestPointInLine) {
    std::string wkt = "LINESTRING (3 1, 3 9)";
    // A point along the line (but not an endpoint) is line interior.
    checkDimLocation(wkt, 3, 8, DimensionLocation::LINE_INTERIOR);
}

// Test using a single POLYGON geometry.
TEST_F(RelatePointLocatorTest, TestPointInArea) {
    // Using a simple rectangle polygon.
    std::string wkt = "POLYGON ((6 5, 6 9, 9 9, 9 5, 6 5))";
    // A point well inside the polygon should be area interior.
    checkDimLocation(wkt, 8, 8, DimensionLocation::AREA_INTERIOR);
}

// Test for a LINESTRING geometry checking interior and boundary.
TEST_F(RelatePointLocatorTest, TestLine) {
    std::string wkt = "LINESTRING (3 1, 3 9)";
    // A point between the endpoints is interior.
    checkDimLocation(wkt, 3, 3, DimensionLocation::LINE_INTERIOR);
    // An endpoint is classified as boundary.
    checkDimLocation(wkt, 3, 1, DimensionLocation::LINE_BOUNDARY);
}

// Test using a POLYGON geometry where a point on its interior edge is checked.
TEST_F(RelatePointLocatorTest, TestLineInArea) {
    // Use a rectangular polygon.
    std::string wkt = "POLYGON ((10 10, 10 16, 16 16, 16 10, 10 10))";
    // Points strictly inside the polygon (away from the vertices) are area interior.
    checkDimLocation(wkt, 11, 11, DimensionLocation::AREA_INTERIOR);
    checkDimLocation(wkt, 14, 14, DimensionLocation::AREA_INTERIOR);
}

// Test using a POLYGON geometry to confirm interior vs. boundary classification.
TEST_F(RelatePointLocatorTest, TestArea) {
    std::string wkt = "POLYGON ((6 5, 6 9, 9 9, 9 5, 6 5))";
    // A point clearly inside.
    checkDimLocation(wkt, 8, 8, DimensionLocation::AREA_INTERIOR);
    // A vertex should be considered a boundary point.
    checkDimLocation(wkt, 9, 9, DimensionLocation::AREA_BOUNDARY);
}

// Test using a different POLYGON geometry to further check interior and boundary.
TEST_F(RelatePointLocatorTest, TestAreaInArea) {
    // Use a square polygon.
    std::string wkt = "POLYGON ((10 10, 10 20, 20 20, 20 10, 10 10))";
    // Points within the polygon away from boundaries.
    checkDimLocation(wkt, 11, 11, DimensionLocation::AREA_INTERIOR);
    checkDimLocation(wkt, 12, 12, DimensionLocation::AREA_INTERIOR);
    // A vertex is on the boundary.
    checkDimLocation(wkt, 10, 10, DimensionLocation::AREA_BOUNDARY);
    // A point on an edge (but not a vertex) is interior. For example, (16,16) lies on the edge between (10,20) and (20,20) only if it is not a vertex.
    // However, note that in a standard polygon, vertices are boundary whereas points on edges (non-vertex) are interior. Here (16,16) is not a vertex so it is interior.
    checkDimLocation(wkt, 16, 16, DimensionLocation::AREA_INTERIOR);
}

// Test for node location on a LINESTRING.
TEST_F(RelatePointLocatorTest, TestLineNode) {
    std::string wkt = "LINESTRING (3 1, 3 9)";
    // The start or end of a line is considered a boundary node.
    checkNodeLocation(wkt, 3, 1, Location::BOUNDARY);
}
