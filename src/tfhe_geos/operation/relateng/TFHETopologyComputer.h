//
// Created by ubuntu on 1/14/25.
//

#ifndef TFHETOPOLOGYCOMPUTER_H
#define TFHETOPOLOGYCOMPUTER_H
#include <tfhe_geos/geom/TFHECoordinate.h>
#include <tfhe_geos/geom/enums.h>

#include <deque>
#include <map>
#include <memory>

#include "TFHENodeSections.h"

// Forward declaration
namespace SpatialFHE::operation::relateng {
    class TFHERelateGeometry;
    class TFHERelateNode;
    class TFHETopologyPredicate;
    class TFHENodeSection;
}  // namespace SpatialFHE::operation::relateng

namespace SpatialFHE::operation::relateng {

    class TFHETopologyComputer {
        using TFHECoordinate = geom::TFHECoordinate;
        using Location = geom::Location;

    private:
        // Members
        TFHETopologyPredicate& predicate;
        TFHERelateGeometry& geomA;
        TFHERelateGeometry& geomB;
        std::map<TFHECoordinate, TFHENodeSections*> nodeMap;
        std::deque<std::unique_ptr<TFHENodeSections>> nodeSectionsStore;

        // Methods
        /**
         * Determine a priori partial EXTERIOR topology based on dimensions.
         */
        void initExteriorDims();

        void initExteriorEmpty(bool geomNonEmpty);

        inline TFHERelateGeometry& getGeometry(bool isA) const {
            return isA ? geomA : geomB;
        };

        void updateDim(Location locA, Location locB, int dimension);

        void updateDim(bool isAB, Location loc1, Location loc2, int dimension);

        /**
         * Update topology for an intersection between A and B.
         *
         * @param a the section for geometry A
         * @param b the section for geometry B
         */
        void updateIntersectionAB(const TFHENodeSection* a, const TFHENodeSection* b);

        void addLineEndOnLine(bool isLineA, Location locLineEnd, Location locLine, const TFHECoordinate* pt);

        void addLineEndOnArea(bool isLineA, Location locLineEnd, Location locArea, const TFHECoordinate* pt);

        /**
         * Updates topology for an area vertex (in Interior or on Boundary)
         * intersecting a point.
         * Note that because the largest dimension of intersecting target is determined,
         * the intersecting point is not part of any other target geometry,
         * and hence its neighbourhood is in the Exterior of the target.
         *
         * @param isAreaA whether the area is the A input
         * @param locArea the location of the vertex in the area
         * @param pt the point at which topology is being updated
         */
        void addAreaVertexOnPoint(bool isAreaA, Location locArea, const TFHECoordinate* pt);

        void addAreaVertexOnLine(bool isAreaA, Location locArea, Location locTarget, const TFHECoordinate* pt);

        void evaluateNode(TFHENodeSections* nodeSections);
        void evaluateNodeEdges(TFHERelateNode* node);

        void addNodeSections(TFHENodeSection* ns0, TFHENodeSection* ns1);

        TFHENodeSections* getNodeSections(const TFHECoordinate& nodePt);

        void updateAreaAreaCross(const TFHENodeSection* a, const TFHENodeSection* b);

        void updateNodeLocation(const TFHENodeSection* a, const TFHENodeSection* b);

    public:
        TFHETopologyComputer(TFHETopologyPredicate& predicate, TFHERelateGeometry& geomA, TFHERelateGeometry& geomB);
        int getDimension(bool isA) const;

        bool isAreaArea() const;

        /**
         * Indicates whether the input geometries require self-noding
         * for correct evaluation of specific spatial predicates.
         * Self-noding is required for geometries which may
         * have self-crossing linework.
         * This causes the coordinates of nodes created by
         * crossing segments to be computed explicitly.
         * This ensures that node locations match in situations
         * where a self-crossing and mutual crossing occur at the same logical location.
         * The canonical example is a self-crossing line tested against a single segment     * identical to one of the
         * crossed segments.
         *
         * @return true if self-noding is required
         */
        bool isSelfNodingRequired() const;

        bool isExteriorCheckRequired(bool isA) const;

        bool isResultKnown() const;

        TFHEBool getResult() const;

        /**
         * Finalize the evaluation.
         */
        void finish();

        void addIntersection(TFHENodeSection* a, TFHENodeSection* b);

        void addPointOnPointInterior(const TFHECoordinate* pt);

        void addPointOnPointExterior(bool isGeomA, const TFHECoordinate* pt);

        void addPointOnGeometry(bool isA, Location locTarget, int dimTarget, const TFHECoordinate* pt);

        /**
         * Add topology for a line end.
         * The line end point must be "significant";
         * i.e. not contained in an area if the source is a mixed-dimension GC.
         *
         * @param isLineA the input containing the line end
         * @param locLineEnd the location of the line end (Interior or Boundary)
         * @param locTarget the location on the target geometry
         * @param dimTarget the dimension of the interacting target geometry element,
         *    (if any), or the dimension of the target
         * @param pt the line end coordinate
         */
        void addLineEndOnGeometry(
            bool isLineA,
            Location locLineEnd,
            Location locTarget,
            int dimTarget,
            const TFHECoordinate* pt);

        /**
         * Adds topology for an area vertex interaction with a target geometry element.
         * Assumes the target geometry element has highest dimension
         * (i.e. if the point lies on two elements of different dimension,
         * the location on the higher dimension element is provided.
         * This is the semantic provided by {@link TFHERelatePointLocator}.
         *
         * Note that in a GeometryCollection containing overlapping or adjacent polygons,
         * the area vertex location may be INTERIOR instead of BOUNDARY.
         *
         * @param isAreaA the input that is the area
         * @param locArea the location on the area
         * @param locTarget the location on the target geometry element
         * @param dimTarget the dimension of the target geometry element
         * @param pt the point of interaction
         */
        void addAreaVertex(bool isAreaA, Location locArea, Location locTarget, int dimTarget, const TFHECoordinate* pt);

        void addAreaVertexOnArea(bool isAreaA, Location locArea, Location locTarget, const TFHECoordinate* pt);

        void evaluateNodes();
    };

}  // namespace SpatialFHE::operation::relateng

#endif  // TFHETOPOLOGYCOMPUTER_H
