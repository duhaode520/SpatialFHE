//
// Created by ubuntu on 1/9/25.
//

#ifndef TFHEREGISTEREDTYPE_H
#define TFHEREGISTEREDTYPE_H
#include "TFHEContext.h"

namespace SpatialFHE {

    template <typename T> class TFHERegisteredType {
    protected:
        static TFHEContext* context;

        bool trivial = false;

    public:
        static void registerContext(TFHEContext* ctx) {
            if (context == nullptr) {
                context = ctx;
            }
        }

        [[nodiscard]] bool isTrivial() const {
            return trivial;
        }

        virtual ~TFHERegisteredType() = default;

        virtual T decrypt() const = 0;

        friend class TFHEContext;
    };

    DynamicBufferView vectorToDynamicBufferView(const std::vector<uint8_t>& vec);

    std::vector<uint8_t> dynamicBufferToVector(const DynamicBuffer& buffer);

}  // namespace SpatialFHE

#endif  // TFHEREGISTEREDTYPE_H
