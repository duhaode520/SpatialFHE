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

    DynamicBufferView vectorToDynamicBufferView(const std::vector<uint8_t> &vec) {
        DynamicBufferView bufferView;
        bufferView.pointer = const_cast<uint8_t *>(vec.data());
        bufferView.length = vec.size();
        return bufferView;
    }

    std::vector<uint8_t> dynamicBufferToVector(const DynamicBuffer &buffer) {
        std::vector<uint8_t> vec(buffer.length);
        std::memcpy(vec.data(), buffer.pointer, buffer.length);
        return vec;
    }

} // SpaitalFHE