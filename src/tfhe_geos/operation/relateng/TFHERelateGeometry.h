//
// Created by ubuntu on 1/14/25.
//

#ifndef TFHERELATEGEOMETRY_H
#define TFHERELATEGEOMETRY_H

#include <tfhe_geos/geom/TFHECoordinate.h>
#include <tfhe_geos/geom/enums.h>
#include <tfhe_geos/noding/TFHESegmentString.h>
#include <tfhe_geos/operation/relateng/TFHERelatePointLocator.h>
#include <tfhe_geos/operation/relateng/TFHERelateSegmentString.h>

#include <memory>
#include <vector>

// forward declaration
namespace SpatialFHE {
    namespace geom {
        class TFHECoordinateSequence;
        class TFHEEnvelope;
        class TFHEGeometry;
        class TFHELinearRing;
        class TFHELineString;
        class TFHEPoint;
    }  // namespace geom

    namespace noding {
        class TFHESegmentString;
    }  // namespace noding
}  // namespace SpatialFHE

namespace SpatialFHE::operation::relateng {
    class TFHERelateGeometry {
        using TFHECoordinate = SpatialFHE::geom::TFHECoordinate;
        using TFHECoordinateSequence = SpatialFHE::geom::TFHECoordinateSequence;
        using Dimension = SpatialFHE::geom::Dimension;
        using TFHEEnvelope = SpatialFHE::geom::TFHEEnvelope;
        using TFHEGeometry = SpatialFHE::geom::TFHEGeometry;
        using Location = SpatialFHE::geom::Location;
        using TFHELinearRing = SpatialFHE::geom::TFHELinearRing;
        using TFHELineString = SpatialFHE::geom::TFHELineString;
        using TFHEPoint = SpatialFHE::geom::TFHEPoint;
        using TFHESegmentString = SpatialFHE::noding::TFHESegmentString;

    private:
        // Members
        const TFHEGeometry* geom;
        bool m_isPrepared = false;
        const TFHEEnvelope* geomEnv;
        int geomDim = Dimension::False;

        // zero-len line is not a point
        // bool isLineZeroLen = false;

        bool isGeomEmpty = false;

        TFHECoordinate::ConstSet uniquePoints;
        std::unique_ptr<TFHERelatePointLocator> locator;
        int elementId = 0;
        bool hasPoints = false;
        bool hasLines = false;
        bool hasAreas = false;

        /*
         * Memory contexts for lower level allocations
         */
        std::vector<std::unique_ptr<const TFHERelateSegmentString>> segStringTempStore;
        std::vector<std::unique_ptr<const TFHERelateSegmentString>> segStringPermStore;
        std::vector<std::unique_ptr<TFHECoordinateSequence>> csStore;

        // Methods

        void analyzeDimensions();

        TFHERelatePointLocator* getLocator();

        TFHECoordinate::ConstSet createUniquePoints();

        void extractSegmentStrings(
            bool isA,
            const TFHEEnvelope* env,
            const TFHEGeometry* geom,
            std::vector<const TFHESegmentString*>& segStrings,
            std::vector<std::unique_ptr<const TFHERelateSegmentString>>& segStore);

        /**
         * Extracts the segment strings from a geometry.
         * The segment strings are extracted into a provided vector.
         * The segment strings are owned by the caller and must be deleted by the caller.
         *
         * @param isA true if the geometry is A, false if it is B
         * @param geom the geometry to extract from
         * @param parentPolygonal the parent polygonal geometry, which is MultiPolygon in GEOS, is not used by us.
         * @param env the envelope to extract within
         * @param segStrings a vector to add the extracted segment strings to
         * @param segStore a vector to add the extracted segment strings to
         */
        void extractSegmentStringsFromAtomic(
            bool isA,
            const TFHEGeometry* geom,
            const TFHEGeometry* parentPolygonal,
            const TFHEEnvelope* env,
            std::vector<const TFHESegmentString*>& segStrings,
            std::vector<std::unique_ptr<const TFHERelateSegmentString>>& segStore);

        void extractRingToSegmentString(
            bool isA,
            const TFHELinearRing* ring,
            int ringId,
            const TFHEEnvelope* env,
            const TFHEGeometry* parentPoly,
            std::vector<const TFHESegmentString*>& segStrings,
            std::vector<std::unique_ptr<const TFHERelateSegmentString>>& segStore);

        static std::unique_ptr<TFHECoordinateSequence> removeRepeatedPointsInternal(const TFHECoordinateSequence* seq);

    public:
        static constexpr bool GEOM_A = true;
        static constexpr bool GEOM_B = false;

        // Constructors
        TFHERelateGeometry(const TFHEGeometry* geom);
        TFHERelateGeometry(const TFHEGeometry* geom, bool isPrepared);

        // Getters
        const TFHEGeometry* getGeometry() const;
        const TFHEEnvelope* getEnvelope() const;
        bool isPrepared() const;
        int getDimension() const;
        TFHECoordinate::ConstSet& getUniquePoints();

        std::vector<const TFHEPoint*> getEffectivePoints();

        int locateWithDim(const TFHECoordinate* p);

        bool hasBoundary();

        bool hasDimension(int dim) const {
            switch (dim) {
            case Dimension::P:
                return hasPoints;
            case Dimension::L:
                return hasLines;
            case Dimension::A:
                return hasAreas;
            }
            return false;
        };

        bool hasEdges() const;

        bool isEmpty() const;

        int locateLineEndWithDim(const TFHECoordinate* p);

        /**
         * Locates a vertex of a polygon.
         * A vertex of a Polygon or MultiPolygon is on
         * the {@link Location#BOUNDARY}.
         * But a vertex of an overlapped polygon in a GeometryCollection
         * may be in the {@link Location#INTERIOR}.
         *
         * @param pt the polygon vertex
         * @return the location of the vertex
         */
        Location locateAreaVertex(const TFHECoordinate* pt);

        Location locateNode(const TFHECoordinate* pt, const TFHEGeometry* parentPolygonal);

        std::vector<const TFHESegmentString*> extractSegmentStrings(bool isA, const TFHEEnvelope* env);

        bool isSelfNodingRequired() const;

        bool hasAreaAndLine() const;

        bool isNodeInArea(const TFHECoordinate* nodePt, const TFHEGeometry* parentPolygonal);

        const TFHECoordinateSequence* removeRepeatedPoints(const TFHECoordinateSequence* seq);

        const TFHECoordinateSequence* orientAndRemoveRepeated(const TFHECoordinateSequence* seq, bool orientCW);

    };

}  // namespace SpatialFHE::operation::relateng

#endif  // TFHERELATEGEOMETRY_H
