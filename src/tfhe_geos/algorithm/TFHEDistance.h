//
// Created by ubuntu on 1/31/25.
//

#ifndef TFHEDISTANCE_H
#define TFHEDISTANCE_H
#include <tfhe_geos/geom/TFHECoordinate.h>

namespace SpatialFHE::algorithm {

    class TFHEDistance {
    public:
        static TFHEDecimal pointToSegmentSquared(const geom::TFHECoordinate& p, const geom::TFHECoordinate& A, const geom::TFHECoordinate& B);
    };

}  // namespace SpatialFHE::algorithm

#endif  // TFHEDISTANCE_H
