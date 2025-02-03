//
// Created by ubuntu on 1/14/25.
//

#ifndef TFHERELATEMATRIXPREDICATE_H
#define TFHERELATEMATRIXPREDICATE_H
#include "TFHEIMPredicate.h"
#include <memory>

namespace SpatialFHE::operation::relateng {

    class TFHERelateMatrixPredicate : public TFHEIMPredicate {
    public:
        TFHERelateMatrixPredicate() {};

        std::string name() const override {
            return "relateMatrix";
        };

        bool requireInteraction() const override {
            //-- ensure entire matrix is computed
            return false;
        };

        bool isDetermined() const override {
            //-- ensure entire matrix is computed
            return false;
        };

        bool valueIM() override {
            //-- indicates full matrix is being evaluated
            return false;
        };

        /**
        * Gets the current state of the IM matrix (which may only be partially complete).
        *
        * @return the IM matrix
        */
        std::unique_ptr<TFHEIntersectionMatrix> getIM() {
            return std::make_unique<TFHEIntersectionMatrix>(intMatrix);
        }
    };

}  // namespace SpatialFHE::operation::relateng

#endif  // TFHERELATEMATRIXPREDICATE_H
