//
// Created by ubuntu on 1/12/25.
//

#include "TFHEGeometryFactory.h"

#include <cassert>
#include <stdexcept>

namespace SpatialFHE::geom {
    void TFHEGeometryFactory::addRef() const {
        ++refCount;
    }

    void TFHEGeometryFactory::removeRef() const {
        if (--refCount == 0 && autoDestroy) {
            delete this;
        }
    }

    TFHEGeometryFactory::Ptr TFHEGeometryFactory::create() {
        return Ptr(new TFHEGeometryFactory());
    }

    TFHEGeometryFactory::Ptr TFHEGeometryFactory::create(const TFHEGeometryFactory &gf) {
        return Ptr(new TFHEGeometryFactory(gf));
    }

    const TFHEGeometryFactory *TFHEGeometryFactory::getDefaultInstance() {
        static TFHEGeometryFactory defInstance;
        return &defInstance;
    }

    std::unique_ptr<TFHEPoint> TFHEGeometryFactory::createPoint() const {
        return std::unique_ptr<TFHEPoint>(new TFHEPoint(nullptr, this));
    }

    std::unique_ptr<TFHEPoint> TFHEGeometryFactory::createPoint(const TFHECoordinate &cs) const {
        if (cs.isNull()) {
            return createPoint();
        }
        return std::unique_ptr<TFHEPoint>(new TFHEPoint(cs, this));
    }

    std::unique_ptr<TFHEPoint> TFHEGeometryFactory::createPoint(TFHECoordinateSequence *coordinates) const {
        return std::unique_ptr<TFHEPoint>(new TFHEPoint(coordinates, this));
    }

    std::unique_ptr<TFHEPoint> TFHEGeometryFactory::createPoint(const TFHECoordinateSequence &fromCoords) const {
        auto newCoords = fromCoords.clone();
        return std::unique_ptr<TFHEPoint>(new TFHEPoint(newCoords.get(), this));
    }

    std::unique_ptr<TFHEPoint> TFHEGeometryFactory::createPoint(
        std::unique_ptr<TFHECoordinateSequence> &&coordinates) const {
        if (!coordinates) {
            return createPoint();
        }
        return std::unique_ptr<TFHEPoint>(new TFHEPoint(std::move(*coordinates), this));
    }

    std::unique_ptr<TFHELineString> TFHEGeometryFactory::createLineString() const {
        return std::unique_ptr<TFHELineString>(new TFHELineString(nullptr, this));
    }

    std::unique_ptr<TFHELineString> TFHEGeometryFactory::createLineString(const TFHELineString &ls) const {
        return std::unique_ptr<TFHELineString>(new TFHELineString(ls));
    }

    std::unique_ptr<TFHELineString> TFHEGeometryFactory::createLineString(TFHECoordinateSequence *coordinates) const {
        return std::unique_ptr<TFHELineString>(new TFHELineString(coordinates, this));
    }

    std::unique_ptr<TFHELineString> TFHEGeometryFactory::createLineString(
        std::unique_ptr<TFHECoordinateSequence> &&coordinates) const {
        return std::unique_ptr<TFHELineString>(new TFHELineString(std::move(coordinates), *this));
    }

    std::unique_ptr<TFHELineString> TFHEGeometryFactory::createLineString(
        std::vector<TFHECoordinate> &&coordinates) const {
        return std::unique_ptr<TFHELineString>(new TFHELineString(std::move(coordinates), *this));
    }

    std::unique_ptr<TFHELineString> TFHEGeometryFactory::createLineString(
        const TFHECoordinateSequence &fromCoords) const {
        auto newCoords = fromCoords.clone();
        return std::unique_ptr<TFHELineString>(new TFHELineString(newCoords.get(), this));
    }

    std::unique_ptr<TFHELinearRing> TFHEGeometryFactory::createLinearRing() const {
        return std::make_unique<TFHELinearRing>(nullptr, this);
    }

    std::unique_ptr<TFHELinearRing> TFHEGeometryFactory::createLinearRing(const TFHELinearRing &lr) const {
        return std::make_unique<TFHELinearRing>(lr);
    }

    std::unique_ptr<TFHELinearRing> TFHEGeometryFactory::createLinearRing(TFHECoordinateSequence *coordinates) const {
        return std::make_unique<TFHELinearRing>(coordinates, this);
    }

    std::unique_ptr<TFHELinearRing> TFHEGeometryFactory::createLinearRing(
        std::unique_ptr<TFHECoordinateSequence> &&coordinates) const {
        return std::make_unique<TFHELinearRing>(std::move(coordinates), *this);
    }

    std::unique_ptr<TFHELinearRing> TFHEGeometryFactory::createLinearRing(
        std::vector<TFHECoordinate> &&coordinates) const {
        return std::make_unique<TFHELinearRing>(std::move(coordinates), *this);
    }

    std::unique_ptr<TFHELinearRing> TFHEGeometryFactory::createLinearRing(
        const TFHECoordinateSequence &fromCoords) const {
        auto newCoords = fromCoords.clone();
        return std::make_unique<TFHELinearRing>(newCoords.get(), this);
    }

    std::unique_ptr<TFHEPolygon> TFHEGeometryFactory::createPolygon() const {
        auto cs = csFactory->create(std::size_t(0));
        auto lr = createLinearRing(cs.release());
        return std::unique_ptr<TFHEPolygon>(createPolygon(lr.release(), nullptr));
    }

    std::unique_ptr<TFHEPolygon> TFHEGeometryFactory::createPolygon(const TFHEPolygon &poly) const {
        return std::unique_ptr<TFHEPolygon>(new TFHEPolygon(poly));
    }

    std::unique_ptr<TFHEPolygon> TFHEGeometryFactory::createPolygon(
        TFHELinearRing *shell,
        std::vector<TFHELinearRing *> *holes) const {
        return std::unique_ptr<TFHEPolygon>(new TFHEPolygon(shell, holes, this));
    }

    std::unique_ptr<TFHEPolygon> TFHEGeometryFactory::createPolygon(
        std::unique_ptr<TFHELinearRing> &&shell,
        std::vector<std::unique_ptr<TFHELinearRing>> &&holes) const {
        return std::unique_ptr<TFHEPolygon>(new TFHEPolygon(std::move(shell), std::move(holes), *this));
    }

    std::unique_ptr<TFHEPolygon> TFHEGeometryFactory::createPolygon(std::unique_ptr<TFHELinearRing> &&shell) const {
        return std::unique_ptr<TFHEPolygon>(new TFHEPolygon(std::move(shell), *this));
    }

    std::unique_ptr<TFHEPolygon> TFHEGeometryFactory::createPolygon(std::vector<TFHECoordinate> &&coords) const {
        auto cs = csFactory->create(std::move(coords));
        auto lr = createLinearRing(std::move(cs));
        return createPolygon(std::move(lr));
    }

    std::unique_ptr<TFHEGeometry> TFHEGeometryFactory::createEmptyGeometry(TFHEGeometryTypeId type) const {
        switch (type) {
        case TFHEGeometryTypeId::TFHE_POINT:
            return createPoint();
        case TFHEGeometryTypeId::TFHE_LINESTRING:
            return createLineString();
        case TFHEGeometryTypeId::TFHE_LINEARRING:
            return createLinearRing();
        case TFHEGeometryTypeId::TFHE_POLYGON:
            return createPolygon();
        default:
            throw std::invalid_argument("Invalid geometry type");
        }
    }

    const TFHECoordinateSequenceFactory *TFHEGeometryFactory::getCoordinateSequenceFactory() const {
        return csFactory;
    }

    void TFHEGeometryFactory::destroy() {
        assert(!autoDestroy);
        autoDestroy = true;
        if (!refCount) {
            delete this;
        }
    }

    TFHEGeometryFactory::TFHEGeometryFactory() : refCount(0), autoDestroy(false) {}

    TFHEGeometryFactory::TFHEGeometryFactory(const TFHEGeometryFactory &gf) = default;

}  // namespace SpatialFHE::geom

// SpatialFHE