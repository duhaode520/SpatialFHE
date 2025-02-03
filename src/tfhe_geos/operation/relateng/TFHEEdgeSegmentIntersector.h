//
// Created by ubuntu on 1/30/25.
//

#ifndef TFHEEDGESEGMENTINTERSECTOR_H
#define TFHEEDGESEGMENTINTERSECTOR_H
#include <tfhe_geos/algorithm/TFHELineIntersector.h>
#include <tfhe_geos/noding/TFHESegmentIntersector.h>

#include <cstddef>

// Forward declarations
namespace SpatialFHE::noding {
    class TFHESegmentString;
}

namespace SpatialFHE::operation::relateng {
    class TFHERelateSegmentString;
    class TFHETopologyComputer;
}

namespace SpatialFHE::operation::relateng {

    class TFHEEdgeSegmentIntersector : public noding::TFHESegmentIntersector {
        using TFHESegmentString = noding::TFHESegmentString;
        using TFHELineIntersector = algorithm::TFHELineIntersector;
    private:

        // Members
        TFHELineIntersector li;
        TFHETopologyComputer& topoComputer;

        // Methods


        void addIntersections(
            TFHERelateSegmentString* ssA, std::size_t segIndexA,
            TFHERelateSegmentString* ssB, std::size_t segIndexB);


    public:

        TFHEEdgeSegmentIntersector(TFHETopologyComputer& p_topoComputer)
            : topoComputer(p_topoComputer)
        {};


        void processIntersections(
            TFHESegmentString* ss0, std::size_t segIndex0,
            TFHESegmentString* ss1, std::size_t segIndex1) override;

        bool isDone() const override;

    };

}  // namespace SpatialFHE::operation::relateng

#endif  // TFHEEDGESEGMENTINTERSECTOR_H
