//
// Created by ubuntu on 1/31/25.
//

#include "TFHEIntersection.h"
#include "TFHEInt32.h"
#include "tfhe_geos/geom/TFHECoordinate.h"

namespace SpatialFHE::algorithm {
    geom::TFHECoordinate TFHEIntersection::intersection(
        const geom::TFHECoordinate &p1,
        const geom::TFHECoordinate &p2,
        const geom::TFHECoordinate &q1,
        const geom::TFHECoordinate &q2) {
        TFHEInt32 px = p1.y - p2.y;
        TFHEInt32 py = p2.x - p1.x;
        TFHEInt32 pw = p1.x * p2.y - p2.x * p1.y;

        TFHEInt32 qx = q1.y - q2.y;
        TFHEInt32 qy = q2.x - q1.x;
        TFHEInt32 qw = q1.x * q2.y - q2.x * q1.y;

        TFHEInt32 x = py * qw - qy * pw;
        TFHEInt32 y = qx * pw - px * qw;
        TFHEInt32 w = px * qy - qx * py;

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