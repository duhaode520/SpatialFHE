//
// Created by ubuntu on 1/14/25.
//

#include "TFHERelatePredicate.h"

#include <memory>

#include "TFHEIMPatternMatcher.h"
#include "tfhe_geos/operation/relateng/TFHEBasicPredicate.h"

namespace SpatialFHE::operation::relateng {
    std::unique_ptr<TFHEBasicPredicate> TFHERelatePredicate::intersects() {
        return std::unique_ptr<TFHEBasicPredicate>(new IntersectsPredicate());
    }

    std::unique_ptr<TFHEBasicPredicate> TFHERelatePredicate::disjoint() {
        return std::unique_ptr<TFHEBasicPredicate>(new DisjointPredicate());
    }

    std::unique_ptr<TFHEIMPredicate> TFHERelatePredicate::contains() {
        return std::unique_ptr<TFHEIMPredicate>(new ContainsPredicate());
    }

    std::unique_ptr<TFHEIMPredicate> TFHERelatePredicate::within() {
        return std::unique_ptr<TFHEIMPredicate>(new WithinPredicate());
    }

    std::unique_ptr<TFHEIMPredicate> TFHERelatePredicate::covers() {
        return std::unique_ptr<TFHEIMPredicate>(new CoversPredicate());
    }

    std::unique_ptr<TFHEIMPredicate> TFHERelatePredicate::coveredBy() {
        return std::unique_ptr<TFHEIMPredicate>(new CoveredByPredicate());
    }

    std::unique_ptr<TFHEIMPredicate> TFHERelatePredicate::crosses() {
        return std::unique_ptr<TFHEIMPredicate>(new CrossesPredicate());
    }

    std::unique_ptr<TFHEIMPredicate> TFHERelatePredicate::equalsTopo() {
        return std::unique_ptr<TFHEIMPredicate>(new EqualsTopoPredicate());
    }

    std::unique_ptr<TFHEIMPredicate> TFHERelatePredicate::overlaps() {
        return std::unique_ptr<TFHEIMPredicate>(new OverlapsPredicate());
    }

    std::unique_ptr<TFHEIMPredicate> TFHERelatePredicate::touches() {
        return std::unique_ptr<TFHEIMPredicate>(new TouchesPredicate());
    }

    std::unique_ptr<TFHETopologyPredicate> TFHERelatePredicate::matches(const std::string &pattern) {
        return std::unique_ptr<TFHETopologyPredicate>(new TFHEIMPatternMatcher(pattern));
    }

}  // namespace SpatialFHE::operation::relateng