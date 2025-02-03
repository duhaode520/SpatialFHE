//
// Created by ubuntu on 1/16/25.
//

#ifndef TFHERELATENODE_H
#define TFHERELATENODE_H
#include "TFHERelateEdge.h"

// Forward declaration
namespace SpatialFHE::operation::relateng {
    class TFHENodeSection;
}

namespace SpatialFHE::geom {
    class TFHECoordinate;
    class TFHEGeometry;
}  // namespace SpatialFHE::geom

namespace SpatialFHE::operation::relateng {

    class TFHERelateNode {
        using TFHECoordinate = geom::TFHECoordinate;
        using TFHEGeometry = geom::TFHEGeometry;

    private:
        // Members
        std::vector<std::unique_ptr<TFHERelateEdge>> edges;
        const TFHECoordinate* nodePt;

        // Methods

        void updateEdgesInArea(bool isA, std::size_t indexFrom, std::size_t indexTo);

        void updateIfAreaPrev(bool isA, std::size_t index);

        void updateIfAreaNext(bool isA, std::size_t index);

        const TFHERelateEdge* addLineEdge(bool isA, const TFHECoordinate* dirPt);

        const TFHERelateEdge* addAreaEdge(bool isA, const TFHECoordinate* dirPt, bool isForward);

        /**
         * Adds or merges an edge to the node.
         *
         * @param isA
         * @param dirPt
         * @param dim dimension of the geometry element containing the edge
         * @param isForward the direction of the edge
         *
         * @return the created or merged edge for this point
         */
        const TFHERelateEdge* addEdge(bool isA, const TFHECoordinate* dirPt, int dim, bool isForward);

        void finishNode(bool isA, bool isAreaInterior);

        void propagateSideLocations(bool isA, std::size_t startIndex);

        static std::size_t prevIndex(std::vector<std::unique_ptr<TFHERelateEdge>>& list, std::size_t index);

        static std::size_t nextIndex(std::vector<std::unique_ptr<TFHERelateEdge>>& list, std::size_t i);

        std::size_t indexOf(const std::vector<std::unique_ptr<TFHERelateEdge>>& edges, const TFHERelateEdge* edge) const;

    public:
        TFHERelateNode(const TFHECoordinate* pt) : nodePt(pt){};

        const TFHECoordinate* getCoordinate() const;

        const std::vector<std::unique_ptr<TFHERelateEdge>>& getEdges() const;

        void addEdges(std::vector<const TFHENodeSection*>& nss);
        void addEdges(std::vector<std::unique_ptr<TFHENodeSection>>& nss);

        void addEdges(const TFHENodeSection* ns);

        /**
         * Computes the final topology for the edges around this node.
         * Although nodes lie on the boundary of areas or the interior of lines,
         * in a mixed GC they may also lie in the interior of an area.
         * This changes the locations of the sides and line to Interior.
         *
         * @param isAreaInteriorA true if the node is in the interior of A
         * @param isAreaInteriorB true if the node is in the interior of B
         */
        void finish(bool isAreaInteriorA, bool isAreaInteriorB);

        bool hasExteriorEdge(bool isA);
    };

}  // namespace SpatialFHE::operation::relateng

#endif  // TFHERELATENODE_H
