//
// Created by ubuntu on 1/24/25.
//

#include "TFHERayCrossingCounter.h"

#include "TFHEOrientation.h"
#include "tfhe_geos/geom/TFHECoordinateSequence.h"

using namespace SpatialFHE::geom;

namespace SpatialFHE::algorithm {
    Location TFHERayCrossingCounter::locatePointInRing(const TFHECoordinate &p, const TFHECoordinateSequence &ring) {
        TFHERayCrossingCounter rcc(p);

        for (std::size_t i = 1, ni = ring.size(); i < ni; i++) {
            const TFHECoordinate &p1 = ring.getAt(i - 1);
            const TFHECoordinate &p2 = ring.getAt(i);

            rcc.countSegment(p1, p2);
            if (rcc.isOnSegment()) {
                break;
            }
        }

        return rcc.getLocation();
    }

    Location TFHERayCrossingCounter::locatePointInRing(
        const TFHECoordinate &p,
        const std::vector<const TFHECoordinate *> &ring) {
        TFHERayCrossingCounter rcc(p);

        for (std::size_t i = 1, ni = ring.size(); i < ni; i++) {
            const TFHECoordinate &p1 = *ring[i - 1];
            const TFHECoordinate &p2 = *ring[i];

            rcc.countSegment(p1, p2);
            if (rcc.isOnSegment()) {
                break;
            }
        }

        return rcc.getLocation();
    }

    void TFHERayCrossingCounter::countSegment(const geom::TFHECoordinate &p1, const geom::TFHECoordinate &p2) {
        // check if the segment is strictly to the left of the test point
        if ((p1.x < point.x && p2.x < point.x).decrypt()) {
            return;
        }

        // check if the point is equal to the current ring vertex
        if ((point.x == p2.x && point.y == p2.y).decrypt()) {
            isPointOnSegment = true;
            return;
        }

        // For horizontal segments, check if the test point is on the segment.
        TFHEBool isHorizontal = (p1.y == point.y) && (p2.y == point.y);
        if (isHorizontal.decrypt()) {
            TFHEInt32 maxx = TFHEInt32::max(p1.x, p2.x);
            TFHEInt32 minx = TFHEInt32::min(p1.x, p2.x);
            isPointOnSegment = ((point.x >= minx) && (point.x <= maxx)).decrypt();
            return;
        }

        // 对于剩下的情况，计算线段与射线的是否会有交点
        // 不需要严格计算交点的位置，只需要根据防线刚和坐标判断有没有
        bool p1_p_p2 = (p1.y > point.y && p2.y <= point.y).decrypt();
        bool p2_p_p1 = (p2.y > point.y && p1.y <= point.y).decrypt();
        if (p1_p_p2 || p2_p_p1) {
            TFHEInt32 sign = TFHEOrientation::index(p1, p2, point);  // 这里不解密的原因是 TFHEInt32 的远程解密开销大
            if ((sign == 0).decrypt()) {
                isPointOnSegment = true;
                return;
            }
            if (p1_p_p2) {
                sign = -sign;
            }

            // The segment crosses the ray if the sign is strictly positive.
            if ((sign > 0).decrypt()) {
                crossingCount++;
            }
        }
    }

    Location TFHERayCrossingCounter::getLocation() const {
        if (isPointOnSegment) {
            return Location::BOUNDARY;
        }

        if (crossingCount % 2 == 1) {
            return Location::INTERIOR;
        }

        return Location::EXTERIOR;
    }
}  // namespace SpatialFHE::algorithm

// SpatialFHE