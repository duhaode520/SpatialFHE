//
// Created by ubuntu on 2/10/25.
//
#include <gtest/gtest.h>

#include <memory>
#include <sstream>
#include <string>
#include <vector>

// geos headers
#include <tfhe_geos/algorithm/TFHELineIntersector.h>
#include <tfhe_geos/geom/TFHECoordinate.h>
#include <tfhe_geos/geom/TFHEGeometry.h>
#include <tfhe_geos/geom/TFHEGeometryFactory.h>
#include <tfhe_geos/geom/TFHELineString.h>
#include <tfhe_geos/io/WKTReader.h>

using namespace SpatialFHE::geom;
using namespace SpatialFHE::io;
using namespace SpatialFHE::algorithm;
using std::string;
using std::unique_ptr;
using std::vector;
using SpatialFHE::TFHEInt32;

class RobustLineIntersectionTest : public ::testing::Test {
protected:

    std::unique_ptr<SpatialFHE::TFHEContext> context;
    TFHEGeometryFactory::Ptr gf;
    WKTReader reader;

    void SetUp() override {
        context = std::make_unique<SpatialFHE::TFHEContext>();
        gf = TFHEGeometryFactory::create();
        reader = WKTReader(gf.get());
    }

    // Constructor: Initialize PrecisionModel, GeometryFactory and WKTReader.
    // Helper method to compare two Coordinates given a tolerance.
    bool equals(const TFHECoordinate& p0, const TFHECoordinate& p1, int distanceTolerance) {
        return (p0.distanceSquared(p1) <= distanceTolerance * distanceTolerance).decrypt();
    }

    // Helper method to check that two intersection points are equal within a tolerance.
    void checkIntPoints(const TFHECoordinate& p, const TFHECoordinate& q, int distanceTolerance) {
        bool isEqual = equals(p, q, distanceTolerance);
        // Log a message if not equal.
        ASSERT_TRUE(isEqual) << "checkIntPoints failed: expected " << p.getX().decrypt() << ", " << p.getY().decrypt()
                             << " obtained " << q.getX().decrypt() << ", " << q.getY().decrypt();
    }

    /**
     * Check intersection when provided a vector of 4 Coordinates representing two segments.
     * @param pt vector of 4 Coordinates: first two for the first segment, last two for the second.
     * @param expectedIntersectionNum Expected number of intersection points.
     * @param intPt The expected intersection points.
     * @param distanceTolerance tolerance in comparing intersection coordinates.
     */
    void checkIntersection(
        const vector<TFHECoordinate>& pt,
        std::size_t expectedIntersectionNum,
        const vector<TFHECoordinate>& intPt,
        int distanceTolerance) {
        TFHELineIntersector li;
        li.computeIntersection(pt[0], pt[1], pt[2], pt[3]);

        size_t intNum = li.getIntersectionNum();
        ASSERT_EQ(expectedIntersectionNum, intNum);

        if (intPt.empty()) {
            return;
        }

        ASSERT_EQ(intPt.size(), intNum)
            << "Expected intersection point vector size different from computed intersections.";

        // For one intersection point
        if (intNum == 1) {
            checkIntPoints(intPt[0], li.getIntersection(0), distanceTolerance);
        }
        // For two intersection points (if applicable)
        else if (intNum == 2) {
            // Check first expected point against both computed intersection points.
            if (!(equals(intPt[0], li.getIntersection(0), distanceTolerance) ||
                  equals(intPt[0], li.getIntersection(1), distanceTolerance))) {
                checkIntPoints(intPt[0], li.getIntersection(0), distanceTolerance);
                checkIntPoints(intPt[0], li.getIntersection(1), distanceTolerance);
            }
            // Similarly for the second expected point.
            else if (!(equals(intPt[1], li.getIntersection(0), distanceTolerance) ||
                       equals(intPt[1], li.getIntersection(1), distanceTolerance))) {
                checkIntPoints(intPt[1], li.getIntersection(0), distanceTolerance);
                checkIntPoints(intPt[1], li.getIntersection(1), distanceTolerance);
            }
        }
    }

    /**
     * Check intersection given two WKT lines and expected intersection as WKT.
     * @param wkt1 WKT for first line.
     * @param wkt2 WKT for second line.
     * @param expectedIntersectionNum Expected number of intersections.
     * @param expectedWKT Expected intersection geometry in WKT.
     * @param distanceTolerance tolerance for comparing intersection coordinates.
     */
    void checkIntersection(
        const string& wkt1,
        const string& wkt2,
        std::size_t expectedIntersectionNum,
        const string& expectedWKT,
        int distanceTolerance) {
        // Read geometries from WKT strings.
        unique_ptr<TFHEGeometry> g1(reader.read(wkt1));
        unique_ptr<TFHEGeometry> g2(reader.read(wkt2));

        auto* l1ptr = dynamic_cast<TFHELineString*>(g1.get());
        auto* l2ptr = dynamic_cast<TFHELineString*>(g2.get());
        ASSERT_NE(nullptr, l1ptr) << "First geometry is not a LineString.";
        ASSERT_NE(nullptr, l2ptr) << "Second geometry is not a LineString.";

        TFHELineString& l1 = *l1ptr;
        TFHELineString& l2 = *l2ptr;

        vector<TFHECoordinate> pt;
        pt.push_back(l1.getCoordinates()->getAt(0));
        pt.push_back(l1.getCoordinates()->getAt(1));
        pt.push_back(l2.getCoordinates()->getAt(0));
        pt.push_back(l2.getCoordinates()->getAt(1));

        unique_ptr<TFHEGeometry> g(reader.read(expectedWKT));
        // Extract coordinates from expected intersection WKT.
        std::unique_ptr<TFHECoordinateSequence> cs(g->getCoordinates());
        vector<TFHECoordinate> intPt;
        intPt.reserve(cs->size());
        for (std::size_t i = 0; i < cs->size(); ++i) {
            intPt.push_back(cs->getAt(i));
        }

        checkIntersection(pt, expectedIntersectionNum, intPt, distanceTolerance);
    }

    /**
     * Check intersection given two WKT lines and expected intersection points as vector.
     * @param wkt1 WKT for first line.
     * @param wkt2 WKT for second line.
     * @param expectedIntersectionNum Expected number of intersections.
     * @param intPt The expected intersection points.
     * @param distanceTolerance tolerance in comparing intersection coordinates.
     */
    void checkIntersection(
        const string& wkt1,
        const string& wkt2,
        std::size_t expectedIntersectionNum,
        const vector<TFHECoordinate>& intPt,
        int distanceTolerance) {
        unique_ptr<TFHEGeometry> g1(reader.read(wkt1));
        unique_ptr<TFHEGeometry> g2(reader.read(wkt2));

        auto* l1ptr = dynamic_cast<TFHELineString*>(g1.get());
        auto* l2ptr = dynamic_cast<TFHELineString*>(g2.get());
        ASSERT_NE(nullptr, l1ptr) << "First geometry is not a LineString.";
        ASSERT_NE(nullptr, l2ptr) << "Second geometry is not a LineString.";

        TFHELineString& l1 = *l1ptr;
        TFHELineString& l2 = *l2ptr;

        vector<TFHECoordinate> pt;
        pt.push_back(l1.getCoordinates()->getAt(0));
        pt.push_back(l1.getCoordinates()->getAt(1));
        pt.push_back(l2.getCoordinates()->getAt(0));
        pt.push_back(l2.getCoordinates()->getAt(1));

        checkIntersection(pt, expectedIntersectionNum, intPt, distanceTolerance);
    }

    // Check that there is no intersection between two WKT lines.
    void checkIntersectionNone(const string& wkt1, const string& wkt2) {
        unique_ptr<TFHEGeometry> g1(reader.read(wkt1));
        unique_ptr<TFHEGeometry> g2(reader.read(wkt2));

        auto* l1ptr = dynamic_cast<TFHELineString*>(g1.get());
        auto* l2ptr = dynamic_cast<TFHELineString*>(g2.get());
        ASSERT_NE(nullptr, l1ptr) << "First geometry is not a LineString.";
        ASSERT_NE(nullptr, l2ptr) << "Second geometry is not a LineString.";

        TFHELineString& l1 = *l1ptr;
        TFHELineString& l2 = *l2ptr;

        vector<TFHECoordinate> pt;
        pt.push_back(l1.getCoordinates()->getAt(0));
        pt.push_back(l1.getCoordinates()->getAt(1));
        pt.push_back(l2.getCoordinates()->getAt(0));
        pt.push_back(l2.getCoordinates()->getAt(1));

        vector<TFHECoordinate> intPt;  // expected empty vector.
        checkIntersection(pt, 0, intPt, 0);
    }

    // Check that the input coordinates are not altered by the intersection computation.
    void checkInputNotAltered(const string& wkt1, const string& wkt2, int scaleFactor) {
        unique_ptr<TFHEGeometry> g1(reader.read(wkt1));
        unique_ptr<TFHEGeometry> g2(reader.read(wkt2));

        auto* l1ptr = dynamic_cast<TFHELineString*>(g1.get());
        auto* l2ptr = dynamic_cast<TFHELineString*>(g2.get());
        ASSERT_NE(nullptr, l1ptr) << "First geometry is not a LineString.";
        ASSERT_NE(nullptr, l2ptr) << "Second geometry is not a LineString.";

        TFHELineString& l1 = *l1ptr;
        TFHELineString& l2 = *l2ptr;

        vector<TFHECoordinate> pt;
        pt.push_back(l1.getCoordinates()->getAt(0));
        pt.push_back(l1.getCoordinates()->getAt(1));
        pt.push_back(l2.getCoordinates()->getAt(0));
        pt.push_back(l2.getCoordinates()->getAt(1));
        checkInputNotAltered(pt, scaleFactor);
    }

    // Check that the input coordinate vector is not altered after computing the intersection.
    void checkInputNotAltered(const vector<TFHECoordinate>& pt, int scaleFactor) {
        // Save input points.
        vector<TFHECoordinate> savePt = pt;

        TFHELineIntersector li;
        li.computeIntersection(pt[0], pt[1], pt[2], pt[3]);

        // Ensure input points remain unchanged.
        ASSERT_EQ(savePt.size(), pt.size());
        for (std::size_t i = 0; i < pt.size(); i++) {
            bool isEqual = savePt[i] == pt[i];
            ASSERT_TRUE(isEqual) << "Input point altered at index " << i << ": "
                                 << "expected " << savePt[i].getX().decrypt() << ", " << savePt[i].getY().decrypt()
                                 << " but got " << pt[i].getX().decrypt() << ", " << pt[i].getY().decrypt();
        }
    }

};

// Test Cases

TEST_F(RobustLineIntersectionTest, Collinear) {
    std::vector<TFHECoordinate> pt;
    pt.emplace_back(TFHEInt32(146), TFHEInt32(1850));
    pt.emplace_back(TFHEInt32(761), TFHEInt32(2507));
    checkIntersection("LINESTRING (-59 1631, 761 2507)", "LINESTRING (146 1850, 966 2726)", 2, pt, 1);
}


TEST_F(RobustLineIntersectionTest, TestCase9) {
    checkIntersection(
        "LINESTRING (1617 -2085, 1659 -2141)",
        "LINESTRING (28 -579, 4696 -5026)",
        1,
        "POINT (1638 -2113)",
        2);
}

TEST_F(RobustLineIntersectionTest, IntersectionOnVertex) {
    checkIntersection(
        "LINESTRING (-580 -14, -5138 -4572)",
        "LINESTRING (-2152 -1586, -2181 -1606)",
        1,
        "POINT (-2152 -1586)",
        1);
}

TEST_F(RobustLineIntersectionTest, TestCase11) {
    checkInputNotAltered("LINESTRING (-5800 -143, -51386 -45729)", "LINESTRING (-21522 -15865, -21812 -16068)", 100000);
}
