//
// Created by ubuntu on 2/5/25.
//
#include "TFHERelateNGTest.h"

// 测试点与点不相交
TEST_F(RelateNGTest, testPointsDisjoint) {

    std::string a = "POINT (0 0)";
    std::string b = "POINT (10 10)";
    checkIntersectsDisjoint(a, b, false);
    checkContainsWithin(a, b, false);
    checkEquals(a, b, false);
    checkRelate(a, b, "FF0FFF0F2");
}


// 测试点相等
TEST_F(RelateNGTest, testPointsEqual) {
    std::string a = "POINT (0 0)";
    std::string b = "POINT (0 0)";
    // checkIntersectsDisjoint(a, b, true);
    // checkContainsWithin(a, b, true);
    checkEquals(a, b, true);
}


// 测试线交叉
TEST_F(RelateNGTest, testLinesCrossProper) {
    std::string a = "LINESTRING (0 0, 900 900)";
    std::string b = "LINESTRING(0 900, 900 0)";
    checkIntersectsDisjoint(a, b, true);
    checkContainsWithin(a, b, false);
}
//
// 测试线重叠
TEST_F(RelateNGTest, testLinesOverlap) {
    std::string a = "LINESTRING (0 0, 500 500)";
    std::string b = "LINESTRING(300 300, 900 900)";
    checkIntersectsDisjoint(a, b, true);
    checkTouches(a, b, false);
    checkOverlaps(a, b, true);
}
//
// 测试线在顶点交叉
TEST_F(RelateNGTest, testLinesCrossVertex) {
    std::string a = "LINESTRING (0 0, 800 800)";
    std::string b = "LINESTRING(0 800, 400 400, 800 0)";
    checkIntersectsDisjoint(a, b, true);
}
//
// 测试线在顶点接触
TEST_F(RelateNGTest, testLinesTouchVertex) {
    std::string a = "LINESTRING (0 0, 800 0)";
    std::string b = "LINESTRING(0 800, 400 0, 800 800)";
    checkIntersectsDisjoint(a, b, true);
}
//
// 测试线不相交（通过包围盒）
TEST_F(RelateNGTest, testLinesDisjointByEnvelope) {
    std::string a = "LINESTRING (0 0, 900 900)";
    std::string b = "LINESTRING(1000 1900, 1900 1000)";
    checkIntersectsDisjoint(a, b, false);
    checkContainsWithin(a, b, false);
}
//
// 测试线不相交
TEST_F(RelateNGTest, testLinesDisjoint) {
    std::string a = "LINESTRING (0 0, 900 900)";
    std::string b = "LINESTRING (400 200, 800 600)";
    checkIntersectsDisjoint(a, b, false);
    checkContainsWithin(a, b, false);
}
//
// 测试闭合线与空线
TEST_F(RelateNGTest, testLinesClosedEmpty) {
    std::string a = "LINESTRING (0 0, 0 100, 100 100, 100 0, 0 0)";
    std::string b = "LINESTRING EMPTY";
    checkRelate(a, b, "FF0FFFFF2");
    checkIntersectsDisjoint(a, b, false);
    checkContainsWithin(a, b, false);
}
//
// 测试线在节点接触
TEST_F(RelateNGTest, testLinesRingTouchAtNode) {
    std::string a = "LINESTRING (500 500, 100 800, 100 100, 500 500)";
    std::string b = "LINESTRING (500 500, 900 500)";
    checkRelate(a, b, "F01FFF102");
    checkIntersectsDisjoint(a, b, true);
    checkContainsWithin(a, b, false);
    checkTouches(a, b, true);
}

// 测试线在边界接触
TEST_F(RelateNGTest, testLinesTouchAtBdy) {
    std::string a = "LINESTRING (500 500, 100 800)";
    std::string b = "LINESTRING (500 500, 900 500)";
    checkRelate(a, b, "FF1F00102");
    checkIntersectsDisjoint(a, b, true);
    checkContainsWithin(a, b, false);
    checkTouches(a, b, true);
}
//
// 测试线不相交但包围盒重叠
TEST_F(RelateNGTest, testLinesDisjointOverlappingEnvelopes) {
    std::string a = "LINESTRING (600 0, 200 800, 1000 800, 800 1200, 400 1400)";
    std::string b = "LINESTRING (600 400, 1400 400, 1400 1600, 0 1600)";
    checkRelate(a, b, "FF1FF0102");
    checkIntersectsDisjoint(a, b, false);
    checkContainsWithin(a, b, false);
    checkTouches(a, b, false);
}


// 测试线包含（JTS 396）
TEST_F(RelateNGTest, testLinesContained_JTS396) {
    std::string a = "LINESTRING (100 0, 0 200, 0 0, 200 200)";
    std::string b = "LINESTRING (0 0, 200 200)";
    checkIntersectsDisjoint(a, b, true);
    checkContainsWithin(a, b, true);
    checkCoversCoveredBy(a, b, true);
    checkCrosses(a, b, false);
    checkOverlaps(a, b, false);
    checkTouches(a, b, false);
}

// 测试线包含自交线
TEST_F(RelateNGTest, testLinesContainedWithSelfIntersection) {
    std::string a = "LINESTRING (200 0, 0 200, 0 0, 200 200)";
    std::string b = "LINESTRING (0 0, 200 200)";
    checkContainsWithin(a, b, true);
    checkCoversCoveredBy(a, b, true);
    checkCrosses(a, b, false);
    checkOverlaps(a, b, false);
    checkTouches(a, b, false);
}

// 测试线包含在环中
TEST_F(RelateNGTest, testLineContainedInRing) {
    std::string a = "LINESTRING(600 600, 1000 1000, 1400 600)";
    std::string b = "LINESTRING(1000 1000, 1800 200, 200 200, 1000 1000)";
    checkIntersectsDisjoint(a, b, true);
    checkContainsWithin(b, a, true);
    checkCoversCoveredBy(b, a, true);
    checkCrosses(a, b, false);
    checkOverlaps(a, b, false);
    checkTouches(a, b, false);
}

// 测试线自交共线
TEST_F(RelateNGTest, testLineSelfIntersectionCollinear) {
    std::string a = "LINESTRING (900 600, 100 600, 100 0, 500 600, 900 600)";
    std::string b = "LINESTRING (900 900, 300 100)";
    checkRelate(a, b, "0F1FFF102");
}

// 测试多边形与点内部
TEST_F(RelateNGTest, testPolygonPointInside) {
    std::string a = "POLYGON ((0 1000, 1000 1000, 1000 0, 0 0, 0 1000))";
    std::string b = "POINT (100 100)";
    checkIntersectsDisjoint(a, b, true);
    checkContainsWithin(a, b, true);
}

// 测试多边形与点外部
TEST_F(RelateNGTest, testPolygonPointOutside) {
    std::string a = "POLYGON ((1000 0, 0 0, 0 1000, 1000 0))";
    std::string b = "POINT (800 800)";
    checkIntersectsDisjoint(a, b, false);
    checkContainsWithin(a, b, false);
}

// 测试多边形与点在边界
TEST_F(RelateNGTest, testPolygonPointInBoundary) {
    std::string a = "POLYGON ((1000 0, 0 0, 0 1000, 1000 0))";
    std::string b = "POINT (100 0)";
    checkIntersectsDisjoint(a, b, true);
    checkContainsWithin(a, b, false);
    checkCoversCoveredBy(a, b, true);
}

// 测试区域与点在外部
TEST_F(RelateNGTest, testAreaPointInExterior) {
    std::string a = "POLYGON ((100 500, 500 500, 500 100, 100 100, 100 500))";
    std::string b = "POINT (700 700)";
    checkRelate(a, b, "FF2FF10F2");
    checkIntersectsDisjoint(a, b, false);
    checkContainsWithin(a, b, false);
    checkCoversCoveredBy(a, b, false);
    checkTouches(a, b, false);
    checkOverlaps(a, b, false);
}

// 测试区域与线在顶点包含
TEST_F(RelateNGTest, testAreaLineContainedAtLineVertex) {
    std::string a = "POLYGON ((100 500, 500 500, 500 100, 100 100, 100 500))";
    std::string b = "LINESTRING (200 300, 300 500, 400 300)";
    checkIntersectsDisjoint(a, b, true);
    checkTouches(a, b, false);
    checkOverlaps(a, b, false);
}
// Test 34: testPolygonLineInside
TEST_F(RelateNGTest, TestPolygonLineInside) {
    std::string a = "POLYGON ((0 1000, 1000 1000, 1000 0, 0 0, 0 1000))";
    std::string b = "LINESTRING (100 800, 300 500, 500 800)";
    checkRelate(a, b, "102FF1FF2");
    checkIntersectsDisjoint(a, b, true);
    checkContainsWithin(a, b, true);
}

// Test 35: testPolygonLineOutside
TEST_F(RelateNGTest, TestPolygonLineOutside) {
    std::string a = "POLYGON ((1000 0, 0 0, 0 1000, 10 0))";
    std::string b = "LINESTRING (400 800, 900 300)";
    checkIntersectsDisjoint(a, b, false);
    checkContainsWithin(a, b, false);
}

// Test 36: testPolygonLineInBoundary
TEST_F(RelateNGTest, TestPolygonLineInBoundary) {
    std::string a = "POLYGON ((1000 0, 0 0, 0 1000, 1000 0))";
    std::string b = "LINESTRING (100 0, 900 0)";
    checkIntersectsDisjoint(a, b, true);
    checkContainsWithin(a, b, false);
    checkCoversCoveredBy(a, b, true);
    checkTouches(a, b, true);
    checkOverlaps(a, b, false);
}

// Test 37: testPolygonLineCrossingContained
TEST_F(RelateNGTest, TestPolygonLineCrossingContained) {
    std::string a = "POLYGON ((200 800, 1800 800, 1000 0, 200 800))";
    std::string b = "LINESTRING (1000 1400, 1000 400)";
    checkRelate(a, b, "1020F1102");
    checkIntersectsDisjoint(a, b, true);
    checkContainsWithin(a, b, false);
    checkCoversCoveredBy(a, b, false);
    checkTouches(a, b, false);
    checkOverlaps(a, b, false);
}

// Test 38: testValidateRelateLA_220
TEST_F(RelateNGTest, TestValidateRelateLA_220) {
    std::string a = "LINESTRING (900 2100, 2100 900)";
    std::string b = "POLYGON ((1500 1500, 4100 1500, 2800 200, 200 200, 1500 1500))";
    checkIntersectsDisjoint(a, b, true);
    checkContainsWithin(a, b, false);
    checkCoversCoveredBy(a, b, false);
    checkTouches(a, b, false);
    checkOverlaps(a, b, false);
}

// Test 39: testLineCrossingPolygonAtShellHolePoint
TEST_F(RelateNGTest, TestLineCrossingPolygonAtShellHolePoint) {
    std::string a = "LINESTRING (60 160, 150 70)";
    std::string b = "POLYGON ((190 190, 360 20, 20 20, 190 190), (110 110, 250 100, 140 30, 110 110))";
    checkRelate(a, b, "F01FF0212");
    checkTouches(a, b, true);
    // checkIntersectsDisjoint(a, b, true);
    // checkContainsWithin(a, b, false);
    // checkCoversCoveredBy(a, b, false);
    // checkOverlaps(a, b, false);
}

// Test 40: testLineCrossingPolygonAtNonVertex
TEST_F(RelateNGTest, TestLineCrossingPolygonAtNonVertex) {
    std::string a = "LINESTRING (200 600, 1500 600)";
    std::string b = "POLYGON ((1500 1500, 4100 1500, 2800 200, 200 200, 1500 1500))";
    checkIntersectsDisjoint(a, b, true);
    checkContainsWithin(a, b, false);
    checkCoversCoveredBy(a, b, false);
    checkTouches(a, b, false);
    checkOverlaps(a, b, false);
}

// Test 41: testPolygonLinesContainedCollinearEdge
TEST_F(RelateNGTest, TestPolygonLinesContainedCollinearEdge) {
    std::string a = "POLYGON ((1100 1100, 2000 200, 200 200, 1100 1100))";
    std::string b = "LINESTRING (1100 1100, 600 400, 700 200, 1500 200, 1700 400)";
    checkRelate(a, b, "102101FF2");
}

// Test 42: testPolygonsEdgeAdjacent
TEST_F(RelateNGTest, TestPolygonsEdgeAdjacent) {
    std::string a = "POLYGON ((100 300, 300 300, 300 100, 100 100, 100 300))";
    std::string b = "POLYGON ((500 300, 500 100, 300 100, 300 300, 500 300))";
    // checkIntersectsDisjoint(a, b, true);
    checkOverlaps(a, b, false);
    checkTouches(a, b, true);
}

// Test 43: testPolygonsEdgeAdjacent2
TEST_F(RelateNGTest, TestPolygonsEdgeAdjacent2) {
    std::string a = "POLYGON ((100 300, 400 300, 300 0, 100 100, 100 300))";
    std::string b = "POLYGON ((500 300, 500 100, 300 0, 400 300, 500 300))";
    // checkIntersectsDisjoint(a, b, true);
    checkOverlaps(a, b, false);
    checkTouches(a, b, true);
}

// Test 44: testPolygonsNested
TEST_F(RelateNGTest, TestPolygonsNested) {
    std::string a = "POLYGON ((100 900, 900 900, 900 100, 100 100, 100 900))";
    std::string b = "POLYGON ((200 800, 800 800, 800 200, 200 200, 200 800))";
    checkIntersectsDisjoint(a, b, true);
    checkContainsWithin(a, b, true);
    checkCoversCoveredBy(a, b, true);
    checkOverlaps(a, b, false);
    checkTouches(a, b, false);
}

// Test 45: testPolygonsOverlapProper
TEST_F(RelateNGTest, TestPolygonsOverlapProper) {
    std::string a = "POLYGON ((100 100, 100 700, 700 700, 700 100, 100 100))";
    std::string b = "POLYGON ((200 800, 800 800, 800 200, 200 200, 200 800))";
    checkIntersectsDisjoint(a, b, true);
    checkContainsWithin(a, b, false);
    checkCoversCoveredBy(a, b, false);
    checkOverlaps(a, b, true);
    checkTouches(a, b, false);
}

// Test 46: testPolygonsOverlapAtNodes
TEST_F(RelateNGTest, TestPolygonsOverlapAtNodes) {
    std::string a = "POLYGON ((100 500, 500 500, 500 100, 100 100, 100 500))";
    std::string b = "POLYGON ((700 300, 500 100, 300 300, 500 500, 700 300))";
    checkIntersectsDisjoint(a, b, true);
    checkContainsWithin(a, b, false);
    checkCoversCoveredBy(a, b, false);
    checkOverlaps(a, b, true);
    checkTouches(a, b, false);
}

// Test 47: testPolygonsContainedAtNodes
TEST_F(RelateNGTest, TestPolygonsContainedAtNodes) {
    std::string a = "POLYGON ((100 500, 500 500, 600 200, 100 100, 100 500))";
    std::string b = "POLYGON ((100 100, 500 500, 600 200, 100 100))";
    // checkIntersectsDisjoint(a, b, true);
    checkContainsWithin(a, b, true);
    checkCoversCoveredBy(a, b, true);
    checkOverlaps(a, b, false);
    checkTouches(a, b, false);
}

// Test 48: testPolygonsNestedWithHole
TEST_F(RelateNGTest, TestPolygonsNestedWithHole) {
    std::string a = "POLYGON ((40 60, 420 60, 420 320, 40 320, 40 60), (200 140, 160 220, 260 200, 200 140))";
    std::string b = "POLYGON ((80 100, 360 100, 360 280, 80 280, 80 100))";
    // checkIntersectsDisjoint(a, b, true);
    checkContainsWithin(a, b, false);
    checkContainsWithin(b, a, false);
    // checkCoversCoveredBy(a, b, false);
    // checkOverlaps(a, b, true);
    // checkTouches(a, b, false);
    checkPredicate(*TFHERelatePredicate::contains(), a, b, false);
    // checkTouches(a, b, false);
}

// Test 49: testPolygonsOverlappingWithBoundaryInside
// TEST_F(RelateNGTest, TestPolygonsOverlappingWithBoundaryInside) {
//     std::string a = "POLYGON ((100 60, 140 100, 100 140, 60 100, 100 60))";
//     std::string b = "MULTIPOLYGON (((80 40, 120 40, 120 80, 80 80, 80 40)), ((120 80, 160 80, 160 120, 120 120, 120 80)), ((80 120, 120 120, 120 160, 80 160, 80 120)), ((40 80, 80 80, 80 120, 40 120, 40 80)))";
//     checkRelate(a, b, "21210F212");
//     checkIntersectsDisjoint(a, b, true);
//     checkContainsWithin(a, b, false);
//     checkContainsWithin(b, a, false);
//     checkCoversCoveredBy(a, b, false);
//     checkOverlaps(a, b, true);
//     checkTouches(a, b, false);
// }

// Test 51: testValidateRelateAA_86
TEST_F(RelateNGTest, TestValidateRelateAA_86) {
    std::string a = "POLYGON ((170 120, 300 120, 250 70, 120 70, 170 120))";
    std::string b = "POLYGON ((150 150, 410 150, 280 20, 20 20, 150 150), (170 120, 330 120, 260 50, 100 50, 170 120))";
    checkIntersectsDisjoint(a, b, true);
    checkContainsWithin(a, b, false);
    checkCoversCoveredBy(a, b, false);
    checkOverlaps(a, b, false);
    checkPredicate(*TFHERelatePredicate::within(), a, b, false);
    checkTouches(a, b, true);
}

// Test 52: testValidateRelateAA_97
TEST_F(RelateNGTest, TestValidateRelateAA_97) {
    std::string a = "POLYGON ((330 150, 200 110, 150 150, 280 190, 330 150))";
    std::string b = "POLYGON ((300 270, 420 270, 340 190, 220 190, 300 270))";
    checkIntersectsDisjoint(a, b, true);
    checkContainsWithin(a, b, false);
    checkCoversCoveredBy(a, b, false);
    checkOverlaps(a, b, false);
    checkPredicate(*TFHERelatePredicate::within(), a, b, false);
    checkTouches(a, b, true);
}

// Test 53: testAdjacentPolygons
TEST_F(RelateNGTest, TestAdjacentPolygons) {
    std::string a = "POLYGON ((100 900, 600 900, 600 100, 100 100, 100 900))";
    std::string b = "POLYGON ((900 900, 900 400, 600 400, 600 900, 900 900))";
    checkRelateMatches(a, b,ADJACENT, true);
}

// Test 54: testAdjacentPolygonsTouchingAtPoint
TEST_F(RelateNGTest, TestAdjacentPolygonsTouchingAtPoint) {
    std::string a = "POLYGON ((100 900, 600 900, 600 100, 100 100, 100 900))";
    std::string b = "POLYGON ((900 900, 900 400, 600 400, 700 900, 900 900))";
    checkRelateMatches(a, b, ADJACENT, false);
}

// Test 55: testAdjacentPolygonsOverlappping
TEST_F(RelateNGTest, TestAdjacentPolygonsOverlappping) {
    std::string a = "POLYGON ((100 900, 600 900, 600 100, 100 100, 100 900))";
    std::string b = "POLYGON ((900 900, 900 400, 600 400, 500 900, 900 900))";
    checkRelateMatches(a, b, ADJACENT, false);
}

// Test 56: testContainsProperlyPolygonContained
TEST_F(RelateNGTest, TestContainsProperlyPolygonContained) {
    std::string a = "POLYGON ((1 9, 9 9, 9 1, 1 1, 1 9))";
    std::string b = "POLYGON ((2 8, 5 8, 5 5, 2 5, 2 8))";
    checkRelateMatches(a, b, CONTAINS_PROPERLY, true);
}

// Test 57: testContainsProperlyPolygonTouching
TEST_F(RelateNGTest, TestContainsProperlyPolygonTouching) {
    std::string a = "POLYGON ((100 900, 900 900, 900 100, 100 100, 100 900))";
    std::string b = "POLYGON ((900 100, 500 100, 500 500, 900 500, 900 100))";
    checkRelateMatches(a, b, CONTAINS_PROPERLY, false);
}

// Test 59: testRepeatedPointLL
TEST_F(RelateNGTest, testRepeatedPointLL) {
    std::string a = "LINESTRING(0 0, 500 500, 500 500, 500 500, 900 900)";
    std::string b = "LINESTRING(0 900, 500 500, 500 500, 500 500, 900 0)";
    checkRelate(a, b, "0F1FF0102");
    checkIntersectsDisjoint(a, b, true);
}

// Test 60: testRepeatedPointAA
TEST_F(RelateNGTest, testRepeatedPointAA) {
    // FIXME: 有死循环
    std::string a = "POLYGON ((10 90, 90 70, 90 10, 10 30, 10 90))";
    std::string b = "POLYGON ((10 30, 10 30, 10 30, 30 70, 90 70, 90 70, 10 30))";
    checkRelate(a, b, "212F01FF2");
}

