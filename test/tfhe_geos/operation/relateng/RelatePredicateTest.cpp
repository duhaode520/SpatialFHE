//
// Created by ubuntu on 2/12/25.
//
#include <gtest/gtest.h>
#include <algorithm>
#include <array>
#include <string>
#include <memory>

// Include GEOS headers
#include <tfhe_geos/geom/enums.h>
#include <tfhe_geos/operation/relateng/TFHETopologyPredicate.h>
#include <tfhe_geos/operation/relateng/TFHERelatePredicate.h>


using namespace SpatialFHE::geom;
using namespace SpatialFHE::operation::relateng;

class RelatePredicateTest : public ::testing::Test {
protected:
    // Test data strings
    const std::string A_EXT_B_INT = "***.***.1**";
    const std::string A_INT_B_INT = "1**.***.***";

    std::unique_ptr<SpatialFHE::TFHEContext> context;

    void SetUp() override {
        context = std::make_unique<SpatialFHE::TFHEContext>();
    }
    // Checks the predicate after applying the intersection matrix specification.
    void checkPred(TFHETopologyPredicate& pred, bool expected) {
        pred.finish();
        bool actual = pred.value().decrypt();
        EXPECT_EQ(expected, actual);
    }

    // Applies the intersection matrix specification, then checks the predicate
    void checkPredicate(TFHETopologyPredicate& pred, const std::string& im, bool expected) {
        applyIM(im, pred);
        checkPred(pred, expected);
    }

    // Applies the intersection matrix specification and ensures the predicate value is known
    void checkPredicatePartial(TFHETopologyPredicate& pred, const std::string& im, bool expected) {
        applyIM(im, pred);
        bool isKnown = pred.isKnown();
        EXPECT_TRUE(isKnown) << "predicate value is not known";
        checkPred(pred, expected);
    }

    // Utility function to apply an intersection matrix string (IM) to the predicate.
    static void applyIM(const std::string& imIn, TFHETopologyPredicate& pred) {
        std::array<Location, 3> locs = { Location::INTERIOR, Location::BOUNDARY, Location::EXTERIOR };
        std::string im = cleanIM(imIn);
        uint32_t i = 0;
        for (Location locA : locs) {
            for (Location locB : locs) {
                char entry = im[i++];
                if (entry == '0' || entry == '1' || entry == '2') {
                    int dim = Dimension::toDimensionValue(entry);
                    pred.updateDimension(locA, locB, dim);
                }
            }
        }
    }

    // Utility function to remove dots from the IM string.
    static std::string cleanIM(const std::string& im) {
        std::string str = im;
        str.erase(std::remove(str.begin(), str.end(), '.'), str.end());
        return str;
    }
};

// Test for the intersects predicate.
TEST_F(RelatePredicateTest, Intersects) {
    // Create an intersects predicate using RelatePredicate.
    auto pred = TFHERelatePredicate::intersects();
    // Test using A_INT_B_INT which is expected to be true.
    checkPredicate(*pred, A_INT_B_INT, true);
}

// Test for disjoint predicate.
TEST_F(RelatePredicateTest, Disjoint) {
    // First, verify that using an intersects predicate with A_EXT_B_INT yields false.
    {
        auto pred = TFHERelatePredicate::intersects();
        checkPredicate(*pred, A_EXT_B_INT, false);
    }
    // Now, use the disjoint predicate which should yield true for A_EXT_B_INT.
    {
        auto pred = TFHERelatePredicate::disjoint();
        checkPredicate(*pred, A_EXT_B_INT, true);
    }
}

// Test for covers predicate.
TEST_F(RelatePredicateTest, Covers) {
    // Using covers predicate on A_INT_B_INT should yield true.
    {
        auto pred = TFHERelatePredicate::covers();
        checkPredicate(*pred, A_INT_B_INT, true);
    }
    // And using covers predicate on A_EXT_B_INT should yield false.
    {
        auto pred = TFHERelatePredicate::covers();
        checkPredicate(*pred, A_EXT_B_INT, false);
    }
}

// Test for covers predicate with partial knowledge.
TEST_F(RelatePredicateTest, CoversFast) {
    // Using partial evaluation on covers for A_EXT_B_INT is expected to yield false.
    auto pred = TFHERelatePredicate::covers();
    checkPredicatePartial(*pred, A_EXT_B_INT, false);
}

// Test for matching a specific pattern.
TEST_F(RelatePredicateTest, Match) {
    // The matches method takes a pattern and is expected to yield true when
    // applied with the given intersection matrix.
    auto pred = TFHERelatePredicate::matches("1***T*0**");
    checkPredicate(*pred, "1**.*2*.0**", true);
}