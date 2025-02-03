//
// Created by ubuntu on 1/27/25.
//

#ifndef TFHESEGMENTSTRING_H
#define TFHESEGMENTSTRING_H
#include <tfhe_geos/geom/TFHECoordinate.h>
#include <tfhe_geos/geom/TFHECoordinateSequence.h>

#include <vector>

namespace geos::algorithm {
    class TFHELineIntersector;
}  // namespace geos::algorithm

namespace SpatialFHE::noding {

    class TFHESegmentString {
    private:
        const void* context;

    protected:
        geom::TFHECoordinateSequence* seq;

    public:
        typedef std::vector<const TFHESegmentString*> ConstVect;
        typedef std::vector<TFHESegmentString> NonConstVect;

        TFHESegmentString(const void* newContext, geom::TFHECoordinateSequence* newSeq) :
                seq(newSeq), context(newContext) {}

        virtual ~TFHESegmentString() = default;

        /** \brief
         * Gets the user-defined data for this segment string.
         *
         * @return the user-defined data
         */
        const void* getData() const {
            return context;
        }

        /** \brief
         * Sets the user-defined data for this segment string.
         *
         * @param data an Object containing user-defined data
         */
        void setData(const void* data) {
            context = data;
        }

        std::size_t size() const {
            return seq->size();
        }

        const geom::TFHECoordinate& getCoordinate(std::size_t i) const {
            return seq->getAt(i);
        }

        /// \brief
        /// Return a pointer to the CoordinateSequence associated
        /// with this SegmentString.
        ///
        /// @note The CoordinateSequence is owned by this SegmentString!
        ///
        const geom::TFHECoordinateSequence* getCoordinates() const {
            return seq;
        }

        geom::TFHECoordinateSequence* getCoordinates() {
            return seq;
        }

        bool isClosed() const;

        /**
         * Gets the previous vertex in a ring from a vertex index.
         *
         * @param index the vertex index
         * @return the previous vertex in the ring
         *
         * @see isClosed
         */
        const geom::TFHECoordinate& prevInRing(size_t index) const;

        /**
         * Gets the next vertex in a ring from a vertex index.
         *
         * @param index the vertex index
         * @return the next vertex in the ring
         *
         * @see isClosed
         */
        const geom::TFHECoordinate& nextInRing(size_t index) const;
    };

}  // namespace SpatialFHE::noding

#endif  // TFHESEGMENTSTRING_H
