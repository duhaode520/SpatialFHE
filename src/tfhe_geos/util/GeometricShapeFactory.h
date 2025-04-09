//
// Created by ubuntu on 2/19/25.
//

#ifndef GEOMETRICSHAPEFACTORY_H
#define GEOMETRICSHAPEFACTORY_H
#include <tfhe_geos/geom/TFHECoordinate.h>

namespace SpatialFHE::geom {
    class TFHECoordinate;
    class TFHEEnvelope;
    class TFHEPolygon;
    class TFHEGeometryFactory;
    class TFHELineString;
}  // namespace SpatialFHE::geom

namespace SpatialFHE::util {
    class GeometricShapeFactory {
    protected:
        class Dimensions {
        public:
            Dimensions();
            geom::TFHECoordinate base;
            geom::TFHECoordinate centre;
            double width;
            double height;
            void setBase(const geom::TFHECoordinate& newBase);
            void setCentre(const geom::TFHECoordinate& newCentre);
            void setSize(double size);
            void setWidth(double nWidth);
            void setHeight(double nHeight);

            // Return newly-allocated object, ownership transferred
            std::unique_ptr<geom::TFHEEnvelope> getEnvelope() const;
        };

        const geom::TFHEGeometryFactory* geomFact;  // externally owned
        Dimensions dim;
        uint32_t nPts;

        [[nodiscard]] geom::TFHECoordinate coord(TFHEDecimal& x, TFHEDecimal& y) const;
        const double MATH_PI = 3.14159265358979323846;

    public:
        /**
         * \brief
         * Create a shape factory which will create shapes using the given
         * GeometryFactory.
         *
         * @param factory the factory to use. You need to keep the factory
         *                alive for the whole GeometricShapeFactory life time.
         */
        explicit GeometricShapeFactory(const geom::TFHEGeometryFactory* factory);

        virtual ~GeometricShapeFactory() = default;

        /**
         * \brief Creates an elliptical arc, as a LineString.
         *
         * The arc is always created in a counter-clockwise direction.
         *
         * @param startAng start angle in radians
         * @param angExtent size of angle in radians
         * @return an elliptical arc
         */
        std::unique_ptr<geom::TFHELineString> createArc(double startAng, double angExtent);

        /**
         * \brief Creates an elliptical arc polygon.
         *
         * The polygon is formed from the specified arc of an ellipse
         * and the two radii connecting the endpoints to the centre of
         * the ellipse.
         *
         * @param startAng start angle in radians
         * @param angExt size of angle in radians
         * @return an elliptical arc polygon
         */
        std::unique_ptr<geom::TFHEPolygon> createArcPolygon(double startAng, double angExt);

        /**
         * \brief Creates a circular Polygon.
         *
         * @return a circle
         */
        std::unique_ptr<geom::TFHEPolygon> createCircle();

        /**
         * \brief Creates a rectangular Polygon.
         *
         * @return a rectangular Polygon
         */
        std::unique_ptr<geom::TFHEPolygon> createRectangle();

        /**
         * \brief
         * Sets the location of the shape by specifying the base coordinate
         * (which in most cases is the * lower left point of the envelope
         * containing the shape).
         *
         * @param base the base coordinate of the shape
         */
        void setBase(const geom::TFHECoordinate& base);

        /**
         * \brief
         * Sets the location of the shape by specifying the centre of
         * the shape's bounding box
         *
         * @param centre the centre coordinate of the shape
         */
        void setCentre(const geom::TFHECoordinate& centre);

        /**
         * \brief Sets the height of the shape.
         *
         * @param height the height of the shape
         */
        void setHeight(double height);

        /**
         * \brief Sets the total number of points in the created Geometry
         */
        void setNumPoints(uint32_t nNPts);

        /**
         * \brief
         * Sets the size of the extent of the shape in both x and y directions.
         *
         * @param size the size of the shape's extent
         */
        void setSize(double size);

        /**
         * \brief Sets the width of the shape.
         *
         * @param width the width of the shape
         */
        void setWidth(double width);
    };

}  // namespace SpatialFHE::util

#endif  // GEOMETRICSHAPEFACTORY_H
