//
// Created by ubuntu on 1/7/25.
//

#include "TFHECoordinate.h"
#include <utility>

namespace SpatialFHE::geom {
    TFHECoordinate::TFHECoordinate(TFHEInt32 x, TFHEInt32 y) :
            x(std::move(x)), y(std::move(y)) {}

    TFHECoordinate::TFHECoordinate(const TFHECoordinate &other) {
        x = other.x;
        y = other.y;
    }

    TFHECoordinate::TFHECoordinate(TFHECoordinate &&other) noexcept {
        x = std::move(other.x);
        y = std::move(other.y);
    }

    TFHECoordinate &TFHECoordinate::operator=(const TFHECoordinate &other) = default;

    TFHECoordinate &TFHECoordinate::operator=(TFHECoordinate &&other) noexcept {
        x = std::move(other.x);
        y = std::move(other.y);
        return *this;
    }

    TFHEBool TFHECoordinate::operator==(const TFHECoordinate &other) const {
        return x == other.x && y == other.y;
    }

    TFHEBool TFHECoordinate::operator!=(const TFHECoordinate &other) const {
        return x != other.x || y != other.y;
    }
    bool TFHECoordinate::isNull() const {
        return x.isNull() && y.isNull();
    }

}  // namespace SpatialFHE::geom