//
// Created by ubuntu on 2/10/25.
//

#ifndef TFHEQUADRANT_H
#define TFHEQUADRANT_H

#include <sstream>

#include "TFHECoordinate.h"

namespace SpatialFHE::geom {

    /** \brief
     * Utility functions for working with quadrants.
     *
     * The quadrants are numbered as follows:
     * <pre>
     * 1 | 0
     * --+--
     * 2 | 3
     * </pre>
     *
     */
    class TFHEQuadrant {
        static const int NE = 0;
        static const int NW = 1;
        static const int SW = 2;
        static const int SE = 3;

    public:
        /**
         * Returns the quadrant of a directed line segment
         * (specified as x and y displacements, which cannot both be 0).
         *
         * @throws IllegalArgumentException if the displacements are both 0
         */
        static int quadrant(TFHEInt32 dx, TFHEInt32 dy) {
            bool dxNotNeg = (dx >= 0).decrypt();
            bool dyNotNeg = (dy >= 0).decrypt();
            if (dxNotNeg) {
                if (dyNotNeg) {
                    return NE;
                } else {
                    return SE;
                }
            } else {
                if (dyNotNeg) {
                    return NW;
                } else {
                    return SW;
                }
            }
    //
    //         int y_sign = (y >> 31) & 1;  // y的符号位
    //         int x_sign = (x >> 31) & 1;  // x的符号位
    //         return (y_sign << 1) | (x_sign ^ y_sign);
    };

        /**
         * Returns the quadrant of a directed line segment from p0 to p1.
         *
         * @throws IllegalArgumentException if the points are equal
         */
        static int quadrant(const geom::TFHECoordinate& p0, const geom::TFHECoordinate& p1) {
            // sacrifice some security for performance
            // if ((p1.x == p0.x) && (p1.y == p0.y)) {
            //     throw util::IllegalArgumentException(
            //         "Cannot compute the quadrant for two identical points " + p0.toString());
            // }

            bool p1xGtP0x = (p1.x >= p0.x).decrypt();
            bool p1yGtP0y = (p1.y >= p0.y).decrypt();

            if (p1xGtP0x) {
                if (p1yGtP0y) {
                    return NE;
                } else {
                    return SE;
                }
            } else {
                if (p1yGtP0y) {
                    return NW;
                } else {
                    return SW;
                }
            }
        };

        /**
         * Returns true if the quadrants are 1 and 3, or 2 and 4
         */
        static bool isOpposite(int quad1, int quad2) {
            if (quad1 == quad2) {
                return false;
            }
            int diff = (quad1 - quad2 + 4) % 4;
            // if quadrants are not adjacent, they are opposite
            if (diff == 2) {
                return true;
            }
            return false;
        };

        /*
         * Returns the right-hand quadrant of the halfplane defined by
         * the two quadrants,
         * or -1 if the quadrants are opposite, or the quadrant if they
         * are identical.
         */
        static int commonHalfPlane(int quad1, int quad2);

        /**
         * Returns whether the given quadrant lies within the given halfplane
         * (specified by its right-hand quadrant).
         */
        static bool isInHalfPlane(int quad, int halfPlane);

        /**
         * Returns true if the given quadrant is 0 or 1.
         */
        static bool isNorthern(int quad) {
            return quad == NE || quad == NW;
        };
    };

}  // namespace SpatialFHE::geom

#endif  // TFHEQUADRANT_H
