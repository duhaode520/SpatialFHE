//
// Created by ubuntu on 1/16/25.
//

#ifndef TFHENODESECTION_H
#define TFHENODESECTION_H

#include <tfhe_geos/geom/TFHECoordinate.h>

// Forward declaration
namespace SpatialFHE::geom {
    class TFHEGeometry;
}

namespace SpatialFHE ::operation ::relateng {

    /**
     * 表示一个计算节点及其两侧的入射边。
     * 用于计算节点的拓扑关系。
     * 要求 shell 是 CW 方向的。
     */
    class TFHENodeSection {
        using TFHECoordinate = SpatialFHE::geom::TFHECoordinate;
        using TFHEGeometry = SpatialFHE::geom::TFHEGeometry;

    private:
        // Members
        bool m_isA;
        int m_dim;
        int m_id;
        int m_ringId;
        const TFHEGeometry* m_poly;
        bool m_isNodeAtVertex;
        const TFHECoordinate* m_v0;
        const TFHECoordinate m_nodePt;
        const TFHECoordinate* m_v1;

        // Methods

        static int compareWithNull(const TFHECoordinate* v0, const TFHECoordinate* v1);

        static int compare(int a, int b);

    public:
        TFHENodeSection(
            bool isA,
            int dim,
            int id,
            int ringId,
            const TFHEGeometry* poly,
            bool isNodeAtVertex,
            const TFHECoordinate* v0,
            const TFHECoordinate nodePt,
            const TFHECoordinate* v1) :
                m_isA(isA),
                m_dim(dim),
                m_id(id),
                m_ringId(ringId),
                m_poly(poly),
                m_isNodeAtVertex(isNodeAtVertex),
                m_v0(v0),
                m_nodePt(nodePt),
                m_v1(v1){};

        TFHENodeSection(const TFHENodeSection* ns) :
                m_isA(ns->isA()),
                m_dim(ns->dimension()),
                m_id(ns->id()),
                m_ringId(ns->ringId()),
                m_poly(ns->getPolygonal()),
                m_isNodeAtVertex(ns->isNodeAtVertex()),
                m_v0(ns->getVertex(0)),
                m_nodePt(ns->nodePt()),
                m_v1(ns->getVertex(1)){};

        [[nodiscard]] const TFHECoordinate* getVertex(int i) const;

        [[nodiscard]] const TFHECoordinate& nodePt() const;

        [[nodiscard]] int dimension() const;

        [[nodiscard]] int id() const;

        [[nodiscard]] int ringId() const;

        /**
         * Gets the polygon this section is part of.
         * Will be null if section is not on a polygon boundary.
         *
         * @return the associated polygon, or null
         */
        [[nodiscard]] const TFHEGeometry* getPolygonal() const;

        [[nodiscard]] bool isShell() const;

        [[nodiscard]] bool isArea() const;

        [[nodiscard]] bool isA() const;

        [[nodiscard]] bool isSamePolygon(const TFHENodeSection& ns) const;

        [[nodiscard]] bool isNodeAtVertex() const;

        [[nodiscard]] int compareTo(const TFHENodeSection& o) const;

        [[nodiscard]] bool isSameGeometry(const TFHENodeSection& ns) const;

        [[nodiscard]] bool isProper() const;

        static bool isAreaArea(const TFHENodeSection& a, const TFHENodeSection& b);

        static bool isProper(const TFHENodeSection& a, const TFHENodeSection& b);
    };

}  // namespace SpatialFHE::operation::relateng

#endif  // TFHENODESECTION_H
