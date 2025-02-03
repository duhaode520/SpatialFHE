//
// Created by ubuntu on 1/30/25.
//

#ifndef TFHEBASICSEGMENTSTRING_H
#define TFHEBASICSEGMENTSTRING_H
#include "TFHESegmentString.h"

namespace SpatialFHE::noding {

    /** \brief
     * 代表一系列连续的 line segments, and supports noding the segments
     *     * Line Segments 可以用一系列的 Coordinates 来代表，这个类可以优化连续线段的性能，减少分配对象的数量
     * SegmentStrings can carry a context object, which is useful for preserving topological and parentage information.
     */
    class TFHEBasicSegmentString : public TFHESegmentString {
    public:
        /// \brief Construct a BasicSegmentString.
        ///
        /// @param newPts CoordinateSequence representing the string, externally owned
        /// @param newContext the context associated to this SegmentString
        ///
        TFHEBasicSegmentString(geom::TFHECoordinateSequence* newPts, const void* newContext) :
                TFHESegmentString(newContext, newPts) {}

        ~TFHEBasicSegmentString() override {}
    };

}  // namespace SpatialFHE::noding

#endif  // TFHEBASICSEGMENTSTRING_H
