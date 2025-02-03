//
// Created by ubuntu on 2/3/25.
//

#ifndef TFHEIMPATTERNMATCHER_H
#define TFHEIMPATTERNMATCHER_H
#include <tfhe_geos/geom/enums.h>

#include "TFHEIMPredicate.h"

namespace SpatialFHE::operation::relateng {

    class TFHEIMPatternMatcher : public TFHEIMPredicate {
        using TFHEEnvelope = geom::TFHEEnvelope;
        using Location = geom::Location;
        using Dimension = geom::Dimension;
        using TFHEIntersectionMatrix = geom::TFHEIntersectionMatrix;

    private:
        std::string imPattern;
        TFHEIntersectionMatrix patternMatrix;

        static bool requireInteraction(const TFHEIntersectionMatrix& im);

        static bool isInteraction(int imDim);

    public:

        TFHEIMPatternMatcher(std::string p_imPattern)
            : imPattern(p_imPattern)
            , patternMatrix(p_imPattern)
        {};

        std::string name() const override;

        using TFHEIMPredicate::init;
        void init(const TFHEEnvelope& envA, const TFHEEnvelope& envB) override;

        bool requireInteraction() const override;

        bool isDetermined() const override;

        bool valueIM() override;

    };

}  // namespace SpatialFHE::operation::relateng

#endif  // TFHEIMPATTERNMATCHER_H
