//
// Created by ubuntu on 2/12/25.
//
#include <gtest/gtest.h>

#include <algorithm>
#include <array>
#include <memory>
#include <set>
#include <string>
#include <vector>

// geos headers
#include <tfhe_geos/geom/TFHECoordinate.h>
#include <tfhe_geos/geom/TFHECoordinateSequence.h>
#include <tfhe_geos/geom/TFHEGeometry.h>
#include <tfhe_geos/geom/enums.h>
#include <tfhe_geos/io/WKTReader.h>
#include <tfhe_geos/operation/relateng/TFHELinearBoundary.h>

using namespace SpatialFHE::geom;
using namespace SpatialFHE::operation::relateng;
using SpatialFHE::io::WKTReader;

// Test fixture for LinearBoundary tests using GoogleTest
class LinearBoundaryTest : public ::testing::Test {
protected:
    // Common data used by all tests
    WKTReader reader;

    std::unique_ptr<SpatialFHE::TFHEContext> context;

    void SetUp() override {
        context = std::make_unique<SpatialFHE::TFHEContext>();
    }

    // Helper method to check the LinearBoundary against an expected boundary WKT.
    void checkLinearBoundary(const std::string& wkt, const std::string& wktBdyExpected) {
        std::unique_ptr<TFHEGeometry> geom(reader.read(wkt));
        auto lines = extractLines(*geom);
        TFHELinearBoundary lb(lines);
        bool hasBoundaryExpected = !wktBdyExpected.empty();
        ASSERT_EQ(hasBoundaryExpected, lb.hasBoundary()) << "Mismatch in hasBoundary flag.";

        checkBoundaryPoints(lb, *geom, wktBdyExpected);
    }

    // Helper method to verify boundary points.
    void checkBoundaryPoints(TFHELinearBoundary& lb, TFHEGeometry& geom, const std::string& wktBdyExpected) {
        TFHECoordinate::Set expectedBoundary;
        extractPoints(wktBdyExpected, expectedBoundary);

        // Check that each expected boundary point is indeed in the boundary.
        for (const TFHECoordinate& p : expectedBoundary) {
            ASSERT_TRUE(lb.isBoundary(&p)) << "Expected boundary point (" << p.getX().decrypt() << ", "
                                           << p.getY().decrypt() << ") not identified as boundary.";
        }

        // Check that coordinates not in the expected boundary are not mistakenly marked as boundary.
        auto allPts = geom.getCoordinates();
        for (std::size_t i = 0; i < allPts->size(); i++) {
            TFHECoordinate p;
            allPts->getAt(i, p);
            if (expectedBoundary.find(p) == expectedBoundary.end()) {
                ASSERT_FALSE(lb.isBoundary(&p))
                    << "Unexpected boundary point (" << p.getX().decrypt() << ", " << p.getY().decrypt() << ") found.";
            }
        }
    }

    // Helper method to extract points from a WKT string into a set.
    void extractPoints(const std::string& wkt, TFHECoordinate::Set& ptSet) {
        if (wkt.empty())
            return;
        std::unique_ptr<TFHEGeometry> geom(reader.read(wkt));
        auto pts = geom->getCoordinates();
        for (std::size_t i = 0; i < pts->size(); i++) {
            TFHECoordinate p;
            pts->getAt(i, p);
            ptSet.insert(p);
        }
    }

    // Helper method to extract lines from a Geometry.
    std::vector<const TFHELineString*> extractLines(const TFHEGeometry& geom) {
        // return TFHELineStringExtracter::getLines(&geom);
        std::vector<const TFHELineString*> lines;
        if (geom.getGeometryTypeId() == TFHEGeometryTypeId::TFHE_LINESTRING) {
            lines.push_back(dynamic_cast<const TFHELineString*>(&geom));
        }
        return lines;
    }
};

// Test Cases

// Test case for single line using BoundaryNodeRule Mod 2
TEST_F(LinearBoundaryTest, TestLineMod2) {
    // checkLinearBoundary("LINESTRING (0 0, 9 9)", "POINT (0 0)");
    // checkLinearBoundary("LINESTRING (0 0, 9 9)", "POINT(0 0)");
}

// // Test case for two lines using BoundaryNodeRule Mod 2
// TEST_F(LinearBoundaryTest, TestLines2Mod2) {
//     checkLinearBoundary("MULTILINESTRING ((0 0, 9 9), (9 9, 5 1))",
//                         "MULTIPOINT((0 0), (5 1))");
// }
//
// // Test case for three lines using BoundaryNodeRule Mod 2
// TEST_F(LinearBoundaryTest, TestLines3Mod2) {
//     checkLinearBoundary("MULTILINESTRING ((0 0, 9 9), (9 9, 5 1), (9 9, 1 5))",
//                         "MULTIPOINT((0 0), (5 1), (1 5), (9 9))");
// }
