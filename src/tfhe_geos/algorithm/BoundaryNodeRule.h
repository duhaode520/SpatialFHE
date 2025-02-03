//
// Created by ubuntu on 1/17/25.
//

#ifndef BOUNDARYNODERULE_H
#define BOUNDARYNODERULE_H

namespace SpatialFHE::algorithm {

    /**
    * @brief Determine whether node points are in boundaries of lineal geometry
    *
    * Compared to GEOS, we only consider MOD2(OGC SFS) rule and simply implement static methods.
    * The boundary node rule is used to determine the boundary nodes of the graph.
    */
    class BoundaryNodeRule {
    public:
        static bool isInBoundary(int boundaryCount);
    };

}  // namespace SpatialFHE::algorithm

#endif  // BOUNDARYNODERULE_H
