#pragma once


namespace SpatialFHE::geom {
    enum class Location {
        NONE = -1,
        INTERIOR = 0,
        BOUNDARY = 1,
        EXTERIOR = 2
    };

    class Dimension {
    public:
        enum DimensionType {
            P = 0,
            L = 1,
            A = 2,
            False = -3,
            TRUE = -2,
            DONTCARE = -1,
        };
//        static char toDimensionSymbol(int dimensionValue);
//        static int toDimensionValue(char dimensionSymbol);
    };

}
