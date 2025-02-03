//
// Created by ubuntu on 1/14/25.
//

#ifndef TFHEBASICPREDICATE_H
#define TFHEBASICPREDICATE_H
#include "TFHEBool.h"
#include "TFHETopologyPredicate.h"

namespace SpatialFHE::operation::relateng {

    class TFHEBasicPredicate : public TFHETopologyPredicate {
    private:
        static constexpr int UNKNOWN = -1;
        static constexpr int FALSE = 0;
        static constexpr int TRUE = 1;


        int m_value = UNKNOWN;

        static bool isKnown(int val);
        static bool toBoolean(int val);

        static int toValue(bool val);

    protected:
        using TFHEEnvelope = geom::TFHEEnvelope;
        using Location = geom::Location;

        void setValue(bool val);

        void setValue(int val);

        void setValueIf(bool val, bool cond);

        void require(bool cond);

        using TFHETopologyPredicate::requireCovers;
        void requireCovers(const TFHEEnvelope& a, const TFHEEnvelope& b);

    public:
        /**
         * Tests if two geometries intersect
         * based on an interaction at given locations.
         *
         * @param locA the location on geometry A
         * @param locB the location on geometry B
         * @return true if the geometries intersect
         */
        static bool isIntersection(Location locA, Location locB);

        std::string name() const override = 0;

        void finish() override = 0;

        bool isKnown() const override;

        TFHEBool value() const override;
    };

}  // namespace SpatialFHE::operation::relateng

#endif  // TFHEBASICPREDICATE_H
