//
// Created by ubuntu on 1/24/25.
//

#ifndef TFHERAYCROSSINGCOUNTER_H
#define TFHERAYCROSSINGCOUNTER_H
#include <vector>

#include "tfhe_geos/geom/enums.h"

// Forward declarations
namespace SpatialFHE::geom {
    class TFHECoordinate;
    class TFHECoordinateSequence;
}  // namespace SpatialFHE::geom

namespace SpatialFHE::algorithm {

    /** \brief
     * Counts the number of segments crossed by a horizontal ray extending to the
     * right from a given point, in an incremental fashion.
     *
     * This can be used to determine whether a point lies in a polygonal geometry.
     * The class determines the situation where the point lies exactly on a segment.
     * When being used for Point-In-Polygon determination, this case allows
     * short-circuiting the evaluation.
     *
     * This class handles polygonal geometries with any number of shells and holes.
     * The orientation of the shell and hole rings is unimportant.
     * In order to compute a correct location for a given polygonal geometry,
     * it is essential that **all** segments are counted which
     *
     * - touch the ray
     * - lie in in any ring which may contain the point
     *
     * The only exception is when the point-on-segment situation is detected, in
     * which case no further processing is required.
     * The implication of the above rule is that segments which can be a priori
     * determined to *not* touch the ray (i.e. by a test of their bounding box or
     * Y-extent) do not need to be counted. This allows for optimization by indexing.
     *
     * @author Martin Davis
     *
     * Curve support is not implemented yet in TFHE.
     */
    class TFHERayCrossingCounter {
    private:
        const geom::TFHECoordinate& point;
        std::size_t crossingCount;
        bool isPointOnSegment;

    public:
        /** \brief
         * Determines the [Location](@ref geom::Location) of a point in a ring.
         *
         * This method is an exemplar of how to use this class.
         *
         * @param p the point to test
         * @param ring an array of Coordinates forming a ring
         * @return the location of the point in the ring
         */
        static geom::Location locatePointInRing(
            const geom::TFHECoordinate& p,
            const geom::TFHECoordinateSequence& ring);

        /// Semantically equal to the above, just different args encoding
        static geom::Location locatePointInRing(
            const geom::TFHECoordinate& p,
            const std::vector<const geom::TFHECoordinate*>& ring);

        explicit TFHERayCrossingCounter(const geom::TFHECoordinate& p_point) :
                point(p_point), crossingCount(0), isPointOnSegment(false) {}

        /** \brief
         * Counts a segment
         *
         * @param p1 an endpoint of the segment
         * @param p2 another endpoint of the segment
         */
        void countSegment(const geom::TFHECoordinate& p1, const geom::TFHECoordinate& p2);

        /** \brief
         * Reports whether the point lies exactly on one of the supplied segments.
         *
         * This method may be called at any time as segments are processed.
         * If the result of this method is `true`, no further segments need
         * be supplied, since the result will never change again.
         *
         * @return `true` if the point lies exactly on a segment
         */
        bool isOnSegment() const {
            return isPointOnSegment;
        }

        /** \brief
         * Gets the [Location](@ref geom::Location) of the point relative to
         * the ring, polygon or multipolygon from which the processed
         * segments were provided.
         *
         * This method only determines the correct location
         * if **all** relevant segments must have been processed.
         *
         * @return the Location of the point
         */
        geom::Location getLocation() const;
    };

}  // namespace SpatialFHE::algorithm

#endif  // TFHERAYCROSSINGCOUNTER_H
