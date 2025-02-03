//
// Created by ubuntu on 1/31/25.
//

#ifndef TFHEINTERSECTION_H
#define TFHEINTERSECTION_H
#include <tfhe_geos/geom/TFHECoordinate.h>

namespace SpatialFHE::algorithm {

    class TFHEIntersection {
    public:
        /** \brief
         * Computes the intersection point of two lines.
         * If the lines are parallel or collinear this case is detected
         * and <code>null</code> is returned.
         *
         * @param p1 an endpoint of line 1
         * @param p2 an endpoint of line 1
         * @param q1 an endpoint of line 2
         * @param q2 an endpoint of line 2
         * @return the intersection point between the lines, if there is one,
         * or null if the lines are parallel or collinear
         *
         * @see CGAlgorithmsDD#intersection(Coordinate, Coordinate, Coordinate, Coordinate)
         */
        static geom::TFHECoordinate intersection(const geom::TFHECoordinate& p1, const geom::TFHECoordinate& p2,
                                               const geom::TFHECoordinate& q1, const geom::TFHECoordinate& q2);
    };

}  // namespace SpatialFHE::algorithm

#endif  // TFHEINTERSECTION_H
