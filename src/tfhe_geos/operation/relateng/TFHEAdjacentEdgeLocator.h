//
// Created by ubuntu on 1/21/25.
//

#ifndef TFHEADJACENTEDGELOCATOR_H
#define TFHEADJACENTEDGELOCATOR_H

#include <tfhe_geos/geom/TFHECoordinateSequence.h>
#include <tfhe_geos/geom/enums.h>

// Forward declarations
namespace SpatialFHE::operation::relateng {
    class TFHENodeSection;
    class TFHENodeSections;
}  // namespace SpatialFHE::operation::relateng

namespace SpatialFHE::geom {
    class TFHECoordinate;
    class TFHEGeometry;
    class TFHELinearRing;
    class TFHEPolygon;
}  // namespace SpatialFHE::geom

namespace SpatialFHE ::operation ::relateng {

    /**
     * Determines the location for a point which is known to lie
     * on at least one edge of a set of polygons.
     * This provides the union-semantics for determining
     * point location in a GeometryCollection, which may
     * have polygons with adjacent edges which are effectively
     * in the interior of the geometry.
     * Note that it is also possible to have adjacent edges which
     * lie on the boundary of the geometry
     * (e.g. a polygon contained within another polygon with adjacent edges).
     *
     * @author mdavis
     *
     */
    class TFHEAdjacentEdgeLocator {
        using Location = geom::Location;
        using TFHEPolygon = geom::TFHEPolygon;
        using TFHECoordinate = geom::TFHECoordinate;
        using TFHELinearRing = geom::TFHELinearRing;
        using TFHEGeometry = geom::TFHEGeometry;
        using TFHECoordinateSequence = geom::TFHECoordinateSequence;

    private:
        // Members
        std::vector<const TFHECoordinateSequence*> rings;

        // reorient rings are stored here if needed
        std::vector<std::unique_ptr<TFHECoordinateSequence>> localRings;

        // Methods

        void init(const TFHEGeometry* geom);
        void addRings(const TFHEGeometry* geom);
        void addRing(const TFHELinearRing* ring, bool requireCCW);

        void addSections(const TFHECoordinate* p, const TFHECoordinateSequence* ring, TFHENodeSections& sections);
        TFHENodeSection* createSection(const TFHECoordinate* p, const TFHECoordinate* prev, const TFHECoordinate* next);

    public:
        /**
         * Creates a new locator for a given set of polygons.
         *
         * @param polys the polygons to locate within
         */
        TFHEAdjacentEdgeLocator(const TFHEGeometry* geom) {
            init(geom);
        }

        /**
         * Determines the location of a point on an edge.
         *
         * @param p the point on the edge
         * @return the location of the point on the edge
         */
        Location locate(const geom::TFHECoordinate* p);
    };

}  // namespace SpatialFHE::operation::relateng

#endif  // TFHEADJACENTEDGELOCATOR_H
