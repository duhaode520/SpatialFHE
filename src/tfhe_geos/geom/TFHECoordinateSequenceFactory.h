#pragma once

#include <memory>
#include <vector>

// Forward declarations
namespace SpatialFHE::geom {
    class TFHECoordinateSequence;
    class TFHECoordinate;
}


namespace SpatialFHE::geom {

    /**
     * Factory class for creating TFHECoordinateSequence objects.
     * A Static singleton class.
     */
    class TFHECoordinateSequenceFactory {
    public:
        [[nodiscard]] std::unique_ptr<TFHECoordinateSequence> create() const;
        std::unique_ptr<TFHECoordinateSequence> create(std::vector<TFHECoordinate>* coordinates) const;
        std::unique_ptr<TFHECoordinateSequence> create(std::vector<TFHECoordinate>&& coordinates) const;
        [[nodiscard]] std::unique_ptr<TFHECoordinateSequence> create(std::size_t size) const;
        [[nodiscard]] std::unique_ptr<TFHECoordinateSequence> create(const TFHECoordinateSequence& cs) const;

        ~TFHECoordinateSequenceFactory() = default;

        static const TFHECoordinateSequenceFactory* instance();

    };

} // geom
// SpatialFHE

