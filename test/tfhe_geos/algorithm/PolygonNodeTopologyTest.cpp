//
// Created by ubuntu on 2/13/25.
//
#include <gtest/gtest.h>
#include <tfhe_geos/io/WKTReader.h>
#include <tfhe_geos/algorithm/TFHEPolygonNodeTopology.h>
#include <tfhe_geos/geom/TFHELineString.h>
#include <tfhe_geos/geom/TFHECoordinateSequence.h>
#include <memory>
#include <string>

using SpatialFHE::algorithm::TFHEPolygonNodeTopology;
using SpatialFHE::geom::TFHECoordinateSequence;
using SpatialFHE::geom::TFHELineString;
using SpatialFHE::geom::TFHEGeometry;

class PolygonNodeTopologyTest : public ::testing::Test {
protected:
    SpatialFHE::io::WKTReader reader;

    std::unique_ptr<SpatialFHE::TFHEContext> context;

    void SetUp() override {
        context = std::make_unique<SpatialFHE::TFHEContext>();

    }
    std::unique_ptr<TFHECoordinateSequence> readPts(const std::string& wkt) {
        std::unique_ptr<TFHEGeometry> geom = reader.read(wkt);
        const TFHELineString* line = dynamic_cast<TFHELineString*>(geom.get());
        if (line)
            return line->getCoordinatesRO()->clone();
        else
            return nullptr;
    }

    void checkCrossing(const std::string& wktA, const std::string& wktB, bool isExpected) {
        std::unique_ptr<TFHECoordinateSequence> a = readPts(wktA);
        std::unique_ptr<TFHECoordinateSequence> b = readPts(wktB);
        bool isCrossing = TFHEPolygonNodeTopology::isCrossing(
            &a->getAt(1), &a->getAt(0), &a->getAt(2), &b->getAt(0), &b->getAt(2));
        EXPECT_EQ(isCrossing, isExpected);
    }

    void checkInteriorSegment(const std::string& wktA, const std::string& wktB, bool isExpected) {
        std::unique_ptr<TFHECoordinateSequence> a = readPts(wktA);
        std::unique_ptr<TFHECoordinateSequence> b = readPts(wktB);
        bool isInterior = TFHEPolygonNodeTopology::isInteriorSegment(
            &a->getAt(1), &a->getAt(0), &a->getAt(2), &b->getAt(1));
        EXPECT_EQ(isInterior, isExpected);
    }
};

TEST_F(PolygonNodeTopologyTest, TestCrossing) {
    checkCrossing(
        "LINESTRING (500 1000, 1000 1000, 1000 1500)",
        "LINESTRING (1000 500, 1000 1000, 500 1500)",
        true);
}

TEST_F(PolygonNodeTopologyTest, TestNonCrossingQuadrant2) {
    checkCrossing(
        "LINESTRING (500 1000, 1000 1000, 1000 1500)",
        "LINESTRING (300 1200, 1000 1000, 500 1500)",
        false);
}

TEST_F(PolygonNodeTopologyTest, TestNonCrossingQuadrant4) {
    checkCrossing(
        "LINESTRING (500 1000, 1000 1000, 1000 1500)",
        "LINESTRING (1000 500, 1000 1000, 1500 1000)",
        false);
}