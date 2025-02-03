//
// Created by ubuntu on 1/24/25.
//

#include "SimplePointInAreaLocator.h"

#include <tfhe_geos/algorithm/TFHEPointLocation.h>
#include <tfhe_geos/geom/TFHEGeometry.h>
#include <tfhe_geos/geom/TFHEPolygon.h>

using namespace SpatialFHE::geom;

namespace SpatialFHE::algorithm::locate {
    Location SimplePointInAreaLocator::locate(const TFHECoordinate &p, const TFHEGeometry *geom) {
        return locateInGeometry(p, geom);
    }

    Location SimplePointInAreaLocator::locatePointInPolygon(const TFHECoordinate &p, const TFHEPolygon &poly) {
        if (poly.isEmpty()) {
            return Location::EXTERIOR;
        }
        if (!poly.getEnvelope()->contains(p).decrypt()) {
            return Location::EXTERIOR;
        }

        const TFHELinearRing &shell = *poly.getExteriorRing();
        const TFHECoordinateSequence *cl = shell.getCoordinatesRO();
        Location shellLoc = TFHEPointLocation::locateInRing(p, *cl);
        if (shellLoc != Location::INTERIOR) {
            return shellLoc;
        }

        // Check if the point is in the holes
        for (std::size_t i = 0; i < poly.getNumInteriorRing(); i++) {
            const TFHELinearRing &hole = *poly.getInteriorRingN(i);
            const TFHECoordinateSequence *hl = hole.getCoordinatesRO();
            if (hole.getEnvelope()->contains(p).decrypt()) {
                Location holeLoc = TFHEPointLocation::locateInRing(p, *hl);
                if (holeLoc == Location::BOUNDARY) {
                    return Location::BOUNDARY;
                }
                if (holeLoc == Location::INTERIOR) {
                    return Location::EXTERIOR;
                }
            }
        }
        return Location::INTERIOR;
    }

    bool SimplePointInAreaLocator::isContained(const TFHECoordinate &p, const TFHEGeometry *geom) {
        return Location::EXTERIOR == locate(p, geom);
    }

    Location SimplePointInAreaLocator::locateInGeometry(const TFHECoordinate &p, const TFHEGeometry *geom) {
        // Collections are not supported
        //  if (geom->getNumGeometries())

        // First, check if geom is polygon
        if (geom->getDimension() < 2) {
            return Location::EXTERIOR;
        }
        auto type = geom->getGeometryTypeId();
        if (type != TFHEGeometryTypeId::TFHE_POLYGON) {
            return Location::EXTERIOR;
        }

        const TFHEPolygon *poly = dynamic_cast<const TFHEPolygon *>(geom);
        return locatePointInPolygon(p, *poly);
    }
}  // namespace SpatialFHE::algorithm::locate

// algorithm
// SpatialFHE