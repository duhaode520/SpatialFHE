// Adapted from GEOS/GeometryFactory.h

#ifndef TFHEGEOMETRYFACTORY_H
#define TFHEGEOMETRYFACTORY_H

#include <memory>
#include <vector>

#include "TFHECoordinateSequenceFactory.h"
#include "TFHEGeometry.h"
#include "TFHELineString.h"
#include "TFHEPoint.h"
#include "TFHEPolygon.h"

namespace SpatialFHE::geom {

    class TFHEGeometryFactory {
    private:
        friend class TFHEGeometry;

        mutable int refCount = 0;
        const TFHECoordinateSequenceFactory* csFactory = TFHECoordinateSequenceFactory::instance();
        bool autoDestroy;

        void addRef() const;
        void removeRef() const;
        friend class TFHEGeometry;

    public:
        // 这个放在public是为了 SWIG 能使用
        struct TFHEGeometryFactoryDeleter {
            void operator()(TFHEGeometryFactory* gf) const {
                gf->destroy();
            }
        };

        using Ptr = std::unique_ptr<TFHEGeometryFactory, TFHEGeometryFactoryDeleter>;

        static Ptr create();
        static Ptr create(const TFHEGeometryFactory& gf);

        /**
         * Return a pointer to the default GeometryFactory.
         * This is a global shared object.
         */
        static const TFHEGeometryFactory* getDefaultInstance();

        // Create an empty point
        [[nodiscard]] std::unique_ptr<TFHEPoint> createPoint() const;

        // Create point from coordinates
        [[nodiscard]] std::unique_ptr<TFHEPoint> createPoint(const TFHECoordinate& cs) const;
        std::unique_ptr<TFHEPoint> createPoint(TFHECoordinateSequence* coordinates) const;
        std::unique_ptr<TFHEPoint> createPoint(const TFHECoordinateSequence& fromCoords) const;
        std::unique_ptr<TFHEPoint> createPoint(std::unique_ptr<TFHECoordinateSequence>&& coordinates) const;

        [[nodiscard]] std::unique_ptr<TFHELineString> createLineString() const;
        [[nodiscard]] std::unique_ptr<TFHELineString> createLineString(const TFHELineString& ls) const;
        std::unique_ptr<TFHELineString> createLineString(TFHECoordinateSequence* coordinates) const;
        std::unique_ptr<TFHELineString> createLineString(std::unique_ptr<TFHECoordinateSequence>&& coordinates) const;
        std::unique_ptr<TFHELineString> createLineString(std::vector<TFHECoordinate>&& coordinates) const;
        std::unique_ptr<TFHELineString> createLineString(const TFHECoordinateSequence& fromCoords) const;

        std::unique_ptr<TFHELinearRing> createLinearRing() const;
        std::unique_ptr<TFHELinearRing> createLinearRing(const TFHELinearRing& lr) const;
        std::unique_ptr<TFHELinearRing> createLinearRing(TFHECoordinateSequence* coordinates) const;
        std::unique_ptr<TFHELinearRing> createLinearRing(std::unique_ptr<TFHECoordinateSequence>&& coordinates) const;
        std::unique_ptr<TFHELinearRing> createLinearRing(std::vector<TFHECoordinate>&& coordinates) const;
        std::unique_ptr<TFHELinearRing> createLinearRing(const TFHECoordinateSequence& fromCoords) const;

        std::unique_ptr<TFHEPolygon> createPolygon() const;
        std::unique_ptr<TFHEPolygon> createPolygon(const TFHEPolygon& poly) const;
        std::unique_ptr<TFHEPolygon> createPolygon(TFHELinearRing* shell, std::vector<TFHELinearRing*>* holes) const;
        std::unique_ptr<TFHEPolygon> createPolygon(
            std::unique_ptr<TFHELinearRing>&& shell,
            std::vector<std::unique_ptr<TFHELinearRing>>&& holes) const;
        std::unique_ptr<TFHEPolygon> createPolygon(std::unique_ptr<TFHELinearRing>&& shell) const;
        std::unique_ptr<TFHEPolygon> createPolygon(std::vector<TFHECoordinate>&& coords) const;

        [[nodiscard]] std::unique_ptr<TFHEGeometry> createEmptyGeometry(TFHEGeometryTypeId type) const;

        const TFHECoordinateSequenceFactory* getCoordinateSequenceFactory() const;

        void destroy();

    protected:
        TFHEGeometryFactory();
        TFHEGeometryFactory(const TFHEGeometryFactory& gf);
    };

}  // namespace SpatialFHE::geom

// SpatialFHE

#endif  // TFHEGEOMETRYFACTORY_H
