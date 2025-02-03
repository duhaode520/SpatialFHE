//
// Created by ubuntu on 1/12/25.
//

#include "TFHECoordinateSequence.h"

#include <memory>

#include "TFHEEnvelope.h"

namespace SpatialFHE::geom {
    TFHECoordinateSequence::TFHECoordinateSequence(std::size_t size) : vect(size) {}

    TFHECoordinateSequence::TFHECoordinateSequence(std::vector<TFHECoordinate> &&coordinates) :
            vect(std::move(coordinates)) {}

    TFHECoordinateSequence::TFHECoordinateSequence(std::vector<TFHECoordinate> *coordinates) {
        std::unique_ptr<std::vector<TFHECoordinate>> coordp(coordinates);
        if (coordp) {
            vect = std::move(*coordp);
        }
    }

    TFHEEnvelope TFHECoordinateSequence::getEnvelope() const {
        TFHEEnvelope e;
        expandEnvelope(e);
        return e;
    }

    void TFHECoordinateSequence::expandEnvelope(TFHEEnvelope &env) const {
        const std::size_t p_size = size();
        for (std::size_t i = 0; i < p_size; i++) {
            env.expandToInclude(getAt(i));
        }
    }

    std::size_t TFHECoordinateSequence::size() const {
        return vect.size();
    }

    TFHEBool TFHECoordinateSequence::isRing() const {
        if (size() < 4) {
            return TFHEBool::tfhe_false;
        }
        return getAt(0).equals(getAt(size() - 1));
    }

    bool TFHECoordinateSequence::isEmpty() const {
        return vect.empty();
    }

    const TFHECoordinate &TFHECoordinateSequence::getAt(std::size_t i) const {
        return vect[i];
    }

    TFHECoordinate &TFHECoordinateSequence::getAt(std::size_t i) {
        return vect[i];
    }

    void TFHECoordinateSequence::getAt(std::size_t i, TFHECoordinate &c) const {
        c = vect[i];
    }

    void TFHECoordinateSequence::toVector(std::vector<TFHECoordinate> &coords) const {
        coords.insert(coords.begin(), vect.begin(), vect.end());
    }

    void TFHECoordinateSequence::setAt(const TFHECoordinate &c, std::size_t pos) {
        vect[pos] = c;
    }

    TFHEBool TFHECoordinateSequence::hasRepeatedPoints() const {
        const std::size_t p_size = size();
        TFHEBool result = getAt(0).equals(getAt(1));
        for (std::size_t i = 2; i < p_size; i++) {
            result = result && (getAt(i) == getAt(i - 1));
        }
        return result;
    }

    TFHEBool TFHECoordinateSequence::equals(const TFHECoordinateSequence *s1, const TFHECoordinateSequence *s2) {
        if (s1 == s2) {
            return TFHEBool::tfhe_true;
        }
        if (s1 == nullptr || s2 == nullptr) {
            return TFHEBool::tfhe_false;
        }
        std::size_t s1_size = s1->size();
        if (s1_size != s2->size()) {
            return TFHEBool::tfhe_false;
        }
        TFHEBool result = TFHEBool::tfhe_true;
        for (std::size_t i = 0; i < s1_size; i++) {
            result = result && (s1->getAt(i) == s2->getAt(i));
        }
        return result;
    }

    TFHEBool TFHECoordinateSequence::operator==(const TFHECoordinateSequence &other) const {
        return equals(this, &other);
    }

    TFHEBool TFHECoordinateSequence::operator!=(const TFHECoordinateSequence &other) const {
        return !equals(this, &other);
    }

    void TFHECoordinateSequence::add(const TFHECoordinate &c) {
        vect.push_back(c);
    }

    void TFHECoordinateSequence::add(const TFHECoordinateSequence &cs) {
        const std::size_t cs_size = cs.size();
        for (std::size_t i = 0; i < cs_size; i++) {
            add(cs.getAt(i));
        }
    }

    void TFHECoordinateSequence::insertAt(const TFHECoordinate &c, std::size_t pos) {
        vect.insert(std::next(vect.begin(), static_cast<std::ptrdiff_t>(pos)), c);
    }

    void TFHECoordinateSequence::closeRing() {
        if (!isEmpty() && !isRing().decrypt()) {
            add(getAt(0));
        }
    }

    TFHECoordinateSequence::Ptr TFHECoordinateSequence::clone() const {
        return std::make_unique<TFHECoordinateSequence>(*this);
    }

    void TFHECoordinateSequence::reverse() {
        auto last = size() - 1;
        auto mid = size() / 2;
        for (size_t i = 0; i < mid; i++) {
            std::swap(vect[i], vect[last - i]);
        }
    }
}  // namespace SpatialFHE::geom

// SpatialFHE