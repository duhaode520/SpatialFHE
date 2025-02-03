//
// Created by ubuntu on 1/14/25.
//

#ifndef TFHERELATESEGMENTSTRING_H
#define TFHERELATESEGMENTSTRING_H
#include <tfhe_geos/noding/TFHEBasicSegmentString.h>

// Forward declarations

namespace SpatialFHE::geom {
    class TFHECoordinate;
    class TFHECoordinateSequence;
    class TFHEGeometry;
}  // namespace SpatialFHE::geom

namespace SpatialFHE::operation::relateng {
    class TFHERelateGeometry;
    class TFHENodeSection;
}  // namespace SpatialFHE::operation::relateng

namespace SpatialFHE::operation::relateng {

    class TFHERelateSegmentString : public noding::TFHEBasicSegmentString {
        using TFHEBasicSegmentString = noding::TFHEBasicSegmentString;
        using TFHEGeometry = geom::TFHEGeometry;
        using TFHECoordinate = geom::TFHECoordinate;
        using TFHECoordinateSequence = geom::TFHECoordinateSequence;

    private:
        // Members
        bool m_isA;
        int m_dimension;
        int m_id;
        int m_ringId;
        const TFHERelateGeometry* m_inputGeom;
        const TFHEGeometry* m_parentPolygonal = nullptr;

        // Constructor
        TFHERelateSegmentString(
            const TFHECoordinateSequence* pts,
            bool isA,
            int dimension,
            int id,
            int ringId,
            const TFHEGeometry* poly,
            const TFHERelateGeometry* inputGeom) :
                TFHEBasicSegmentString(const_cast<TFHECoordinateSequence*>(pts), nullptr),
                m_isA(isA),
                m_dimension(dimension),
                m_id(id),
                m_ringId(ringId),
                m_inputGeom(inputGeom),
                m_parentPolygonal(poly) {}

        // methods
        static const TFHERelateSegmentString* createSegmentString(
            const TFHECoordinateSequence* pts,
            bool isA,
            int dimension,
            int id,
            int ringId,
            const TFHEGeometry* poly,
            const TFHERelateGeometry* parent);

        const TFHECoordinate* prevVertex(std::size_t segIndex, const TFHECoordinate* pt) const;
        const TFHECoordinate* nextVertex(std::size_t segIndex, const TFHECoordinate* pt) const;

    public:
        static const TFHERelateSegmentString* createLine(
            const TFHECoordinateSequence* pts,
            bool isA,
            int elementId,
            const TFHERelateGeometry* parent);

        static const TFHERelateSegmentString* createRing(
            const TFHECoordinateSequence* pts,
            bool isA,
            int elementId,
            int ringId,
            const TFHEGeometry* poly,
            const TFHERelateGeometry* parent);

        inline bool isA() const {
            return m_isA;
        }

        inline const TFHERelateGeometry* getGeometry() const {
            return m_inputGeom;
        }

        inline const TFHEGeometry* getPolygonal() const {
            return m_parentPolygonal;
        }

        TFHENodeSection* createNodeSection(std::size_t segIndex, const TFHECoordinate intPt) const;

        /**
         * Tests if a segment intersection point has that segment as its
         * canonical containing segment.
         * Segments are half-closed, and contain their start point but not the endpoint,
         * except for the final segment in a non-closed segment string, which contains
         * its endpoint as well.
         * This test ensures that vertices are assigned to a unique segment in a segment string.
         * In particular, this avoids double-counting intersections which lie exactly
         * at segment endpoints.
         *
         * @param segIndex the segment the point may lie on
         * @param pt the point
         * @return true if the segment contains the point
         */
        bool isContainingSegment(std::size_t segIndex, const TFHECoordinate* pt) const;
    };

}  // namespace SpatialFHE::operation::relateng

#endif  // TFHERELATESEGMENTSTRING_H
