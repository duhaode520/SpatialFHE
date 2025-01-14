//
// Created by ubuntu on 1/13/25.
//

#include "TFHELinearRing.h"

#include <sstream>

namespace SpatialFHE::geom {
    void TFHELinearRing::validateConstruction() {
        if (points->isEmpty()) {
            return;
        }

        // Do not check isClosed() here, because it returns ciphered value

        if (points->size() < MINIMUM_VALID_SIZE) {
            std::ostringstream os;
            os << "Invalid number of points in LinearRing found " << points->size() << " - must be 0 or >= 4";
            throw std::invalid_argument(os.str());
        }
    }

    TFHELinearRing *TFHELinearRing::cloneInternal() const {
        return new TFHELinearRing(*this);
    }

    TFHELinearRing::TFHELinearRing(const TFHELinearRing &other) = default;

    TFHELinearRing::TFHELinearRing(TFHECoordinateSequence *points, const TFHEGeometryFactory *factory) :
            TFHELineString(points, factory) {
        validateConstruction();
    }

    TFHELinearRing::TFHELinearRing(TFHECoordinateSequence::Ptr &&points, const TFHEGeometryFactory &factory) :
            TFHELineString(std::move(points), factory) {
        validateConstruction();
    }

    TFHELinearRing::TFHELinearRing(std::vector<TFHECoordinate> &&pts, const TFHEGeometryFactory &factory) :
            TFHELineString(std::move(pts), factory) {
        validateConstruction();
    }

    std::unique_ptr<TFHELinearRing> TFHELinearRing::clone() const {
        return std::unique_ptr<TFHELinearRing>(cloneInternal());
    }

    TFHEBool TFHELinearRing::isClosed() const {
        if (points->isEmpty()) {
            return TFHEBool::tfhe_true;
        }
        return TFHELineString::isClosed();
    }

    std::string TFHELinearRing::getGeometryType() const {
        return "LinearRing";
    }

    TFHEGeometryTypeId TFHELinearRing::getGeometryTypeId() const {
        return TFHEGeometryTypeId::TFHE_LINEARRING;
    }

    void TFHELinearRing::setPoints(const TFHECoordinateSequence *points) {
        this->points = points->clone();
    }
}  // namespace SpatialFHE::geom
   // SpatialFHE