//
// Created by ubuntu on 2/10/25.
//

#include "TFHEQuadrant.h"


namespace SpatialFHE::geom {
        int TFHEQuadrant::commonHalfPlane(int quad1, int quad2) {

            // if quadrants are the same they do not determine a unique
            // common halfplane.
            // Simply return one of the two possibilities
            if(quad1 == quad2) {
                return quad1;
            }
            int diff = (quad1 - quad2 + 4) % 4;
            // if quadrants are not adjacent, they do not share a common halfplane
            if(diff == 2) {
                return -1;
            }
            //
            int min = (quad1 < quad2) ? quad1 : quad2;
            int max = (quad1 > quad2) ? quad1 : quad2;
            // for this one case, the righthand plane is NOT the minimum index;
            if(min == 0 && max == 3) {
                return 3;
            }
            // in general, the halfplane index is the minimum of the two
            // adjacent quadrants
            return min;
        }

        bool TFHEQuadrant::isInHalfPlane(int quad, int halfPlane) {
            if(halfPlane == SE) {
                return quad == SE || quad == SW;
            }
            return quad == halfPlane || quad == halfPlane + 1;
        }
    } // geom
// SpatialFHE