#ifndef TFHECOORDINATE_H
#define TFHECOORDINATE_H
#include <map>
#include <set>

#include "TFHEDecimal.h"

namespace SpatialFHE::geom {
    struct TFHECoordinateLessThan;

    class TFHECoordinate {
    public:
        typedef std::set<const TFHECoordinate*, TFHECoordinateLessThan> ConstSet;
        typedef std::map<const TFHECoordinate*, int, TFHECoordinateLessThan> ConstIntMap;
        typedef std::set<TFHECoordinate, TFHECoordinateLessThan> Set;

        TFHECoordinate() = default;
        TFHECoordinate(TFHEDecimal x, TFHEDecimal y);
        TFHECoordinate(const TFHECoordinate &other);
        TFHECoordinate(TFHECoordinate &&other) noexcept;
        TFHECoordinate &operator=(const TFHECoordinate &other);
        TFHECoordinate &operator=(TFHECoordinate &&other) noexcept;
        ~TFHECoordinate() = default;

        TFHEDecimal x;
        TFHEDecimal y;

        [[nodiscard]] TFHEBool equals(const TFHECoordinate &other) const;
        [[nodiscard]] TFHEDecimal distanceSquared(const TFHECoordinate& p) const;

        bool operator==(const TFHECoordinate &other) const;

        [[nodiscard]] bool isNull() const;

        // This comparison might be expensive, as it requires decryption
        [[nodiscard]] TFHEInt32 compareTo(const TFHECoordinate& other) const;

        [[nodiscard]] TFHEDecimal GetX() const;

        [[nodiscard]] TFHEDecimal GetY() const;
    };
    struct TFHECoordinateLessThan {
        bool operator()(const TFHECoordinate &lhs, const TFHECoordinate &rhs) const {
            return compare(&lhs, &rhs);
        }

        bool operator()(const TFHECoordinate *lhs, const TFHECoordinate *rhs) const {
            return compare(lhs, rhs);
        }

        static bool compare(const TFHECoordinate *lhs, const TFHECoordinate *rhs) {
            TFHEBool encrypted = lhs->x < rhs->x || (lhs->x == rhs->x && lhs->y < rhs->y);
            // TODO: add cache to avoid re-encryption
            return encrypted.decrypt();
        }

    };

    inline bool operator<(const TFHECoordinate &lhs, const TFHECoordinate &rhs) {
        return TFHECoordinateLessThan()(lhs, rhs);
    }
}  // namespace SpatialFHE::geom

#endif  // TFHECOORDINATE_H
