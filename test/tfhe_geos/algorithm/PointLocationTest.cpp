#include <gtest/gtest.h>

#include <memory>
#include <sstream>
#include <string>

// GEOS headers
#include <TFHEInt32.h>
#include <tfhe_geos/algorithm/TFHEPointLocation.h>
#include <tfhe_geos/geom/TFHECoordinate.h>
#include <tfhe_geos/geom/TFHECoordinateSequence.h>
#include <tfhe_geos/geom/TFHEGeometry.h>
#include <tfhe_geos/geom/TFHELineString.h>
#include <tfhe_geos/io/WKTReader.h>

using SpatialFHE::TFHEInt32;
using SpatialFHE::algorithm::TFHEPointLocation;
using SpatialFHE::geom::TFHECoordinate;
using SpatialFHE::geom::TFHECoordinateSequence;
using SpatialFHE::geom::TFHEGeometry;
using SpatialFHE::geom::TFHELineString;
using SpatialFHE::io::WKTReader;

class PointLocationTest : public ::testing::Test {
protected:
    // WKTReader to parse WKT strings
    WKTReader reader;

    std::unique_ptr<SpatialFHE::TFHEContext> context;

    void SetUp() override {
        context = std::make_unique<SpatialFHE::TFHEContext>();
    }

    // Reads a WKT line and returns a unique_ptr to its coordinate sequence.
    std::unique_ptr<TFHECoordinateSequence> readPts(const std::string& wkt) {
        std::unique_ptr<TFHEGeometry> geom(reader.read(wkt));
        // dynamic_cast to LineString to extract coordinate sequence
        const auto* line = dynamic_cast<const TFHELineString*>(geom.get());
        if (line)
            return std::unique_ptr<TFHECoordinateSequence>(line->getCoordinatesRO()->clone());
        else
            return nullptr;
    }

    // Checks if the given point (x, y) lies on the line described by the WKT string.
    void checkOnLine(TFHEInt32&& x, TFHEInt32&& y, const std::string& wktLine, bool isExpected) {
        TFHECoordinate p(x, y);
        auto lineCoords = readPts(wktLine);
        ASSERT_NE(lineCoords, nullptr) << "Failed to read coordinates from: " << wktLine;
        bool isOnLine = TFHEPointLocation::isOnLine(p, lineCoords.get()).decrypt();
        ASSERT_EQ(isOnLine, isExpected) << "Expected isOnLine(" << x.decrypt() << ", " << y.decrypt() << ") to be "
                                        << isExpected << " for " << wktLine;
    }

    // Checks if the given point (x, y) lies on the segment defined by the first two points of the line described by the
    // WKT string.
    void checkOnSegment(TFHEInt32&& x, TFHEInt32&& y, const std::string& wktLine, bool isExpected) {
        TFHECoordinate p(x, y);
        auto lineCoords = readPts(wktLine);
        ASSERT_NE(lineCoords, nullptr) << "Failed to read coordinates from: " << wktLine;
        ASSERT_GE(lineCoords->size(), 2) << "Coordinate sequence must have at least 2 points";
        bool isOnSeg = TFHEPointLocation::isOnSegment(p, lineCoords->getAt(0), lineCoords->getAt(1)).decrypt();
        ASSERT_EQ(isOnSeg, isExpected) << "Expected isOnSegment(" << x.decrypt() << ", " << y.decrypt() << ") to be "
                                       << isExpected << " for " << wktLine;
    }
};

TEST_F(PointLocationTest, OnLineOnVertex) {
    checkOnLine(TFHEInt32(20), TFHEInt32(20), "LINESTRING (0 0, 20 20, 30 30)", true);
}

TEST_F(PointLocationTest, OnLineInSegment) {
    checkOnLine(TFHEInt32(10), TFHEInt32(10), "LINESTRING (0 0, 20 20, 0 40)", true);
    checkOnLine(TFHEInt32(10), TFHEInt32(30), "LINESTRING (0 0, 20 20, 0 40)", true);
}

TEST_F(PointLocationTest, NotOnLine) {
    checkOnLine(TFHEInt32(0), TFHEInt32(100), "LINESTRING (10 10, 20 10, 30 10)", false);
}

TEST_F(PointLocationTest, OnSegment) {
    checkOnSegment(TFHEInt32(5), TFHEInt32(5), "LINESTRING(0 0, 9 9)", true);
    checkOnSegment(TFHEInt32(0), TFHEInt32(0), "LINESTRING(0 0, 9 9)", true);
    checkOnSegment(TFHEInt32(9), TFHEInt32(9), "LINESTRING(0 0, 9 9)", true);
}

TEST_F(PointLocationTest, NotOnSegment) {
    checkOnSegment(TFHEInt32(5), TFHEInt32(6), "LINESTRING(0 0, 9 9)", false);
    checkOnSegment(TFHEInt32(10000), TFHEInt32(10000), "LINESTRING(0 0, 9000 9000)", false);
    checkOnSegment(TFHEInt32(9000), TFHEInt32(9001), "LINESTRING(0 0, 9000 9000)", false);
}

TEST_F(PointLocationTest, OnZeroLengthSegment) {
    checkOnSegment(TFHEInt32(1), TFHEInt32(1), "LINESTRING(1 1, 1 1)", true);
    checkOnSegment(TFHEInt32(10), TFHEInt32(20), "LINESTRING(10 10, 10 10)", false);
}
