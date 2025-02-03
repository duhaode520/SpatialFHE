//
// Created by ubuntu on 1/30/25.
//

#include "TFHEEdgeSegmentIntersector.h"

#include "TFHERelateSegmentString.h"
#include "TFHETopologyComputer.h"

using SpatialFHE::geom::TFHECoordinate;

namespace SpatialFHE::operation::relateng {
    void TFHEEdgeSegmentIntersector::addIntersections(
        TFHERelateSegmentString *ssA,
        std::size_t segIndexA,
        TFHERelateSegmentString *ssB,
        std::size_t segIndexB) {
        const TFHECoordinate &a0 = ssA->getCoordinate(segIndexA);
        const TFHECoordinate &a1 = ssA->getCoordinate(segIndexA + 1);
        const TFHECoordinate &b0 = ssB->getCoordinate(segIndexB);
        const TFHECoordinate &b1 = ssB->getCoordinate(segIndexB + 1);

        li.computeIntersection(a0, a1, b0, b1);
        if (!li.hasIntersection()) {
            return;
        }

        for (size_t i = 0; i < li.getIntersectionNum(); i++) {
            const TFHECoordinate &intPt = li.getIntersection(i);
            if (li.isProper() ||
                ssA->isContainingSegment(segIndexA, &intPt) && ssB->isContainingSegment(segIndexB, &intPt)) {
                TFHENodeSection *nsa = ssA->createNodeSection(segIndexA, intPt);
                TFHENodeSection *nsb = ssB->createNodeSection(segIndexB, intPt);
                topoComputer.addIntersection(nsa, nsb);
            }
        }
    }

    void TFHEEdgeSegmentIntersector::processIntersections(
        TFHESegmentString *ss0,
        std::size_t segIndex0,
        TFHESegmentString *ss1,
        std::size_t segIndex1) {
        // don't intersect a segment with itself
        if (ss0 == ss1 && segIndex0 == segIndex1)
            return;

        TFHERelateSegmentString *rss0 = dynamic_cast<TFHERelateSegmentString *>(ss0);
        TFHERelateSegmentString *rss1 = dynamic_cast<TFHERelateSegmentString *>(ss1);

        // TODO: move this ordering logic to TopologyBuilder
        if (rss0->isA()) {
            addIntersections(rss0, segIndex0, rss1, segIndex1);
        } else {
            addIntersections(rss1, segIndex1, rss0, segIndex0);
        }
    }

    bool TFHEEdgeSegmentIntersector::isDone() const {
        return topoComputer.isResultKnown();
    }
}  // namespace SpatialFHE::operation::relateng
