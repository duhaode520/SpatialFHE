//
// Created by ubuntu on 2/12/25.
//
#include <gtest/gtest.h>

#include <memory>
#include <string>
#include <vector>

// geos
#include <tfhe_geos/geom/TFHEGeometry.h>
#include <tfhe_geos/io/WKTReader.h>
#include <tfhe_geos/operation/relateng/DimensionLocation.h>
#include <tfhe_geos/operation/relateng/TFHERelateGeometry.h>

using namespace SpatialFHE::geom;
using namespace SpatialFHE::operation::relateng;
using SpatialFHE::io::WKTReader;

class RelateGeometryTest : public ::testing::Test {
protected:
    WKTReader reader;

    std::unique_ptr<SpatialFHE::TFHEContext> context;

    void SetUp() override {
        context = std::make_unique<SpatialFHE::TFHEContext>();
    }

    // Helper function to verify the dimension and real dimension of a geometry given as WKT.
    void checkDimension(const std::string &wkt, int expectedDim, int expectedDimReal) {
        std::unique_ptr<TFHEGeometry> geom(reader.read(wkt));
        TFHERelateGeometry rgeom(geom.get());
        ASSERT_EQ(expectedDim, rgeom.getDimension()) << "Dimension does not match for WKT: " << wkt;
        ASSERT_EQ(expectedDimReal, rgeom.getDimension()) << "Real dimension does not match for WKT: " << wkt;
    }
};


TEST_F(RelateGeometryTest, BoundaryExists) {
    std::unique_ptr<TFHEGeometry> geom(reader.read("LINESTRING (0 0,9 9, 9 9, 5 1)"));
    TFHERelateGeometry rgeom(geom.get());
    ASSERT_TRUE(rgeom.hasBoundary()) << "Geometry should have a boundary.";
}

TEST_F(RelateGeometryTest, Dimension_Point) {
    checkDimension("POINT (0 0)", 0, 0);
}

TEST_F(RelateGeometryTest, Dimension_LineString) {
    checkDimension("LINESTRING (0 0, 9 9)", 1, 1);
}

TEST_F(RelateGeometryTest, Dimension_Polygon) {
    checkDimension("POLYGON ((1 9, 5 9, 5 5, 1 5, 1 9))", 2, 2);
}

TEST_F(RelateGeometryTest, Dimension_LineStringWithRepeatedMidPoint) {
    checkDimension("LINESTRING (0 0, 0 0, 9 9)", 1, 1);
}