#pragma once
#include <sstream>
#include <stdexcept>

namespace SpatialFHE::geom {
    enum class Location { NONE = -1, INTERIOR = 0, BOUNDARY = 1, EXTERIOR = 2 };

    class Dimension {
    public:
        enum DimensionType {
            P = 0,
            L = 1,
            A = 2,
            False = -3,
            True = -2,
            DONTCARE = -1,
        };

        static int toDimensionValue(char dimensionSymbol) {
            switch (dimensionSymbol) {
            case 'F':
            case 'f':
                return False;
            case 'T':
            case 't':
                return True;
            case '*':
                return DONTCARE;
            case '0':
                return P;
            case '1':
                return L;
            case '2':
                return A;
            default:

                std::ostringstream s;
                s << "Unknown dimension symbol: " << dimensionSymbol << std::endl;
                throw std::invalid_argument(s.str());
            }
        }

        //        static char toDimensionSymbol(int dimensionValue);
        //        static int toDimensionValue(char dimensionSymbol);
    };

    class Position {
    public:
        enum { ON = 0, LEFT, RIGHT };
    };

}  // namespace SpatialFHE::geom
