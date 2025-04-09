//
// Created by ubuntu on 1/7/25.
//

#include "TFHEEnvelope.h"

#include <cassert>

namespace SpatialFHE::geom {
    void TFHEEnvelope::init(const TFHEDecimal &x1, const TFHEDecimal &x2, const TFHEDecimal &y1, const TFHEDecimal &y2) {
        minx = TFHEDecimal::min(x1, x2);
        maxx = TFHEDecimal::max(x1, x2);
        miny = TFHEDecimal::min(y1, y2);
        maxy = TFHEDecimal::max(y1, y2);
    }

    TFHEEnvelope::TFHEEnvelope() {
        minx = TFHEDecimal();
        maxx = TFHEDecimal();
        miny = TFHEDecimal();
        maxy = TFHEDecimal();
    }

    TFHEEnvelope::TFHEEnvelope(const TFHEDecimal &x1, const TFHEDecimal &x2, const TFHEDecimal &y1, const TFHEDecimal &y2) {
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

    TFHEDecimal TFHEEnvelope::getWidth() const {
        if (isNull()) {
            return {};
        }
        return maxx - minx;
    }

    TFHEDecimal TFHEEnvelope::getHeight() const {
        if (isNull()) {
            return {};
        }
        return maxy - miny;
    }

    TFHEDecimal TFHEEnvelope::getMinX() const {
        assert(!isNull());
        return minx;
    }

    TFHEDecimal TFHEEnvelope::getMinY() const {
        assert(!isNull());
        return miny;
    }

    TFHEDecimal TFHEEnvelope::getMaxX() const {
        assert(!isNull());
        return maxx;
    }

    TFHEDecimal TFHEEnvelope::getMaxY() const {
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
            TFHEDecimal x1 = TFHEDecimal::max(minx, other.minx);
            TFHEDecimal x2 = TFHEDecimal::min(maxx, other.maxx);
            TFHEDecimal y1 = TFHEDecimal::max(miny, other.miny);
            TFHEDecimal y2 = TFHEDecimal::min(maxy, other.maxy);
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

    TFHEBool TFHEEnvelope::intersects(const TFHEEnvelope *other) const {
        return intersects(*other);
    }

    TFHEBool TFHEEnvelope::intersects(const TFHEDecimal &x, const TFHEDecimal &y) const {
        if (this->isNull()) {
            return TFHEBool::tfhe_false;
        } else {
            return (x >= minx && x <= maxx && y >= miny && y <= maxy);
        }
    }

    TFHEBool TFHEEnvelope::intersects(const TFHECoordinate &p1, const TFHECoordinate &p2, const TFHECoordinate &q) {
        TFHEEnvelope env = TFHEEnvelope(p1, p2);
        return env.intersects(q);
    }

    TFHEBool TFHEEnvelope::disjoint(const TFHEEnvelope &other) const {
        if (!this->isNull() && !other.isNull()) {
            return (maxx < other.minx || minx > other.maxx || maxy < other.miny || miny > other.maxy);
        } else {
            return TFHEBool::tfhe_true;
        }
    }

    TFHEBool TFHEEnvelope::disjoint(const TFHEEnvelope *other) const {
        return disjoint(*other);
    }

    TFHEBool TFHEEnvelope::contains(const TFHEEnvelope &other) const {
        return covers(other);
    }

    TFHEBool TFHEEnvelope::contains(const TFHECoordinate &p) const {
        return covers(p);
    }

    TFHEBool TFHEEnvelope::contains(const TFHEDecimal &x, const TFHEDecimal &y) const {
        return covers(x, y);
    }

    TFHEBool TFHEEnvelope::contains(const TFHEEnvelope *other) const {
        return covers(other);
    }

    TFHEBool TFHEEnvelope::covers(const TFHEEnvelope *other) const {
        return covers(*other);
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

    TFHEBool TFHEEnvelope::covers(const TFHEDecimal &x, const TFHEDecimal &y) const {
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

    void TFHEEnvelope::expandToInclude(TFHEDecimal x, TFHEDecimal y) {
        if (isNull()) {
            minx = x;
            maxx = x;
            miny = y;
            maxy = y;
        } else {
            minx = TFHEDecimal::min(minx, x);
            maxx = TFHEDecimal::max(maxx, x);
            miny = TFHEDecimal::min(miny, y);
            maxy = TFHEDecimal::max(maxy, y);
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
            minx = TFHEDecimal::min(minx, other.minx);
            maxx = TFHEDecimal::max(maxx, other.maxx);
            miny = TFHEDecimal::min(miny, other.miny);
            maxy = TFHEDecimal::max(maxy, other.maxy);
        }
    }

    TFHEBool TFHEEnvelope::intersects(
        const TFHECoordinate &p1,
        const TFHECoordinate &p2,
        const TFHECoordinate &q1,
        const TFHECoordinate &q2) {
        TFHEDecimal minqx = TFHEDecimal::min(q1.x, q2.x);
        TFHEDecimal maxqx = TFHEDecimal::max(q1.x, q2.x);
        TFHEDecimal minpx = TFHEDecimal::min(p1.x, p2.x);
        TFHEDecimal maxpx = TFHEDecimal::max(p1.x, p2.x);
        TFHEDecimal minqy = TFHEDecimal::min(q1.y, q2.y);
        TFHEDecimal maxqy = TFHEDecimal::max(q1.y, q2.y);
        TFHEDecimal minpy = TFHEDecimal::min(p1.y, p2.y);
        TFHEDecimal maxpy = TFHEDecimal::max(p1.y, p2.y);
        return (maxqx >= minpx && minqx <= maxpx && maxqy >= minpy && minqy <= maxpy);
    }

}  // namespace SpatialFHE::geom