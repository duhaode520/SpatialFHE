//
// Created by ubuntu on 1/16/25.
//

#include "TFHERelateNode.h"

#include "TFHENodeSection.h"
#include "TFHERelateGeometry.h"

using namespace SpatialFHE::geom;

namespace SpatialFHE::operation::relateng {
    void TFHERelateNode::updateEdgesInArea(bool isA, std::size_t indexFrom, std::size_t indexTo) {
        size_t index = nextIndex(edges, indexFrom);
        while (index != indexTo) {
            TFHERelateEdge *e = edges[index].get();
            e->setAreaInterior(isA);
            index = nextIndex(edges, index);
        }
    }

    void TFHERelateNode::updateIfAreaPrev(bool isA, std::size_t index) {
        std::size_t indexPrev = prevIndex(edges, index);
        auto &edgePrev = edges[indexPrev];
        if (edgePrev->isInterior(isA, Position::LEFT)) {
            std::unique_ptr<TFHERelateEdge> &edge = edges[index];
            edge->setAreaInterior(isA);
        }
    }

    void TFHERelateNode::updateIfAreaNext(bool isA, std::size_t index) {
        std::size_t indexNext = nextIndex(edges, index);
        auto &edgeNext = edges[indexNext];
        if (edgeNext->isInterior(isA, Position::RIGHT)) {
            auto &edge = edges[index];
            edge->setAreaInterior(isA);
        }
    }

    const TFHERelateEdge *TFHERelateNode::addLineEdge(bool isA, const TFHECoordinate *dirPt) {
        return addEdge(isA, dirPt, Dimension::L, true);
    }

    const TFHERelateEdge *TFHERelateNode::addAreaEdge(bool isA, const TFHECoordinate *dirPt, bool isForward) {
        return addEdge(isA, dirPt, Dimension::A, isForward);
    }

    const TFHERelateEdge *TFHERelateNode::addEdge(bool isA, const TFHECoordinate *dirPt, int dim, bool isForward) {
        if (dirPt == nullptr) {
            return nullptr;
        }
        // same point
        if (nodePt == dirPt) {
            return nullptr;
        }
        // zero length edge
        if (nodePt->equals(*dirPt).decrypt()) {
            return nullptr;
        }

        std::size_t insertIndex = -1;
        for (std::size_t i = 0; i < edges.size(); i++) {
            auto *e = edges[i].get();
            // 这里必须解密了
            int comp = e->compareToEdge(dirPt).decrypt();
            if (comp == 0) {
                e->merge(isA, dim, isForward);
                return e;
            }
            if (comp > 0) {
                // found insertion point
                insertIndex = i;
                break;
            }
        }

        // create new edge
        TFHERelateEdge *newEdge = TFHERelateEdge::create(this, dirPt, isA, dim, isForward);
        if (insertIndex == -1) {
            edges.push_back(std::unique_ptr<TFHERelateEdge>(newEdge));
        } else {
            edges.insert(
                edges.begin() + static_cast<long>(insertIndex), std::move(std::unique_ptr<TFHERelateEdge>(newEdge)));
        }
        return newEdge;
    }

    void TFHERelateNode::finishNode(bool isA, bool isAreaInterior) {
        if (isAreaInterior) {
            TFHERelateEdge::setAreaInterior(edges, isA);
        } else {
            std::size_t startIndex = TFHERelateEdge::findKnownEdgeIndex(edges, isA);
            //-- only interacting nodes are finished, so this should never happen
            // Assert.isTrue(startIndex >= 0l, "Node at "+ nodePt + "does not have AB interaction");
            propagateSideLocations(isA, startIndex);
        }
    }

    void TFHERelateNode::propagateSideLocations(bool isA, std::size_t startIndex) {
        Location currLoc = edges[startIndex]->location(isA, Position::LEFT);
        //-- edges are stored in CCW order
        std::size_t index = nextIndex(edges, startIndex);
        while (index != startIndex) {
            const auto &e = edges[index];
            e->setUnknownLocations(isA, currLoc);
            currLoc = e->location(isA, Position::LEFT);
            index = nextIndex(edges, index);
        }
    }

    std::size_t TFHERelateNode::prevIndex(std::vector<std::unique_ptr<TFHERelateEdge>> &list, std::size_t index) {
        if (index > 0 && index != -1) {
            return index - 1;
        }
        //-- index == 0
        return list.size() - 1;
    }

    std::size_t TFHERelateNode::nextIndex(std::vector<std::unique_ptr<TFHERelateEdge>> &list, std::size_t index) {
        if (index >= list.size() - 1 || index == -1) {
            return 0;
        }
        return index + 1;
    }

    std::size_t TFHERelateNode::indexOf(
        const std::vector<std::unique_ptr<TFHERelateEdge>> &edges,
        const TFHERelateEdge *edge) const {
        for (std::size_t i = 0; i < edges.size(); i++) {
            const std::unique_ptr<TFHERelateEdge> &e = edges[i];
            if (e.get() == edge)
                return i;
        }
        return -1;
    }

    const TFHERelateNode::TFHECoordinate *TFHERelateNode::getCoordinate() const {
        return nodePt;
    }

    const std::vector<std::unique_ptr<TFHERelateEdge>> &TFHERelateNode::getEdges() const {
        return edges;
    }

    void TFHERelateNode::addEdges(std::vector<const TFHENodeSection *> &nss) {
        for (auto *ns : nss) {
            addEdges(ns);
        }
    }

    void TFHERelateNode::addEdges(std::vector<std::unique_ptr<TFHENodeSection>> &nss) {
        for (auto &ns : nss) {
            addEdges(ns.get());
        }
    }

    void TFHERelateNode::addEdges(const TFHENodeSection *ns) {
        switch (ns->dimension()) {
        case Dimension::L:
            addLineEdge(ns->isA(), ns->getVertex(0));
            addLineEdge(ns->isA(), ns->getVertex(1));
            break;
        case Dimension::A:
            // assumes node edges have CW orientation
            const TFHERelateEdge *e0 = addAreaEdge(ns->isA(), ns->getVertex(0), false);
            const TFHERelateEdge *e1 = addAreaEdge(ns->isA(), ns->getVertex(1), true);

            std::size_t index0 = indexOf(edges, e0);
            std::size_t index1 = indexOf(edges, e1);
            updateEdgesInArea(ns->isA(), index0, index1);
            updateIfAreaPrev(ns->isA(), index0);
            updateIfAreaNext(ns->isA(), index1);
        }
    }

    void TFHERelateNode::finish(bool isAreaInteriorA, bool isAreaInteriorB) {
        finishNode(TFHERelateGeometry::GEOM_A, isAreaInteriorA);
        finishNode(TFHERelateGeometry::GEOM_B, isAreaInteriorB);
    }

    bool TFHERelateNode::hasExteriorEdge(bool isA) {
        for (auto &e : edges) {
            if (Location::EXTERIOR == e->location(isA, Position::LEFT) ||
                Location::EXTERIOR == e->location(isA, Position::RIGHT)) {
                return true;
            }
        }
        return false;
    }
}  // namespace SpatialFHE::operation::relateng

// operation
// SpatialFHE