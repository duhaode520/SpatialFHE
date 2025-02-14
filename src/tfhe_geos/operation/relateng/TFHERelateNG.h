//
// Created by ubuntu on 1/14/25.
//

#ifndef TFHERELATENG_H
#define TFHERELATENG_H
#include <tfhe_geos/geom/TFHECoordinate.h>
#include <tfhe_geos/operation/relateng/TFHERelateGeometry.h>
#include <tfhe_geos/noding/TFHESegmentString.h>
#include <tfhe_geos/noding/TFHESegmentSetMutualIntersector.h>

// Forward declaration
namespace SpatialFHE::geom {
    class TFHEIntersectionMatrix;
}

namespace SpatialFHE::operation::relateng {
    class TFHETopologyComputer;
    class TFHETopologyPredicate;
    class TFHEEdgeSegmentIntersector;
}  // namespace SpatialFHE::operation::relateng

namespace SpatialFHE::operation::relateng {

    class TFHERelateNG {
        using TFHECoordinate = geom::TFHECoordinate;
        using TFHEGeometry = geom::TFHEGeometry;
        using TFHELinearRing = geom::TFHELinearRing;
        using TFHEEnvelope = geom::TFHEEnvelope;
        using TFHEIntersectionMatrix = geom::TFHEIntersectionMatrix;
        using TFHESegmentString = noding::TFHESegmentString;
        using TFHESegmentSetMutualIntersector = noding::TFHESegmentSetMutualIntersector;
        using Dimension = geom::Dimension;

    private:
        TFHERelateGeometry geomA;
        // TODO: Support the following fields edgeMutualInt
        std::unique_ptr<TFHESegmentSetMutualIntersector> edgeMutualInt = nullptr;

        TFHERelateNG(const TFHEGeometry* inputA, bool isPrepared);

        void computePointPoint(TFHERelateGeometry& geomB, TFHETopologyComputer& topoComputer);

        void computeAtPoints(
            TFHERelateGeometry& geom,
            bool isA,
            TFHERelateGeometry& geomTarget,
            TFHETopologyComputer& topoComputer);

        bool computePoints(
            TFHERelateGeometry& geom,
            bool isA,
            TFHERelateGeometry& geomTarget,
            TFHETopologyComputer& topoComputer);

        void computePoint(
            bool isA,
            const TFHECoordinate* pt,
            TFHERelateGeometry& geomTarget,
            TFHETopologyComputer& topoComputer);

        bool computeLineEnds(
            TFHERelateGeometry& geom,
            bool isA,
            TFHERelateGeometry& geomTarget,
            TFHETopologyComputer& topoComputer);

        /**
         * Compute the topology of a line endpoint.
         * Also reports if the line end is in the exterior of the target geometry,
         * to optimize testing multiple exterior endpoints.
         *
         * @param geom
         * @param isA
         * @param pt
         * @param geomTarget
         * @param topoComputer
         * @return true if the line endpoint is in the exterior of the target
         */
        bool computeLineEnd(
            TFHERelateGeometry& geom,
            bool isA,
            const TFHECoordinate* pt,
            TFHERelateGeometry& geomTarget,
            TFHETopologyComputer& topoComputer);

        bool computeAreaVertex(
            TFHERelateGeometry& geom,
            bool isA,
            TFHERelateGeometry& geomTarget,
            TFHETopologyComputer& topoComputer);

        bool computeAreaVertex(
            TFHERelateGeometry& geom,
            bool isA,
            const TFHELinearRing* ring,
            TFHERelateGeometry& geomTarget,
            TFHETopologyComputer& topoComputer);
       void computeAtEdges(TFHERelateGeometry& geomB, TFHETopologyComputer& topoComputer);


        void computeEdgesAll(std::vector<const TFHESegmentString*>& edgesB, const TFHEEnvelope* envInt,
        TFHEEdgeSegmentIntersector& intersector);

        void computeEdgesMutual(std::vector<const TFHESegmentString*>& edgesB, const TFHEEnvelope* envInt,
        TFHEEdgeSegmentIntersector& intersector);

        /**
         * Fast envelope checks for used in evaluate.
         *
         * <b>May decrypt some encrypted data</b>
         */
        bool hasRequiredEnvelopeInteraction(const TFHEGeometry* b, TFHETopologyPredicate& predicate);

        TFHEBool finishValue(TFHETopologyPredicate& predicate);

    public:
        /**
         * Tests whether the topological relationship between two geometries
         * satisfies a topological predicate.
         *
         * @param a the A input geometry
         * @param b the B input geometry
         * @param pred the topological predicate
         * @return TFHE true if the topological relationship is satisfied
         */
        static TFHEBool relate(const TFHEGeometry* a, const TFHEGeometry* b, TFHETopologyPredicate& pred);

        /**
         * Tests whether the topological relationship to a geometry
         * matches a DE-9IM matrix pattern.
         *
         * @param a the A input geometry
         * @param b the B input geometry
         * @param imPattern the DE-9IM pattern to match
         * @return true if the geometries relationship matches the DE-9IM pattern
         *
         * @see IntersectionMatrixPattern
         */
        static TFHEBool relate(const TFHEGeometry* a, const TFHEGeometry* b, const std::string& imPattern);

        /**
         * Computes the DE-9IM matrix
         * for the topological relationship between two geometries.
         *
         * @param a the A input geometry
         * @param b the B input geometry
         * @return the DE-9IM matrix for the topological relationship
         */
        static std::unique_ptr<TFHEIntersectionMatrix> relate(const TFHEGeometry* a, const TFHEGeometry* b);

        /**
         * Creates a prepared RelateNG instance to optimize the
         * evaluation of relationships against a single geometry.
         *
         * @param a the A input geometry
         * @return a prepared instance
         */
        static std::unique_ptr<TFHERelateNG> prepare(const TFHEGeometry* a);

        /**
         * Computes the DE-9IM matrix for the topological relationship to a geometry.
         *
         * @param b the B geometry to test against
         * @return the DE-9IM matrix
         */
        std::unique_ptr<TFHEIntersectionMatrix> evaluate(const TFHEGeometry* b);

        /**
         * Tests whether the topological relationship to a geometry
         * matches a DE-9IM matrix pattern.
         *
         * @param b the B geometry to test against
         * @param imPattern the DE-9IM pattern to match
         * @return true if the geometries' topological relationship matches the DE-9IM pattern
         *
         * @see IntersectionMatrixPattern
         */
        TFHEBool evaluate(const TFHEGeometry* b, const std::string& imPattern);

        /**
         * Tests whether the topological relationship to a geometry
         * satisfies a topology predicate.
         *
         * @param b the B geometry to test against
         * @param predicate the topological predicate
         * @return true if the predicate is satisfied
         */
        TFHEBool evaluate(const TFHEGeometry* b, TFHETopologyPredicate& predicate);

        static TFHEBool intersects(const TFHEGeometry* a, const TFHEGeometry* b);
        static TFHEBool crosses(const TFHEGeometry* a, const TFHEGeometry* b);
        static TFHEBool disjoint(const TFHEGeometry* a, const TFHEGeometry* b);
        static TFHEBool touches(const TFHEGeometry* a, const TFHEGeometry* b);
        static TFHEBool within(const TFHEGeometry* a, const TFHEGeometry* b);
        static TFHEBool contains(const TFHEGeometry* a, const TFHEGeometry* b);
        static TFHEBool overlaps(const TFHEGeometry* a, const TFHEGeometry* b);
        static TFHEBool covers(const TFHEGeometry* a, const TFHEGeometry* b);
        static TFHEBool coveredBy(const TFHEGeometry* a, const TFHEGeometry* b);
        static TFHEBool equalsTopo(const TFHEGeometry* a, const TFHEGeometry* b);

        TFHEBool intersects(const TFHEGeometry* b);
        TFHEBool crosses(const TFHEGeometry* b);
        TFHEBool disjoint(const TFHEGeometry* b);
        TFHEBool touches(const TFHEGeometry* b);
        TFHEBool within(const TFHEGeometry* b);
        TFHEBool contains(const TFHEGeometry* b);
        TFHEBool overlaps(const TFHEGeometry* b);
        TFHEBool covers(const TFHEGeometry* b);
        TFHEBool coveredBy(const TFHEGeometry* b);
        TFHEBool equalsTopo(const TFHEGeometry* b);
        TFHEBool relate(const TFHEGeometry* b, const std::string& pat);
        std::unique_ptr<TFHEIntersectionMatrix> relate(const TFHEGeometry* b);
    };

}  // namespace SpatialFHE::operation::relateng

#endif  // TFHERELATENG_H
