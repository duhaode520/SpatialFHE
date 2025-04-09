//
// Created by ubuntu on 1/21/25.
//

#include "TFHEOrientation.h"

namespace SpatialFHE::algorithm {
    TFHEOrientation::TFHEOrientationType TFHEOrientation::index(
        const geom::TFHECoordinate& p1,
        const geom::TFHECoordinate& p2,
        const geom::TFHECoordinate& q) {
        // compute orientation
        TFHEDecimal dx1 = p2.x - p1.x;
        TFHEDecimal dy1 = p2.y - p1.y;
        TFHEDecimal dx2 = q.x - p2.x;
        TFHEDecimal dy2 = q.y - p2.y;
        TFHEDecimal cross_product = dx1 * dy2 - dy1 * dx2;
        // return orientation
        TFHEBool is_positive = cross_product > 0;
        TFHEBool is_negative = cross_product < 0;
        return TFHEInt32(is_positive) - TFHEInt32(is_negative);
    }

    TFHEBool TFHEOrientation::isCCW(const geom::TFHECoordinateSequence* ring) {
        // # of points without closing endpoint
        int inPts = static_cast<int>(ring->size()) - 1;
        // sanity check
        if (inPts < 3)
            return TFHEBool::tfhe_false;

        uint32_t nPts = static_cast<uint32_t>(inPts);

        // 找到一个凸包上的最高点，同时找到这个店对应的前面一个低点
        const geom::TFHECoordinate* upHiPt = &ring->getAt(0);
        const geom::TFHECoordinate* upLoPt = &ring->getAt(1);

        TFHEDecimal prevY = upHiPt->y;
        uint32_t iUpHi = 0;
        for (uint32_t i = 1; i <= nPts; i++) {
            const geom::TFHECoordinate* pt = &ring->getAt(i);
            // 找到了更高的点
            if ((pt->y > prevY && pt->y >= upHiPt->y).decrypt()) {
                iUpHi = i;
                upHiPt = pt;
                upLoPt = &ring->getAt(i - 1);
            }
            prevY = pt->y;
        }

        // Check if ring is flat and return default value if so
        if (iUpHi <= 0) {
            return TFHEBool::tfhe_false;
        }

        // Find next lower point after the highest point
        uint32_t iDownLo = iUpHi;
        do {
            iDownLo = (iDownLo + 1) % nPts;
        } while (iDownLo != iUpHi && (ring->getAt(iDownLo).y == upHiPt->y).decrypt());

        const geom::TFHECoordinate& downLoPt = ring->getAt(iDownLo);
        uint32_t iDownHi = iDownLo > 0 ? iDownLo - 1 : nPts - 1;
        const geom::TFHECoordinate& downHiPt = ring->getAt(iDownHi);
        /**
         * Two cases can occur:
         * 1) the hiPt and the downPrevPt are the same.
         *    This is the general position case of a "pointed cap".
         *    The ring orientation is determined by the orientation of the cap
         * 2) The hiPt and the downPrevPt are different.
         *    In this case the top of the cap is flat.
         *    The ring orientation is given by the direction of the flat segment
         */
        if (*upHiPt == downHiPt) {
            /**
             * Check for the case where the cap has configuration A-B-A.
             * This can happen if the ring does not contain 3 distinct points
             * (including the case where the input array has fewer than 4 elements), or
             * it contains coincident line segments.
             */
            if ((upLoPt->equals(*upHiPt) || downLoPt.equals(*upHiPt) || upLoPt->equals(downLoPt)).decrypt())
                return TFHEBool::tfhe_false;

            /**
             * It can happen that the top segments are coincident.
             * This is an invalid ring, which cannot be computed correctly.
             * In this case the orientation is 0, and the result is false.
             */
            TFHEOrientationType orientationIndex = index(*upLoPt, *upHiPt, downLoPt);
            return orientationIndex == COUNTERCLOCKWISE;
        } else {
            /**
             * Flat cap - direction of flat top determines orientation
             */
            return downHiPt.x - upHiPt->x < 0;
        }
    }
} // algorithm
// SpatialFHE