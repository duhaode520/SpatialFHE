//
// Created by ubuntu on 1/12/25.
//

#include "TFHEPolygon.h"

#include <cassert>
#include <stdexcept>

#include "TFHEGeometryFactory.h"

namespace SpatialFHE::geom {
    std::unique_ptr<TFHEEnvelope> TFHEPolygon::computeEnvelopeInternal() const {
        return std::make_unique<TFHEEnvelope>(*shell->getEnvelope());
    }

    TFHEPolygon *TFHEPolygon::cloneInternal() const {
        return new TFHEPolygon(*this);
    }

    TFHEPolygon::TFHEPolygon(const TFHEPolygon &other) :
            TFHEGeometry(other), shell(std::make_unique<TFHELinearRing>(*other.shell)), holes(other.holes.size()) {
        for (std::size_t i = 0; i < other.holes.size(); i++) {
            holes[i] = std::make_shared<TFHELinearRing>(*other.holes[i]);
        }
    }

    TFHEPolygon::TFHEPolygon(
        TFHELinearRing *p_shell,
        std::vector<TFHELinearRing *> *p_holes,
        const TFHEGeometryFactory *p_factory) :
            TFHEGeometry(p_factory) {
        if (p_shell == nullptr) {
            this->shell = getFactory()->createLinearRing();
        } else {
            if (p_holes != nullptr && p_shell->isEmpty() && hasNonEmptyElements(p_holes)) {
                throw std::invalid_argument("shell is empty and holes is empty");
            }
            this->shell.reset(p_shell);
        }

        if (p_holes != nullptr) {
            if (hasNullElements(p_holes)) {
                throw std::invalid_argument("holes contains null elements");
            }
            for (const auto &hole : *p_holes) {
                this->holes.emplace_back(hole);
            }
            delete p_holes;
        }
    }

    TFHEPolygon::TFHEPolygon(std::unique_ptr<TFHELinearRing> p_shell, const TFHEGeometryFactory &p_factory) :
            TFHEGeometry(&p_factory), shell(std::move(p_shell)) {
        if (this->shell == nullptr) {
            this->shell = getFactory()->createLinearRing();
        }
    }

    TFHEPolygon::TFHEPolygon(
        std::unique_ptr<TFHELinearRing> p_shell,
        std::vector<std::unique_ptr<TFHELinearRing>> p_holes,
        const TFHEGeometryFactory &p_factory) :
            TFHEGeometry(&p_factory), shell(std::move(p_shell)){
        if (this->shell == nullptr) {
            this->shell = getFactory()->createLinearRing();
        }
        for (auto &hole : p_holes) {
            holes.emplace_back(std::move(hole));
        }
        if (this->shell->isEmpty() && hasNonEmptyElements(&this->holes)) {
            throw std::invalid_argument("shell is empty and holes is empty");
        }
        if (hasNullElements(&this->holes)) {
            throw std::invalid_argument("holes contains null elements");
        }
    }

    bool TFHEPolygon::isRectangle() const {
        if (getNumInteriorRing() != 0) {
            return false;
        }
        assert(shell != nullptr);
        if (shell->getNumPoints() != 5) {
            return false;
        }

        const TFHECoordinateSequence &cs = *shell->getCoordinatesRO();

        const TFHEEnvelope &env = *getEnvelope();

        // check vertices coordinates
        TFHEBool result = TFHEBool::tfhe_true;
        for (int i = 0; i < 5; i++) {
            const TFHECoordinate &c = cs.getAt(i);
            result = result && (c.x == env.getMinX() || c.x == env.getMaxX());
            result = result && (c.y == env.getMinY() || c.y == env.getMaxY());
        }

        // check vertices order
        TFHEInt32 prevX = cs.getAt(0).x;
        TFHEInt32 prevY = cs.getAt(0).y;
        for (int i = 1; i < 5; i++) {
            TFHEInt32 x = cs.getAt(i).x;
            TFHEInt32 y = cs.getAt(i).y;
            result = result && ((x != prevX) != (y != prevY));
            prevX = x;
            prevY = y;
        }

        // must decrypt the result here as isRectangle is used for branching
        return result.decrypt();
    }

    const TFHELinearRing *TFHEPolygon::getExteriorRing() const {
        return shell.get();
    }

    std::size_t TFHEPolygon::getNumInteriorRing() const {
        return holes.size();
    }

    const TFHELinearRing *TFHEPolygon::getInteriorRingN(std::size_t n) const {
        return holes[n].get();
    }

    bool TFHEPolygon::isEmpty() const {
        return shell->isEmpty();
    }

    const TFHECoordinate *TFHEPolygon::getCoordinate() const {
        return shell->getCoordinate();
    }

    std::unique_ptr<TFHECoordinateSequence> TFHEPolygon::getCoordinates() const {
        if (isEmpty()) {
            return getFactory()->getCoordinateSequenceFactory()->create();
        }

        std::vector<TFHECoordinate> cl;
        cl.reserve(getNumPoints());

        // Add shell points
        const TFHECoordinateSequence *shellCoords = shell->getCoordinatesRO();
        shellCoords->toVector(cl);

        // Add hole points
        for (const auto &hole : holes) {
            const TFHECoordinateSequence *holeCoords = hole->getCoordinatesRO();
            holeCoords->toVector(cl);
        }

        return getFactory()->getCoordinateSequenceFactory()->create(std::move(cl));
    }

    std::size_t TFHEPolygon::getNumPoints() const {
        std::size_t numPoints = shell->getNumPoints();
        for (const auto &hole : holes) {
            numPoints += hole->getNumPoints();
        }
        return numPoints;
    }

    std::string TFHEPolygon::getGeometryType() const {
        return "Polygon";
    }

    TFHEGeometryTypeId TFHEPolygon::getGeometryTypeId() const {
        return TFHEGeometryTypeId::TFHE_POLYGON;
    }

    Dimension::DimensionType TFHEPolygon::getDimension() const {
        return Dimension::A;
    }

    std::unique_ptr<TFHEGeometry> TFHEPolygon::getBoundary() const {
        const TFHEGeometryFactory *factory = getFactory();
        if (isEmpty()) {
            return factory->createLineString();
        }
        // FIXME: fix this when implement multi line string
        return factory->createLineString(*shell);
    }

    std::unique_ptr<TFHEPolygon> TFHEPolygon::clone() const {
        return std::unique_ptr<TFHEPolygon>(cloneInternal());
    }

}  // namespace SpatialFHE::geom
