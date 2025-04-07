//
// Created by ubuntu on 2/19/25.
//

#ifndef SINESTARFACTORY_H
#define SINESTARFACTORY_H
#include <memory>

#include "GeometricShapeFactory.h"

namespace SpatialFHE::geom {
    class TFHECoordinate;
    class TFHEEnvelope;
    class TFHEPolygon;
    class TFHEGeometryFactory;
    class TFHELineString;
}  // namespace SpatialFHE::geom

namespace SpatialFHE::util {

    class SineStarFactory : public GeometricShapeFactory {
    protected:
        int numArms;
        double armLengthRatio;

    public:
        /**
         * Creates a factory which will create sine stars using the given
         * {@link TFHEGeometryFactory}.
         *
         * @param fact the factory to use. You need to keep the
         *	factory alive for the whole SineStarFactory
         *	life time.
         */
        explicit SineStarFactory(const geom::TFHEGeometryFactory* fact) :
                GeometricShapeFactory(fact), numArms(8), armLengthRatio(0.5) {}

        /**
         * Sets the number of arms in the star
         *
         * @param nArms the number of arms to generate
         */
        void setNumArms(int nArms) {
            numArms = nArms;
        }

        /**
         * Sets the ration of the length of each arm to the distance from the tip
         * of the arm to the centre of the star.
         * Value should be between 0.0 and 1.0
         *
         * @param armLenRatio
         */
        void setArmLengthRatio(double armLenRatio) {
            armLengthRatio = armLenRatio;
        }

        /**
         * Generates the geometry for the sine star
         *
         * @return the geometry representing the sine star
         */
        [[nodiscard]] std::unique_ptr<geom::TFHEPolygon> createSineStar() const;

        static std::unique_ptr<geom::TFHEPolygon> create(
            geom::TFHECoordinate& origin,
            double size,
            int nPts,
            int nArms,
            double armLengthRatio);

        // return shared TFHEPolygon for java
        static std::shared_ptr<geom::TFHEPolygon> createShared(
            geom::TFHECoordinate& origin,
            double size,
            int nPts,
            int nArms,
            double armLengthRatio);
    };

}  // namespace SpatialFHE::util

// SpatialFHE

#endif  // SINESTARFACTORY_H
