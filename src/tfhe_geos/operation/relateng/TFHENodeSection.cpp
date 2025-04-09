//
// Created by ubuntu on 1/16/25.
//

#include "TFHENodeSection.h"
#include "TFHEInt32.h"

#include <tfhe_geos/geom/enums.h>

using namespace SpatialFHE::geom;

namespace SpatialFHE::operation::relateng {
    TFHEInt32 TFHENodeSection::compareWithNull(const TFHECoordinate *v0, const TFHECoordinate *v1) {
        if (v0 == nullptr) {
            if (v1 == nullptr) {
                return TFHEInt32(0, true);
            }
            // null is lower than any non-null
            return TFHEInt32(-1, true);
        }
        if (v1 == nullptr) {
            // non-null is higher than null
            return TFHEInt32(1, true);
        }
        return v0->compareTo(*v1);
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

    TFHEInt32 TFHENodeSection::compareTo(const TFHENodeSection &o) const {
        // sort A before B
        if (m_isA != o.m_isA) {
            if (m_isA)
                return TFHEInt32(-1, true);
            return TFHEInt32(1, true);
        }
        //-- sort on dimensions
        int compDim = compare(m_dim, o.m_dim);
        if (compDim != 0)
            return TFHEInt32(compDim, true);

        //-- sort on id and ring id
        int compId = compare(m_id, o.m_id);
        if (compId != 0)
            return TFHEInt32(compId, true);

        int compRingId = compare(m_ringId, o.m_ringId);
        if (compRingId != 0)
            return TFHEInt32(compRingId, true);

        //-- sort on edge coordinates
        TFHEInt32 compV0 = compareWithNull(m_v0, o.m_v0);
        if (compV0.neTrivial(0))
            return compV0;

        return compareWithNull(m_v1, o.m_v1);
    }

    bool TFHENodeSection::isSameGeometry(const TFHENodeSection &ns) const {
        return isA() == ns.isA();
    }

    bool TFHENodeSection::isProper() const {
        return !m_isNodeAtVertex;
    }

    bool TFHENodeSection::isAreaArea(const TFHENodeSection &a, const TFHENodeSection &b) {
        return a.dimension() == Dimension::A && b.dimension() == Dimension::A;
    }

    bool TFHENodeSection::isProper(const TFHENodeSection &a, const TFHENodeSection &b) {
        return a.isProper() && b.isProper();
    }
}  // namespace SpatialFHE::operation::relateng

// operation
// SpatialFHE