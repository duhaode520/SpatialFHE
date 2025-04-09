//
// Created by ubuntu on 1/31/25.
//

#include "TFHEIntersection.h"
#include "TFHEDecimal.h"
#include "tfhe_geos/geom/TFHECoordinate.h"

namespace SpatialFHE::algorithm {
    geom::TFHECoordinate TFHEIntersection::intersection(
        const geom::TFHECoordinate &p1,
        const geom::TFHECoordinate &p2,
        const geom::TFHECoordinate &q1,
        const geom::TFHECoordinate &q2) {
        TFHEDecimal px = p1.y - p2.y;
        TFHEDecimal py = p2.x - p1.x;
        TFHEDecimal pw = p1.x * p2.y - p2.x * p1.y;

        TFHEDecimal qx = q1.y - q2.y;
        TFHEDecimal qy = q2.x - q1.x;
        TFHEDecimal qw = q1.x * q2.y - q2.x * q1.y;

        TFHEDecimal x = py * qw - qy * pw;
        TFHEDecimal y = qx * pw - px * qw;
        TFHEDecimal w = px * qy - qx * py;

        geom::TFHECoordinate rv;

        if ((w == 0).decrypt()) {
            return rv;
        }

        rv.x = x / w;
        rv.y = y / w;
        return rv;
    }
}  // namespace SpatialFHE::algorithm

// SpatialFHE