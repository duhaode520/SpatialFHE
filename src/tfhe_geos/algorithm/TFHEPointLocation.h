//
// Created by ubuntu on 1/21/25.
//

#ifndef TFHEPOINTLOCATION_H
#define TFHEPOINTLOCATION_H
#include <tfhe_geos/geom/enums.h>
#include <TFHEBool.h>
#include <vector>

// Forward declaration
namespace SpatialFHE::geom {
    class TFHECoordinate;
    class TFHECoordinateSequence;
}  // namespace SpatialFHE::geom

namespace SpatialFHE ::algorithm {

    class TFHEPointLocation {
    public:
        /** \brief
         * Tests whether a point lies on a line segment.
         *
         * @param p the point to test
         * @param p0 a point of the line segment
         * @param p1 a point of the line segment
         * @return true if the point lies on the line segment
         */
        static TFHEBool isOnSegment(
            const geom::TFHECoordinate& p,
            const geom::TFHECoordinate& p0,
            const geom::TFHECoordinate& p1);

        /** \brief
         * Tests whether a point lies on the line defined by a
         * [CoordinateSequence](@ref geom::CoordinateSequence).
         *
         * @param p the point to test
         * @param line the line coordinates
         * @return true if the point is a vertex of the line or lies in the interior
         *         of a line segment in the line
         */
        static TFHEBool isOnLine(const geom::TFHECoordinate& p, const geom::TFHECoordinateSequence* line);

        /** \brief
         * Determines whether a point lies in the interior, on the boundary, or in the
         * exterior of a ring. The ring may be oriented in either direction.
         *
         * This method does *not* first check the point against the envelope of
         * the ring.
         *
         * @param p point to check for ring inclusion
         * @param ring an array of coordinates representing the ring (which must have
         *             first point identical to last point)
         * @return the [Location](@ref geom::Location) of p relative to the ring
         */
        static geom::Location locateInRing(const geom::TFHECoordinate& p, const std::vector<const geom::TFHECoordinate*>& ring);
        static geom::Location locateInRing(const geom::TFHECoordinate& p, const geom::TFHECoordinateSequence& ring);
    };

}  // namespace SpatialFHE::algorithm

#endif  // TFHEPOINTLOCATION_H
