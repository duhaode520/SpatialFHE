//
// Created by ubuntu on 2/5/25.
//

#ifndef TFHERELATENGTEST_H
#define TFHERELATENGTEST_H
// geos
#include <tfhe_geos/algorithm/BoundaryNodeRule.h>
#include <tfhe_geos/geom/TFHEGeometry.h>
#include <tfhe_geos/operation/relateng/TFHERelateNG.h>
#include <tfhe_geos/operation/relateng/TFHERelatePredicate.h>
#include <tfhe_geos/operation/relateng/TFHERelateMatrixPredicate.h>
#include <tfhe_geos/geom/TFHEGeometryFactory.h>
#include <tfhe_geos/io/WKTReader.h>

// std
#include <memory>
#include <gtest/gtest.h>

using namespace SpatialFHE::geom;
using namespace SpatialFHE::operation::relateng;
using SpatialFHE::io::WKTReader;

class RelateNGTest : public ::testing::Test {
protected:

    // void checkPrepared(const std::string& wkta, const std::string& wktb) {
    //     std::unique_ptr<Geometry> a = r.read(wkta);
    //     std::unique_ptr<Geometry> b = r.read(wktb);
    //     checkPrepared(a.get(), b.get());
    // }

    /**
     * A DE-9IM pattern to detect whether two polygonal geometries are adjacent along
     * an edge, but do not overlap.
     */
    static constexpr const char* ADJACENT = "F***1****";

    /**
     * A DE-9IM pattern to detect a geometry which properly contains another
     * geometry (i.e. which lies entirely in the interior of the first geometry).
     */
    static constexpr const char* CONTAINS_PROPERLY = "T**FF*FF*";

    /**
     * A DE-9IM pattern to detect if two geometries intersect in their interiors.
     * This can be used to determine if a polygonal coverage contains any overlaps
     * (although not whether they are correctly noded).
     */
    static constexpr const char* INTERIOR_INTERSECTS = "T********";

    TFHEGeometryFactory::Ptr factory_;
    WKTReader r;
    std::unique_ptr<SpatialFHE::TFHEContext> context;

    void SetUp() override {
        factory_ = TFHEGeometryFactory::create();
        context = std::make_unique<SpatialFHE::TFHEContext>();
    }

    void checkPrepared(const TFHEGeometry* a, const TFHEGeometry* b) {
        // std::cout << "checkPrepared is disabled as prepared geometry and indices have not been implemented" << std::endl;
        // auto prep_a = TFHERelateNG::prepare(a);
        // auto prep_b = TFHERelateNG::prepare(b);
        // EXPECT_EQ(prep_a->equalsTopo(b).decrypt(), a->equals(b).decrypt()) << "preparedEqualsTopo";
        // EXPECT_EQ(prep_a->intersects(b).decrypt(), a->intersects(b).decrypt()) << "preparedIntersects";
        // EXPECT_EQ(prep_a->disjoint(b).decrypt(), a->disjoint(b).decrypt()) << "preparedDisjoint";
        // EXPECT_EQ(prep_a->covers(b).decrypt(), a->covers(b).decrypt()) << "preparedCovers";
        // EXPECT_EQ(prep_a->coveredBy(b).decrypt(), a->coveredBy(b).decrypt()) << "preparedCoveredby";
        // EXPECT_EQ(prep_a->within(b).decrypt(), a->within(b).decrypt()) << "preparedWithin";
        // EXPECT_EQ(prep_a->contains(b).decrypt(), a->contains(b).decrypt()) << "preparedContains";
        // EXPECT_EQ(prep_a->crosses(b).decrypt(), a->crosses(b).decrypt()) << "preparedCrosses";
        // EXPECT_EQ(prep_a->touches(b).decrypt(), a->touches(b).decrypt()) << "preparedTouches";
        // EXPECT_EQ(prep_a->relate(b)->toString(), a->relate(b)->toString()) << "preparedRelate";
    }

    void checkIntersectsDisjoint(const std::string& wkta, const std::string& wktb, bool expectedValue) {
        checkPredicate(*TFHERelatePredicate::intersects(), wkta, wktb, expectedValue);
        checkPredicate(*TFHERelatePredicate::intersects(), wktb, wkta, expectedValue);
        checkPredicate(*TFHERelatePredicate::disjoint(), wkta, wktb, !expectedValue);
        checkPredicate(*TFHERelatePredicate::disjoint(), wktb, wkta, !expectedValue);
    }

    void checkContainsWithin(const std::string& wkta, const std::string& wktb, bool expectedValue) {
        checkPredicate(*TFHERelatePredicate::contains(), wkta, wktb, expectedValue);
        checkPredicate(*TFHERelatePredicate::within(), wktb, wkta, expectedValue);
    }

    void checkCoversCoveredBy(const std::string& wkta, const std::string& wktb, bool expectedValue) {
        checkPredicate(*TFHERelatePredicate::covers(), wkta, wktb, expectedValue);
        checkPredicate(*TFHERelatePredicate::coveredBy(), wktb, wkta, expectedValue);
    }

    void checkCrosses(const std::string& wkta, const std::string& wktb, bool expectedValue) {
        checkPredicate(*TFHERelatePredicate::crosses(), wkta, wktb, expectedValue);
        checkPredicate(*TFHERelatePredicate::crosses(), wktb, wkta, expectedValue);
    }

    void checkOverlaps(const std::string& wkta, const std::string& wktb, bool expectedValue) {
        checkPredicate(*TFHERelatePredicate::overlaps(), wkta, wktb, expectedValue);
        checkPredicate(*TFHERelatePredicate::overlaps(), wktb, wkta, expectedValue);
    }

    void checkTouches(const std::string& wkta, const std::string& wktb, bool expectedValue) {
        checkPredicate(*TFHERelatePredicate::touches(), wkta, wktb, expectedValue);
        checkPredicate(*TFHERelatePredicate::touches(), wktb, wkta, expectedValue);
    }

    void checkEquals(const std::string& wkta, const std::string& wktb, bool expectedValue) {
        checkPredicate(*TFHERelatePredicate::equalsTopo(), wkta, wktb, expectedValue);
        checkPredicate(*TFHERelatePredicate::equalsTopo(), wktb, wkta, expectedValue);
    }

    void checkRelate(const std::string& wkta, const std::string& wktb, const std::string expectedValue) {
        std::unique_ptr<TFHEGeometry> a = r.read(wkta);
        std::unique_ptr<TFHEGeometry> b = r.read(wktb);
        TFHERelateMatrixPredicate pred;
        TFHERelateNG::relate(a.get(), b.get(), pred);
        std::string actualVal = pred.getIM()->toString();
        EXPECT_EQ(actualVal, expectedValue) << "checkRelate";
        checkPrepared(a.get(), b.get());
    }

    void checkRelateMatches(const std::string& wkta, const std::string& wktb, const std::string pattern, bool expectedValue) {
        auto pred = TFHERelatePredicate::matches(pattern);
        checkPredicate(*pred, wkta, wktb, expectedValue);
    }

    void checkPredicate(TFHETopologyPredicate& pred, const std::string& wkta, const std::string& wktb, bool expectedValue) {
        std::unique_ptr<TFHEGeometry> a = r.read(wkta);
        std::unique_ptr<TFHEGeometry> b = r.read(wktb);
        bool actualVal = TFHERelateNG::relate(a.get(), b.get(), pred).decrypt();
        EXPECT_EQ(actualVal, expectedValue) << "checkPredicate " << pred.name();
        checkPrepared(a.get(), b.get());
    }
};
#endif //TFHERELATENGTEST_H
