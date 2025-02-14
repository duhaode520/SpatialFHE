//
// Created by ubuntu on 1/7/25.
//

#ifndef TFHEENVELOPE_H
#define TFHEENVELOPE_H
#include <TFHEInt32.h>

#include "TFHECoordinate.h"

namespace SpatialFHE::geom {

    class TFHEEnvelope {
    private:
        TFHEInt32 minx, miny, maxx, maxy;
        void init(const TFHEInt32 &x1, const TFHEInt32 &x2, const TFHEInt32 &y1, const TFHEInt32 &y2);

    public:
        TFHEEnvelope();
        TFHEEnvelope(const TFHEInt32 &x1, const TFHEInt32 &x2, const TFHEInt32 &y1, const TFHEInt32 &y2);
        TFHEEnvelope(const TFHECoordinate &p1, const TFHECoordinate &p2);
        explicit TFHEEnvelope(const TFHECoordinate &p);
        TFHEEnvelope(const TFHEEnvelope &other);
        TFHEEnvelope(TFHEEnvelope &&other) noexcept;
        TFHEEnvelope &operator=(const TFHEEnvelope &other);
        TFHEEnvelope &operator=(TFHEEnvelope &&other) noexcept;
        ~TFHEEnvelope() = default;

        bool isNull() const;
        [[nodiscard]] TFHEInt32 getWidth() const;
        [[nodiscard]] TFHEInt32 getHeight() const;
        [[nodiscard]] TFHEInt32 getMinX() const;
        [[nodiscard]] TFHEInt32 getMinY() const;
        [[nodiscard]] TFHEInt32 getMaxX() const;
        [[nodiscard]] TFHEInt32 getMaxY() const;
        [[nodiscard]] TFHECoordinate getCenter() const;

        [[nodiscard]] TFHEEnvelope intersection(const TFHEEnvelope &other) const;

        [[nodiscard]] TFHEBool intersects(const TFHEEnvelope &other) const;
        [[nodiscard]] TFHEBool intersects(const TFHECoordinate &p) const;
        [[nodiscard]] TFHEBool intersects(const TFHEEnvelope *other) const;
        [[nodiscard]] TFHEBool intersects(const TFHEInt32 &x, const TFHEInt32 &y) const;
        [[nodiscard]] TFHEBool disjoint(const TFHEEnvelope &other) const;
        [[nodiscard]] TFHEBool disjoint(const TFHEEnvelope *other) const;
        [[nodiscard]] TFHEBool contains(const TFHEEnvelope &other) const;
        [[nodiscard]] TFHEBool contains(const TFHECoordinate &p) const;
        [[nodiscard]] TFHEBool contains(const TFHEInt32 &x, const TFHEInt32 &y) const;
        TFHEBool contains(const TFHEEnvelope *other) const;
        [[nodiscard]] TFHEBool covers(const TFHEEnvelope &other) const;
        [[nodiscard]] TFHEBool covers(const TFHECoordinate &p) const;
        [[nodiscard]] TFHEBool covers(const TFHEInt32 &x, const TFHEInt32 &y) const;
        TFHEBool covers(const TFHEEnvelope *other) const;

        TFHEBool operator==(const TFHEEnvelope &other) const;

        void expandToInclude(TFHEInt32 x, TFHEInt32 y);
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
