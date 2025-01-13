//
// Created by ubuntu on 1/7/25.
//

#include "TFHEEnvelope.h"

#include <cassert>
#include <memory>

namespace SpatialFHE::geom {
    void TFHEEnvelope::init(const TFHEInt32 &x1, const TFHEInt32 &x2, const TFHEInt32 &y1, const TFHEInt32 &y2) {
        minx = TFHEInt32::min(x1, x2);
        maxx = TFHEInt32::max(x1, x2);
        miny = TFHEInt32::min(y1, y2);
        maxy = TFHEInt32::max(y1, y2);
    }

    TFHEEnvelope::TFHEEnvelope() {
        minx = TFHEInt32();
        maxx = TFHEInt32();
        miny = TFHEInt32();
        maxy = TFHEInt32();
    }

    TFHEEnvelope::TFHEEnvelope(const TFHEInt32 &x1, const TFHEInt32 &x2, const TFHEInt32 &y1, const TFHEInt32 &y2) {
        init(x1, x2, y1, y2);
    }

    TFHEEnvelope::TFHEEnvelope(const TFHECoordinate &p1, const TFHECoordinate &p2) {
        init(p1.x, p2.x, p1.y, p2.y);
    }

    TFHEEnvelope::TFHEEnvelope(const TFHECoordinate &p) {
        init(p.x, p.x, p.y, p.y);
    }

    TFHEEnvelope::TFHEEnvelope(const TFHEEnvelope &other) {
        minx = other.minx;
        maxx = other.maxx;
        miny = other.miny;
        maxy = other.maxy;
    }

    TFHEEnvelope::TFHEEnvelope(TFHEEnvelope &&other) noexcept {
        minx = std::move(other.minx);
        maxx = std::move(other.maxx);
        miny = std::move(other.miny);
        maxy = std::move(other.maxy);
    }

    TFHEEnvelope &TFHEEnvelope::operator=(const TFHEEnvelope &other) = default;

    TFHEEnvelope &TFHEEnvelope::operator=(TFHEEnvelope &&other) noexcept {
        minx = std::move(other.minx);
        maxx = std::move(other.maxx);
        miny = std::move(other.miny);
        maxy = std::move(other.maxy);
        return *this;
    }

    bool TFHEEnvelope::isNull() const {
        return minx.isNull() || maxx.isNull() || miny.isNull() || maxy.isNull();
    }

    TFHEInt32 TFHEEnvelope::getWidth() const {
        if (isNull()) {
            return {};
        }
        return maxx - minx;
    }

    TFHEInt32 TFHEEnvelope::getHeight() const {
        if (isNull()) {
            return {};
        }
        return maxy - miny;
    }

    TFHEInt32 TFHEEnvelope::getMinX() const {
        assert(!isNull());
        return minx;
    }

    TFHEInt32 TFHEEnvelope::getMinY() const {
        assert(!isNull());
        return miny;
    }

    TFHEInt32 TFHEEnvelope::getMaxX() const {
        assert(!isNull());
        return maxx;
    }

    TFHEInt32 TFHEEnvelope::getMaxY() const {
        assert(!isNull());
        return maxy;
    }

    TFHECoordinate TFHEEnvelope::getCenter() const {
        if (isNull()) {
            return {};
        }
        return {(minx + maxx) / 2, (miny + maxy) / 2};
    }

    TFHEEnvelope TFHEEnvelope::intersection(const TFHEEnvelope &other) const {
        if (!this->isNull() && !other.isNull()) {
            TFHEInt32 x1 = TFHEInt32::max(minx, other.minx);
            TFHEInt32 x2 = TFHEInt32::min(maxx, other.maxx);
            TFHEInt32 y1 = TFHEInt32::max(miny, other.miny);
            TFHEInt32 y2 = TFHEInt32::min(maxy, other.maxy);
            return {x1, x2, y1, y2};
        } else {
            return {};
        }
    }

    TFHEBool TFHEEnvelope::intersects(const TFHEEnvelope &other) const {
        if (!this->isNull() && !other.isNull()) {
            return (maxx >= other.minx && minx <= other.maxx && maxy >= other.miny && miny <= other.maxy);
        } else {
            return TFHEBool::tfhe_false;
        }
    }

    TFHEBool TFHEEnvelope::intersects(const TFHECoordinate &p) const {
        return intersects(p.x, p.y);
    }

    TFHEBool TFHEEnvelope::intersects(const TFHEInt32 &x, const TFHEInt32 &y) const {
        if (this->isNull()) {
            return TFHEBool::tfhe_false;
        } else {
            return (x >= minx && x <= maxx && y >= miny && y <= maxy);
        }
    }

    TFHEBool TFHEEnvelope::disjoint(const TFHEEnvelope &other) const {
        if (!this->isNull() && !other.isNull()) {
            return (maxx < other.minx || minx > other.maxx || maxy < other.miny || miny > other.maxy);
        } else {
            return TFHEBool::tfhe_true;
        }
    }

    TFHEBool TFHEEnvelope::contains(const TFHEEnvelope &other) const {
        return covers(other);
    }

    TFHEBool TFHEEnvelope::contains(const TFHECoordinate &p) const {
        return covers(p);
    }

    TFHEBool TFHEEnvelope::contains(const TFHEInt32 &x, const TFHEInt32 &y) const {
        return covers(x, y);
    }

    TFHEBool TFHEEnvelope::covers(const TFHEEnvelope &other) const {
        if (!this->isNull() && !other.isNull()) {
            return (minx <= other.minx && maxx >= other.maxx && miny <= other.miny && maxy >= other.maxy);
        } else {
            return TFHEBool::tfhe_false;
        }
    }

    TFHEBool TFHEEnvelope::covers(const TFHECoordinate &p) const {
        return covers(p.x, p.y);
    }

    TFHEBool TFHEEnvelope::covers(const TFHEInt32 &x, const TFHEInt32 &y) const {
        if (this->isNull()) {
            return TFHEBool::tfhe_false;
        } else {
            return (x >= minx && x <= maxx && y >= miny && y <= maxy);
        }
    }

    TFHEBool TFHEEnvelope::operator==(const TFHEEnvelope &other) const {
        if (this->isNull() && other.isNull()) {
            return TFHEBool::tfhe_true;
        } else if (!this->isNull() && !other.isNull()) {
            return (minx == other.minx && maxx == other.maxx && miny == other.miny && maxy == other.maxy);
        } else {
            return TFHEBool::tfhe_false;
        }
    }

    void TFHEEnvelope::expandToInclude(TFHEInt32 x, TFHEInt32 y) {
        if (isNull()) {
            minx = x;
            maxx = x;
            miny = y;
            maxy = y;
        } else {
            minx = TFHEInt32::min(minx, x);
            maxx = TFHEInt32::max(maxx, x);
            miny = TFHEInt32::min(miny, y);
            maxy = TFHEInt32::max(maxy, y);
        }
    }
    void TFHEEnvelope::expandToInclude(const TFHECoordinate &p) {
        expandToInclude(p.x, p.y);
    }
    void TFHEEnvelope::expandToInclude(const TFHEEnvelope &other) {
        if (isNull()) {
            minx = other.minx;
            maxx = other.maxx;
            miny = other.miny;
            maxy = other.maxy;
        } else {
            minx = TFHEInt32::min(minx, other.minx);
            maxx = TFHEInt32::max(maxx, other.maxx);
            miny = TFHEInt32::min(miny, other.miny);
            maxy = TFHEInt32::max(maxy, other.maxy);
        }
    }

} // SpatialFHE