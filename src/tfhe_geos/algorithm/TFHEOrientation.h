//
// Created by ubuntu on 1/21/25.
//

#ifndef TFHEORIENTATION_H
#define TFHEORIENTATION_H
#include <TFHEInt32.h>

#include "tfhe_geos/geom/TFHECoordinate.h"
#include "tfhe_geos/geom/TFHECoordinateSequence.h"

namespace SpatialFHE::algorithm {

    class TFHEOrientation {
    public:
        /* A value that indicates an orientation or turn */
        enum { CLOCKWISE = -1, COLLINEAR = 0, COUNTERCLOCKWISE = 1, RIGHT = -1, LEFT = 1, STRAIGHT = 0 };

        typedef TFHEInt32 TFHEOrientationType;

        /** \brief
         * Returns the orientation index of the direction of the point q relative to
         * a directed infinite line specified by p1-p2.
         *
         * The index indicates whether the point lies to the
         * `Orientation::LEFT` or `Orientation::RIGHT`
         * of the line, or lies on it `Orientation::COLLINEAR`.
         * The index also indicates the orientation of the triangle formed
         * by the three points
         * ( `Orientation::COUNTERCLOCKWISE`,
         * `Orientation::CLOCKWISE`, or `Orientation::STRAIGHT` )
         */
        static TFHEOrientationType index(
            const geom::TFHECoordinate &p1,
            const geom::TFHECoordinate &p2,
            const geom::TFHECoordinate &q);


      /**
      * Computes whether a ring defined by a geom::CoordinateSequence is
      * oriented counter-clockwise.
      *
      *  * The list of points is assumed to have the first and last points equal.
      *  * This handles coordinate lists which contain repeated points.
      *  * This handles rings which contain collapsed segments
      *    (in particular, along the top of the ring).
      *
      * This algorithm is guaranteed to work with valid rings.
      * It also works with "mildly invalid" rings
      * which contain collapsed (coincident) flat segments along the top of the ring.
      * If the ring is "more" invalid (e.g. self-crosses or touches),
      * the computed result may not be correct.
      *
      * @param ring a CoordinateSequence forming a ring (with first and last point identical)
      * @return true if the ring is oriented counter-clockwise.
      * @throws IllegalArgumentException if there are too few points to determine orientation (< 4)
      */
      static TFHEBool isCCW(const geom::TFHECoordinateSequence *ring);
    };

}  // namespace SpatialFHE::algorithm

#endif  // TFHEORIENTATION_H
