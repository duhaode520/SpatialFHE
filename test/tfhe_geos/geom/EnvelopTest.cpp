//
// Created by ubuntu on 2/12/25.
//
#include <gtest/gtest.h>
#include <limits>
#include <cfenv>
#include <cmath>
#include <array>
#include <unordered_set>
#include <algorithm>
#include <vector>

// geos
#include <tfhe_geos/geom/TFHEEnvelope.h>
#include <tfhe_geos/geom/TFHECoordinate.h>

using SpatialFHE::geom::TFHECoordinate;
using SpatialFHE::geom::TFHEEnvelope;

class TFHEEnvelopeTest : public ::testing::Test {
protected:
    // Helper function similar to test_envelope_data::absdiff
    static std::size_t absDiff(std::size_t a, std::size_t b) {
        auto p = std::minmax(a, b);
        return p.second - p.first;
    }

    // Helper function similar to test_envelope_data::ensure_no_fp_except
    static void ensureNoFpExcept() {
        // FE_INEXACT is omitted as in original
        ASSERT_FALSE(std::fetestexcept(FE_DIVBYZERO)) << "FE_DIVBYZERO raised";
        ASSERT_FALSE(std::fetestexcept(FE_INVALID)) << "FE_INVALID raised";
        ASSERT_FALSE(std::fetestexcept(FE_OVERFLOW)) << "FE_OVERFLOW raised";
        ASSERT_FALSE(std::fetestexcept(FE_UNDERFLOW)) << "FE_UNDERFLOW raised";
    }

    // Helper functions for checking intersection between two envelopes
    static void checkIntersects(const TFHEEnvelope& e1, const TFHEEnvelope& e2, bool expected) {
        // Using both overloads for intersects and disjoint
        ASSERT_EQ(e1.intersects(e2).decrypt(), expected);
        ASSERT_EQ(e1.intersects(&e2).decrypt(), expected);
        ASSERT_EQ(e1.disjoint(e2).decrypt(), !expected);
        ASSERT_EQ(e1.disjoint(&e2).decrypt(), !expected);

        ASSERT_EQ(e2.intersects(e1).decrypt(), expected);
        ASSERT_EQ(e2.intersects(&e1).decrypt(), expected);
        ASSERT_EQ(e2.disjoint(e1).decrypt(), !expected);
        ASSERT_EQ(e2.disjoint(&e1).decrypt(), !expected);

        TFHECoordinate p0, p1, q0, q1;
        if (!e2.isNull()) {
            q0 = TFHECoordinate(e2.getMinX(), e2.getMaxY());
            q1 = TFHECoordinate(e2.getMaxX(), e2.getMinY());
            ASSERT_EQ(e1.intersects(q0, q1), expected);
            ASSERT_EQ(e1.intersects(q1, q0), expected);
        }
        if (!e1.isNull()) {
            p0 = TFHECoordinate(e1.getMinX(), e1.getMinY());
            p1 = TFHECoordinate(e1.getMaxX(), e1.getMaxY());
            ASSERT_EQ(e2.intersects(p0, p1), expected);
            ASSERT_EQ(e2.intersects(p1, p0), expected);
        }
        if (!e1.isNull() && !e2.isNull()) {
            ASSERT_EQ(TFHEEnvelope::intersects(p0, p1, q0, q1), expected);
            ASSERT_EQ(TFHEEnvelope::intersects(p1, p0, q0, q1), expected);
            ASSERT_EQ(TFHEEnvelope::intersects(p1, p0, q1, q0), expected);
            ASSERT_EQ(TFHEEnvelope::intersects(p1, p0, q0, q1), expected);
        }
    }

    // Helper function for checking intersection between an envelope and a coordinate
    static void checkIntersects(const TFHEEnvelope& e1, const TFHECoordinate& q, bool expected) {
        ASSERT_EQ(e1.intersects(q), expected);
        ASSERT_EQ(e1.intersects(q.x, q.y), expected);
        ASSERT_EQ(e1.contains(q), expected);
        ASSERT_EQ(e1.contains(q.x, q.y), expected);
        ASSERT_EQ(e1.covers(&q), expected);
        ASSERT_EQ(e1.covers(q.x, q.y), expected);

        if (!e1.isNull()) {
            TFHECoordinate p0(e1.getMinX(), e1.getMinY());
            TFHECoordinate p1(e1.getMaxX(), e1.getMaxY());
            ASSERT_EQ(TFHEEnvelope::intersects(p0, p1, q), expected);
        }
    }
};

// 1 - Test of default constructor
TEST_F(TFHEEnvelopeTest, DefaultConstructor) {
    TFHEEnvelope empty;
    ASSERT_TRUE(empty.isNull());
    ASSERT_EQ(empty.getWidth(), 0);
    ASSERT_EQ(empty.getHeight(), 0);
    ensureNoFpExcept();
}

// 2 - Test of overridden constructor
TEST_F(TFHEEnvelopeTest, OverriddenConstructor) {
    TFHEEnvelope box(0, 100, 0, 100);
    ASSERT_FALSE(box.isNull());
    ASSERT_EQ(box.getMinX(), 0);
    ASSERT_EQ(box.getMaxX(), 100);
    ASSERT_EQ(box.getMinY(), 0);
    ASSERT_EQ(box.getMaxY(), 100);
    ASSERT_EQ(box.getMinX(), box.getMinY());
    ASSERT_EQ(box.getMaxX(), box.getMaxY());
    ASSERT_EQ(box.getWidth(), box.getHeight());
    ensureNoFpExcept();
}

// 3 - Test of copy constructor
TEST_F(TFHEEnvelopeTest, CopyConstructor) {
    TFHEEnvelope box(0, 100, 0, 100);
    ASSERT_FALSE(box.isNull());
    ASSERT_EQ(box.getWidth(), box.getHeight());

    TFHEEnvelope copied(box);
    ASSERT_FALSE(copied.isNull());
    ASSERT_TRUE(copied == box);
    ASSERT_EQ(copied.getWidth(), copied.getHeight());
    ensureNoFpExcept();
}

// 4 - Test of setToNull()
TEST_F(TFHEEnvelopeTest, SetToNull) {
    TFHEEnvelope e(100, 200, 100, 200);
    ASSERT_FALSE(e.isNull());
    e.setToNull();
    ASSERT_TRUE(e.isNull());
    ensureNoFpExcept();
}

// 5 - Test of equals()
TEST_F(TFHEEnvelopeTest, Equals) {
    TFHEEnvelope empty;
    TFHEEnvelope zero(0, 0, 0, 0);
    TFHEEnvelope zero2(0, 0, 0, 0);
    TFHEEnvelope box(0, 100, 0, 100);
    TFHEEnvelope inf(0, 100, 0, std::numeric_limits<double>::infinity());

    ASSERT_TRUE(empty.isNull());
    ASSERT_FALSE(zero.isNull());
    ASSERT_FALSE(zero2.isNull());
    ASSERT_FALSE(box.isNull());

    // See http://trac.osgeo.org/geos/ticket/703
    ASSERT_TRUE(empty.equals(&empty)) << "Empty envelope should equal itself";

    ASSERT_FALSE(empty.equals(&zero));
    ASSERT_FALSE(zero.equals(&empty));

    ASSERT_TRUE(zero.equals(&zero2));
    ASSERT_TRUE(zero2.equals(&zero));

    ASSERT_FALSE(box.equals(&empty));
    ASSERT_FALSE(box.equals(&zero));

    ASSERT_FALSE(inf.isfinite());

    ensureNoFpExcept();
}

// 6 - Test of contains()
TEST_F(TFHEEnvelopeTest, Contains) {
    TFHEEnvelope empty;
    TFHEEnvelope small(-1, 1, -1, 1);
    TFHEEnvelope big(-5, 5, -5, 5);

    ASSERT_TRUE(empty.isNull());
    ASSERT_FALSE(small.isNull());
    ASSERT_FALSE(big.isNull());

    // Test empty envelope by reference
    ASSERT_FALSE(empty.contains(small)) << "empty envelope should not contain a non-empty envelope";
    ASSERT_FALSE(small.contains(empty)) << "non-empty envelope should not contain an empty envelope";
    ASSERT_FALSE(empty.contains(empty)) << "empty envelope should not contain itself";

    // Test empty envelope by pointer
    ASSERT_FALSE(empty.contains(&small));
    ASSERT_FALSE(small.contains(&empty));
    ASSERT_FALSE(empty.contains(&empty));

    // Test non-empty envelope by reference
    ASSERT_FALSE(small.contains(big)) << "small envelope should not contain big envelope";
    ASSERT_TRUE(big.contains(small)) << "big envelope should contain small envelope";
    ASSERT_TRUE(big.contains(big)) << "non-empty envelope should contain itself";

    // Test points
    checkIntersects(small, TFHECoordinate(0, 0), true);
    checkIntersects(small, TFHECoordinate(-1, -1), true);
    checkIntersects(small, TFHECoordinate(5, 5), false);

    checkIntersects(empty, TFHECoordinate(0, 0), false);
    ensureNoFpExcept();
}

// 7 - Test of intersects() and disjoint()
TEST_F(TFHEEnvelopeTest, IntersectsAndDisjoint) {
    constexpr std::size_t nrow = 3;
    constexpr std::size_t ncol = 3;
    std::array<std::array<TFHEEnvelope, nrow>, ncol> envelopes; // [col][row]

    double xmin = 0.0;
    double xmax = 1.0;
    double ymin = 0.0;
    double ymax = 2.0;
    double dx = (xmax - xmin) / static_cast<double>(ncol);
    double dy = (ymax - ymin) / static_cast<double>(nrow);
    for (std::size_t i = 0; i < ncol; i++) {
        for (std::size_t j = 0; j < nrow; j++) {
            double x0 = xmin + static_cast<double>(i) * dx;
            double x1 = xmin + static_cast<double>(i + 1) * dx;
            double y0 = ymax - static_cast<double>(j + 1) * dy;
            double y1 = ymax - static_cast<double>(j) * dy;
            envelopes[i][j] = TFHEEnvelope(x0, x1, y0, y1);
        }
    }
    TFHEEnvelope empty;
    // check intersection against empty
    for (std::size_t ia = 0; ia < ncol; ia++) {
        for (std::size_t ja = 0; ja < nrow; ja++) {
            checkIntersects(envelopes[ia][ja], empty, false);
        }
    }
    // check intersection against adjacent envelopes
    for (std::size_t ia = 0; ia < ncol; ia++) {
        for (std::size_t ja = 0; ja < nrow; ja++) {
            for (std::size_t ib = 0; ib < ncol; ib++) {
                for (std::size_t jb = 0; jb < nrow; jb++) {
                    bool shouldIntersect = (absDiff(ia, ib) <= 1) && (absDiff(ja, jb) <= 1);
                    checkIntersects(envelopes[ia][ja], envelopes[ib][jb], shouldIntersect);
                }
            }
        }
    }
    ensureNoFpExcept();
}

// 8 - Test of expandToInclude()
TEST_F(TFHEEnvelopeTest, ExpandToInclude) {
    TFHEEnvelope empty;
    TFHEEnvelope box(-5, 5, -5, 5); // includes (0,0)
    const TFHEEnvelope exemplar(-5, 5, -5, 5);

    ASSERT_TRUE(empty.isNull());
    ASSERT_FALSE(box.isNull());
    ASSERT_FALSE(exemplar.isNull());

    // Expand box envelope to include null envelope (should have no effect)
    box.expandToInclude(&empty);
    ASSERT_TRUE(box == exemplar);

    // Expand null envelope to include box envelope
    empty.expandToInclude(&box);
    ASSERT_TRUE(empty == exemplar);
    ensureNoFpExcept();
}

// 9 - Second test of expandToInclude()
TEST_F(TFHEEnvelopeTest, ExpandToInclude2) {
    TFHEEnvelope empty;
    TFHEEnvelope box(100, 101, 100, 101); // Does not include (0,0)
    const TFHEEnvelope exemplar(100, 101, 100, 101);

    // Expand box envelope to include null envelope (should have no effect)
    box.expandToInclude(&empty);
    ASSERT_TRUE(box == exemplar);

    // Expand empty envelope to include bigger envelope
    empty.expandToInclude(&box);
    ASSERT_TRUE(empty == exemplar);
    ensureNoFpExcept();
}

// 10 - Test point-to-envelope distance
TEST_F(TFHEEnvelopeTest, DistanceToCoordinate) {
    // Create a 5x5 grid of points
    std::vector<Coordinate> coords(25);
    for (std::size_t i = 0; i < coords.size(); i++) {
        coords[i].x = static_cast<double>(i % 5);
        coords[i].y = static_cast<double>(5 - (i / 5));
    }
    // point contained in envelope
    ASSERT_EQ(TFHEEnvelope::distanceToCoordinate(coords[18], coords[22], coords[9]), 0);
    ASSERT_EQ(TFHEEnvelope::distanceToCoordinate(coords[18], coords[14], coords[18]), 0);
    ASSERT_EQ(TFHEEnvelope::distanceToCoordinate(coords[18], coords[14], coords[17]), 0);
    ASSERT_EQ(TFHEEnvelope::distanceToCoordinate(coords[18], coords[19], coords[22]), 0);
    // envelope above point
    ASSERT_EQ(TFHEEnvelope::distanceToCoordinate(coords[17], coords[5], coords[4]), 2);
    // envelope below point
    ASSERT_EQ(TFHEEnvelope::distanceToCoordinate(coords[7], coords[20], coords[19]), 2);
    // envelope left of point
    ASSERT_EQ(TFHEEnvelope::distanceToCoordinate(coords[13], coords[20], coords[11]), 2);
    // envelope right of point
    ASSERT_EQ(TFHEEnvelope::distanceToCoordinate(coords[5], coords[9], coords[8]), 3);
    // envelope upper-left of point
    ASSERT_EQ(TFHEEnvelope::distanceToCoordinate(coords[17], coords[6], coords[0]),
              coords[17].distance(coords[6]));
    // envelope upper-right of point
    ASSERT_EQ(TFHEEnvelope::distanceToCoordinate(coords[21], coords[9], coords[13]),
              coords[21].distance(coords[13]));
    // envelope lower-left of point
    ASSERT_EQ(TFHEEnvelope::distanceToCoordinate(coords[3], coords[10], coords[21]),
              coords[3].distance(coords[11]));
    // envelope lower-right of point
    ASSERT_EQ(TFHEEnvelope::distanceToCoordinate(coords[6], coords[12], coords[14]),
              coords[6].distance(coords[12]));
    ensureNoFpExcept();
}

// 11 - Test envelope distance
TEST_F(TFHEEnvelopeTest, TFHEEnvelopeDistance) {
    // b touches a
    TFHEEnvelope a({0, 0}, {5, 5});
    TFHEEnvelope b({5, 5}, {10, 10});
    ASSERT_EQ(a.distance(b), 0);
    ASSERT_EQ(a.distance(b), b.distance(a));

    // b within a
    a = TFHEEnvelope({0, 0}, {10, 10});
    b = TFHEEnvelope({3, 3}, {3, 3});
    ASSERT_EQ(a.distance(b), 0);
    ASSERT_EQ(a.distance(b), b.distance(a));

    // b overlaps a
    a = TFHEEnvelope({0, 0}, {5, 5});
    b = TFHEEnvelope({2, 2}, {8, 8});
    ASSERT_EQ(a.distance(b), 0);
    ASSERT_EQ(a.distance(b), b.distance(a));

    // b above a
    a = TFHEEnvelope({2, 3}, {5, 7});
    b = TFHEEnvelope({0, 10}, {10, 20});
    ASSERT_EQ(a.distance(b), 3);
    ASSERT_EQ(a.distance(b), b.distance(a));

    // b right of a
    a = TFHEEnvelope({2, 3}, {5, 7});
    b = TFHEEnvelope({9, 4}, {11, 12});
    ASSERT_EQ(a.distance(b), 4);
    ASSERT_EQ(a.distance(b), b.distance(a));

    // b above and right of a
    a = TFHEEnvelope({0, 0}, {5, 7});
    b = TFHEEnvelope({9, 13}, {12, 28});
    ASSERT_EQ(a.distance(b), Coordinate(5, 7).distance(Coordinate(9, 13)));
    ASSERT_EQ(a.distance(b), b.distance(a));

    // b below and right of a
    a = TFHEEnvelope({10, 11}, {13, 28});
    b = TFHEEnvelope({17, 3}, {20, 5});
    ASSERT_EQ(a.distance(b), Coordinate(13, 11).distance(Coordinate(17, 5)));
    ASSERT_EQ(a.distance(b), b.distance(a));
    ensureNoFpExcept();
}

// 12 - Comparison of empty envelopes
TEST_F(TFHEEnvelopeTest, EmptyTFHEEnvelopeComparison) {
    TFHEEnvelope empty1;
    TFHEEnvelope empty2;
    ASSERT_FALSE(empty1 < empty2);
    ASSERT_FALSE(empty2 < empty1);
    ensureNoFpExcept();
}

// 13 - TFHEEnvelope::intersects(Coordinate, Coordinate)
TEST_F(TFHEEnvelopeTest, IntersectsCoordinates) {
    TFHEEnvelope empty;
    ASSERT_FALSE(empty.intersects({1, 1}, {2, 2}));
    ensureNoFpExcept();
}

// 14 - Test of expandBy
TEST_F(TFHEEnvelopeTest, ExpandBy) {
    // expanding null envelope is still null
    TFHEEnvelope empty;
    empty.expandBy(10, 10);
    ASSERT_TRUE(empty.isNull());

    // expanding a regular envelope gives expected result
    TFHEEnvelope e(0, 1, -2, 2);
    e.expandBy(2, 1);
    ASSERT_EQ(e.getMinX(), -2);
    ASSERT_EQ(e.getMaxX(), 3);
    ASSERT_EQ(e.getMinY(), -3);
    ASSERT_EQ(e.getMaxY(), 3);

    // expanding envelope by negative amount shrinks it
    e.expandBy(-2, -1);
    ASSERT_TRUE(e == TFHEEnvelope(0, 1, -2, 2));

    // shrinking it until it disappears makes it null
    e.expandBy(-100, -100);
    ASSERT_TRUE(e.isNull());
    ensureNoFpExcept();
}

// 15 - Test of intersection (calculating intersection envelope)
TEST_F(TFHEEnvelopeTest, Intersection) {
    TFHEEnvelope a(0, 1, 0, 1);
    TFHEEnvelope b(1, 2, 1, 1);
    TFHEEnvelope c;
    TFHEEnvelope d(100, 200, 100, 200);

    // A - B
    TFHEEnvelope abIntersection;
    bool abIntersect = a.intersection(b, abIntersection);
    ASSERT_TRUE(abIntersect);
    ASSERT_TRUE(abIntersection == TFHEEnvelope(1, 1, 1, 1));

    // A - C
    TFHEEnvelope acIntersection;
    bool acIntersect = a.intersection(c, acIntersection);
    ASSERT_FALSE(acIntersect);
    ASSERT_TRUE(acIntersection.isNull());

    // A - D (Note: original test calls a.intersection(c, adIntersection) with c, not d,
    // assuming it was a typo; so testing with d)
    TFHEEnvelope adIntersection;
    bool adIntersect = a.intersection(d, adIntersection);
    ASSERT_FALSE(adIntersect);
    ASSERT_TRUE(adIntersection.isNull());

    // B - C
    TFHEEnvelope bcIntersection;
    bool bcIntersect = a.intersection(c, bcIntersection);
    ASSERT_FALSE(bcIntersect);
    ASSERT_TRUE(bcIntersection.isNull());

    ensureNoFpExcept();
}

// 16 - Test of centre
TEST_F(TFHEEnvelopeTest, Centre) {
    TFHEEnvelope e(0, 1, 2, 4);
    TFHECoordinate center;
    bool success = e.centre(center);
    ASSERT_TRUE(success);
    ASSERT_EQ(center, TFHECoordinate(0.5, 3));

    // null envelope
    TFHEEnvelope empty;
    success = empty.centre(center);
    ASSERT_FALSE(success);
    // center remains unchanged; original test expected it to be {0.5,3} from previous call.
    ASSERT_EQ(center, TFHECoordinate(0.5, 3));

    ensureNoFpExcept();
}

// 17 - Test of translate
TEST_F(TFHEEnvelopeTest, Translate) {
    TFHEEnvelope e(0, 1, 2, 4);
    e.translate(1, 2);
    ASSERT_TRUE(e == TFHEEnvelope(1, 2, 4, 6));

    TFHEEnvelope empty;
    empty.translate(1, 2);
    ASSERT_TRUE(empty.isNull());
    ensureNoFpExcept();
}

// 18 - Test of hashCode
TEST_F(TFHEEnvelopeTest, HashCode) {
    TFHEEnvelope a(0, 1, 2, 3);
    TFHEEnvelope b;
    ASSERT_NE(a.hashCode(), b.hashCode());
    ensureNoFpExcept();
}

// 19 - Test of expandToInclude(Coordinate)
TEST_F(TFHEEnvelopeTest, ExpandToIncludeCoordinate) {
    TFHEEnvelope e;
    e.expandToInclude(TFHECoordinate(6, 7));
    ASSERT_TRUE(e == TFHEEnvelope(6, 6, 7, 7));

    e.expandToInclude(0, 1);
    ASSERT_TRUE(e == TFHEEnvelope(0, 6, 1, 7));
    ensureNoFpExcept();
}

// 20 - Test of HashCode in unordered_set
TEST_F(TFHEEnvelopeTest, HashCodeUsage) {
    std::unordered_set<TFHEEnvelope, TFHEEnvelope::HashCode> set;
    set.emplace(0, 1, 0, 1);
    set.emplace(1, 0, 1, 0);
    set.emplace(2, 0, 2, 1);
    // The original test expects set.size() to equal 2.
    ASSERT_EQ(set.size(), 2u);
    ensureNoFpExcept();
}
