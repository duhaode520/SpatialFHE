//
// Created by ubuntu on 1/9/25.
//

#ifndef TFHEREGISTEREDTYPE_H
#define TFHEREGISTEREDTYPE_H
#include "TFHEContext.h"


namespace SpatialFHE {

    template <typename T>
    class TFHERegisteredType {
    protected:
        static TFHEContext* context;

    public:
        static void registerContext(TFHEContext* ctx) {
            if (context == nullptr) {
                context = ctx;
            }
        }

        virtual ~TFHERegisteredType() = default;

        virtual T decrypt() = 0;
    };

} // SpaitalFHE

#endif //TFHEREGISTEREDTYPE_H
