//
// Created by ubuntu on 1/17/25.
//

#include "BoundaryNodeRule.h"

namespace SpatialFHE::algorithm {
    bool BoundaryNodeRule::isInBoundary(int boundaryCount) {
        return boundaryCount % 2 == 1;
    }
}  // namespace SpatialFHE::algorithm

// SpatialFHE