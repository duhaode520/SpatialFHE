//
// Created by ubuntu on 1/23/25.
//

#ifndef TFHEPOLYGONNODECONVERTER_H
#define TFHEPOLYGONNODECONVERTER_H
#include "tfhe_geos/operation/relateng/TFHENodeSection.h"
#include <vector>
#include <memory>

namespace SpatialFHE::operation::relateng {
    /**
     * Converts the node sections at a polygon node where
     * a shell and one or more holes touch, or two or more holes touch.
     * This converts the node topological structure from
     * the OGC "touching-rings" (AKA "minimal-ring") model to the equivalent "self-touch"
     * (AKA "inverted/exverted ring" or "maximal ring") model.
     * In the "self-touch" model the converted NodeSection corners enclose areas
     * which all lies inside the polygon
     * (i.e. they does not enclose hole edges).
     * This allows RelateNode to use simple area-additive semantics
     * for adding edges and propagating edge locations.
     *
     * The input node sections are assumed to have canonical orientation
     * (CW shells and CCW holes).
     * The arrangement of shells and holes must be topologically valid.
     * Specifically, the node sections must not cross or be collinear.
     *
     * This supports multiple shell-shell touches
     * (including ones containing holes), and hole-hole touches,
     * This generalizes the relate algorithm to support
     * both the OGC model and the self-touch model.
     *
     * @ author Martin Davis
     * @ see RelateNode
     */
    class TFHEPolygonNodeConverter {
        using TFHECoordinate = geom::TFHECoordinate;

    public:
        /**
         * Converts a list of sections of valid polygon rings
         * to have "self-touching" structure.
         * There are the same number of output sections as input ones.
         *
         * @param polySections the original sections
         * @return the converted sections
         */
        static std::vector<std::unique_ptr<TFHENodeSection>> convert(std::vector<const TFHENodeSection*>& polySections);

    private:
        static std::size_t convertShellAndHoles(
            std::vector<const TFHENodeSection*>& sections,
            std::size_t shellIndex,
            std::vector<std::unique_ptr<TFHENodeSection>>& convertedSections);

        static std::vector<std::unique_ptr<TFHENodeSection>> convertHoles(std::vector<const TFHENodeSection*>& sections);

        static TFHENodeSection* createSection(const TFHENodeSection* ns, const TFHECoordinate* v0, const TFHECoordinate* v1);

        static std::vector<const TFHENodeSection*> extractUnique(std::vector<const TFHENodeSection*>& sections);

        static std::size_t next(std::vector<const TFHENodeSection*>& ns, std::size_t i);

        static std::size_t findShell(std::vector<const TFHENodeSection*>& polySections);
    };

}  // namespace SpatialFHE::operation::relateng

#endif  // TFHEPOLYGONNODECONVERTER_H
