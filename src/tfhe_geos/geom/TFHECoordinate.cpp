//
// Created by ubuntu on 1/7/25.
//

#include "TFHECoordinate.h"

#include <utility>

namespace SpatialFHE::geom {
    TFHECoordinate::TFHECoordinate(TFHEInt32 x, TFHEInt32 y) : x(std::move(x)), y(std::move(y)) {}

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

    TFHEBool TFHECoordinate::equals(const TFHECoordinate &other) const {
        if (other.isNull()) {
            return isNull()? TFHEBool(true) : TFHEBool(false);
        }
        return x == other.x && y == other.y;
    }

    TFHEInt32 TFHECoordinate::distanceSquared(const TFHECoordinate &p) const {
        TFHEInt32 dx = x - p.x;
        TFHEInt32 dy = y - p.y;
        return dx * dx + dy * dy;
    }

    bool TFHECoordinate::operator==(const TFHECoordinate &other) const {
        // fast check address equality
        if (this == &other) {
            return true;
        }
        return equals(other).decrypt();
    }

    bool TFHECoordinate::isNull() const {
        return x.isNull() && y.isNull();
    }

    TFHEInt32 TFHECoordinate::compareTo(const TFHECoordinate &other) const {
        TFHEInt32 xLess = TFHEInt32(x < other.x);
        TFHEInt32 yLess = TFHEInt32(y < other.y);
        TFHEInt32 xGreater = TFHEInt32(x > other.x);
        TFHEInt32 yGreater = TFHEInt32(y > other.y);
        return (xGreater - xLess) * 2 + (yGreater - yLess);
    }

    TFHEInt32 TFHECoordinate::GetX() const {
        return x;
    }

    TFHEInt32 TFHECoordinate::GetY() const {
        return y;
    }

}  // namespace SpatialFHE::geom