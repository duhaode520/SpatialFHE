//
// Created by ubuntu on 1/23/25.
//

#include "TFHEPolygonNodeTopology.h"

#include "TFHEOrientation.h"
#include "tfhe_geos/geom/TFHECoordinate.h"

namespace SpatialFHE::algorithm {
    bool TFHEPolygonNodeTopology::isCrossing(
        const TFHECoordinate *nodePt,
        const TFHECoordinate *a0,
        const TFHECoordinate *a1,
        const TFHECoordinate *b0,
        const TFHECoordinate *b1) {
        const TFHECoordinate* aLo = a0;
        const TFHECoordinate* aHi = a1;

    }

    bool TFHEPolygonNodeTopology::isInteriorSegment(
        const TFHECoordinate *nodePt,
        const TFHECoordinate *a0,
        const TFHECoordinate *a1,
        const TFHECoordinate *b) {}

    TFHEInt32 TFHEPolygonNodeTopology::compareAngle(
        const TFHECoordinate *origin,
        const TFHECoordinate *p,
        const TFHECoordinate *q) {
        // 不用象限做优化了，象限优化又需要 decrypt，感觉是负优化

        // P > Q if P is CCW of Q
        TFHEInt32 orient = TFHEOrientation::index(*origin, *p, *q);
        // 直接return orient，orient 这里恰好是 CCW = 1, CW = -1
        return orient;
    }

    bool TFHEPolygonNodeTopology::isBetween(
        const TFHECoordinate *origin,
        const TFHECoordinate *p,
        const TFHECoordinate *e0,
        const TFHECoordinate *e1) {}

    int TFHEPolygonNodeTopology::compareBetween(
        const TFHECoordinate *origin,
        const TFHECoordinate *p,
        const TFHECoordinate *e0,
        const TFHECoordinate *e1) {}

    bool TFHEPolygonNodeTopology::isAngleGreater(
        const TFHECoordinate *origin,
        const TFHECoordinate *p,
        const TFHECoordinate *q) {}

    int TFHEPolygonNodeTopology::quadrant(const TFHECoordinate *origin, const TFHECoordinate *p) {
        TFHEInt32 dx = p->x - origin->x;
        TFHEInt32 dy = p->y - origin->y;

    }
}  // namespace SpatialFHE::algorithm