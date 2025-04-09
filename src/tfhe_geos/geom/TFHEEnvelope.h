//
// Created by ubuntu on 1/7/25.
//

#ifndef TFHEENVELOPE_H
#define TFHEENVELOPE_H
#include <TFHEInt32.h>
#include <TFHEDecimal.h>

#include "TFHECoordinate.h"

namespace SpatialFHE::geom {

    class TFHEEnvelope {
    private:
        TFHEDecimal minx, miny, maxx, maxy;
        void init(const TFHEDecimal &x1, const TFHEDecimal &x2, const TFHEDecimal &y1, const TFHEDecimal &y2);

    public:
        TFHEEnvelope();
        TFHEEnvelope(const TFHEDecimal &x1, const TFHEDecimal &x2, const TFHEDecimal &y1, const TFHEDecimal &y2);
        TFHEEnvelope(const TFHECoordinate &p1, const TFHECoordinate &p2);
        explicit TFHEEnvelope(const TFHECoordinate &p);
        TFHEEnvelope(const TFHEEnvelope &other);
        TFHEEnvelope(TFHEEnvelope &&other) noexcept;
        TFHEEnvelope &operator=(const TFHEEnvelope &other);
        TFHEEnvelope &operator=(TFHEEnvelope &&other) noexcept;
        ~TFHEEnvelope() = default;

        bool isNull() const;
        [[nodiscard]] TFHEDecimal getWidth() const;
        [[nodiscard]] TFHEDecimal getHeight() const;
        [[nodiscard]] TFHEDecimal getMinX() const;
        [[nodiscard]] TFHEDecimal getMinY() const;
        [[nodiscard]] TFHEDecimal getMaxX() const;
        [[nodiscard]] TFHEDecimal getMaxY() const;
        [[nodiscard]] TFHECoordinate getCenter() const;

        [[nodiscard]] TFHEEnvelope intersection(const TFHEEnvelope &other) const;

        [[nodiscard]] TFHEBool intersects(const TFHEEnvelope &other) const;
        [[nodiscard]] TFHEBool intersects(const TFHECoordinate &p) const;
        [[nodiscard]] TFHEBool intersects(const TFHEEnvelope *other) const;
        [[nodiscard]] TFHEBool intersects(const TFHEDecimal &x, const TFHEDecimal &y) const;
        [[nodiscard]] TFHEBool disjoint(const TFHEEnvelope &other) const;
        [[nodiscard]] TFHEBool disjoint(const TFHEEnvelope *other) const;
        [[nodiscard]] TFHEBool contains(const TFHEEnvelope &other) const;
        [[nodiscard]] TFHEBool contains(const TFHECoordinate &p) const;
        [[nodiscard]] TFHEBool contains(const TFHEDecimal &x, const TFHEDecimal &y) const;
        TFHEBool contains(const TFHEEnvelope *other) const;
        [[nodiscard]] TFHEBool covers(const TFHEEnvelope &other) const;
        [[nodiscard]] TFHEBool covers(const TFHECoordinate &p) const;
        [[nodiscard]] TFHEBool covers(const TFHEDecimal &x, const TFHEDecimal &y) const;
        TFHEBool covers(const TFHEEnvelope *other) const;

        TFHEBool operator==(const TFHEEnvelope &other) const;

        void expandToInclude(TFHEDecimal x, TFHEDecimal y);
        void expandToInclude(const TFHECoordinate &p);
        void expandToInclude(const TFHEEnvelope &other);

        static TFHEBool intersects(
            const TFHECoordinate &p1,
            const TFHECoordinate &p2,
            const TFHECoordinate &q1,
            const TFHECoordinate &q2);
        static TFHEBool intersects(const TFHECoordinate &p1, const TFHECoordinate &p2, const TFHECoordinate& q);
    };

}  // namespace SpatialFHE::geom

#endif  // TFHEENVELOPE_H
