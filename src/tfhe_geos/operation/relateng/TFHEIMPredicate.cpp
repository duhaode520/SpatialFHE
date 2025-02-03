//
// Created by ubuntu on 1/15/25.
//

#include "TFHEIMPredicate.h"

#include <tfhe_geos/geom/TFHEIntersectionMatrix.h>

namespace SpatialFHE::operation::relateng {
    bool TFHEIMPredicate::intersectsExteriorOf(bool isA) const {
        if (isA) {
            return isIntersects(Location::EXTERIOR, Location::INTERIOR) ||
                isIntersects(Location::EXTERIOR, Location::BOUNDARY);
        } else {
            return isIntersects(Location::INTERIOR, Location::EXTERIOR) ||
                isIntersects(Location::BOUNDARY, Location::EXTERIOR);
        }
    }

    bool TFHEIMPredicate::isIntersects(Location locA, Location locB) const {
        return intMatrix.get(locA, locB) > Dimension::P;
    }

    bool TFHEIMPredicate::isDimsCompatibleWithCovers(int dim0, int dim1) {
        //- allow Points coveredBy zero-length Lines
        if (dim0 == Dimension::P && dim1 == Dimension::L)
            return true;
        return dim0 >= dim1;
    }

    void TFHEIMPredicate::init(int dA, int dB) {
        dimA = dA;
        dimB = dB;
    }

    void TFHEIMPredicate::updateDimension(Location locA, Location locB, int dimension) {
        // only record an increased dimension value
        if (isDimChanged(locA, locB, dimension)) {
            intMatrix.set(locA, locB, dimension);
            if (isDetermined()) {
                setValue(valueIM());
            }
        }
    }

    bool TFHEIMPredicate::isDimChanged(Location locA, Location locB, int dimension) const {
        return dimension > intMatrix.get(locA, locB);
    }

    bool TFHEIMPredicate::isKnown(Location locA, Location locB) const {
        return intMatrix.get(locA, locB) != DIM_UNKNOWN;
    }

    bool TFHEIMPredicate::isDimension(Location locA, Location locB, int dimension) const {
        return intMatrix.get(locA, locB) == dimension;
    }

    int TFHEIMPredicate::getDimension(Location locA, Location locB) const {
        return intMatrix.get(locA, locB);
    }

    void TFHEIMPredicate::finish() {
        setValue(valueIM());
    }
}  // namespace SpatialFHE::operation::relateng

// operation
// SpatialFHE