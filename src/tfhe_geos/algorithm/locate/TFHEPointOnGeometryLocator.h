//
// Created by ubuntu on 1/24/25.
//

#ifndef TFHEPOINTONGEOMETRYLOCATOR_H
#define TFHEPOINTONGEOMETRYLOCATOR_H

#include "tfhe_geos/geom/enums.h"

// Forward declarations
namespace SpatialFHE::geom {
    class TFHECoordinate;
}

namespace SpatialFHE::algorithm::locate {

    /** \brief
     * An interface for classes which determine the [Location](@ref geom::Location) of
     * points in [Polygon](@ref geom::Polygon) or [MultiPolygon](@ref geom::MultiPolygon) geometries.
     *
     * @author Martin Davis
     */
    class TFHEPointOnGeometryLocator {
    public:
        virtual ~TFHEPointOnGeometryLocator() = default;

        /**
         * Determines the [Location](@ref geom::Location) of a point in an areal [Geometry](@ref geom::Geometry).
         *
         * @param p the point to test
         * @return the location of the point in the geometry
         */
        virtual geom::Location locate(const geom::TFHECoordinate*  p) = 0;
    };

}  // namespace SpatialFHE::algorithm::locate

#endif  // TFHEPOINTONGEOMETRYLOCATOR_H
