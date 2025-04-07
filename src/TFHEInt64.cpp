
#include "TFHEInt64.h"

#include <cuda_runtime_api.h>
#include <driver_types.h>

#include <c++/9/stdexcept>
#include <cstddef>
#include <cstdint>

#include "tfhe.h"

namespace SpatialFHE {

    void TFHEInt64::registerContext(TFHEContext *ctx) {
        TFHERegisteredType::registerContext(ctx);
    }

    std::vector<uint8_t> TFHEInt64::serialize(const TFHEInt64 &item) {
        if (item.data == nullptr) {
            return {};
        }

        DynamicBuffer buffer;
        CompressedFheInt64 *compressed;
        fhe_int64_compress(item.data, &compressed);
        compressed_fhe_int64_serialize(compressed, &buffer);
        std::vector<uint8_t> result(buffer.pointer, buffer.pointer + buffer.length);
        result.push_back(item.trivial);
        return result;
    }

    TFHEInt64 TFHEInt64::deserialize(std::vector<uint8_t> &data) {
        TFHEInt64 result;
        if (data.empty()) {
            return result;
        }

        DynamicBufferView view;
        view.pointer = data.data();
        view.length = data.size() - 1;
        CompressedFheInt64 *compressed;
        compressed_fhe_int64_deserialize(view, &compressed);
        compressed_fhe_int64_decompress(compressed, &result.data);

#ifdef DEBUG
        // 如果在DEBUG模式下，可能需要设置原始值
        // 但由于序列化数据中不包含原始值，这里只能设置默认值
        result.ori = 0;
#endif
        result.trivial = data.back();
        return result;
    }

    const TFHEContext *TFHEInt64::javaGetContext() {
        return context;
    }

    TFHEInt64::TFHEInt64(int64_t value, bool trivial) {
        data = nullptr;
#ifdef DEBUG
        this->trivial = true;
        ori = value;
#else
        this->trivial = trivial;
#endif
        if (this->trivial) {
            // GPU key cannot encrypt and decrypt trivial values
#ifdef WITH_FEATURE_GPU
            std::thread([&]() {
                context->setServerKey();
                fhe_int64_try_encrypt_trivial_i64(value, &data);
            }).join();
#else
            fhe_int64_try_encrypt_trivial_i64(value, &data);
#endif
        } else {
            fhe_int64_try_encrypt_with_public_key_i64(value, context->getPublicKey(), &data);
        }
    }

    TFHEInt64::TFHEInt64(FheInt64 *data) : data(data) {
#ifdef DEBUG
        ori = 0;
        this->trivial = true;
#endif
    }

    TFHEInt64::TFHEInt64() : data(nullptr) {
#ifdef DEBUG
        ori = 0;
#endif
    }

    TFHEInt64::TFHEInt64(const TFHEInt64 &other) : TFHERegisteredType(other) {
        data = nullptr;
        fhe_int64_clone(other.data, &data);
        trivial = other.trivial;
#ifdef DEBUG
        ori = other.ori;
#endif
    }

    TFHEInt64::TFHEInt64(TFHEInt64 &&other) noexcept {
        data = other.data;
        other.data = nullptr;
        trivial = other.trivial;
#ifdef DEBUG
        ori = other.ori;
#endif
    }

    TFHEInt64 &TFHEInt64::operator=(const TFHEInt64 &other) {
        if (this == &other)
            return *this;
        fhe_int64_clone(other.data, &data);
        trivial = other.trivial;
#ifdef DEBUG
        ori = other.ori;
#endif
        return *this;
    }

    TFHEInt64 &TFHEInt64::operator=(TFHEInt64 &&other) noexcept {
        if (this == &other)
            return *this;
        data = other.data;
        other.data = nullptr;
        trivial = other.trivial;
#ifdef DEBUG
        ori = other.ori;
#endif
        return *this;
    }

    bool TFHEInt64::isNull() const {
        return data == nullptr;
    }

    TFHEInt64::~TFHEInt64() {
        if (data != nullptr) {
            fhe_int64_destroy(data);
        }
    }

    int64_t TFHEInt64::decrypt() const {
        int64_t result;
        if (trivial) {
#ifdef WITH_FEATURE_GPU
            std::thread([&]() {
                context->setServerKey();
                fhe_int64_try_decrypt_trivial(data, &result);
            }).join();
#else
            fhe_int64_try_decrypt_trivial(data, &result);
#endif
        } else {
            if (context->getClientKey() != nullptr) {
                fhe_int64_decrypt(data, context->getClientKey(), &result);
            } else {
                throw std::logic_error("Remote decryption is not supported");
            }
        }

        return result;
    }

    TFHEInt64 TFHEInt64::operator+(const TFHEInt64 &other) const {
        TFHEInt64 result;
        fhe_int64_add(data, other.data, &result.data);
#ifdef DEBUG
        result.ori = ori + other.ori;
#endif
        return result;
    }

    TFHEInt64 TFHEInt64::operator-(const TFHEInt64 &other) const {
        TFHEInt64 result;
        fhe_int64_sub(data, other.data, &result.data);
#ifdef DEBUG
        result.ori = ori - other.ori;
#endif
        return result;
    }

    TFHEInt64 TFHEInt64::operator*(const TFHEInt64 &other) const {
        TFHEInt64 result;
        fhe_int64_mul(data, other.data, &result.data);
#ifdef DEBUG
        result.ori = ori * other.ori;
#endif
        return result;
    }

    TFHEInt64 TFHEInt64::operator/(const TFHEInt64 &other) const {
        TFHEInt64 result;
        fhe_int64_div(data, other.data, &result.data);
#ifdef DEBUG
        result.ori = ori / other.ori;
#endif
        return result;
    }

    TFHEInt64 TFHEInt64::operator%(const TFHEInt64 &other) const {
        TFHEInt64 result;
        fhe_int64_rem(data, other.data, &result.data);
#ifdef DEBUG
        result.ori = ori % other.ori;
#endif
        return result;
    }

    TFHEBool TFHEInt64::operator==(const TFHEInt64 &other) const {
        TFHEBool result;
        fhe_int64_eq(data, other.data, &result.data);
#ifdef DEBUG
        result.ori = ori == other.ori;
#endif
        return result;
    }

    TFHEBool TFHEInt64::operator!=(const TFHEInt64 &other) const {
        TFHEBool result;
        fhe_int64_ne(data, other.data, &result.data);
#ifdef DEBUG
        result.ori = ori != other.ori;
#endif
        return result;
    }

    TFHEBool TFHEInt64::operator>(const TFHEInt64 &other) const {
        TFHEBool result;
        fhe_int64_gt(data, other.data, &result.data);
#ifdef DEBUG
        result.ori = ori > other.ori;
#endif
        return result;
    }

    TFHEBool TFHEInt64::operator<(const TFHEInt64 &other) const {
        TFHEBool result;
        fhe_int64_lt(data, other.data, &result.data);
#ifdef DEBUG
        result.ori = ori < other.ori;
#endif
        return result;
    }

    TFHEBool TFHEInt64::operator>=(const TFHEInt64 &other) const {
        TFHEBool result;
        fhe_int64_ge(data, other.data, &result.data);
#ifdef DEBUG
        result.ori = ori >= other.ori;
#endif
        return result;
    }

    TFHEBool TFHEInt64::operator<=(const TFHEInt64 &other) const {
        TFHEBool result;
        fhe_int64_le(data, other.data, &result.data);
#ifdef DEBUG
        result.ori = ori <= other.ori;
#endif
        return result;
    }

    TFHEInt64 TFHEInt64::operator&(const TFHEInt64 &other) const {
        TFHEInt64 result;
        fhe_int64_bitand(data, other.data, &result.data);
#ifdef DEBUG
        result.ori = ori & other.ori;
#endif
        return result;
    }

    TFHEInt64 TFHEInt64::operator|(const TFHEInt64 &other) const {
        TFHEInt64 result;
        fhe_int64_bitor(data, other.data, &result.data);
#ifdef DEBUG
        result.ori = ori | other.ori;
#endif
        return result;
    }

    TFHEInt64 TFHEInt64::operator^(const TFHEInt64 &other) const {
        TFHEInt64 result;
        fhe_int64_bitxor(data, other.data, &result.data);
#ifdef DEBUG
        result.ori = ori ^ other.ori;
#endif
        return result;
    }

    TFHEInt64 TFHEInt64::operator~() const {
        TFHEInt64 result;
        fhe_int64_not(data, &result.data);
#ifdef DEBUG
        result.ori = ~ori;
#endif
        return result;
    }

    TFHEInt64 TFHEInt64::operator-() const {
        TFHEInt64 result;
        fhe_int64_neg(data, &result.data);
#ifdef DEBUG
        result.ori = -ori;
#endif
        return result;
    }

    TFHEInt64 TFHEInt64::operator+=(const TFHEInt64 &other) {
        fhe_int64_add_assign(data, other.data);
#ifdef DEBUG
        ori += other.ori;
#endif
        return *this;
    }

    TFHEInt64 TFHEInt64::operator-=(const TFHEInt64 &other) {
        fhe_int64_sub_assign(data, other.data);
#ifdef DEBUG
        ori -= other.ori;
#endif
        return *this;
    }

    TFHEInt64 TFHEInt64::operator*=(const TFHEInt64 &other) {
        fhe_int64_mul_assign(data, other.data);
#ifdef DEBUG
        ori *= other.ori;
#endif
        return *this;
    }

    TFHEInt64 TFHEInt64::operator/=(const TFHEInt64 &other) {
        fhe_int64_div_assign(data, other.data);
#ifdef DEBUG
        ori /= other.ori;
#endif
        return *this;
    }

    TFHEInt64 TFHEInt64::operator+=(int64_t other) {
        fhe_int64_scalar_add_assign(data, other);
#ifdef DEBUG
        ori += other;
#endif
        return *this;
    }

    TFHEInt64 TFHEInt64::operator-=(int64_t other) {
        fhe_int64_scalar_sub_assign(data, other);
#ifdef DEBUG
        ori -= other;
#endif
        return *this;
    }

    TFHEInt64 TFHEInt64::operator*=(int64_t other) {
        fhe_int64_scalar_mul_assign(data, other);
#ifdef DEBUG
        ori *= other;
#endif
        return *this;
    }

    TFHEInt64 TFHEInt64::operator/=(int64_t other) {
        fhe_int64_scalar_div_assign(data, other);
#ifdef DEBUG
        ori /= other;
#endif
        return *this;
    }

    bool TFHEInt64::eqTrivial(int64_t other) const {
        if (this->isTrivial()) {
            return decrypt() == other;
        }
        return (*this == other).decrypt();
    }

    bool TFHEInt64::gtTrivial(int64_t other) const {
        if (this->isTrivial()) {
            return decrypt() > other;
        }
        return (*this > other).decrypt();
    }

    bool TFHEInt64::geTrivial(int64_t other) const {
        if (this->isTrivial()) {
            return decrypt() >= other;
        }
        return (*this >= other).decrypt();
    }

    bool TFHEInt64::ltTrivial(int64_t other) const {
        if (this->isTrivial()) {
            return decrypt() < other;
        }
        return (*this < other).decrypt();
    }

    bool TFHEInt64::leTrivial(int64_t other) const {
        if (this->isTrivial()) {
            return decrypt() <= other;
        }
        return (*this <= other).decrypt();
    }

    bool TFHEInt64::neTrivial(int64_t other) const {
        if (this->isTrivial()) {
            return decrypt() != other;
        }
        return (*this != other).decrypt();
    }

    TFHEInt64 TFHEInt64::operator+(int64_t other) const {
        TFHEInt64 result;
        fhe_int64_scalar_add(data, other, &result.data);
#ifdef DEBUG
        result.ori = ori + other;
#endif
        return result;
    }

    TFHEInt64 TFHEInt64::operator-(int64_t other) const {
        TFHEInt64 result;
        fhe_int64_scalar_sub(data, other, &result.data);
#ifdef DEBUG
        result.ori = ori - other;
#endif
        return result;
    }

    TFHEInt64 TFHEInt64::operator*(int64_t other) const {
        TFHEInt64 result;
        fhe_int64_scalar_mul(data, other, &result.data);
#ifdef DEBUG
        result.ori = ori * other;
#endif
        return result;
    }

    TFHEInt64 TFHEInt64::operator/(int64_t other) const {
        TFHEInt64 result;
        fhe_int64_scalar_div(data, other, &result.data);
#ifdef DEBUG
        result.ori = ori / other;
#endif
        return result;
    }

    TFHEInt64 TFHEInt64::operator%(int64_t other) const {
        TFHEInt64 result;
        fhe_int64_scalar_rem(data, other, &result.data);
#ifdef DEBUG
        result.ori = ori % other;
#endif
        return result;
    }

    TFHEBool TFHEInt64::operator==(int64_t other) const {
        TFHEBool result;
        fhe_int64_scalar_eq(data, other, &result.data);
#ifdef DEBUG
        result.ori = ori == other;
#endif
        return result;
    }

    TFHEBool TFHEInt64::operator!=(int64_t other) const {
        TFHEBool result;
        fhe_int64_scalar_ne(data, other, &result.data);
#ifdef DEBUG
        result.ori = ori != other;
#endif
        return result;
    }

    TFHEBool TFHEInt64::operator>(int64_t other) const {
        TFHEBool result;
        fhe_int64_scalar_gt(data, other, &result.data);
#ifdef DEBUG
        result.ori = ori > other;
#endif
        return result;
    }

    TFHEBool TFHEInt64::operator<(int64_t other) const {
        TFHEBool result;
        fhe_int64_scalar_lt(data, other, &result.data);
#ifdef DEBUG
        result.ori = ori < other;
#endif
        return result;
    }

    TFHEBool TFHEInt64::operator>=(int64_t other) const {
        TFHEBool result;
        fhe_int64_scalar_ge(data, other, &result.data);
#ifdef DEBUG
        result.ori = ori >= other;
#endif
        return result;
    }

    TFHEBool TFHEInt64::operator<=(int64_t other) const {
        TFHEBool result;
        fhe_int64_scalar_le(data, other, &result.data);
#ifdef DEBUG
        result.ori = ori <= other;
#endif
        return result;
    }

    TFHEInt64 TFHEInt64::operator&(int64_t other) const {
        TFHEInt64 result;
        fhe_int64_scalar_bitand(data, other, &result.data);
#ifdef DEBUG
        result.ori = ori & other;
#endif
        return result;
    }

    TFHEInt64 TFHEInt64::operator|(int64_t other) const {
        TFHEInt64 result;
        fhe_int64_scalar_bitor(data, other, &result.data);
#ifdef DEBUG
        result.ori = ori | other;
#endif
        return result;
    }

    TFHEInt64 TFHEInt64::operator^(int64_t other) const {
        TFHEInt64 result;
        fhe_int64_scalar_bitxor(data, other, &result.data);
#ifdef DEBUG
        result.ori = ori ^ other;
#endif
        return result;
    }

    TFHEInt64 TFHEInt64::operator<<(int64_t other) const {
        TFHEInt64 result;
        fhe_int64_scalar_shl(data, other, &result.data);
#ifdef DEBUG
        result.ori = ori << other;
#endif
        return result;
    }

    TFHEInt64 TFHEInt64::operator>>(int64_t other) const {
        TFHEInt64 result;
        fhe_int64_scalar_shr(data, other, &result.data);
#ifdef DEBUG
        result.ori = ori >> other;
#endif
        return result;
    }

    TFHEInt64 TFHEInt64::max(const TFHEInt64 &a, const TFHEInt64 &b) {
        TFHEInt64 result;
        fhe_int64_max(a.data, b.data, &result.data);
#ifdef DEBUG
        result.ori = std::max(a.ori, b.ori);
#endif
        return result;
    }

    TFHEInt64 TFHEInt64::min(const TFHEInt64 &a, const TFHEInt64 &b) {
        TFHEInt64 result;
        fhe_int64_min(a.data, b.data, &result.data);
#ifdef DEBUG
        result.ori = std::min(a.ori, b.ori);
#endif
        return result;
    }
}  // namespace SpatialFHE