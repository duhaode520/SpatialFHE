//
// Created by ubuntu on 1/30/25.
//

#include "TFHELineIntersector.h"

#include <tfhe_geos/geom/TFHEEnvelope.h>

#include "TFHEDistance.h"
#include "TFHEIntersection.h"
#include "TFHEOrientation.h"

namespace SpatialFHE::algorithm {
    bool TFHELineIntersector::isCollinear() const {
        return result == COLLINEAR_INTERSECTION;
    }

    uint8_t TFHELineIntersector::computeIntersect(
        const TFHECoordinate &p1,
        const TFHECoordinate &p2,
        const TFHECoordinate &q1,
        const TFHECoordinate &q2) {
        isProperVar = false;

        // first try a fast test to see if the envelopes of the lines intersect
        if (!geom::TFHEEnvelope::intersects(p1, p2, q1, q2).decrypt()) {
            return NO_INTERSECTION;
        }

        // 对于每一个 endpoint，测试其相对于另外一个 line segment 的位置，如果是同侧的话说明没有相交
        TFHEInt32 Pq1 = TFHEOrientation::index(p1, p2, q1);
        TFHEInt32 Pq2 = TFHEOrientation::index(p1, p2, q2);
        if ((Pq1 * Pq2 > 0).decrypt()) {
            return NO_INTERSECTION;
        }

        TFHEInt32 Qp1 = TFHEOrientation::index(q1, q2, p1);
        TFHEInt32 Qp2 = TFHEOrientation::index(q1, q2, p2);
        if ((Qp1 * Qp2 > 0).decrypt()) {
            return NO_INTERSECTION;
        }
        bool pq10 = (Pq1 == 0).decrypt();
        bool pq20 = (Pq2 == 0).decrypt();
        bool qp10 = (Qp1 == 0).decrypt();
        bool qp20 = (Qp2 == 0).decrypt();

        /**
         * Intersection is collinear if each endpoint lies on the other line.
         */
        if (pq10 && pq20 && qp10 && qp20) {
            return computeCollinearIntersection(p1, p2, q1, q2);
        }
        /*
         * At this point we know that there is a single intersection point
         * (since the lines are not collinear).
         */

        /*
         * 检查交点是不是端点，如果是的话可以直接copy end point
         */
        geom::TFHECoordinate p;
        if (pq10 || pq20 || qp10 || qp20) {
            isProperVar = false;
            // 先检查端点重合，再看有没有端点落在另外一个segment上
            if (p1 == q1 || p1 == q2) {
                p = p1;
            } else if (p2 == q2 || p2 == q1) {
                p = p2;
            } else if (pq10) {
                p = q1;
            } else if (pq20) {
                p = q2;
            } else if (qp10) {
                p = p1;
            } else if (qp20) {
                p = p2;
            }
        } else {
            isProperVar = true;
            p = intersection(p1, p2, q1, q2);
            intPt[0] = p;
        }
        return POINT_INTERSECTION;
    }

    uint8_t TFHELineIntersector::computeCollinearIntersection(
        const TFHECoordinate &p1,
        const TFHECoordinate &p2,
        const TFHECoordinate &q1,
        const TFHECoordinate &q2) {
        geom::TFHEEnvelope envP(p1, p2), envQ(q1, q2);
        bool q1inP = envP.intersects(q1).decrypt();
        bool q2inP = envP.intersects(q2).decrypt();
        bool p1inQ = envQ.intersects(p1).decrypt();
        bool p2inQ = envQ.intersects(p2).decrypt();

        if (q1inP && q2inP) {
            intPt[0] = q1;
            intPt[1] = q2;
            return COLLINEAR_INTERSECTION;
        }

        if (p1inQ && p2inQ) {
            intPt[0] = p1;
            intPt[1] = p2;
            return COLLINEAR_INTERSECTION;
        }

        if (q1inP && p1inQ) {
            intPt[0] = q1;
            intPt[1] = p1;
            return (q1 == p1) && !q2inP && !p2inQ ? POINT_INTERSECTION : COLLINEAR_INTERSECTION;
        }
        if (q1inP && p2inQ) {
            intPt[0] = q1;
            intPt[1] = p2;
            return (q1 == p2) && !q2inP && !p1inQ ? POINT_INTERSECTION : COLLINEAR_INTERSECTION;
        }
        if (q2inP && p1inQ) {
            intPt[0] = q2;
            intPt[1] = p1;
            return (q2 == p1) && !q1inP && !p2inQ ? POINT_INTERSECTION : COLLINEAR_INTERSECTION;
        }
        if (q2inP && p2inQ) {
            intPt[0] = q2;
            intPt[1] = p2;
            return (q2 == p2) && !q1inP && !p1inQ ? POINT_INTERSECTION : COLLINEAR_INTERSECTION;
        }
        return NO_INTERSECTION;
    }

    TFHELineIntersector::TFHECoordinate TFHELineIntersector::intersection(
        const TFHECoordinate &p1,
        const TFHECoordinate &p2,
        const TFHECoordinate &q1,
        const TFHECoordinate &q2) const {
        TFHECoordinate intPtOut = intersectionSafe(p1, p2, q1, q2);

        // 由于舍入的原因可能会出现计算出来的交点在 input segment 之外的情况，这里纠正为端点
        if (!isInSegmentEnvelopes(intPtOut).decrypt()) {
            // intPt = CentralEndpointIntersector::getIntersection(p1, p2, q1, q2);
            intPtOut = nearestEndpoint(p1, p2, q1, q2);
        }
        return intPtOut;
    }

    TFHELineIntersector::TFHECoordinate TFHELineIntersector::intersectionSafe(
        const TFHECoordinate &p1,
        const TFHECoordinate &p2,
        const TFHECoordinate &q1,
        const TFHECoordinate &q2) const {
        TFHECoordinate ptInt(TFHEIntersection::intersection(p1, p2, q1, q2));
        if (ptInt.isNull()) {
            ptInt = nearestEndpoint(p1, p2, q1, q2);
        }
        return ptInt;
    }

    const geom::TFHECoordinate &TFHELineIntersector::nearestEndpoint(
        const TFHECoordinate &p1,
        const TFHECoordinate &p2,
        const TFHECoordinate &q1,
        const TFHECoordinate &q2) {
        const TFHECoordinate *nearestPt = &p1;
        TFHEInt32 minDist = TFHEDistance::pointToSegmentSquared(p1, q1, q2);

        TFHEInt32 dist = TFHEDistance::pointToSegmentSquared(p2, q1, q2);
        if ((dist < minDist).decrypt()) {
            minDist = dist;
            nearestPt = &p2;
        }
        dist = TFHEDistance::pointToSegmentSquared(q1, p1, p2);
        if ((dist < minDist).decrypt()) {
            minDist = dist;
            nearestPt = &q1;
        }
        dist = TFHEDistance::pointToSegmentSquared(q2, p1, p2);
        if ((dist < minDist).decrypt()) {
            nearestPt = &q2;
        }
        return *nearestPt;
    }

    TFHEBool TFHELineIntersector::isInSegmentEnvelopes(const TFHECoordinate &pt) const {
        geom::TFHEEnvelope env0(*inputLines[0][0], *inputLines[0][1]);
        geom::TFHEEnvelope env1(*inputLines[1][0], *inputLines[1][1]);
        return env0.contains(pt) && env1.contains(pt);
    }

    void TFHELineIntersector::computeIntersection(
        const TFHECoordinate &p1,
        const TFHECoordinate &p2,
        const TFHECoordinate &q1,
        const TFHECoordinate &q2) {
        inputLines[0][0] = &p1;
        inputLines[0][1] = &p2;
        inputLines[1][0] = &q1;
        inputLines[1][1] = &q2;
        result = computeIntersect(p1, p2, q1, q2);
    }

    bool TFHELineIntersector::hasIntersection() const {
        return result != NO_INTERSECTION;
    }

    size_t TFHELineIntersector::getIntersectionNum() const {
        return result;
    }

    bool TFHELineIntersector::isProper() const {
        return hasIntersection() && isProperVar;
    }

    const TFHELineIntersector::TFHECoordinate & TFHELineIntersector::getIntersection(std::size_t index) const{
        return intPt[index];
    }

}  // namespace SpatialFHE::algorithm

// SpatialFHE