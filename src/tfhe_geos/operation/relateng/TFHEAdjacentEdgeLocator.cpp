//
// Created by ubuntu on 1/21/25.
//

#include "TFHEAdjacentEdgeLocator.h"

#include <tfhe_geos/algorithm/TFHEOrientation.h>
#include <tfhe_geos/algorithm/TFHEPointLocation.h>

#include "TFHENodeSections.h"
#include "TFHERelateNode.h"
#include "tfhe_geos/geom/TFHEGeometry.h"
#include "tfhe_geos/geom/TFHELinearRing.h"
#include "tfhe_geos/geom/TFHEPolygon.h"

using namespace SpatialFHE::geom;
using namespace  SpatialFHE::algorithm;

namespace SpatialFHE::operation::relateng {
    void TFHEAdjacentEdgeLocator::init(const TFHEGeometry *geom) {
        if (geom->isEmpty()) {
            return;
        }
        addRings(geom);
    }

    void TFHEAdjacentEdgeLocator::addRings(const TFHEGeometry *geom) {
        if (const TFHEPolygon* poly = dynamic_cast<const TFHEPolygon*>(geom)) {
            const TFHELinearRing* shell = poly->getExteriorRing();
            addRing(shell, true);
            for (size_t i = 0; i < poly->getNumInteriorRing(); i++) {
                const TFHELinearRing* hole = poly->getInteriorRingN(i);
                addRing(hole, false);
            }
        }

        // Currently, only support Polygon
        // else if (geom->isCollection()) {
        //}
    }

    void TFHEAdjacentEdgeLocator::addRing(const TFHELinearRing *ring, bool requireCCW) {
        const TFHECoordinateSequence *pts = ring->getCoordinatesRO();
        TFHEBool isFlipped = TFHEOrientation::isCCW(pts) == requireCCW;
        // In case of flipped rings, we need to keep a local copy of the reversed rings
        if (isFlipped.decrypt()) {
            std::unique_ptr<TFHECoordinateSequence> localPts = pts->clone();
            localPts->reverse();
            rings.push_back(localPts.get());
            localRings.push_back(std::move(localPts));
        } else {
            rings.push_back(pts);
        }
    }

    void TFHEAdjacentEdgeLocator::addSections(
        const TFHECoordinate *p,
        const TFHECoordinateSequence *ring,
        TFHENodeSections &sections) {

        for (std::size_t i = 0; i < ring->size() - 1; i++) {
            const TFHECoordinate &p0 = ring->getAt(i);
            const TFHECoordinate &pnext = ring->getAt(i + 1);
            if (p0 == *p) {
                std::size_t iprev = i > 0 ? i - 1 : ring->size() - 2;
                const TFHECoordinate &pprev = ring->getAt(iprev);
                TFHENodeSection* ns = createSection(p, &pprev, &pnext);
                sections.addNodeSection(ns);
            } else if (TFHEPointLocation::isOnSegment(*p, p0, pnext).decrypt()) {
                TFHENodeSection *ns = createSection(p, &p0, &pnext);
                sections.addNodeSection(ns);
            }
        }
    }

    TFHENodeSection *TFHEAdjacentEdgeLocator::createSection(
        const TFHECoordinate *p,
        const TFHECoordinate *prev,
        const TFHECoordinate *next) {
            return new TFHENodeSection(true, Dimension::A, 1, 0, nullptr, false, prev, *p, next);
        }

    TFHEAdjacentEdgeLocator::Location TFHEAdjacentEdgeLocator::locate(const geom::TFHECoordinate *p) {
        TFHENodeSections sections(p);
        for (const TFHECoordinateSequence* ring : rings) {
            addSections(p, ring, sections);
        }
        std::unique_ptr<TFHERelateNode> node = sections.createNode();

        return node->hasExteriorEdge(true) ? Location::BOUNDARY : Location::INTERIOR;
    }
}  // namespace SpatialFHE::operation::relateng

// operation
// SpatialFHE