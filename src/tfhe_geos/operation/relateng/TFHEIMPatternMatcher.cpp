//
// Created by ubuntu on 2/3/25.
//

#include "TFHEIMPatternMatcher.h"

#include <tfhe_geos/geom/TFHEEnvelope.h>

namespace SpatialFHE {
    namespace operation {
        namespace relateng {
            bool TFHEIMPatternMatcher::requireInteraction(const TFHEIntersectionMatrix &im) {
                bool requiresInteraction = isInteraction(im.get(Location::INTERIOR, Location::INTERIOR)) ||
                    isInteraction(im.get(Location::INTERIOR, Location::BOUNDARY)) ||
                    isInteraction(im.get(Location::BOUNDARY, Location::INTERIOR)) ||
                    isInteraction(im.get(Location::BOUNDARY, Location::BOUNDARY));
                return requiresInteraction;
            }

            std::string TFHEIMPatternMatcher::name() const {
                return "IMPatternMatcher";
            }

            void TFHEIMPatternMatcher::init(const TFHEEnvelope &envA, const TFHEEnvelope &envB) {
                TFHEIMPredicate::init(dimA, dimB);

                //-- if pattern specifies any non-E/non-E interaction, envelopes must not be disjoint
                bool requiresInteraction = requireInteraction(patternMatrix);
                bool isDisjoint = envA.disjoint(&envB).decrypt();
                setValueIf(false, requiresInteraction && isDisjoint);
            }

            bool TFHEIMPatternMatcher::isInteraction(int imDim) {
                return imDim == Dimension::True || imDim >= Dimension::P;
            }

            bool TFHEIMPatternMatcher::requireInteraction() const {
                return requireInteraction(patternMatrix);
            }

            bool TFHEIMPatternMatcher::isDetermined() const {
                /**
                 * Matrix entries only increase in dimension as topology is computed.
                 * The predicate can be short-circuited (as false) if
                 * any computed entry is greater than the mask value.
                 */
                std::array<Location, 3> locs = {Location::INTERIOR, Location::BOUNDARY, Location::EXTERIOR};

                for (Location i : locs) {
                    for (Location j : locs) {
                        int patternEntry = patternMatrix.get(i, j);

                        if (patternEntry == Dimension::DONTCARE)
                            continue;

                        int matrixVal = getDimension(i, j);

                        //-- mask entry TRUE requires a known matrix entry
                        if (patternEntry == Dimension::True) {
                            if (matrixVal < 0)
                                return false;
                        }
                        //-- result is known (false) if matrix entry has exceeded mask
                        else if (matrixVal > patternEntry)
                            return true;
                    }
                }
                return false;
            }

            bool TFHEIMPatternMatcher::valueIM() {
                bool val = intMatrix.matches(imPattern);
                return val;
            }
        }  // namespace relateng
    }  // namespace operation
}  // namespace SpatialFHE