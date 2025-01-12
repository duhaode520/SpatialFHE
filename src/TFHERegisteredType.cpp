//
// Created by ubuntu on 1/9/25.
//

#include "TFHERegisteredType.h"

namespace SpatialFHE {
    template <typename T>
    TFHEContext* TFHERegisteredType<T>::context = nullptr;

    template<>
    TFHEContext *TFHERegisteredType<int>::context = nullptr;
    template<>
    TFHEContext *TFHERegisteredType<bool>::context = nullptr;

} // SpaitalFHE