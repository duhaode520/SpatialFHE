//
// Created by ubuntu on 1/14/25.
//

#include "TFHETopologyComputer.h"

#include <tfhe_geos/algorithm/TFHEPolygonNodeTopology.h>

#include <c++/9/stdexcept>

#include "TFHERelateGeometry.h"
#include "TFHERelateNode.h"
#include "TFHETopologyPredicate.h"
#include "tfhe_geos/geom/enums.h"

using namespace SpatialFHE::geom;

namespace SpatialFHE::operation::relateng {
    void TFHETopologyComputer::initExteriorDims() {
        int dimA = geomA.getDimension();
        int dimB = geomB.getDimension();

        // Point and Line case: P exterior intersects L interior
        if (dimA == Dimension::P && dimB == Dimension::L) {
            updateDim(Location::EXTERIOR, Location::INTERIOR, Dimension::L);
        } else if (dimA == Dimension::L && dimB == Dimension::P) {
            updateDim(Location::INTERIOR, Location::EXTERIOR, Dimension::L);
        }
        // P/A case: A Interior and Boundary intersect P Exterior
        else if (dimA == Dimension::P && dimB == Dimension::A) {
            updateDim(Location::EXTERIOR, Location::INTERIOR, Dimension::A);
            updateDim(Location::EXTERIOR, Location::BOUNDARY, Dimension::L);
        } else if (dimA == Dimension::A && dimB == Dimension::P) {
            updateDim(Location::INTERIOR, Location::EXTERIOR, Dimension::A);
            updateDim(Location::BOUNDARY, Location::EXTERIOR, Dimension::L);
        }
        // L/A case: A Interior and Boundary intersect L Exterior
        else if (dimA == Dimension::L && dimB == Dimension::A) {
            updateDim(geom::Location::EXTERIOR, geom::Location::INTERIOR, Dimension::A);
        } else if (dimA == Dimension::A && dimB == Dimension::L) {
            updateDim(geom::Location::INTERIOR, geom::Location::EXTERIOR, Dimension::A);
        }
        // Empty case
        else if (dimA == Dimension::False || dimB == Dimension::False) {
            if (dimA != Dimension::False) {
                initExteriorEmpty(TFHERelateGeometry::GEOM_A);
            } else {
                initExteriorEmpty(TFHERelateGeometry::GEOM_B);
            }
        }
    }

    void TFHETopologyComputer::initExteriorEmpty(bool geomNonEmpty) {
        int dimNonEmpty = getDimension(geomNonEmpty);
        switch (dimNonEmpty) {
        case Dimension::P:
            updateDim(geomNonEmpty, Location::INTERIOR, Location::EXTERIOR, Dimension::P);
            break;
        case Dimension::L:
            if (getGeometry(geomNonEmpty).hasBoundary()) {
                updateDim(geomNonEmpty, Location::BOUNDARY, Location::EXTERIOR, Dimension::P);
            }
            updateDim(geomNonEmpty, Location::INTERIOR, Location::EXTERIOR, Dimension::L);
            break;
        case Dimension::A:
            updateDim(geomNonEmpty, Location::BOUNDARY, Location::EXTERIOR, Dimension::L);
            updateDim(geomNonEmpty, Location::INTERIOR, Location::EXTERIOR, Dimension::A);
            break;
        }
    }

    void TFHETopologyComputer::updateDim(Location locA, Location locB, int dimension) {
        predicate.updateDimension(locA, locB, dimension);
    }

    void TFHETopologyComputer::updateDim(bool isAB, Location loc1, Location loc2, int dimension) {
        if (isAB) {
            updateDim(loc1, loc2, dimension);
        } else {
            updateDim(loc2, loc1, dimension);
        }
    }

    void TFHETopologyComputer::updateIntersectionAB(const TFHENodeSection *a, const TFHENodeSection *b) {
        if (TFHENodeSection::isAreaArea(*a, *b)) {
            updateAreaAreaCross(a, b);
        }
        updateNodeLocation(a, b);
    }

    void TFHETopologyComputer::addLineEndOnLine(
        bool isLineA,
        Location locLineEnd,
        Location locLine,
        const TFHECoordinate *pt) {
        (void)pt;
        (void)locLineEnd;

        /**
         * When a line end is in the EXTERIOR of a Line,
         * some length of the source Line INTERIOR
         * is also in the target Line EXTERIOR.
         * This works for zero-length lines as well.
         */
        if (locLine == Location::EXTERIOR) {
            updateDim(isLineA, Location::INTERIOR, Location::EXTERIOR, Dimension::L);
        }
    }

    void TFHETopologyComputer::addLineEndOnArea(
        bool isLineA,
        Location locLineEnd,
        Location locArea,
        const TFHECoordinate *pt) {
        (void)pt;
        (void)locLineEnd;

        if (locArea != Location::BOUNDARY) {
            /**
             * When a line end is in an Area INTERIOR or EXTERIOR
             * some length of the source Line Interior
             * AND the Exterior of the line
             * is also in that location of the target.
             * NOTE: this assumes the line end is NOT also in an Area of a mixed-dim GC
             */
            updateDim(isLineA, Location::INTERIOR, locArea, Dimension::L);
            updateDim(isLineA, Location::EXTERIOR, locArea, Dimension::A);
        }
    }

    void TFHETopologyComputer::addAreaVertexOnPoint(bool isAreaA, Location locArea, const TFHECoordinate *pt) {
        (void)pt;
        //-- Assert: locArea != EXTERIOR
        //-- Assert: locTarget == INTERIOR
        /**
         * The vertex location intersects the Point.
         */
        updateDim(isAreaA, locArea, Location::INTERIOR, Dimension::P);
        /**
         * The area interior intersects the point's exterior neighbourhood.
         */
        updateDim(isAreaA, Location::INTERIOR, Location::EXTERIOR, Dimension::A);
        /**
         * If the area vertex is on the boundary,
         * the area boundary and exterior intersect the point's exterior neighbourhood
         */
        if (locArea == Location::BOUNDARY) {
            updateDim(isAreaA, Location::BOUNDARY, Location::EXTERIOR, Dimension::L);
            updateDim(isAreaA, Location::EXTERIOR, Location::EXTERIOR, Dimension::A);
        }
    }

    void TFHETopologyComputer::addAreaVertexOnLine(
        bool isAreaA,
        Location locArea,
        Location locTarget,
        const TFHECoordinate *pt) {
        (void)pt;
        //-- Assert: locArea != EXTERIOR
        /**
         * If an area vertex intersects a line, all we know is the
         * intersection at that point.
         * e.g. the line may or may not be collinear with the area boundary,
         * and the line may or may not intersect the area interior.
         * Full topology is determined later by node analysis
         */
        updateDim(isAreaA, locArea, locTarget, Dimension::P);
        if (locArea == Location::INTERIOR) {
            /**
             * The area interior intersects the line's exterior neighbourhood.
             */
            updateDim(isAreaA, Location::INTERIOR, Location::EXTERIOR, Dimension::A);
        }
    }

    void TFHETopologyComputer::evaluateNode(TFHENodeSections *nodeSections) {
        const TFHECoordinate *p = nodeSections->getCoordinate();
        std::unique_ptr<TFHERelateNode> node = nodeSections->createNode();
        //-- Node must have edges for geom, but may also be in interior of a overlapping GC
        bool isAreaInteriorA = geomA.isNodeInArea(p, nodeSections->getPolygonal(TFHERelateGeometry::GEOM_A));
        bool isAreaInteriorB = geomB.isNodeInArea(p, nodeSections->getPolygonal(TFHERelateGeometry::GEOM_B));
        node->finish(isAreaInteriorA, isAreaInteriorB);
        evaluateNodeEdges(node.get());
    }

    void TFHETopologyComputer::evaluateNodeEdges(TFHERelateNode *node) {
        for (const std::unique_ptr<TFHERelateEdge> &e : node->getEdges()) {
            //-- An optimization to avoid updates for cases with a linear geometry
            if (isAreaArea()) {
                updateDim(
                    e->location(TFHERelateGeometry::GEOM_A, Position::LEFT),
                    e->location(TFHERelateGeometry::GEOM_B, Position::LEFT),
                    Dimension::A);
                updateDim(
                    e->location(TFHERelateGeometry::GEOM_A, Position::RIGHT),
                    e->location(TFHERelateGeometry::GEOM_B, Position::RIGHT),
                    Dimension::A);
            }
            updateDim(
                e->location(TFHERelateGeometry::GEOM_A, Position::ON),
                e->location(TFHERelateGeometry::GEOM_B, Position::ON),
                Dimension::L);
        }
    }

    void TFHETopologyComputer::addNodeSections(TFHENodeSection *ns0, TFHENodeSection *ns1) {
        TFHENodeSections *sections = getNodeSections(ns0->nodePt());
        sections->addNodeSection(ns0);
        sections->addNodeSection(ns1);
    }

    TFHENodeSections *TFHETopologyComputer::getNodeSections(const TFHECoordinate &nodePt) {
        TFHENodeSections *ns;
        auto result = nodeMap.find(nodePt);
        if (result == nodeMap.end()) {
            ns = new TFHENodeSections(&nodePt);
            nodeSectionsStore.emplace_back(ns);
            nodeMap[nodePt] = ns;
        } else {
            ns = result->second;
        }
        return ns;
    }

    void TFHETopologyComputer::updateAreaAreaCross(const TFHENodeSection *a, const TFHENodeSection *b) {
        bool isProper = TFHENodeSection::isProper(*a, *b);
        if (isProper ||
            algorithm::TFHEPolygonNodeTopology::isCrossing(
                &(a->nodePt()), a->getVertex(0), a->getVertex(1), b->getVertex(0), b->getVertex(1))) {
            updateDim(Location::INTERIOR, Location::INTERIOR, Dimension::A);
        }
    }

    void TFHETopologyComputer::updateNodeLocation(const TFHENodeSection *a, const TFHENodeSection *b) {
        const TFHECoordinate &pt = a->nodePt();
        Location locA = geomA.locateNode(&pt, a->getPolygonal());
        Location locB = geomB.locateNode(&pt, b->getPolygonal());
        updateDim(locA, locB, Dimension::P);
    }

    TFHETopologyComputer::TFHETopologyComputer(
        TFHETopologyPredicate &predicate,
        TFHERelateGeometry &geomA,
        TFHERelateGeometry &geomB) :
            predicate(predicate), geomA(geomA), geomB(geomB) {
        initExteriorDims();
    }

    int TFHETopologyComputer::getDimension(bool isA) const {
        return getGeometry(isA).getDimension();
    }

    bool TFHETopologyComputer::isAreaArea() const {
        return getDimension(TFHERelateGeometry::GEOM_A) == Dimension::A &&
            getDimension(TFHERelateGeometry::GEOM_B) == Dimension::A;
    }

    bool TFHETopologyComputer::isSelfNodingRequired() const {
        if (!predicate.requireSelfNoding())
            return false;

        if (geomA.isSelfNodingRequired())
            return true;

        //-- if B is a mixed GC with A and L require full noding
        if (geomB.hasAreaAndLine())
            return true;

        return false;
    }

    bool TFHETopologyComputer::isExteriorCheckRequired(bool isA) const {
        return predicate.requireExteriorCheck(isA);
    }

    bool TFHETopologyComputer::isResultKnown() const {
        return predicate.isKnown();
    }

    TFHEBool TFHETopologyComputer::getResult() const {
        return predicate.value();
    }

    void TFHETopologyComputer::finish() {
        predicate.finish();
    }

    void TFHETopologyComputer::addIntersection(TFHENodeSection *a, TFHENodeSection *b) {
        // add edges to node to allow full topology evaluation later
        // we run this first (unlike JTS) in case the subsequent test throws
        // an exception and the NodeSection pointers are not correctly
        // saved in the memory managed store on the NodeSections, causing
        // a small memory leak
        addNodeSections(a, b);

        if (!a->isSameGeometry(b)) {
            updateIntersectionAB(a, b);
        }
    }

    void TFHETopologyComputer::addPointOnPointInterior(const TFHECoordinate *pt) {
        (void)pt;  // pt is not used
        updateDim(Location::INTERIOR, Location::INTERIOR, Dimension::P);
    }

    void TFHETopologyComputer::addPointOnPointExterior(bool isGeomA, const TFHECoordinate *pt) {
        (void)pt;  // pt is not used
        updateDim(isGeomA, Location::INTERIOR, Location::EXTERIOR, Dimension::P);
    }

    void TFHETopologyComputer::addPointOnGeometry(
        bool isPointA,
        Location locTarget,
        int dimTarget,
        const TFHECoordinate *pt) {
        (void)pt;
        //
        updateDim(isPointA, Location::INTERIOR, locTarget, Dimension::P);

        //-- an empty geometry has no points to infer entries from
        if (getGeometry(!isPointA).isEmpty())
            return;

        switch (dimTarget) {
        case Dimension::P:
            return;
        case Dimension::L:
            /**
             * Because zero-length lines are handled,
             * a point lying in the exterior of the line target
             * may imply either P or L for the Exterior interaction
             */
            // FIXME: 我们没有实现 zero-length lines 的处理,所以这里可能会有问题，test的时候看情况
            // updateDim(isGeomA, Location::EXTERIOR, locTarget, Dimension::P);
            return;
        case Dimension::A:
            /**
             * If a point intersects an area target, then the area interior and boundary
             * must extend beyond the point and thus interact with its exterior.
             */
            updateDim(isPointA, Location::EXTERIOR, Location::INTERIOR, Dimension::A);
            updateDim(isPointA, Location::EXTERIOR, Location::BOUNDARY, Dimension::L);
            return;
        }
        throw std::runtime_error("Unknown target dimension: " + std::to_string(dimTarget));
    }

    void TFHETopologyComputer::addLineEndOnGeometry(
        bool isLineA,
        Location locLineEnd,
        Location locTarget,
        int dimTarget,
        const TFHECoordinate *pt) {
        (void)pt;

        //-- record topology at line end point
        updateDim(isLineA, locLineEnd, locTarget, Dimension::P);

        //-- an empty geometry has no points to infer entries from
        if (getGeometry(!isLineA).isEmpty())
            return;

        //-- Line and Area targets may have additional topology
        switch (dimTarget) {
        case Dimension::P:
            return;
        case Dimension::L:
            addLineEndOnLine(isLineA, locLineEnd, locTarget, pt);
            return;
        case Dimension::A:
            addLineEndOnArea(isLineA, locLineEnd, locTarget, pt);
            return;
        }
        throw std::runtime_error("Unknown target dimension: " + std::to_string(dimTarget));
    }

    void TFHETopologyComputer::addAreaVertex(
        bool isAreaA,
        Location locArea,
        Location locTarget,
        int dimTarget,
        const TFHECoordinate *pt) {
        (void)pt;
        if (locTarget == Location::EXTERIOR) {
            updateDim(isAreaA, Location::INTERIOR, Location::EXTERIOR, Dimension::A);
            /**
             * If area vertex is on Boundary further topology can be deduced
             * from the neighbourhood around the boundary vertex.
             * This is always the case for polygonal geometries.
             * For GCs, the vertex may be either on boundary or in interior
             * (i.e. of overlapping or adjacent polygons)
             */
            if (locArea == Location::BOUNDARY) {
                updateDim(isAreaA, Location::BOUNDARY, Location::EXTERIOR, Dimension::L);
                updateDim(isAreaA, Location::EXTERIOR, Location::EXTERIOR, Dimension::A);
            }
            return;
        }

        switch (dimTarget) {
        case Dimension::P:
            addAreaVertexOnPoint(isAreaA, locArea, pt);
            return;
        case Dimension::L:
            addAreaVertexOnLine(isAreaA, locArea, locTarget, pt);
            return;
        case Dimension::A:
            addAreaVertexOnArea(isAreaA, locArea, locTarget, pt);
            return;
        }
        throw std::runtime_error("Unknown target dimension: " + std::to_string(dimTarget));
    }

    void TFHETopologyComputer::addAreaVertexOnArea(
        bool isAreaA,
        Location locArea,
        Location locTarget,
        const TFHECoordinate *pt) {
        (void)pt;
        if (locTarget == Location::BOUNDARY) {
            if (locArea == Location::BOUNDARY) {
                //-- B/B topology is fully computed later by node analysis
                updateDim(isAreaA, Location::BOUNDARY, Location::BOUNDARY, Dimension::P);
            } else {
                // locArea == INTERIOR
                updateDim(isAreaA, Location::INTERIOR, Location::INTERIOR, Dimension::A);
                updateDim(isAreaA, Location::INTERIOR, Location::BOUNDARY, Dimension::L);
                updateDim(isAreaA, Location::INTERIOR, Location::EXTERIOR, Dimension::A);
            }
        } else {
            //-- locTarget is INTERIOR or EXTERIOR`
            updateDim(isAreaA, Location::INTERIOR, locTarget, Dimension::A);
            /**
             * If area vertex is on Boundary further topology can be deduced
             * from the neighbourhood around the boundary vertex.
             * This is always the case for polygonal geometries.
             * For GCs, the vertex may be either on boundary or in interior
             * (i.e. of overlapping or adjacent polygons)
             */
            if (locArea == Location::BOUNDARY) {
                updateDim(isAreaA, Location::BOUNDARY, locTarget, Dimension::L);
                updateDim(isAreaA, Location::EXTERIOR, locTarget, Dimension::A);
            }
        }
    }

    void TFHETopologyComputer::evaluateNodes() {
        for (auto &kv : nodeMap) {
            TFHENodeSections *nodeSections = kv.second;
            if (nodeSections->hasInteractionAB()) {
                evaluateNode(nodeSections);
                if (isResultKnown())
                    return;
            }
        }
    }
}  // namespace SpatialFHE::operation::relateng