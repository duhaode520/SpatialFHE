//
// Created by ubuntu on 1/14/25.
//

#ifndef TFHERELATEPOINTLOCATOR_H
#define TFHERELATEPOINTLOCATOR_H
#include <tfhe_geos/geom/TFHECoordinate.h>
#include <tfhe_geos/geom/TFHEGeometry.h>
#include "TFHEAdjacentEdgeLocator.h"
#include "TFHELinearBoundary.h"
#include "tfhe_geos/algorithm/locate/TFHEPointOnGeometryLocator.h"

namespace SpatialFHE::operation::relateng {

    class TFHERelatePointLocator {
        using TFHECoordinate = SpatialFHE::geom::TFHECoordinate;
        using TFHEGeometry = SpatialFHE::geom::TFHEGeometry;
        using TFHEPoint = SpatialFHE::geom::TFHEPoint;
        using TFHELineString = SpatialFHE::geom::TFHELineString;
        using Location = SpatialFHE::geom::Location;
        using TFHEPointOnGeometryLocator = SpatialFHE::algorithm::locate::TFHEPointOnGeometryLocator;
    private:
        // Members
        const TFHEGeometry *geom;
        bool isPrepared = false;
        // used in locatePolygonal
        std::unique_ptr<TFHEAdjacentEdgeLocator> adjEdgeLocator;

        // only one point given that collections are not supported
        TFHECoordinate::ConstSet points;
        // only one line given that collections are not supported
        std::vector<const TFHELineString *> lines;
        // only one polygon given that collections are not supported
        std::vector<const TFHEGeometry *> polygons;
        std::vector<std::unique_ptr<TFHEPointOnGeometryLocator>> polyLocator;
        std::unique_ptr<TFHELinearBoundary> lineBoundary;
        bool isEmpty;

        // Methods

        /**
        * Computes the topological location (Location) of a single point
        * in a Geometry, as well as the dimension of the geometry element the point
        * is located in (if not in the Exterior).
        * It handles both single-element and multi-element Geometries.
        * The algorithm for multi-part Geometries
        * takes into account the SFS Boundary Determination Rule.
        *
        * @param p the coordinate to locate
        * @param isNode whether the coordinate is a node (on an edge) of the geometry
        * @param polygon
        * @return the Location of the point relative to the input Geometry
        */
        int locateWithDim(const TFHECoordinate* p, bool isNode, const TFHEGeometry* parentPolygonal);

        int computeDimLocation(const TFHECoordinate* p, bool isNode, const TFHEGeometry* parentPolygonal);

        Location locateOnPoints(const TFHECoordinate* p) const;

        Location locateOnLines(const TFHECoordinate* p, bool isNode);

        Location locateOnLine(const TFHECoordinate* p, const TFHELineString* l);

        Location locateOnPolygons(const TFHECoordinate* p, bool isNode, const TFHEGeometry* parentPolygonal);

        Location locateOnPolygonal(const TFHECoordinate* p,
            bool isNode,
            const TFHEGeometry* parentPolygonal,
            std::size_t index);

        TFHEPointOnGeometryLocator* getLocator(std::size_t index);


    public:
        TFHERelatePointLocator(const TFHEGeometry *geom);

        TFHERelatePointLocator(const TFHEGeometry *geom, bool isPrepared);

        void init(const TFHEGeometry *geom);

        bool hasBoundary() const;

        void extractElements(const TFHEGeometry* geom);

        void addPoint(const TFHEPoint* pt);

        void addLine(const TFHELineString* line);

        void addPolygonal(const TFHEGeometry* polygonal);

        Location locate(const TFHECoordinate* p);

        int locateLineEndWithDim(const TFHECoordinate* p);

        /*
        * Locates a point which is known to be a node of the geometry
        * (i.e. a vertex or on an edge).
        *
        * @param p the node point to locate
        * @param parentPolygonal the polygon the point is a node of
        * @return the location of the node point
        */
        Location locateNode(const TFHECoordinate* p, const TFHEGeometry* parentPolygonal);

        /**
        * Locates a point which is known to be a node of the geometry,
        * as a DimensionLocation.
        *
        * @param p the point to locate
        * @param parentPolygonal the polygon the point is a node of
        * @return the dimension and location of the point
        */
        int locateNodeWithDim(const TFHECoordinate* p, const TFHEGeometry* parentPolygonal);

        /**
        * Computes the topological location ( Location) of a single point
        * in a Geometry, as well as the dimension of the geometry element the point
        * is located in (if not in the Exterior).
        * It handles both single-element and multi-element Geometries.
        * The algorithm for multi-part Geometries
        * takes into account the SFS Boundary Determination Rule.
        *
        * @param p the point to locate
        * @return the Location of the point relative to the input Geometry
        */
        int locateWithDim(const TFHECoordinate* p);
    };

}  // namespace SpatialFHE::operation::relateng

#endif  // TFHERELATEPOINTLOCATOR_H
