//
// Created by ubuntu on 1/14/25.
//

#include "TFHERelatePointLocator.h"

#include <tfhe_geos/algorithm/TFHEPointLocation.h>
#include <tfhe_geos/algorithm/locate/SimplePointInAreaLocator.h>
#include <tfhe_geos/geom/TFHELineString.h>
#include <tfhe_geos/geom/TFHEPoint.h>

#include "DimensionLocation.h"

using namespace SpatialFHE::geom;
using namespace SpatialFHE::algorithm;

namespace SpatialFHE::operation::relateng {
    int TFHERelatePointLocator::locateWithDim(
        const TFHECoordinate *p,
        bool isNode,
        const TFHEGeometry *parentPolygonal) {
        if (isEmpty) {
            return DimensionLocation::EXTERIOR;
        }

        /**
         * In a polygonal geometry a node must be on the boundary.
         * (This is not the case for a mixed collection, since
         * the node may be in the interior of a polygon.)
         */
        TFHEGeometryTypeId geomType = geom->getGeometryTypeId();
        if (isNode && geomType == TFHEGeometryTypeId::TFHE_POLYGON )
            return DimensionLocation::AREA_BOUNDARY;

        int dimLoc = computeDimLocation(p, isNode, parentPolygonal);
        return dimLoc;
    }

    int TFHERelatePointLocator::computeDimLocation(
        const TFHECoordinate *p,
        bool isNode,
        const TFHEGeometry *parentPolygonal) {

        //-- check dimensions in order of precedence
        if (!polygons.empty()) {
            Location locPoly = locateOnPolygons(p, isNode, parentPolygonal);
            if (locPoly != Location::EXTERIOR)
                return DimensionLocation::locationArea(locPoly);
        }
        if (!lines.empty()) {
            Location locLine = locateOnLines(p, isNode);
            if (locLine != Location::EXTERIOR)
                return DimensionLocation::locationLine(locLine);
        }
        if (!points.empty()) {
            Location locPt = locateOnPoints(p);
            if (locPt != Location::EXTERIOR)
                return DimensionLocation::locationPoint(locPt);
        }
        return DimensionLocation::EXTERIOR;
    }

    Location TFHERelatePointLocator::locateOnPoints(const TFHECoordinate *p) const {
        auto search = points.find(p);
        if (search != points.end()) {
            return Location::INTERIOR;
        } else {
            return Location::EXTERIOR;
        }
    }

    Location TFHERelatePointLocator::locateOnLines(const TFHECoordinate *p, bool isNode) {
        if (lineBoundary != nullptr && lineBoundary->isBoundary(p)) {
            return Location::BOUNDARY;
        }
        // node must be on line, in interior
        if (isNode) {
            return Location::INTERIOR;
        }

        for (const TFHELineString* line: lines) {
            // check every line
            Location loc = locateOnLine(p, line);
            if (loc != Location::EXTERIOR) {
                return loc;
            }
        }
        return Location::EXTERIOR;
    }

    Location TFHERelatePointLocator::locateOnLine(const TFHECoordinate *p, const TFHELineString *l) {
        if (!l->getEnvelope()->intersects(*p).decrypt()) {
            return Location::EXTERIOR;
        }

        const TFHECoordinateSequence *seq = l->getCoordinatesRO();
        // TODO: return an encrypted Location?
        if (TFHEPointLocation::isOnLine(*p, seq).decrypt()) {
            return Location::INTERIOR;
        }
        return Location::EXTERIOR;
    }

    Location TFHERelatePointLocator::locateOnPolygons(
        const TFHECoordinate *p,
        bool isNode,
        const TFHEGeometry *parentPolygonal) {
        int numBdy = 0;
        for (std::size_t i = 0; i < polygons.size(); i++) {
            Location loc = locateOnPolygonal(p, isNode, parentPolygonal, i);
            if (loc == Location::INTERIOR) {
                return Location::INTERIOR;
            }
            if (loc == Location::BOUNDARY) {
                numBdy++;
            }
        }
        if (numBdy == 1) {
            return Location::BOUNDARY;
        } else if (numBdy > 1) {
            // check for poing lying on adjacent boundaries
            if (adjEdgeLocator == nullptr) {
                adjEdgeLocator.reset();
            }
            return adjEdgeLocator->locate(p);
        }
        return Location::EXTERIOR;
    }

    Location TFHERelatePointLocator::locateOnPolygonal(
        const TFHECoordinate *p,
        bool isNode,
        const TFHEGeometry *parentPolygonal,
        std::size_t index) {
        const TFHEGeometry *polygonal = polygons[index];
        if (isNode && parentPolygonal == polygonal) {
            return Location::BOUNDARY;
        }
        TFHEPointOnGeometryLocator *locator = getLocator(index);
        return locator->locate(p);
    }

    TFHERelatePointLocator::TFHEPointOnGeometryLocator *TFHERelatePointLocator::getLocator(std::size_t index) {
        std::unique_ptr<TFHEPointOnGeometryLocator>& locator = polyLocator[index];
        if (locator == nullptr) {
            // TODO: is prepared should use IndexedPointInAreaLocator, but it is not implemented yet
            const TFHEGeometry *geom = polygons[index];
            locator.reset(new locate::SimplePointInAreaLocator(geom));
        }
        return locator.get();
    }

    TFHERelatePointLocator::TFHERelatePointLocator(const TFHEGeometry *geom) : TFHERelatePointLocator(geom, false) {}

    TFHERelatePointLocator::TFHERelatePointLocator(const TFHEGeometry *geom, bool isPrepared) :
            geom(geom), isPrepared(isPrepared) {
        init(this->geom);
    }

    void TFHERelatePointLocator::init(const TFHEGeometry *geom) {
        isEmpty = geom->isEmpty();
        extractElements(geom);
        if (!lines.empty()) {
            lineBoundary.reset(new TFHELinearBoundary(lines));
        }

        if (!polygons.empty()) {
            polyLocator.resize(polygons.size());
        }
    }

    bool TFHERelatePointLocator::hasBoundary() const {
        return lineBoundary->hasBoundary();
    }

    void TFHERelatePointLocator::extractElements(const TFHEGeometry *geom) {
        if (geom->isEmpty()) {
            return;
        }

        if (geom->getGeometryTypeId() == TFHEGeometryTypeId::TFHE_POINT) {
            addPoint(dynamic_cast<const TFHEPoint *>(geom));
        } else if (
            geom->getGeometryTypeId() == TFHEGeometryTypeId::TFHE_LINEARRING ||
            geom->getGeometryTypeId() == TFHEGeometryTypeId::TFHE_LINESTRING) {
            addLine(dynamic_cast<const TFHELineString *>(geom));
        } else if (geom->getGeometryTypeId() == TFHEGeometryTypeId::TFHE_POLYGON) {
            addPolygonal(geom);
        } else {
            // collections
            // Do nothing
        }
    }

    void TFHERelatePointLocator::addPoint(const TFHEPoint *pt) {
        points.insert(pt->getCoordinate());
    }

    void TFHERelatePointLocator::addLine(const TFHELineString *line) {
        lines.push_back(line);
    }

    void TFHERelatePointLocator::addPolygonal(const TFHEGeometry *polygonal) {
        polygons.push_back(polygonal);
    }

    Location TFHERelatePointLocator::locate(const TFHECoordinate *p) {}

    int TFHERelatePointLocator::locateLineEndWithDim(const TFHECoordinate *p) {
        if (!polygons.empty()) {
            Location locPoly =  locateOnPolygons(p, false, nullptr);
            if (locPoly != Location::EXTERIOR) {
                return DimensionLocation::locationArea(locPoly);
            }
        }
        return lineBoundary->isBoundary(p) ? DimensionLocation::LINE_BOUNDARY : DimensionLocation::LINE_INTERIOR;
    }

    TFHERelatePointLocator::Location TFHERelatePointLocator::locateNode(
        const TFHECoordinate *p,
        const TFHEGeometry *parentPolygonal) {
        return DimensionLocation::location(locateNodeWithDim(p, parentPolygonal));
    }

    int TFHERelatePointLocator::locateNodeWithDim(const TFHECoordinate *p, const TFHEGeometry *parentPolygonal) {
        return locateWithDim(p, true, parentPolygonal);
    }

    int TFHERelatePointLocator::locateWithDim(const TFHECoordinate *p) {
        return locateWithDim(p, false, nullptr);
    }
}  // namespace SpatialFHE::operation::relateng

// operation
// SpatialFHE