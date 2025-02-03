#ifndef TFHECOORDINATESEQUENCE_H
#define TFHECOORDINATESEQUENCE_H
#include <memory>
#include <vector>

#include "TFHECoordinate.h"

namespace SpatialFHE::geom {
    class TFHEEnvelope;
}

namespace SpatialFHE::geom {

    class TFHECoordinateSequence {
    private:
        std::vector<TFHECoordinate> vect;

    public:
        using Ptr = std::unique_ptr<TFHECoordinateSequence>;

        TFHECoordinateSequence() = default;
        explicit TFHECoordinateSequence(std::size_t size);
        explicit TFHECoordinateSequence(std::vector<TFHECoordinate>&& coordinates);
        explicit TFHECoordinateSequence(std::vector<TFHECoordinate>* coordinates);
        TFHECoordinateSequence(const TFHECoordinateSequence& other) = default;

        ~TFHECoordinateSequence() = default;

        [[nodiscard]] TFHEEnvelope getEnvelope() const;
        void expandEnvelope(TFHEEnvelope& env) const;

        [[nodiscard]] std::size_t size() const;

        [[nodiscard]] TFHEBool isRing() const;
        [[nodiscard]] bool isEmpty() const;

        [[nodiscard]] const TFHECoordinate& getAt(std::size_t i) const;

        TFHECoordinate& getAt(std::size_t i);

        // Copy Coordinate at i to Coordinate c
        void getAt(std::size_t i, TFHECoordinate& c) const;

        [[nodiscard]] const TFHECoordinate& back() const {
            return getAt(size() - 1);
        }

        [[nodiscard]] const TFHECoordinate& front() const {
            return getAt(0);
        }

        const TFHECoordinate& operator[](std::size_t i) const {
            return getAt(i);
        }

        TFHECoordinate& operator[](std::size_t i) {
            return getAt(i);
        }

        void toVector(std::vector<TFHECoordinate>& coords) const;

        void setAt(const TFHECoordinate& c, std::size_t pos);

        [[nodiscard]] TFHEBool hasRepeatedPoints() const;

        static TFHEBool equals(const TFHECoordinateSequence* s1, const TFHECoordinateSequence* s2);
        TFHEBool operator==(const TFHECoordinateSequence& other) const;
        TFHEBool operator!=(const TFHECoordinateSequence& other) const;

        void add(const TFHECoordinate& c);
        void add(const TFHECoordinateSequence& cs);
        void insertAt(const TFHECoordinate& c, std::size_t pos);
        void closeRing();

        Ptr clone() const;

        // Reverse the coordinates order in the sequence
        void reverse();
    };

}  // namespace SpatialFHE::geom

// SpatialFHE

#endif  // TFHECOORDINATESEQUENCE_H
