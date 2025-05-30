//
// Created by ubuntu on 1/12/25.
//

#include "TFHEPoint.h"

#include <stdexcept>

#include "TFHEGeometryFactory.h"

namespace SpatialFHE::geom {
    std::unique_ptr<TFHEEnvelope> TFHEPoint::computeEnvelopeInternal() const {
        if (isEmpty()) {
            return std::make_unique<TFHEEnvelope>();
        } else {
            return std::make_unique<TFHEEnvelope>(coordinates.getAt(0));
        }
    }

    TFHEPoint *TFHEPoint::cloneInternal() const {
        return new TFHEPoint(*this);
    }

    TFHEPoint::Ptr TFHEPoint::clone() const {
        return Ptr(cloneInternal());
    }

    TFHEPoint::TFHEPoint(const TFHECoordinate &c, const TFHEGeometryFactory *newFactory) : TFHEGeometry(newFactory) {
        coordinates.add(c);
    }

    TFHEPoint::TFHEPoint(TFHECoordinateSequence &&c, const TFHEGeometryFactory *newFactory) :
            TFHEGeometry(newFactory), coordinates(c) {
        if (c.size() != 1) {
            throw std::invalid_argument("Point must have exactly one coordinate");
        }
    }

    TFHEPoint::TFHEPoint(TFHECoordinateSequence *newCoords, const TFHEGeometryFactory *newFactory) :
            TFHEGeometry(newFactory) {
        std::unique_ptr<TFHECoordinateSequence> coords(newCoords);
        if (coords == nullptr || coords->isEmpty()) {
            coordinates = TFHECoordinateSequence();
            return;
        }
        if (coords->size() == 1) {
            coordinates.add(coords->getAt(0));
        } else if (coords->size() > 1) {
            throw std::invalid_argument("Point must have exactly one coordinate");
        }
    }

    TFHEPoint::TFHEPoint(const TFHEPoint &p) = default;

    bool TFHEPoint::isEmpty() const {
        return coordinates.isEmpty();
    }

    const TFHECoordinate *TFHEPoint::getCoordinate() const {
        return isEmpty() ? nullptr : &coordinates.front();
    }

    std::unique_ptr<TFHECoordinateSequence> TFHEPoint::getCoordinates() const {
        if (isEmpty()) {
            return std::make_unique<TFHECoordinateSequence>();
        } else {
            return coordinates.clone();
        }
    }

    std::size_t TFHEPoint::getNumPoints() const {
        return coordinates.size();
    }

    std::string TFHEPoint::getGeometryType() const {
        return "Point";
    }

    TFHEGeometryTypeId TFHEPoint::getGeometryTypeId() const {
        return TFHEGeometryTypeId::TFHE_POINT;
    }

    Dimension::DimensionType TFHEPoint::getDimension() const {
        return Dimension::P;
    }

    std::unique_ptr<TFHEGeometry> TFHEPoint::getBoundary() const {
        return factory->createEmptyGeometry(TFHEGeometryTypeId::TFHE_POINT);
    }

    void TFHEPoint::setXY(TFHEDecimal x, TFHEDecimal y) {
        coordinates.setAt({x, y}, 0);
    }

    TFHEDecimal TFHEPoint::getX() const {
        return coordinates.getAt(0).x;
    }

    TFHEDecimal TFHEPoint::getY() const {
        return coordinates.getAt(0).y;
    }
}  // namespace SpatialFHE::geom
