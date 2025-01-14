//
// Created by ubuntu on 1/13/25.
//

#include "TFHECoordinateSequenceFactory.h"

#include <memory>
#include "TFHECoordinateSequence.h"


namespace SpatialFHE::geom {
    static TFHECoordinateSequenceFactory defaultInstance;

    std::unique_ptr<TFHECoordinateSequence> TFHECoordinateSequenceFactory::create() const {
        return std::make_unique<TFHECoordinateSequence>();
    }

    std::unique_ptr<TFHECoordinateSequence> TFHECoordinateSequenceFactory::create(
        std::vector<TFHECoordinate> *coordinates) const {
        return std::make_unique<TFHECoordinateSequence>(coordinates);
    }

    std::unique_ptr<TFHECoordinateSequence> TFHECoordinateSequenceFactory::create(
        std::vector<TFHECoordinate> &&coordinates) const {
        return std::make_unique<TFHECoordinateSequence>(std::move(coordinates));
    }

    std::unique_ptr<TFHECoordinateSequence> TFHECoordinateSequenceFactory::create(std::size_t size) const {
        return std::make_unique<TFHECoordinateSequence>(size);
    }

    std::unique_ptr<TFHECoordinateSequence> TFHECoordinateSequenceFactory::create(
        const TFHECoordinateSequence &cs) const {
        return std::make_unique<TFHECoordinateSequence>(cs);
    }

    const TFHECoordinateSequenceFactory *TFHECoordinateSequenceFactory::instance() {
        return &defaultInstance;
    }

} // geom
// SpatialFHE