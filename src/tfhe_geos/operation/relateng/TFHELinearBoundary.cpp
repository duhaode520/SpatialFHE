//
// Created by ubuntu on 1/17/25.
//

#include "TFHELinearBoundary.h"

#include <tfhe_geos/algorithm/BoundaryNodeRule.h>
#include <tfhe_geos/geom/TFHELineString.h>

using namespace SpatialFHE::geom;
using namespace SpatialFHE::algorithm;

namespace SpatialFHE::operation::relateng {
    bool TFHELinearBoundary::checkBoundary(TFHECoordinate::ConstIntMap &vertexDegree) const {
        for (const auto& pair: vertexDegree) {
            int degree = pair.second;
            if (BoundaryNodeRule::isInBoundary(degree)) {
                return true;
            }
        }
        return false;
    }

    void TFHELinearBoundary::computeBoundaryPoints(
        std::vector<const TFHELineString *> &lines,
        TFHECoordinate::ConstIntMap &vertexDegree) {
        for (const TFHELineString* line: lines) {
            if (line->isEmpty()) {
                continue;
            }

            const TFHECoordinateSequence* cs = line->getCoordinatesRO();
            const TFHECoordinate& p0 = cs->getAt(0);
            const TFHECoordinate& pn = cs->getAt(cs->size() - 1);
            addEndpoint(&p0, vertexDegree);
            addEndpoint(&pn, vertexDegree);
        }
    }

    void TFHELinearBoundary::addEndpoint(const TFHECoordinate *p, TFHECoordinate::ConstIntMap &vertexDegree) {
        int dim = 0;
        auto it = vertexDegree.find(p);

        if (it != vertexDegree.end()) {
            dim = it->second;
        }
        vertexDegree[p] = dim + 1;
    }

    TFHELinearBoundary::TFHELinearBoundary(std::vector<const TFHELineString *> &lines) {
        computeBoundaryPoints(lines, m_vertexDegree);
        m_hasBoundary = checkBoundary(m_vertexDegree);
    }

    bool TFHELinearBoundary::hasBoundary() const {
        return m_hasBoundary;
    }

    bool TFHELinearBoundary::isBoundary(const TFHECoordinate *pt) const {
        auto it = m_vertexDegree.find(pt);
        if (it == m_vertexDegree.end()) {
            return false;
        }
        int degree = it->second;
        return BoundaryNodeRule::isInBoundary(degree);
    }
}  // namespace SpatialFHE::operation::relateng

// operation
// SpatialFHE