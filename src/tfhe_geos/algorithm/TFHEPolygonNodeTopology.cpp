//
// Created by ubuntu on 1/23/25.
//

#include "TFHEPolygonNodeTopology.h"

#include <tfhe_geos/geom/TFHEQuadrant.h>

#include "TFHEOrientation.h"
#include "tfhe_geos/geom/TFHECoordinate.h"

namespace SpatialFHE::algorithm {
    bool TFHEPolygonNodeTopology::isCrossing(
        const TFHECoordinate *nodePt,
        const TFHECoordinate *a0,
        const TFHECoordinate *a1,
        const TFHECoordinate *b0,
        const TFHECoordinate *b1) {
        const TFHECoordinate *aLo = a0;
        const TFHECoordinate *aHi = a1;
        if (isAngleGreater(nodePt, aLo, aHi).decrypt()) {
            aLo = a1;
            aHi = a0;
        }

        // bool bBetween0 = isBetween(nodePt, b0, aLo, aHi);
        // bool bBetween1 = isBetween(nodePt, b1, aLo, aHi);
        // return bBetween0 != bBetween1;

        /**
         * Find positions of b0 and b1.
         * If they are the same they do not cross the other edge
         */
        int compBetween0 = compareBetween(nodePt, b0, aLo, aHi);
        if (compBetween0 == 0)
            return false;
        int compBetween1 = compareBetween(nodePt, b1, aLo, aHi);
        if (compBetween1 == 0)
            return false;

        return compBetween0 != compBetween1;
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
        int quadrantP = quadrant(origin, p);
        int quadrantQ = quadrant(origin, q);

        /**
         * If the vectors are in different quadrants,
         * that determines the ordering
         */
        if (quadrantP > quadrantQ)
            // TODO: this can be trivial
            return TFHEInt32(1);
        if (quadrantP < quadrantQ)
            return TFHEInt32(-1);
        // P > Q if P is CCW of Q
        TFHEInt32 orient = TFHEOrientation::index(*origin, *q, *p);
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
        const TFHECoordinate *e1) {
        TFHEInt32 comp0 = compareAngle(origin, p, e0);
        if ((comp0 == 0).decrypt())
            return 0;
        TFHEInt32 comp1 = compareAngle(origin, p, e1);
        if ((comp1 == 0).decrypt())
            return 0;
        if ((comp0 > 0 && comp1 < 0).decrypt())
            return 1;
        return -1;
    }

    TFHEBool TFHEPolygonNodeTopology::isAngleGreater(
        const TFHECoordinate *origin,
        const TFHECoordinate *p,
        const TFHECoordinate *q) {
        int quadrantP = quadrant(origin, p);
        int quadrantQ = quadrant(origin, q);

        /**
         * If the vectors are in different quadrants,
         * that determines the ordering
         */
        if (quadrantP > quadrantQ)
            return TFHEBool::tfhe_true;
        if (quadrantP < quadrantQ)
            return TFHEBool::tfhe_false;

        //--- vectors are in the same quadrant
        // Check relative orientation of vectors
        // P > Q if it is CCW of Q
        TFHEInt32 orient = TFHEOrientation::index(*origin, *q, *p);
        return orient == TFHEOrientation::COUNTERCLOCKWISE;
    }

    int TFHEPolygonNodeTopology::quadrant(const TFHECoordinate *origin, const TFHECoordinate *p) {
        TFHEInt32 dx = p->x - origin->x;
        TFHEInt32 dy = p->y - origin->y;
        return geom::TFHEQuadrant::quadrant(dx, dy);
    }
}  // namespace SpatialFHE::algorithm