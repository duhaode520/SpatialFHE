//
// Created by ubuntu on 2/9/25.
//

#ifndef SIMPLESEGMENTSETMUTALINTERSECTOR_H
#define SIMPLESEGMENTSETMUTALINTERSECTOR_H
#include "TFHESegmentSetMutualIntersector.h"

namespace SpatialFHE::noding {

    class SimpleSegmentSetMutualIntersector : public TFHESegmentSetMutualIntersector {
    public:
        SimpleSegmentSetMutualIntersector() = default;
        explicit SimpleSegmentSetMutualIntersector(const geom::TFHEEnvelope* p_envelope): envelope(p_envelope) {}
        ~SimpleSegmentSetMutualIntersector() override = default;
        void setBaseSegments(TFHESegmentString::ConstVect *segStrings) override;
        void process(TFHESegmentString::ConstVect *segStrings) override;

    private:
        std::vector<TFHESegmentString *> baseSegments;
        const geom::TFHEEnvelope* envelope;
    };
}  // namespace SpatialFHE::noding

#endif  // SIMPLESEGMENTSETMUTALINTERSECTOR_H
