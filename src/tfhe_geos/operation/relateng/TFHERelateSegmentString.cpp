//
// Created by ubuntu on 1/14/25.
//

#include "TFHERelateSegmentString.h"

#include <tfhe_geos/constants.h>
#include <tfhe_geos/geom/enums.h>

#include "TFHENodeSection.h"

using namespace SpatialFHE::geom;

namespace SpatialFHE::operation::relateng {
    const TFHERelateSegmentString *TFHERelateSegmentString::createSegmentString(
        const TFHECoordinateSequence *pts,
        bool isA,
        int dimension,
        int id,
        int ringId,
        const TFHEGeometry *poly,
        const TFHERelateGeometry *parent) {
        return new TFHERelateSegmentString(pts, isA, dimension, id, ringId, poly, parent);
    }

    const TFHECoordinate *TFHERelateSegmentString::prevVertex(std::size_t segIndex, const TFHECoordinate *pt) const {
        const TFHECoordinate &segStart = getCoordinate(segIndex);
        if (!(segStart == *pt)) {
            return &segStart;
        }

        // pt is the start of the segment
        if (segIndex > 0) {
            const TFHECoordinate &prev = getCoordinate(segIndex - 1);
            return &prev;
        }

        if (isClosed()) {
            return &(prevInRing(segIndex));
        }
        return nullptr;
    }

    const TFHECoordinate *TFHERelateSegmentString::nextVertex(std::size_t segIndex, const TFHECoordinate *pt) const {
        const TFHECoordinate &segEnd = getCoordinate(segIndex + 1);
        if (!(segEnd == *pt)) {
            return &segEnd;
        }

        // pt is the end of the segment
        if (size() == 2 && segIndex == INDEX_UNKNOWN) {
            const TFHECoordinate &seg = getCoordinate(0);
            return &seg;
        }
 if (segIndex < size() - 2) {
            const TFHECoordinate &next = getCoordinate(segIndex + 2);
            return &next;
        }
 if (isClosed()) {
            return &(nextInRing(segIndex));
        }
        return nullptr;
    }

    const TFHERelateSegmentString *TFHERelateSegmentString::createLine(
        const TFHECoordinateSequence *pts,
        bool isA,
        int elementId,
        const TFHERelateGeometry *parent) {
        return createSegmentString(pts, isA, Dimension::L, elementId, -1, nullptr, parent);
    }

    const TFHERelateSegmentString *TFHERelateSegmentString::createRing(
        const TFHECoordinateSequence *pts,
        bool isA,
        int elementId,
        int ringId,
        const TFHEGeometry *poly,
        const TFHERelateGeometry *parent) {
        return createSegmentString(pts, isA, Dimension::A, elementId, ringId, poly, parent);
    }

    TFHENodeSection *TFHERelateSegmentString::createNodeSection(std::size_t segIndex, const TFHECoordinate intPt)
        const {
        const TFHECoordinate &c0 = getCoordinate(segIndex);
        const TFHECoordinate &c1 = getCoordinate(segIndex + 1);
        TFHEBool isNodeAtVertex = intPt.equals(c0) || intPt.equals(c1);
        const TFHECoordinate *prev = prevVertex(segIndex, &intPt);
        const TFHECoordinate *next = nextVertex(segIndex, &intPt);
        return new TFHENodeSection(
            m_isA, m_dimension, m_id, m_ringId, m_parentPolygonal, isNodeAtVertex.decrypt(), prev, intPt, next);
    }

    bool TFHERelateSegmentString::isContainingSegment(std::size_t segIndex, const TFHECoordinate *pt) const {
        const TFHECoordinate &c0 = getCoordinate(segIndex);
        if (*pt == c0)
            return true;
        const TFHECoordinate &c1 = getCoordinate(segIndex + 1);
        if (*pt == c1) {
            bool isFinalSegment = segIndex == size() - 2;
            if (isClosed() || !isFinalSegment)
                return false;
            //-- for final segment, process intersections with final endpoint
            return true;
        }
        //-- intersection is interior - process it
        return true;
    }

}  // namespace SpatialFHE::operation::relateng

// operation
// SpatialFHE