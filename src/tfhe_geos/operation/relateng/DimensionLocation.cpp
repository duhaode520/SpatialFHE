//
// Created by ubuntu on 1/18/25.
//

#include "DimensionLocation.h"

namespace SpatialFHE::operation::relateng {
    int DimensionLocation::locationArea(Location loc) {
        switch (loc) {
        case Location::INTERIOR:
            return AREA_INTERIOR;
        case Location::BOUNDARY:
            return AREA_BOUNDARY;
        default:
            return EXTERIOR;
        }
    }

    int DimensionLocation::locationLine(Location loc) {
        switch (loc) {
        case Location::INTERIOR:
            return LINE_INTERIOR;
        case Location::BOUNDARY:
            return LINE_BOUNDARY;
        default:
            return EXTERIOR;
        }
    }

    int DimensionLocation::locationPoint(Location loc) {
        switch (loc) {
        case Location::INTERIOR:
            return POINT_INTERIOR;
        default:
            return EXTERIOR;
        }
    }

    DimensionLocation::Location DimensionLocation::location(int dimLoc) {
        switch (dimLoc) {
        case POINT_INTERIOR:
        case LINE_INTERIOR:
        case AREA_INTERIOR:
            return Location::INTERIOR;
        case LINE_BOUNDARY:
        case AREA_BOUNDARY:
            return Location::BOUNDARY;
        default:
            return Location::EXTERIOR;
        }
    }

    int DimensionLocation::dimension(int dimLoc) {
        switch (dimLoc) {
        case POINT_INTERIOR:
            return Dimension::P;
        case LINE_INTERIOR:
        case LINE_BOUNDARY:
            return Dimension::L;
        case AREA_INTERIOR:
        case AREA_BOUNDARY:
            return Dimension::A;
        default:
            return Dimension::False;
        }
    }

    int DimensionLocation::dimension(int dimLoc, int exteriorDim) {
        if (dimLoc == EXTERIOR)
            return exteriorDim;
        else
            return dimension(dimLoc);
    }
}  // namespace SpatialFHE::operation::relateng

// operation
// SpatialFHE