//
// Created by ubuntu on 1/30/25.
//

#ifndef TFHESEGMENTINTERSECTOR_H
#define TFHESEGMENTINTERSECTOR_H
#include <cstddef>

namespace SpatialFHE::noding {
    class TFHESegmentString;
}

namespace SpatialFHE::noding {

    /**
     * \brief
     * Processes possible intersections detected by a Noder.
     *
     * The SegmentIntersector is passed to a Noder.
     * The addIntersections method is called whenever the Noder
     * detects that two SegmentStrings <i>might</i> intersect.
     * This class may be used either to find all intersections, or
     * to detect the presence of an intersection.  In the latter case,
     * Noders may choose to short-circuit their computation by calling the
     * isDone method.
     * This class is an example of the <i>Strategy</i> pattern.
     *
     * @version 1.7
     */
    class TFHESegmentIntersector {
    public:
        /**
         * This method is called by clients
         * of the SegmentIntersector interface to process
         * intersections for two segments of the SegmentStrings
         * being intersected.
         */
        virtual void processIntersections(
            TFHESegmentString* e0,
            std::size_t segIndex0,
            TFHESegmentString* e1,
            std::size_t segIndex1) = 0;

        /**
         * \brief
         * Reports whether the client of this class
         * needs to continue testing all intersections in an arrangement.
         *
         * @return true if there is not need to continue testing segments
         *
         * The default implementation always return false (process all intersections).
         */
        virtual bool isDone() const {
            return false;
        }

        virtual ~TFHESegmentIntersector() {}

    protected:
        TFHESegmentIntersector() {}
    };

}  // namespace SpatialFHE::noding

#endif  // TFHESEGMENTINTERSECTOR_H
