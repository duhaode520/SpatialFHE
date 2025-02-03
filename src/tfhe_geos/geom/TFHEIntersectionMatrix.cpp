//
// Created by ubuntu on 1/8/25.
//

#include "TFHEIntersectionMatrix.h"

#include <sstream>

namespace SpatialFHE::geom {
    const int TFHEIntersectionMatrix::firstDim = 3;
    const int TFHEIntersectionMatrix::secondDim = 3;

    TFHEIntersectionMatrix::TFHEIntersectionMatrix() {
        setAll(Dimension::False);
    }

    TFHEIntersectionMatrix::TFHEIntersectionMatrix(const std::string &elements) {
        setAll(Dimension::False);
        set(elements);
    }

    bool TFHEIntersectionMatrix::matches(const std::string &requiredDimensionSymbols) const {
        if (requiredDimensionSymbols.length() != 9) {
            std::ostringstream s;
            s << "IllegalArgumentException: Should be length 9, is "
              << "[" << requiredDimensionSymbols << "] instead" << std::endl;
            throw std::invalid_argument(s.str());
        }
        for (std::size_t ai = 0; ai < firstDim; ai++) {
            for (std::size_t bi = 0; bi < secondDim; bi++) {
                if (!matches(matrix[ai][bi], requiredDimensionSymbols[3 * ai + bi])) {
                    return false;
                }
            }
        }
        return true;
    }

    bool TFHEIntersectionMatrix::matches(int actualDimensionValue, char requiredDimensionSymbol) {
        if (requiredDimensionSymbol == '*') {
            return true;
        }

        if (requiredDimensionSymbol == 'T' && (actualDimensionValue >= 0 || actualDimensionValue == Dimension::True)) {
            return true;
        }

        if (requiredDimensionSymbol == 'F' && actualDimensionValue == Dimension::False) {
            return true;
        }

        if (requiredDimensionSymbol == '0' && actualDimensionValue == Dimension::P) {
            return true;
        }

        if (requiredDimensionSymbol == '1' && actualDimensionValue == Dimension::L) {
            return true;
        }

        if (requiredDimensionSymbol == '2' && actualDimensionValue == Dimension::A) {
            return true;
        }

        return false;
    }

    bool TFHEIntersectionMatrix::matches(
        const std::string &actualDimensionSymbols,
        const std::string &requiredDimensionSymbols) {
        TFHEIntersectionMatrix m(actualDimensionSymbols);
        bool result = m.matches(requiredDimensionSymbols);

        return result;
    }

    void TFHEIntersectionMatrix::set(Location row, Location col, int dimension) {
        matrix[static_cast<size_t>(row)][static_cast<size_t>(col)] = dimension;
    }

    void TFHEIntersectionMatrix::set(const std::string &dimensionSymbols) {
        auto limit = std::min(dimensionSymbols.length(), static_cast<std::size_t>(9));

        for (std::size_t i = 0; i < limit; i++) {
            auto row = i / firstDim;
            auto col = i % secondDim;
            matrix[row][col] = Dimension::toDimensionValue(dimensionSymbols[i]);
        }
    }

    void TFHEIntersectionMatrix::setAll(int dimensionValue) {
        for (int ai = 0; ai < firstDim; ai++) {
            for (int bi = 0; bi < secondDim; bi++) {
                set(static_cast<Location>(ai), static_cast<Location>(bi), dimensionValue);
            }
        }
    }

    bool TFHEIntersectionMatrix::isDisjoint() const {
        return get(Location::INTERIOR, Location::INTERIOR) == Dimension::False &&
            get(Location::INTERIOR, Location::BOUNDARY) == Dimension::False &&
            get(Location::BOUNDARY, Location::INTERIOR) == Dimension::False &&
            get(Location::BOUNDARY, Location::BOUNDARY) == Dimension::False;
    }

    bool TFHEIntersectionMatrix::isIntersects() const {
        return !isDisjoint();
    }

    bool TFHEIntersectionMatrix::isTouches(int dimensionOfGeometryA, int dimensionOfGeometryB) const {
        if (dimensionOfGeometryA > dimensionOfGeometryB) {
            // no need to get transpose because pattern matrix is symmetrical
            return isTouches(dimensionOfGeometryB, dimensionOfGeometryA);
        }
        if ((dimensionOfGeometryA == Dimension::A && dimensionOfGeometryB == Dimension::A) ||
            (dimensionOfGeometryA == Dimension::L && dimensionOfGeometryB == Dimension::L) ||
            (dimensionOfGeometryA == Dimension::L && dimensionOfGeometryB == Dimension::A) ||
            (dimensionOfGeometryA == Dimension::P && dimensionOfGeometryB == Dimension::A) ||
            (dimensionOfGeometryA == Dimension::P && dimensionOfGeometryB == Dimension::L)) {
            return get(Location::INTERIOR, Location::INTERIOR) == Dimension::False &&
                (matches(get(Location::INTERIOR, Location::BOUNDARY), 'T') ||
                 matches(get(Location::BOUNDARY, Location::INTERIOR), 'T') ||
                 matches(get(Location::BOUNDARY, Location::BOUNDARY), 'T'));
        }
        return false;
    }

    bool TFHEIntersectionMatrix::isCrosses(int dimensionOfGeometryA, int dimensionOfGeometryB) const {
        if ((dimensionOfGeometryA == Dimension::P && dimensionOfGeometryB == Dimension::L) ||
            (dimensionOfGeometryA == Dimension::P && dimensionOfGeometryB == Dimension::A) ||
            (dimensionOfGeometryA == Dimension::L && dimensionOfGeometryB == Dimension::A)) {
            return matches(get(Location::INTERIOR, Location::INTERIOR), 'T') &&
                matches(get(Location::INTERIOR, Location::EXTERIOR), 'T');
        }
        if ((dimensionOfGeometryA == Dimension::L && dimensionOfGeometryB == Dimension::P) ||
            (dimensionOfGeometryA == Dimension::A && dimensionOfGeometryB == Dimension::P) ||
            (dimensionOfGeometryA == Dimension::A && dimensionOfGeometryB == Dimension::L)) {
            return matches(get(Location::INTERIOR, Location::INTERIOR), 'T') &&
                matches(get(Location::EXTERIOR, Location::INTERIOR), 'T');
        }
        if (dimensionOfGeometryA == Dimension::L && dimensionOfGeometryB == Dimension::L) {
            return get(Location::INTERIOR, Location::INTERIOR) == 0;
        }
        return false;
    }

    bool TFHEIntersectionMatrix::isWithin() const {
        return matches(get(Location::INTERIOR, Location::INTERIOR), 'T') &&
            get(Location::INTERIOR, Location::EXTERIOR) == Dimension::False &&
            get(Location::BOUNDARY, Location::EXTERIOR) == Dimension::False;
    }

    bool TFHEIntersectionMatrix::isContains() const {
        return matches(get(Location::INTERIOR, Location::INTERIOR), 'T') &&
            get(Location::EXTERIOR, Location::INTERIOR) == Dimension::False &&
            get(Location::EXTERIOR, Location::BOUNDARY) == Dimension::False;
    }

    bool TFHEIntersectionMatrix::isEquals(int dimensionOfGeometryA, int dimensionOfGeometryB) const {
        if (dimensionOfGeometryA != dimensionOfGeometryB) {
            return false;
        }
        return matches(get(Location::INTERIOR, Location::INTERIOR), 'T') &&
            get(Location::EXTERIOR, Location::INTERIOR) == Dimension::False &&
            get(Location::INTERIOR, Location::EXTERIOR) == Dimension::False &&
            get(Location::EXTERIOR, Location::BOUNDARY) == Dimension::False &&
            get(Location::BOUNDARY, Location::EXTERIOR) == Dimension::False;
    }

    bool TFHEIntersectionMatrix::isOverlaps(int dimensionOfGeometryA, int dimensionOfGeometryB) const {

        if((dimensionOfGeometryA == Dimension::P && dimensionOfGeometryB == Dimension::P) ||
                (dimensionOfGeometryA == Dimension::A && dimensionOfGeometryB == Dimension::A)) {
            return matches(get(Location::INTERIOR, Location::INTERIOR), 'T') &&
                   matches(get(Location::INTERIOR, Location::EXTERIOR), 'T') &&
                   matches(get(Location::EXTERIOR, Location::INTERIOR), 'T');
                }
        if(dimensionOfGeometryA == Dimension::L && dimensionOfGeometryB == Dimension::L) {
            return get(Location::INTERIOR, Location::INTERIOR) == 1 &&
                   matches(get(Location::INTERIOR, Location::EXTERIOR), 'T') &&
                   matches(get(Location::EXTERIOR, Location::INTERIOR), 'T');
        }
        return false;
    }

    bool TFHEIntersectionMatrix::isCovers() const {
        bool hasPointInCommon =
            matches(get(Location::INTERIOR, Location::INTERIOR), 'T')
            ||
            matches(get(Location::INTERIOR, Location::BOUNDARY), 'T')
            ||
            matches(get(Location::BOUNDARY, Location::INTERIOR), 'T')
            ||
            matches(get(Location::BOUNDARY, Location::BOUNDARY), 'T');

        return hasPointInCommon
               &&
               get(Location::EXTERIOR, Location::INTERIOR) ==
               Dimension::False
               &&
               get(Location::EXTERIOR, Location::BOUNDARY) ==
               Dimension::False;
    }

    bool TFHEIntersectionMatrix::isCoveredBy() const {

        bool hasPointInCommon =
            matches(get(Location::INTERIOR, Location::INTERIOR), 'T')
            ||
            matches(get(Location::INTERIOR, Location::BOUNDARY), 'T')
            ||
            matches(get(Location::BOUNDARY, Location::INTERIOR), 'T')
            ||
            matches(get(Location::BOUNDARY, Location::BOUNDARY), 'T');

        return
            hasPointInCommon
            &&
            get(Location::INTERIOR, Location::EXTERIOR) ==
            Dimension::False
            &&
            get(Location::BOUNDARY, Location::EXTERIOR) ==
            Dimension::False;
    }
}  // namespace SpatialFHE::geom
