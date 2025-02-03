//
// Created by ubuntu on 1/7/25.
//

#ifndef TFHEGEOMETRY_H
#define TFHEGEOMETRY_H
#include <algorithm>
#include <memory>
#include <string>
#include <vector>

#include "TFHEEnvelope.h"
#include "TFHEIntersectionMatrix.h"
#include "TFHECoordinateSequence.h"
#include "enums.h"

namespace SpatialFHE::geom {
    class TFHECoordinateSequence;
}

namespace SpatialFHE::geom {
    // Forward declarations
    class TFHEGeometryFactory;
    class TFHEPoint;
    class TFHELineString;

    // Geometry types
    enum class TFHEGeometryTypeId {
        TFHE_POINT,
        TFHE_LINESTRING,
        TFHE_LINEARRING,
        TFHE_POLYGON,
    };

    class TFHEGeometry {
    protected:
        mutable std::unique_ptr<TFHEEnvelope> envelope;
        const TFHEGeometryFactory *factory;

        TFHEGeometry(const TFHEGeometry &geom);
        explicit TFHEGeometry(const TFHEGeometryFactory *factory);

        virtual std::unique_ptr<TFHEEnvelope> computeEnvelopeInternal() const = 0;
        [[nodiscard]] virtual TFHEGeometry *cloneInternal() const = 0;

    public:
        friend class TFHEGeometryFactory;

        virtual ~TFHEGeometry() = default;

        virtual bool isEmpty() const = 0;
        // virtual bool isSimple() = 0;

        // Returns a vertex of the geometry, or nullptr if the geometry is empty.
        virtual const TFHECoordinate *getCoordinate() const = 0;

        virtual std::unique_ptr<TFHECoordinateSequence> getCoordinates() const = 0;

        // Returns the number of vertices in the geometry.
        virtual std::size_t getNumPoints() const = 0;

        // Returns a string representation of the geometry.
        virtual std::string getGeometryType() const = 0;

        virtual TFHEGeometryTypeId getGeometryTypeId() const = 0;

        // Polygon overrides to check for rectangle
        virtual bool isRectangle() const {
            return false;
        }

        // Returns the dimension of the geometry.
        virtual Dimension::DimensionType getDimension() const = 0;

        virtual bool isDimension(Dimension::DimensionType dimension) const {
            return getDimension() == dimension;
        }

        virtual const TFHEEnvelope *getEnvelope() const {
            if (!envelope) {
                envelope = computeEnvelopeInternal();
            }
            return envelope.get();
        }

        [[nodiscard]] std::unique_ptr<TFHEGeometry> clone() const {
            return std::unique_ptr<TFHEGeometry>(cloneInternal());
        }

        [[nodiscard]] const TFHEGeometryFactory *getFactory() const {
            return factory;
        }

        virtual TFHEBool intersects(const TFHEGeometry *g) const;
        virtual TFHEBool contains(const TFHEGeometry *geom) const;
        virtual TFHEBool within(const TFHEGeometry *geom) const;
        virtual TFHEBool touches(const TFHEGeometry *geom) const;
        virtual TFHEBool overlaps(const TFHEGeometry *geom) const;
        virtual TFHEBool crosses(const TFHEGeometry *geom) const;
        virtual TFHEBool disjoint(const TFHEGeometry *geom) const;
        virtual TFHEBool equals(const TFHEGeometry *geom) const;
        virtual TFHEBool covers(const TFHEGeometry *geom) const;
        virtual TFHEBool coveredBy(const TFHEGeometry *geom) const;

        virtual TFHEBool relate(const TFHEGeometry *geom, const std::string &pattern) const;
        virtual std::unique_ptr<TFHEIntersectionMatrix> relate(const TFHEGeometry *geom);
        virtual std::unique_ptr<TFHEIntersectionMatrix> relate(const TFHEGeometry& geom);

        // virtual TFHEGeometry* buffer(double distance) const = 0;
        // virtual TFHEGeometry* convexHull() const = 0;
        // virtual TFHEGeometry* intersection(const TFHEGeometry &geom) const = 0;
        // virtual TFHEGeometry* union_(const TFHEGeometry &geom) const = 0;
        // virtual TFHEGeometry* difference(const TFHEGeometry &geom) const = 0;
        // virtual TFHEGeometry* symDifference(const TFHEGeometry &geom) const = 0;

        /// Returns true if the array contains any non-empty Geometrys.
        template <typename T> static bool hasNonEmptyElements(const std::vector<T> *geometries) {
            return std::any_of(geometries->begin(), geometries->end(), [](const T &g) { return !g->isEmpty(); });
        }

        /// Returns true if the CoordinateSequence contains any null elements.
        static bool hasNullElements(const TFHECoordinateSequence *list);

        /// Returns true if the vector contains any null elements.
        template <typename T> static bool hasNullElements(const std::vector<T> *geometries) {
            return std::any_of(geometries->begin(), geometries->end(), [](const T &g) { return g == nullptr; });
        }
    };
}  // namespace SpatialFHE::geom

#endif  // TFHEGEOMETRY_H
