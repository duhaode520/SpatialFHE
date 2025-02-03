//
// Created by ubuntu on 1/16/25.
//

#ifndef TFHENODESECTIONS_H
#define TFHENODESECTIONS_H
#include <memory>
#include <vector>

#include "TFHENodeSection.h"

// Forward declaration
namespace SpatialFHE::operation::relateng {
    class TFHERelateNode;
}

namespace SpatialFHE::geom {
    class TFHECoordinate;
    class TFHEGeometry;
}  // namespace SpatialFHE::geom

namespace SpatialFHE ::operation ::relateng {
    class TFHENodeSections {
        using TFHECoordinate = geom::TFHECoordinate;
        using TFHEGeometry = geom::TFHEGeometry;

    private:
        // Members
        const TFHECoordinate* nodePt;
        std::vector<std::unique_ptr<TFHENodeSection>> sections;

        // Methods

        /**
         * Sorts the sections so that:
         *  * lines are before areas
         *  * edges from the same polygon are contiguous
         */
        void prepareSections();

        static bool hasMultiplePolygonSections(std::vector<std::unique_ptr<TFHENodeSection>>& sections, std::size_t i);

        static std::vector<const TFHENodeSection*> collectPolygonSections(
            std::vector<std::unique_ptr<TFHENodeSection>>& sections,
            std::size_t i);

    public:
        TFHENodeSections(const TFHECoordinate* pt) : nodePt(pt){};

        const TFHECoordinate* getCoordinate() const;

        void addNodeSection(TFHENodeSection* e);

        bool hasInteractionAB() const;

        const TFHEGeometry* getPolygonal(bool isA) const;

        std::unique_ptr<TFHERelateNode> createNode();
    };

}  // namespace SpatialFHE::operation::relateng

#endif  // TFHENODESECTIONS_H
