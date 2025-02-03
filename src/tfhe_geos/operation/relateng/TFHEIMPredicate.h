//
// Created by ubuntu on 1/15/25.
//

#ifndef TFHEIMPREDICATE_H
#define TFHEIMPREDICATE_H
#include <tfhe_geos/geom/TFHEIntersectionMatrix.h>

#include "TFHEBasicPredicate.h"

namespace SpatialFHE::operation::relateng {

    class TFHEIMPredicate : public TFHEBasicPredicate {
    protected:
        using Dimension = geom::Dimension;
        using TFHEIntersectionMatrix = geom::TFHEIntersectionMatrix;
        int dimA, dimB;
        TFHEIntersectionMatrix intMatrix;

        static constexpr int DIM_UNKNOWN = Dimension::DONTCARE;

        /**
         * Gets the value of the predicate according to the current
         * intersection matrix state.
         *
         * @return the current predicate value
         */
        virtual bool valueIM() = 0;

        /**
         * Tests whether predicate evaluation can be short-circuited
         * due to the current state of the matrix providing
         * enough information to determine the predicate value.
         *
         * If this value is true then valueIM()
         * must provide the correct result of the predicate.
         *
         * @return true if the predicate value is determined
         */
        virtual bool isDetermined() const = 0;

        /**
         * Tests whether the exterior of the specified input geometry
         * is intersected by any part of the other input.
         *
         * @param isA the input geometry
         * @return true if the input geometry exterior is intersected
         */
        bool intersectsExteriorOf(bool isA) const;

        bool isIntersects(Location locA, Location locB) const;

    public:
        TFHEIMPredicate() {
            // intMatrix = new IntersectionMatrix();
            //-- E/E is always dim = 2
            intMatrix.set(Location::EXTERIOR, Location::EXTERIOR, Dimension::A);
        }

        static bool isDimsCompatibleWithCovers(int dim0, int dim1);

        void init(int dA, int dB) override;

        void updateDimension(Location locA, Location locB, int dimension) override;

        bool isDimChanged(Location locA, Location locB, int dimension) const;

        using TFHETopologyPredicate::isKnown;
        bool isKnown(Location locA, Location locB) const;

        bool isDimension(Location locA, Location locB, int dimension) const;

        int getDimension(Location locA, Location locB) const;

        /**
         * Sets the final value based on the state of the IM.
         */
        void finish() override;
    };

}  // namespace SpatialFHE::operation::relateng

#endif  // TFHEIMPREDICATE_H
