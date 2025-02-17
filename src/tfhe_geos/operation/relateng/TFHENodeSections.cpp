//
// Created by ubuntu on 1/16/25.
//

#include "TFHENodeSections.h"
#include "TFHERelateNode.h"
#include "TFHEPolygonNodeConverter.h"
#include <algorithm>

namespace SpatialFHE::operation::relateng {
    void TFHENodeSections::prepareSections() {
        auto comparator = [](const std::unique_ptr<TFHENodeSection> &a, const std::unique_ptr<TFHENodeSection> &b) {
            TFHEInt32 comp = a->compareTo(*b);
            return comp.ltTrivial(0);
        };

        std::sort(sections.begin(), sections.end(), comparator);
    }

    bool TFHENodeSections::hasMultiplePolygonSections(
        std::vector<std::unique_ptr<TFHENodeSection>> &sections,
        std::size_t i) {
        //-- if last section can only be one
        if (i >= sections.size() - 1)
            return false;
        //-- check if there are at least two sections for same polygon
        std::unique_ptr<TFHENodeSection> &ns = sections[i];
        std::unique_ptr<TFHENodeSection> &nsNext = sections[i + 1];
        return ns->isSamePolygon(*nsNext);
    }

    std::vector<const TFHENodeSection *> TFHENodeSections::collectPolygonSections(
        std::vector<std::unique_ptr<TFHENodeSection>> &sections,
        std::size_t i) {
        std::vector<const TFHENodeSection *> polySections;
        //-- note ids are only unique to a geometry
        std::unique_ptr<TFHENodeSection> &polySection = sections[i];
        while (i < sections.size() && polySection->isSamePolygon(*(sections[i]))) {
            polySections.push_back(sections[i].get());
            i++;
        }
        return polySections;
    }

    const TFHENodeSections::TFHECoordinate *TFHENodeSections::getCoordinate() const {
        return nodePt;
    }

    void TFHENodeSections::addNodeSection(TFHENodeSection *e) {
        sections.emplace_back(e);
    }

    bool TFHENodeSections::hasInteractionAB() const {
        bool isA = false;
        bool isB = false;
        for (auto& ns : sections) {
            if (ns->isA()) {
                isA = true;
            } else {
                isB = true;
            }
            if (isA && isB) {
                return true;
            }
        }
        return false;
    }

    const TFHENodeSections::TFHEGeometry *TFHENodeSections::getPolygonal(bool isA) const {
        for (auto& ns : sections) {
            if (ns->isA() == isA) {
                const TFHEGeometry *g = ns->getPolygonal();
                if (g != nullptr) {
                    return g;
                }
            }
        }
        return nullptr;
    }

    std::unique_ptr<TFHERelateNode> TFHENodeSections::createNode() {
        prepareSections();

        std::unique_ptr<TFHERelateNode> node(new TFHERelateNode(nodePt));
        std::size_t i = 0;
        while (i < sections.size()) {
            const std::unique_ptr<TFHENodeSection> &ns = sections[i];
            //-- if there multiple polygon sections incident at node convert them to maximal-ring structure
            if (ns->isArea() && hasMultiplePolygonSections(sections, i)) {
                std::vector<const TFHENodeSection *> polySections = collectPolygonSections(sections, i);
                std::vector<std::unique_ptr<TFHENodeSection>> nsConvert = TFHEPolygonNodeConverter::convert(polySections);
                node->addEdges(nsConvert);
                i += polySections.size();
            } else {
                //-- the most common case is a line or a single polygon ring section
                node->addEdges(ns.get());
                i += 1;
            }
        }
        return node;
    }
}  // namespace SpatialFHE::operation::relateng

// operation
// SpatialFHE