//
// Created by ubuntu on 1/12/25.
//

#include "TFHELineString.h"

#include <cassert>
#include <stdexcept>

#include "TFHEGeometryFactory.h"

namespace SpatialFHE::geom {
    std::unique_ptr<TFHEEnvelope> TFHELineString::computeEnvelopeInternal() const {
        if (isEmpty()) {
            return std::make_unique<TFHEEnvelope>();
        }
        return std::make_unique<TFHEEnvelope>(points->getEnvelope());
    }

    TFHELineString *TFHELineString::cloneInternal() const {
        return new TFHELineString(*this);
    }

    TFHELineString::Ptr TFHELineString::clone() const {
        return std::unique_ptr<TFHELineString>(cloneInternal());
    }

    void TFHELineString::validateConstruction() {
        if (points.get() == nullptr) {
            points = factory->getCoordinateSequenceFactory()->create();
            return;
        }

        if (points->size() == 1) {
            throw std::invalid_argument("LineString must have at least 2 points");
        }
    }

    TFHELineString::TFHELineString(const TFHELineString &other) : TFHEGeometry(other), points(other.points->clone()) {}

    TFHELineString::TFHELineString(TFHECoordinateSequence *pts, const TFHEGeometryFactory *factory) :
            TFHEGeometry(factory), points(pts) {}

    TFHELineString::TFHELineString(TFHECoordinateSequence::Ptr &&pts, const TFHEGeometryFactory &factory) :
            TFHEGeometry(&factory), points(std::move(pts)) {
        validateConstruction();
    }

    TFHELineString::TFHELineString(std::vector<TFHECoordinate> &&pts, const TFHEGeometryFactory &factory) :
            TFHEGeometry(&factory), points(new TFHECoordinateSequence(std::move(pts))) {
        validateConstruction();
    }

    bool TFHELineString::isEmpty() const {
        assert(points.get());
        return points->isEmpty();
    }

    const TFHECoordinate *TFHELineString::getCoordinate() const {
        if (isEmpty()) {
            return nullptr;
        }
        return &(points->getAt(0));
    }

    std::unique_ptr<TFHECoordinateSequence> TFHELineString::getCoordinates() const {
        assert(points.get());
        return points->clone();
    }

    const TFHECoordinateSequence *TFHELineString::getCoordinatesRO() const {
        assert(points.get());
        return points.get();
    }

    std::size_t TFHELineString::getNumPoints() const {
        assert(points.get());
        return points->size();
    }

    std::string TFHELineString::getGeometryType() const {
        return "LineString";
    }

    TFHEGeometryTypeId TFHELineString::getGeometryTypeId() const {
        return TFHEGeometryTypeId::TFHE_LINESTRING;
    }

    Dimension::DimensionType TFHELineString::getDimension() const {
        return Dimension::L;
    }

    std::unique_ptr<TFHEPoint> TFHELineString::getPointN(std::size_t n) const {
        assert(getFactory());
        assert(points.get());
        return getFactory()->createPoint(points->getAt(n));
    }

    std::unique_ptr<TFHEPoint> TFHELineString::getStartPoint() const {
        if (isEmpty()) {
            return nullptr;
        }
        return getPointN(0);
    }

    std::unique_ptr<TFHEPoint> TFHELineString::getEndPoint() const {
        if (isEmpty()) {
            return nullptr;
        }
        return getPointN(getNumPoints() - 1);
    }

    TFHEBool TFHELineString::isClosed() const {
        if (isEmpty()) {
            return TFHEBool::tfhe_false;
        }
        return points.get()->getAt(0).equals(points.get()->getAt(getNumPoints()));
    }

    TFHEBool TFHELineString::isRing() const {
        // TODO: change this to isClosed && isSimple after implementing isSimple
        return isClosed();
    }

}  // namespace SpatialFHE::geom

// SpatialFHE