//
// Created by ubuntu on 2/9/25.
//

#include "SimpleSegmentSetMutualIntersector.h"

namespace SpatialFHE::noding {

    void SpatialFHE::noding::SimpleSegmentSetMutualIntersector::setBaseSegments(
        TFHESegmentString::ConstVect* segStrings) {
        baseSegments.clear();
        for (const TFHESegmentString* css : *segStrings) {
            if (css->size() == 0)
                continue;
            auto* ss = const_cast<TFHESegmentString*>(css);
            baseSegments.push_back(ss);
        }
    }

    void SpatialFHE::noding::SimpleSegmentSetMutualIntersector::process(TFHESegmentString::ConstVect* segStrings) {
        for (const TFHESegmentString* baseSS : baseSegments) {
            for (const TFHESegmentString* css : *segStrings) {
                if (css->size() == 0)
                    continue;
                auto* ss = const_cast<TFHESegmentString*>(css);

                for (size_t i = 0; i < baseSS->size() - 1; ++i) {
                    for (size_t j = 0; j < ss->size() - 1; ++j) {
                        segInt->processIntersections(const_cast<TFHESegmentString*>(baseSS), i, ss, j);
                    }
                }
            }
        }
    }
}  // namespace SpatialFHE::noding