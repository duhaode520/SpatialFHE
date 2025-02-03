//
// Created by ubuntu on 1/27/25.
//

#include "TFHESegmentString.h"

namespace SpatialFHE::noding {
    bool TFHESegmentString::isClosed() const {
        return seq->front() == seq->back();
    }

    const geom::TFHECoordinate &TFHESegmentString::prevInRing(size_t index) const {
        size_t prevIndex = index - 1;
        if (prevIndex == 0) {
            prevIndex = size() - 2;
        } else {
            prevIndex = index - 1;
        }
        return getCoordinate(prevIndex);

    }

    const geom::TFHECoordinate &TFHESegmentString::nextInRing(size_t index) const {
        size_t nextIndex = index + 1;
        if (nextIndex >= size() - 1) {
            nextIndex = 1;
        }
        return getCoordinate(nextIndex);
    }
}  // namespace SpatialFHE::noding

// SpatialFHE