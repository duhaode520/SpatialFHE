//
// Created by ubuntu on 1/24/25.
//

#ifndef SIMPLEPOINTINAREALOCATOR_H
#define SIMPLEPOINTINAREALOCATOR_H

#include "TFHEPointOnGeometryLocator.h"

// Forward declarations
namespace SpatialFHE::geom {
    class TFHECoordinate;
    class TFHEGeometry;
    class TFHEPolygon;
}  // namespace SpatialFHE::geom

namespace SpatialFHE::algorithm::locate {

    /** \brief
     * Computes the location of points relative to a polygonal
     * [Geometry](@ref geom::Geometry), using a simple `O(n)` algorithm.
     *
     * The algorithm used reports if a point lies in the interior, exterior,
     * or exactly on the boundary of the Geometry.
     *
     * Instance methods are provided to implement the interface `PointInAreaLocator`.
     * However, they provide no performance advantage over the class methods.
     *
     * This algorithm is suitable for use in cases where only a few points will be tested.
     * If many points will be tested, IndexedPointInAreaLocator may provide better performance.
     */
    class SimplePointInAreaLocator : public TFHEPointOnGeometryLocator {
    public:
        static geom::Location locate(const geom::TFHECoordinate& p, const geom::TFHEGeometry* geom);


        /** \brief
         * Determines the Location of a point in a [Polygon](@ref geom::TFHEPolygon).
         *
         * The origin locatePointInSurface method in GEOS is changed to locatePointInPolygon
         * as we are not using Surface class
         *
         * The return value is one of:
         *
         * - geom::Location::INTERIOR
         *   if the point is in the geometry interior
         * - geom::Location::BOUNDARY
         *   if the point lies exactly on the boundary
         * - geom::Location::EXTERIOR
         *   if the point is outside the geometry
         *
         * Computes `geom::Location::BOUNDARY` if the point lies exactly
         * on the polygon boundary.
         *
         * @param p the point to test
         * @param poly the geometry to test
         * @return the Location of the point in the polygon
         */
        static geom::Location locatePointInPolygon(const geom::TFHECoordinate& p, const geom::TFHEPolygon& poly);

        /** \brief
         * Determines whether a point is contained in a [Geometry](@ref geom::Geometry),
         * or lies on its boundary.
         *
         * This is a convenience method for
         *
         *      Location::EXTERIOR != locate(p, geom)
         *
         * @param p the point to test
         * @param geom the geometry to test
         * @return true if the point lies in or on the geometry
         */
        static bool isContained(const geom::TFHECoordinate& p, const geom::TFHEGeometry* geom);

        SimplePointInAreaLocator(const geom::TFHEGeometry* p_g) : g(p_g) {}

        SimplePointInAreaLocator(const geom::TFHEGeometry& p_g) : g(&p_g) {}

        geom::Location locate(const geom::TFHECoordinate* p) override {
            return locate(*p, g);
        }

    private:
        static geom::Location locateInGeometry(const geom::TFHECoordinate& p, const geom::TFHEGeometry* geom);

        const geom::TFHEGeometry* g;
    };

}  // namespace SpatialFHE::algorithm::locate

#endif  // SIMPLEPOINTINAREALOCATOR_H
