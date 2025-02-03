//
// Created by ubuntu on 1/31/25.
//

#include "TFHEDistance.h"

namespace SpatialFHE::algorithm {
    TFHEInt32 TFHEDistance::pointToSegmentSquared(
        const geom::TFHECoordinate &p,
        const geom::TFHECoordinate &A,
        const geom::TFHECoordinate &B) {

        if (A == B) {
            return p.distanceSquared(A);
        }
        /*
            otherwise use comp.graphics.algorithms method:
            (1)
                            AC dot AB
                        r = ---------
                            ||AB||^2

            r has the following meaning:
            r=0 P = A
            r=1 P = B
            r<0 P is on the backward extension of AB
            r>1 P is on the forward extension of AB
            0<r<1 P is interior to AB
        */

        TFHEInt32 dot = (p.x - A.x) * (B.x - A.x) + (p.y - A.y) * (B.y - A.y);
        TFHEInt32 L2 = A.distanceSquared(B);

        if((dot <= 0).decrypt()) {
            return p.distanceSquared(A);
        }
        if((dot >= L2).decrypt()) {
            return p.distanceSquared(B);
        }

        /*
            (2)
                    (Ay-Cy)(Bx-Ax)-(Ax-Cx)(By-Ay) = D
                s = -----------------------------
                                L^2

            Then the distance from C to P = |s|*L.
            The square of distance = s^2*L^2 = D^2/L^2
        */

        TFHEInt32 d = (A.y - p.y) * (B.x - A.x) - (A.x - p.x) * (B.y - A.y);

        return d*d/L2;
    }
}  // namespace SpatialFHE::algorithm

// SpatialFHE