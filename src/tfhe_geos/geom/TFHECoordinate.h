#ifndef TFHECOORDINATE_H
#define TFHECOORDINATE_H
#include "TFHEInt32.h"

namespace SpatialFHE::geom {
    class TFHECoordinate {
    public:
        TFHECoordinate() = default;
        TFHECoordinate(TFHEInt32 x, TFHEInt32 y);
        TFHECoordinate(const TFHECoordinate &other);
        TFHECoordinate(TFHECoordinate &&other) noexcept;
        TFHECoordinate &operator=(const TFHECoordinate &other);
        TFHECoordinate &operator=(TFHECoordinate &&other) noexcept;
        ~TFHECoordinate() = default;

        TFHEInt32 x;
        TFHEInt32 y;

        TFHEBool operator==(const TFHECoordinate &other) const;
        TFHEBool operator!=(const TFHECoordinate &other) const;

        [[nodiscard]] bool isNull() const;
    };
}  // namespace SpatialFHE::geom

#endif  // TFHECOORDINATE_H
