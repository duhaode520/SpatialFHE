//
// Created by ubuntu on 1/16/25.
//

#include "TFHENodeSection.h"

#include <tfhe_geos/geom/enums.h>

namespace SpatialFHE::operation::relateng {
    int TFHENodeSection::compareWithNull(const TFHECoordinate *v0, const TFHECoordinate *v1) {
        if (v0 == nullptr) {
            if (v1 == nullptr) {
                return 0;
            }
            // null is lower than any non-null
            return -1;
        }
        if (v1 == nullptr) {
            // non-null is higher than null
            return 1;
        }
        return v0->compareTo(*v1).decrypt();
    }

    int TFHENodeSection::compare(int a, int b) {
        if (a < b)
            return -1;
        if (a > b)
            return 1;
        return 0;
    }

    const TFHENodeSection::TFHECoordinate *TFHENodeSection::getVertex(int i) const {
        return (i == 0) ? m_v0 : m_v1;
    }

    const TFHENodeSection::TFHECoordinate &TFHENodeSection::nodePt() const {
        return m_nodePt;
    }

    int TFHENodeSection::dimension() const {
        return m_dim;
    }

    int TFHENodeSection::id() const {
        return m_id;
    }

    int TFHENodeSection::ringId() const {
        return m_ringId;
    }

    const TFHENodeSection::TFHEGeometry *TFHENodeSection::getPolygonal() const {
        return m_poly;
    }

    bool TFHENodeSection::isShell() const {
        return m_ringId == 0;
    }

    bool TFHENodeSection::isArea() const {
        return m_dim == geom::Dimension::A;
    }

    bool TFHENodeSection::isA() const {
        return m_isA;
    }

    bool TFHENodeSection::isSamePolygon(const TFHENodeSection &ns) const {
        return isA() == ns.isA() && id() == ns.id();
    }

    bool TFHENodeSection::isNodeAtVertex() const {
        return m_isNodeAtVertex;
    }

    int TFHENodeSection::compareTo(const TFHENodeSection &o) const {
        // sort A before B
        if (m_isA != o.m_isA) {
            if (m_isA)
                return -1;
            return 1;
        }
        //-- sort on dimensions
        int compDim = compare(m_dim, o.m_dim);
        if (compDim != 0)
            return compDim;

        //-- sort on id and ring id
        int compId = compare(m_id, o.m_id);
        if (compId != 0)
            return compId;

        int compRingId = compare(m_ringId, o.m_ringId);
        if (compRingId != 0)
            return compRingId;

        //-- sort on edge coordinates
        int compV0 = compareWithNull(m_v0, o.m_v0);
        if (compV0 != 0)
            return compV0;

        return compareWithNull(m_v1, o.m_v1);
    }
}  // namespace SpatialFHE::operation::relateng

// operation
// SpatialFHE