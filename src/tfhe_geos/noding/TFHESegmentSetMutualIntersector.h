//
// Created by ubuntu on 2/9/25.
//

#ifndef TFHESEGMENTSETMUTUALINTERSECTOR_H
#define TFHESEGMENTSETMUTUALINTERSECTOR_H
#include "TFHESegmentIntersector.h"
#include "TFHESegmentString.h"

namespace SpatialFHE::noding {  // geos::noding

    /** \brief
     * An intersector for the red-blue intersection problem.
     *
     * In this class of line arrangement problem,
     * two disjoint sets of linestrings are provided.
     * It is assumed that within
     * each set, no two linestrings intersect except possibly at their endpoints.
     * Implementations can take advantage of this fact to optimize processing.
     *
     * @author Martin Davis
     * @version 1.10
     */
    class TFHESegmentSetMutualIntersector {
    public:
        TFHESegmentSetMutualIntersector() : segInt(nullptr) {}

        virtual ~TFHESegmentSetMutualIntersector() = default;

        /**
         * Sets the SegmentIntersector to use with this intersector.
         * The SegmentIntersector will either record or add intersection nodes
         * for the input segment strings.
         *
         * @param si the segment intersector to use
         */
        void setSegmentIntersector(TFHESegmentIntersector* si) {
            segInt = si;
        }

        /**
         *
         * @param segStrings a collection of [SegmentStrings](@ref SegmentString) to node
         */
        virtual void setBaseSegments(TFHESegmentString::ConstVect* segStrings) = 0;

        /**
         * Computes the intersections for two collections of [SegmentStrings](@ref SegmentString).
         *
         * @param segStrings a collection of [SegmentStrings](@ref SegmentString) to node
         */
        virtual void process(TFHESegmentString::ConstVect* segStrings) = 0;

    protected:
        TFHESegmentIntersector* segInt;
    };

}  // namespace SpatialFHE::noding
#endif  // TFHESEGMENTSETMUTUALINTERSECTOR_H
