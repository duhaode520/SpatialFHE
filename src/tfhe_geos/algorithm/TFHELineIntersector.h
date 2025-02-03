//
// Created by ubuntu on 1/30/25.
//

#ifndef TFHELINEINTERSECTOR_H
#define TFHELINEINTERSECTOR_H
#include <tfhe_geos/geom/TFHECoordinate.h>

#include <cstddef>

namespace SpatialFHE::algorithm {

    /** \brief
     * A LineIntersector is an algorithm that can both test whether
     * two line segments intersect and compute the intersection point
     * if they do.
     *
     * The intersection point may be computed in a precise or non-precise manner.
     * Computing it precisely involves rounding it to an integer.  (This assumes
     * that the input coordinates have been made precise by scaling them to
     * an integer grid.)
     *
     */
    class TFHELineIntersector {
        using TFHECoordinate = geom::TFHECoordinate;

    private:
        std::size_t result;
        const TFHECoordinate* inputLines[2][2];

        // 记录交点的计算结果
        TFHECoordinate intPt[2];

        std::size_t intLineIndex[2][2];

        bool isProperVar;
        bool isCollinear() const;
        uint8_t computeIntersect(
            const TFHECoordinate& p1,
            const TFHECoordinate& p2,
            const TFHECoordinate& q1,
            const TFHECoordinate& q2);

        uint8_t computeCollinearIntersection(
            const TFHECoordinate& p1,
            const TFHECoordinate& p2,
            const TFHECoordinate& q1,
            const TFHECoordinate& q2);

        TFHECoordinate intersection(
            const TFHECoordinate& p1,
            const TFHECoordinate& p2,
            const TFHECoordinate& q1,
            const TFHECoordinate& q2) const;
        TFHECoordinate intersectionSafe(
            const TFHECoordinate& p1,
            const TFHECoordinate& p2,
            const TFHECoordinate& q1,
            const TFHECoordinate& q2) const;

        /**
         * Finds the endpoint of the segments P and Q which
         * is closest to the other segment.
         * This is a reasonable surrogate for the true
         * intersection points in ill-conditioned cases
         * (e.g. where two segments are nearly coincident,
         * or where the endpoint of one segment lies almost on the other segment).
         * <p>
         * This replaces the older CentralEndpoint heuristic,
         * which chose the wrong endpoint in some cases
         * where the segments had very distinct slopes
         * and one endpoint lay almost on the other segment.
         *
         * @param p1 an endpoint of segment P
         * @param p2 an endpoint of segment P
         * @param q1 an endpoint of segment Q
         * @param q2 an endpoint of segment Q
         * @return the nearest endpoint to the other segment
         */
        static const TFHECoordinate& nearestEndpoint(
            const TFHECoordinate& p1,
            const TFHECoordinate& p2,
            const TFHECoordinate& q1,
            const TFHECoordinate& q2);

        TFHEBool isInSegmentEnvelopes(const TFHECoordinate& pt) const;

    public:
        TFHELineIntersector() {}

        ~TFHELineIntersector() = default;

        enum intersection_type : uint8_t {
            /// Indicates that line segments do not intersect
            NO_INTERSECTION = 0,
            /// Indicates that line segments intersect in a single point
            POINT_INTERSECTION = 1,
            /// Indicates that line segments intersect in a line segment
            COLLINEAR_INTERSECTION = 2
        };

        void computeIntersection(
            const TFHECoordinate& p1,
            const TFHECoordinate& p2,
            const TFHECoordinate& q1,
            const TFHECoordinate& q2);

        bool hasIntersection() const;
        size_t getIntersectionNum() const;
        bool isProper() const;
        const TFHECoordinate& getIntersection(std::size_t index) const;


    };
}  // namespace SpatialFHE::algorithm

#endif  // TFHELINEINTERSECTOR_H
