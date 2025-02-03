//
// Created by ubuntu on 1/17/25.
//

#ifndef TFHELINEARBOUNDARY_H
#define TFHELINEARBOUNDARY_H
#include <tfhe_geos/geom/TFHECoordinate.h>

#include <vector>

// Forward declaration
namespace SpatialFHE::geom {
    class TFHELineString;
}

namespace SpatialFHE::operation::relateng {

    class TFHELinearBoundary {
        using TFHECoordinate = geom::TFHECoordinate;
        using TFHELineString = geom::TFHELineString;

    private:
        // Members
        TFHECoordinate::ConstIntMap m_vertexDegree;
        bool m_hasBoundary;

        // Methods
        bool checkBoundary(TFHECoordinate::ConstIntMap& vertexDegree) const;

        static void computeBoundaryPoints(
            std::vector<const TFHELineString*>& lines,
            TFHECoordinate::ConstIntMap& vertexDegree);

        static void addEndpoint(const TFHECoordinate* p, TFHECoordinate::ConstIntMap& vertexDegree);

    public:
        TFHELinearBoundary(std::vector<const TFHELineString*>& lines);

        bool hasBoundary() const;

        bool isBoundary(const TFHECoordinate* pt) const;
    };

}  // namespace SpatialFHE::operation::relateng

#endif  // TFHELINEARBOUNDARY_H
