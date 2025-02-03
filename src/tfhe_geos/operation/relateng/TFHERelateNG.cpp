//
// Created by ubuntu on 1/14/25.
//

#include "TFHERelateNG.h"

#include <tfhe_geos/geom/TFHEGeometry.h>
#include <tfhe_geos/geom/TFHELineString.h>
#include <tfhe_geos/geom/TFHELinearRing.h>
#include <tfhe_geos/geom/TFHEPoint.h>
#include <tfhe_geos/geom/TFHEPolygon.h>
#include <stdexcept>

#include "DimensionLocation.h"
#include "TFHEEdgeSegmentIntersector.h"
#include "TFHERelateGeometry.h"
#include "TFHERelateMatrixPredicate.h"
#include "TFHERelatePredicate.h"
#include "TFHETopologyComputer.h"

using namespace SpatialFHE::geom;
using SpatialFHE::noding::TFHESegmentString;

namespace SpatialFHE::operation::relateng {
    TFHERelateNG::TFHERelateNG(const TFHEGeometry *inputA, bool isPrepared) : geomA(inputA, isPrepared) {}

    void TFHERelateNG::computePointPoint(TFHERelateGeometry &geomB, TFHETopologyComputer &topoComputer) {
        TFHECoordinate::ConstSet &ptsA = geomA.getUniquePoints();
        TFHECoordinate::ConstSet &ptsB = geomB.getUniquePoints();

        size_t numBinA = 0;
        for (const TFHECoordinate *ptB : ptsB) {
            auto it = ptsA.find(ptB);
            bool found = (it != ptsA.end());
            if (found) {
                numBinA++;
                topoComputer.addPointOnPointInterior(ptB);
            } else {
                topoComputer.addPointOnPointExterior(TFHERelateGeometry::GEOM_B, ptB);
            }
            if (topoComputer.isResultKnown()) {
                return;
            }
        }

        if (numBinA < ptsA.size()) {
            topoComputer.addPointOnPointExterior(TFHERelateGeometry::GEOM_A, nullptr);
        }
    }

    void TFHERelateNG::computeAtPoints(
        TFHERelateGeometry &geom,
        bool isA,
        TFHERelateGeometry &geomTarget,
        TFHETopologyComputer &topoComputer) {
        bool isResultKnown = false;
        isResultKnown = computePoints(geom, isA, geomTarget, topoComputer);
        if (isResultKnown) {
            return;
        }

        // 只在 geomTarget 是 Area 或者 需要监测 Exterior 的情况下才会执行
        bool checkDisjointPoints = geomTarget.hasDimension(Dimension::A) || topoComputer.isExteriorCheckRequired(isA);
        if (!checkDisjointPoints) {
            return;
        }

        isResultKnown = computeLineEnds(geom, isA, geomTarget, topoComputer);
        if (isResultKnown) {
            return;
        }
        computeAreaVertex(geom, isA, geomTarget, topoComputer);
    }

    bool TFHERelateNG::computePoints(
        TFHERelateGeometry &geom,
        bool isA,
        TFHERelateGeometry &geomTarget,
        TFHETopologyComputer &topoComputer) {
        if (!geom.hasDimension(Dimension::P)) {
            return false;
        }

        std::vector<const TFHEPoint *> points = geom.getEffectivePoints();
        for (const TFHEPoint *point : points) {
            if (point->isEmpty()) {
                continue;
            }
            const TFHECoordinate *pt = point->getCoordinate();
            computePoint(isA, pt, geomTarget, topoComputer);
            if (topoComputer.isResultKnown()) {
                return true;
            }
        }
        return false;
    }

    void TFHERelateNG::computePoint(
        bool isA,
        const TFHECoordinate *pt,
        TFHERelateGeometry &geomTarget,
        TFHETopologyComputer &topoComputer) {
        int locDimTarget = geomTarget.locateWithDim(pt);
        Location locTarget = DimensionLocation::location(locDimTarget);
        int dimTarget = DimensionLocation::dimension(locDimTarget, topoComputer.getDimension(!isA));
        topoComputer.addPointOnGeometry(isA, locTarget, dimTarget, pt);
    }

    bool TFHERelateNG::computeLineEnds(
        TFHERelateGeometry &geom,
        bool isA,
        TFHERelateGeometry &geomTarget,
        TFHETopologyComputer &topoComputer) {
        // only handle lines
        if (!geom.hasDimension(Dimension::L)) {
            return false;
        }

        bool hasExteriorIntersection = false;
        std::vector<const TFHEGeometry *> elems;
        // The codes below is used for geometry collections, which is not supported in this version
        // GeometryLister::list(geom.getGeometry(), elems);
        // simply push the geometry into elems
        elems.push_back(geom.getGeometry());

        for (const TFHEGeometry *elem : elems) {
            if (elem->isEmpty()) {
                continue;
            }
            if (elem->getGeometryTypeId() == TFHEGeometryTypeId::TFHE_LINEARRING ||
                elem->getGeometryTypeId() == TFHEGeometryTypeId::TFHE_LINESTRING) {
                if (hasExteriorIntersection && elem->getEnvelope()->disjoint(geomTarget.getEnvelope()).decrypt()) {
                    continue;
                }

                const TFHELineString *line = dynamic_cast<const TFHELineString *>(elem);
                const TFHECoordinate &e0 = line->getCoordinatesRO()->getAt(0);
                hasExteriorIntersection |= computeLineEnd(geom, isA, &e0, geomTarget, topoComputer);
                if (topoComputer.isResultKnown()) {
                    return true;
                }

                if (!line->isClosed().decrypt()) {
                    const TFHECoordinate &e1 = line->getCoordinatesRO()->getAt(line->getNumPoints() - 1);
                    hasExteriorIntersection |= computeLineEnd(geom, isA, &e1, geomTarget, topoComputer);
                    if (topoComputer.isResultKnown()) {
                        return true;
                    }
                }
            }
        }
        return false;
    }

    bool TFHERelateNG::computeLineEnd(
        TFHERelateGeometry &geom,
        bool isA,
        const TFHECoordinate *pt,
        TFHERelateGeometry &geomTarget,
        TFHETopologyComputer &topoComputer) {
        int locDimLineEnd = geom.locateLineEndWithDim(pt);
        int dimLineEnd = DimensionLocation::dimension(locDimLineEnd, topoComputer.getDimension(isA));
        //-- skip line ends which are in a GC area
        if (dimLineEnd != Dimension::L)
            return false;
        Location locLineEnd = DimensionLocation::location(locDimLineEnd);
        int locDimTarget = geomTarget.locateWithDim(pt);
        Location locTarget = DimensionLocation::location(locDimTarget);
        int dimTarget = DimensionLocation::dimension(locDimTarget, topoComputer.getDimension(!isA));
        topoComputer.addLineEndOnGeometry(isA, locLineEnd, locTarget, dimTarget, pt);
        return locTarget == Location::EXTERIOR;
    }

    bool TFHERelateNG::computeAreaVertex(
        TFHERelateGeometry &geom,
        bool isA,
        TFHERelateGeometry &geomTarget,
        TFHETopologyComputer &topoComputer) {
        // 只处理 area/area
        if (!geom.hasDimension(Dimension::A)) {
            return false;
        }
        if (geomTarget.getDimension() < Dimension::L) {
            return false;
        }

        bool hasExteriorIntersections = false;

        std::vector<const TFHEGeometry *> elems;
        // GeometryLister::list(geom.getGeometry(), elems);
        elems.push_back(geom.getGeometry());
        for (const TFHEGeometry *elem : elems) {
            if (elem->isEmpty()) {
                continue;
            }

            if (elem->getGeometryTypeId() == TFHEGeometryTypeId::TFHE_POLYGON) {
                if (hasExteriorIntersections && elem->getEnvelope()->disjoint(geomTarget.getEnvelope()).decrypt()) {
                    continue;
                }
                const TFHEPolygon *poly = dynamic_cast<const TFHEPolygon *>(elem);
                hasExteriorIntersections |=
                    computeAreaVertex(geom, isA, poly->getExteriorRing(), geomTarget, topoComputer);
                if (topoComputer.isResultKnown()) {
                    return true;
                }
                for (std::size_t j = 0; j < poly->getNumInteriorRing(); j++) {
                    hasExteriorIntersections |=
                        computeAreaVertex(geom, isA, poly->getInteriorRingN(j), geomTarget, topoComputer);
                    if (topoComputer.isResultKnown()) {
                        return true;
                    }
                }
            }
        }
        return false;
    }

    bool TFHERelateNG::computeAreaVertex(
        TFHERelateGeometry &geom,
        bool isA,
        const TFHELinearRing *ring,
        TFHERelateGeometry &geomTarget,
        TFHETopologyComputer &topoComputer) {
        const TFHECoordinate *pt = ring->getCoordinate();

        Location locArea = geom.locateAreaVertex(pt);
        int locDimTarget = geomTarget.locateWithDim(pt);
        Location locTarget = DimensionLocation::location(locDimTarget);
        int dimTarget = DimensionLocation::dimension(locDimTarget, topoComputer.getDimension(!isA));
        topoComputer.addAreaVertex(isA, locArea, locTarget, dimTarget, pt);
        return locTarget == Location::EXTERIOR;
    }

    void TFHERelateNG::computeAtEdges(TFHERelateGeometry &geomB, TFHETopologyComputer &topoComputer) {
        TFHEEnvelope envInt = geomA.getEnvelope()->intersection(*geomB.getEnvelope());
        if (envInt.isNull()) {
            return;
        }

        std::vector<const TFHESegmentString *> edgesB =
            geomB.extractSegmentStrings(TFHERelateGeometry::GEOM_B, &envInt);
        TFHEEdgeSegmentIntersector intersector(topoComputer);

        if (topoComputer.isSelfNodingRequired()) {
            computeEdgesAll(edgesB, &envInt, intersector);
        } else {
            computeEdgesMutual(edgesB, &envInt, intersector);
        }
        if (topoComputer.isResultKnown()) {
            return;
        }

        topoComputer.evaluateNodes();
    }

    void TFHERelateNG::computeEdgesAll(
        std::vector<const TFHESegmentString *> &edgesB,
        const TFHEEnvelope *envInt,
        TFHEEdgeSegmentIntersector &intersector) {
        std::vector<const TFHESegmentString *> edgesA = geomA.extractSegmentStrings(TFHERelateGeometry::GEOM_A, envInt);
        // TODO: Change from GEOS. We haven't supported index yet
        // 我们没有使用 EdgeSetIntersector 和相关的逻辑, 从 EdgeSetIntersector 的 process 函数跟踪到了一个 overlapAction
        // 这个overlapAction 实际上是从 index 的节点中取出两个 SegmentString 并用 EdgeSegmentIntersector 进行计算
        // 但是我们暂时没有实现 index, 所以这里直接使用 EdgeSegmentIntersector 进行计算
        for (const TFHESegmentString *edgeA : edgesA) {
            for (const TFHESegmentString *edgeB : edgesB) {
                for (int i = 0; i < edgeA->size() - 1; i++) {
                    for (int j = 0; j < edgeB->size() - 1; j++) {
                        // copy
                        intersector.processIntersections(
                            const_cast<TFHESegmentString *>(edgeA), i, const_cast<TFHESegmentString *>(edgeA), j);
                        if (intersector.isDone()) {
                            return;
                        }
                    }
                }
                if (intersector.isDone()) {
                    return;
                }
            }
        }
    }

    void TFHERelateNG::computeEdgesMutual(
        std::vector<const TFHESegmentString *> &edgesB,
        const TFHEEnvelope *envInt,
        TFHEEdgeSegmentIntersector &intersector) {
        /**
        * 感觉 selfnoding 这个是针对 collections 的，但是我们暂时不支持 collections
        */
        throw std::runtime_error("computeEdgesMutual is not implemented yet.");
    }

    bool TFHERelateNG::hasRequiredEnvelopeInteraction(const TFHEGeometry *b, TFHETopologyPredicate &predicate) {
        const TFHEEnvelope *envB = b->getEnvelope();
        const TFHEEnvelope *envA = geomA.getEnvelope();
        bool requireCoversA = predicate.requireCovers(TFHERelateGeometry::GEOM_A);
        bool requireCoversB = predicate.requireCovers(TFHERelateGeometry::GEOM_B);
        bool requireInteraction = predicate.requireInteraction();

        // Branching optimization to reduce decrypt ops. The method has passed correctness check
        if (!requireCoversA && !requireCoversB && requireInteraction) {
            return envA->intersects(envB).decrypt();
        }
        if (requireCoversB && !requireCoversA) {
            return envB->covers(envA).decrypt();
        }
        if (requireCoversA) {
            return envA->covers(envB).decrypt();
        }
        return true;
    }

    TFHEBool TFHERelateNG::finishValue(TFHETopologyPredicate &predicate) {
        predicate.finish();
        return predicate.value();
    }

    TFHEBool TFHERelateNG::relate(const TFHEGeometry *a, const TFHEGeometry *b, TFHETopologyPredicate &pred) {
        TFHERelateNG rng(a, false);
        return rng.evaluate(b, pred);
    }

    TFHEBool TFHERelateNG::relate(const TFHEGeometry *a, const TFHEGeometry *b, const std::string &imPattern) {
        TFHERelateNG rng(a, false);
        return rng.evaluate(b, imPattern);
    }

    std::unique_ptr<TFHERelateNG::TFHEIntersectionMatrix> TFHERelateNG::relate(
        const TFHEGeometry *a,
        const TFHEGeometry *b) {
        TFHERelateNG rng(a, false);
        return rng.evaluate(b);
    }

    std::unique_ptr<TFHERelateNG> TFHERelateNG::prepare(const TFHEGeometry *a) {
        return std::unique_ptr<TFHERelateNG>(new TFHERelateNG(a, true));
    }

    std::unique_ptr<TFHERelateNG::TFHEIntersectionMatrix> TFHERelateNG::evaluate(const TFHEGeometry *b) {
        TFHERelateMatrixPredicate rel;
        evaluate(b, rel);
        return rel.getIM();
    }

    TFHEBool TFHERelateNG::evaluate(const TFHEGeometry *b, const std::string &imPattern) {
        auto predicate = TFHERelatePredicate::matches(imPattern);
        return evaluate(b, *predicate);
    }

    TFHEBool TFHERelateNG::evaluate(const TFHEGeometry *b, TFHETopologyPredicate &predicate) {
        // fast envelope check
        if (!hasRequiredEnvelopeInteraction(b, predicate)) {
            return TFHEBool::tfhe_false;
        }

        TFHERelateGeometry geomB(b, false);

        int dimA = geomA.getDimension();
        int dimB = geomB.getDimension();

        // check if predicate is determined by dimension or envelope
        predicate.init(dimA, dimB);
        if (predicate.isKnown()) {
            return finishValue(predicate);
        }
        predicate.init(*geomA.getEnvelope(), *geomB.getEnvelope());
        if (predicate.isKnown()) {
            return finishValue(predicate);
        }

        TFHETopologyComputer topoComputer(predicate, geomA, geomB);

        // optimized p/p evaluation
        if (dimA == Dimension::P && dimB == Dimension::P) {
            computePointPoint(geomB, topoComputer);
            topoComputer.finish();
            return topoComputer.getResult();
        }

        // points against non-points
        computeAtPoints(geomB, TFHERelateGeometry::GEOM_B, geomA, topoComputer);
        if (topoComputer.isResultKnown()) {
            return topoComputer.getResult();
        }
        computeAtPoints(geomA, TFHERelateGeometry::GEOM_A, geomB, topoComputer);
        if (topoComputer.isResultKnown()) {
            return topoComputer.getResult();
        }

        if (geomA.hasEdges() && geomB.hasEdges()) {
            computeAtEdges(geomB, topoComputer);
        }

        topoComputer.finish();
        return topoComputer.getResult();
    }

    TFHEBool TFHERelateNG::intersects(const TFHEGeometry *a, const TFHEGeometry *b) {
        TFHERelatePredicate::IntersectsPredicate pred;
        return relate(a, b, pred);
    }

    TFHEBool TFHERelateNG::crosses(const TFHEGeometry *a, const TFHEGeometry *b) {
        TFHERelatePredicate::CrossesPredicate pred;
        return relate(a, b, pred);
    }

    TFHEBool TFHERelateNG::disjoint(const TFHEGeometry *a, const TFHEGeometry *b) {
        TFHERelatePredicate::DisjointPredicate pred;
        return relate(a, b, pred);
    }

    TFHEBool TFHERelateNG::touches(const TFHEGeometry *a, const TFHEGeometry *b) {
        TFHERelatePredicate::TouchesPredicate pred;
        return relate(a, b, pred);
    }

    TFHEBool TFHERelateNG::within(const TFHEGeometry *a, const TFHEGeometry *b) {
        TFHERelatePredicate::WithinPredicate pred;
        return relate(a, b, pred);
    }

    TFHEBool TFHERelateNG::contains(const TFHEGeometry *a, const TFHEGeometry *b) {
        TFHERelatePredicate::ContainsPredicate pred;
        return relate(a, b, pred);
    }

    TFHEBool TFHERelateNG::overlaps(const TFHEGeometry *a, const TFHEGeometry *b) {
        TFHERelatePredicate::OverlapsPredicate pred;
        return relate(a, b, pred);
    }

    TFHEBool TFHERelateNG::covers(const TFHEGeometry *a, const TFHEGeometry *b) {
        TFHERelatePredicate::CoversPredicate pred;
        return relate(a, b, pred);
    }

    TFHEBool TFHERelateNG::coveredBy(const TFHEGeometry *a, const TFHEGeometry *b) {
        TFHERelatePredicate::CoveredByPredicate pred;
        return relate(a, b, pred);
    }

    TFHEBool TFHERelateNG::equalsTopo(const TFHEGeometry *a, const TFHEGeometry *b) {
        TFHERelatePredicate::EqualsTopoPredicate pred;
        return relate(a, b, pred);
    }

    TFHEBool TFHERelateNG::intersects(const TFHEGeometry *b) {
        TFHERelatePredicate::IntersectsPredicate pred;
        return evaluate(b, pred);
    }

    TFHEBool TFHERelateNG::crosses(const TFHEGeometry *b) {
        TFHERelatePredicate::CrossesPredicate pred;
        return evaluate(b, pred);
    }

    TFHEBool TFHERelateNG::disjoint(const TFHEGeometry *b) {
        TFHERelatePredicate::DisjointPredicate pred;
        return evaluate(b, pred);
    }

    TFHEBool TFHERelateNG::touches(const TFHEGeometry *b) {
        TFHERelatePredicate::TouchesPredicate pred;
        return evaluate(b, pred);
    }

    TFHEBool TFHERelateNG::within(const TFHEGeometry *b) {
        TFHERelatePredicate::WithinPredicate pred;
        return evaluate(b, pred);
    }

    TFHEBool TFHERelateNG::contains(const TFHEGeometry *b) {
        TFHERelatePredicate::ContainsPredicate pred;
        return evaluate(b, pred);
    }

    TFHEBool TFHERelateNG::overlaps(const TFHEGeometry *b) {
        TFHERelatePredicate::OverlapsPredicate pred;
        return evaluate(b, pred);
    }

    TFHEBool TFHERelateNG::covers(const TFHEGeometry *b) {
        TFHERelatePredicate::CoversPredicate pred;
        return evaluate(b, pred);
    }

    TFHEBool TFHERelateNG::coveredBy(const TFHEGeometry *b) {
        TFHERelatePredicate::CoveredByPredicate pred;
        return evaluate(b, pred);
    }

    TFHEBool TFHERelateNG::equalsTopo(const TFHEGeometry *b) {
        TFHERelatePredicate::EqualsTopoPredicate pred;
        return evaluate(b, pred);
    }

    TFHEBool TFHERelateNG::relate(const TFHEGeometry *b, const std::string &pat) {
        return evaluate(b, pat);
    }

    std::unique_ptr<TFHERelateNG::TFHEIntersectionMatrix> TFHERelateNG::relate(const TFHEGeometry *b) {
        return evaluate(b);
    }
}  // namespace SpatialFHE::operation::relateng

// operation
// SpatialFHE