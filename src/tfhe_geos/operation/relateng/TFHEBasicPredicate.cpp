//
// Created by ubuntu on 1/14/25.
//

#include "TFHEBasicPredicate.h"
#include <tfhe_geos/geom/TFHEEnvelope.h>

namespace SpatialFHE::operation::relateng {
    bool TFHEBasicPredicate::isKnown(int val) {
        return val > UNKNOWN;
    }

    bool TFHEBasicPredicate::toBoolean(int val) {
        return val == TRUE;
    }

    int TFHEBasicPredicate::toValue(bool val) {
        return val ? TRUE : FALSE;
    }

    void TFHEBasicPredicate::setValue(bool val) {
        if (isKnown()) {
            return;
        }
        m_value = toValue(val);
    }

    void TFHEBasicPredicate::setValue(int val) {
        if (isKnown()) {
            return;
        }
        m_value = val;
    }

    void TFHEBasicPredicate::setValueIf(bool val, bool cond) {
        if (cond) {
            setValue(val);
        }
    }

    void TFHEBasicPredicate::require(bool cond) {
        if (!cond) {
            setValue(FALSE);
        }
    }

    void TFHEBasicPredicate::requireCovers(const TFHEEnvelope &a, const TFHEEnvelope &b) {
        require(a.covers(b).decrypt());
    }

    bool TFHEBasicPredicate::isIntersection(Location locA, Location locB) {
        return locA != Location::EXTERIOR && locB != Location::EXTERIOR;
    }

    bool TFHEBasicPredicate::isKnown() const {
        return isKnown(m_value);
    }

    TFHEBool TFHEBasicPredicate::value() const {
         if (toBoolean(m_value)) {
             return TFHEBool::tfhe_true;
         } else {
             return TFHEBool::tfhe_false;
         }
    }
}  // namespace SpatialFHE::operation::relateng
