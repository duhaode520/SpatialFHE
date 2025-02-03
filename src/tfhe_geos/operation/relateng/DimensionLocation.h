//
// Created by ubuntu on 1/18/25.
//

#ifndef DIMENSIONLOCATION_H
#define DIMENSIONLOCATION_H
#include <tfhe_geos/geom/enums.h>

namespace SpatialFHE ::operation ::relateng {

    class DimensionLocation {
        using Location = geom::Location;
        using Dimension = geom::Dimension;

    public:
        enum DimensionLocationType {
            EXTERIOR = 2,  // == Location.EXTERIOR
            POINT_INTERIOR = 103,
            LINE_INTERIOR = 110,
            LINE_BOUNDARY = 111,
            AREA_INTERIOR = 120,
            AREA_BOUNDARY = 121
        };

        static int locationArea(Location loc);

        static int locationLine(Location loc);

        static int locationPoint(Location loc);

        static Location location(int dimLoc);

        static int dimension(int dimLoc);

        static int dimension(int dimLoc, int exteriorDim);

    };

}  // namespace SpatialFHE::operation::relateng

#endif  // DIMENSIONLOCATION_H
