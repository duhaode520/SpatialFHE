//
// Created by ubuntu on 1/23/25.
//

#include "TFHEPolygonNodeConverter.h"

#include <tfhe_geos/algorithm/TFHEPolygonNodeTopology.h>
#include <tfhe_geos/geom/enums.h>

#include <algorithm>
#include "tfhe_geos/operation/relateng/TFHENodeSection.h"

using SpatialFHE::algorithm::TFHEPolygonNodeTopology;

namespace SpatialFHE::operation::relateng {
    std::vector<std::unique_ptr<TFHENodeSection>> TFHEPolygonNodeConverter::convert(
        std::vector<const TFHENodeSection *> &polySections) {
        auto comparator = [](const TFHENodeSection *a, const TFHENodeSection *b) {
            int comp = TFHEPolygonNodeTopology::compareAngle(&a->nodePt(), a->getVertex(0), b->getVertex(0)).decrypt();
            return comp < 0;
        };

        std::sort(polySections.begin(), polySections.end(), comparator);

        std::vector<const TFHENodeSection *> sections = extractUnique(polySections);
        if (sections.size() == 1) {
            // only one section
            std::vector<std::unique_ptr<TFHENodeSection>> nss;
            nss.emplace_back(new TFHENodeSection(sections[0]));
            return nss;
        }

        // find shell section
        std::size_t shellIndex = findShell(sections);
        if (shellIndex == -1) {
            // no shell section
            return convertHoles(sections);
        }

        // at least one shell section
        std::vector<std::unique_ptr<TFHENodeSection>> convertedSections;
        std::size_t nextShellIndex = shellIndex;
        do {
            nextShellIndex = convertShellAndHoles(sections, shellIndex, convertedSections);
        } while (nextShellIndex != shellIndex);
        return convertedSections;
    }

    std::size_t TFHEPolygonNodeConverter::convertShellAndHoles(
        std::vector<const TFHENodeSection *> &sections,
        std::size_t shellIndex,
        std::vector<std::unique_ptr<TFHENodeSection>> &convertedSections) {
        const TFHENodeSection *shell = sections[shellIndex];
        const TFHECoordinate *inVertex = shell->getVertex(0);
        std::size_t i = next(sections, shellIndex);
        const TFHENodeSection* holeSection = nullptr;
        while (!sections[i]->isShell()) {
            holeSection = sections[i];
            const TFHECoordinate *outVertex = holeSection->getVertex(1);
            TFHENodeSection *ns = createSection(shell, inVertex, outVertex);
            convertedSections.emplace_back(ns);
            inVertex = holeSection->getVertex(0);
            i = next(sections, i);
        }

        // create final section from last hole to shell
        const TFHECoordinate *outVertex = shell->getVertex(1);
        TFHENodeSection *ns = createSection(shell, inVertex, outVertex);
        convertedSections.emplace_back(ns);
        return i;
    }

    std::vector<std::unique_ptr<TFHENodeSection>> TFHEPolygonNodeConverter::convertHoles(
        std::vector<const TFHENodeSection *> &sections) {
        std::vector<std::unique_ptr<TFHENodeSection>> convertedSections;
        const TFHENodeSection *copySection = sections[0];
        for (std::size_t i = 0; i < sections.size(); i++) {
            std::size_t inext = next(sections, i);
            const TFHECoordinate *inVertex = sections[i]->getVertex(0);
            const TFHECoordinate *outVertex = sections[inext]->getVertex(1);
            TFHENodeSection *ns = createSection(copySection, inVertex, outVertex);
            convertedSections.emplace_back(ns);
        }
        return convertedSections;
    }

    TFHENodeSection *TFHEPolygonNodeConverter::createSection(
        const TFHENodeSection *ns,
        const TFHECoordinate *v0,
        const TFHECoordinate *v1) {
        return new TFHENodeSection(
            ns->isA(), geom::Dimension::A, ns->id(), 0, ns->getPolygonal(), ns->isNodeAtVertex(), v0, ns->nodePt(), v1);
    }

    std::vector<const TFHENodeSection *> TFHEPolygonNodeConverter::extractUnique(
        std::vector<const TFHENodeSection *> &sections) {
        std::vector<const TFHENodeSection *> uniqueSections;
        const TFHENodeSection *lastUnique = sections[0];
        uniqueSections.push_back(lastUnique);
        for (const TFHENodeSection *ns : sections) {
            if (lastUnique->compareTo(ns) != 0) {
                uniqueSections.push_back(ns);
                lastUnique = ns;
            }
        }
        return uniqueSections;
    }

    std::size_t TFHEPolygonNodeConverter::next(std::vector<const TFHENodeSection *> &ns, std::size_t i) {
        std::size_t nxt = i;
        if (nxt == -1)
            nxt = 0;
        else
            nxt = i + 1;

        if (nxt >= ns.size())
            nxt = 0;

        return nxt;
    }

    std::size_t TFHEPolygonNodeConverter::findShell(std::vector<const TFHENodeSection *> &polySections) {
        for (std::size_t i = 0; i < polySections.size(); i++) {
            if (polySections[i]->isShell()) {
                return i;
            }
        }
        return -1;
    }
}  // namespace SpatialFHE::operation::relateng

// operation
// SpatialFHE