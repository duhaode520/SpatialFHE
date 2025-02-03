//
// Created by ubuntu on 1/25/25.
//

#ifndef INDEXPOINTINAREALOCATOR_H
#define INDEXPOINTINAREALOCATOR_H
#include "TFHEPointOnGeometryLocator.h"

// Forward declaration
namespace SpatialFHE::algorithm {
    class TFHERayCrossingCounter;
}

namespace SpatialFHE::geom {
    class TFHEGeometry;
    class TFHECoordinate;
    class TFHECoordinateSequence;
}  // namespace SpatialFHE::geom

namespace SpatialFHE::algorithm::locate {

    /** \brief
     * Determines the location of [Coordinates](@ref geom::Coordinate) relative to
     * an areal geometry, using indexing for efficiency.
     *
     * The Location is computed precisely, in that points located on the geometry boundary
     * or segments will return [geom::Location::BOUNDARY](@ref geom::Location).
     *
     * Polygonal and [LinearRing](@ref geom::LinearRing) geometries are supported.
     *
     * The index is lazy-loaded, which allows creating instances even if they are not used.
     *
     */
    class IndexPointInAreaLocator : public TFHEPointOnGeometryLocator {
    private:
        const geom::TFHEGeometry& areaGeom;

    public:
        IndexPointInAreaLocator(const geom::TFHEGeometry& g);

        [[nodiscard]] const geom::TFHEGeometry& getGeometry() const;

        /** \brief
         * Determines the [Location](@ref geom::Location) of a point in an areal
         * [Geometry](@ref geom::Geometry).
         *
         * @param p the point to test
         * @return the location of the point in the geometry
         */
        geom::Location locate(const geom::TFHECoordinate* p) override;

    };

}  // namespace SpatialFHE::algorithm::locate

#endif  // INDEXPOINTINAREALOCATOR_H
