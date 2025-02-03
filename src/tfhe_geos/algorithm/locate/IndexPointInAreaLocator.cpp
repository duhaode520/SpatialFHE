//
// Created by ubuntu on 1/25/25.
//

#include "IndexPointInAreaLocator.h"
#include <stdexcept>


namespace SpatialFHE::algorithm::locate {
    IndexPointInAreaLocator::IndexPointInAreaLocator(const geom::TFHEGeometry &g) : areaGeom(g){}

    const geom::TFHEGeometry &IndexPointInAreaLocator::getGeometry() const {
        return areaGeom;
    }

    geom::Location IndexPointInAreaLocator::locate(const geom::TFHECoordinate *p) {
        throw std::runtime_error("Not implemented yet");
    }
} // locate