#include "TFHEDecimal.h"

#include <c++/9/stdexcept>
#include <cstdint>

#include "tfhe.h"

namespace SpatialFHE {

    void TFHEDecimal::registerContext(TFHEContext *ctx) {
        TFHERegisteredType::registerContext(ctx);
    }

    std::vector<uint8_t> TFHEDecimal::serialize(const TFHEDecimal &item) {
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

    TFHEDecimal TFHEDecimal::deserialize(std::vector<uint8_t> &data) {
        TFHEDecimal result;
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

    const TFHEContext *TFHEDecimal::javaGetContext() {
        return context;
    }

    TFHEDecimal::TFHEDecimal(double value, int m_scale, bool trivial) {
        data = nullptr;
        scale = m_scale;
        value = value * (1 << scale);
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

    TFHEDecimal::TFHEDecimal() : data(nullptr) {
#ifdef DEBUG
        ori = 0;
#endif
        scale = 0;
    }

    TFHEDecimal::TFHEDecimal(const TFHEDecimal &other) : TFHERegisteredType(other) {
        data = nullptr;
        fhe_int64_clone(other.data, &data);
        trivial = other.trivial;
        scale = other.scale;
#ifdef DEBUG
        ori = other.ori;
#endif
    }

    TFHEDecimal::TFHEDecimal(TFHEDecimal &&other) noexcept {
        data = other.data;
        other.data = nullptr;
        trivial = other.trivial;
        scale = other.scale;
#ifdef DEBUG
        ori = other.ori;
#endif
    }

    TFHEDecimal &TFHEDecimal::operator=(const TFHEDecimal &other) {
        if (this == &other)
            return *this;
        fhe_int64_clone(other.data, &data);
        trivial = other.trivial;
#ifdef DEBUG
        ori = other.ori;
#endif
        return *this;
    }

    TFHEDecimal &TFHEDecimal::operator=(TFHEDecimal &&other) noexcept {
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

    bool TFHEDecimal::isNull() const {
        return data == nullptr;
    }

    TFHEDecimal::~TFHEDecimal() {
        if (data != nullptr) {
            fhe_int64_destroy(data);
        }
    }

    double TFHEDecimal::decrypt() const {
        int64_t result_int;
        if (trivial) {
#ifdef WITH_FEATURE_GPU
            std::thread([&]() {
                context->setServerKey();
                fhe_int64_try_decrypt_trivial(data, &result);
            }).join();
#else
            fhe_int64_try_decrypt_trivial(data, &result_int);
#endif
        } else {
            if (context->getClientKey() != nullptr) {
                fhe_int64_decrypt(data, context->getClientKey(), &result_int);
            } else {
                throw std::logic_error("Remote decryption is not supported");
            }
        }

        return ((double)result_int) / (1 << scale);
    }

    TFHEDecimal TFHEDecimal::operator+(const TFHEDecimal &other) const {
        TFHEDecimal result;
        assert(scale == other.scale);
        fhe_int64_add(data, other.data, &result.data);
#ifdef DEBUG
        result.ori = ori + other.ori;
#endif
        return result;
    }

    TFHEDecimal TFHEDecimal::operator-(const TFHEDecimal &other) const {
        TFHEDecimal result;
        assert(scale == other.scale);
        fhe_int64_sub(data, other.data, &result.data);
#ifdef DEBUG
        result.ori = ori - other.ori;
#endif
        return result;
    }

    TFHEDecimal TFHEDecimal::operator*(const TFHEDecimal &other) const {
        TFHEDecimal result;
        fhe_int64_mul(data, other.data, &result.data);
        fhe_int64_scalar_shr_assign(data, scale);
#ifdef DEBUG
        result.ori = ori * other.ori;
#endif
        return result;
    }

    TFHEDecimal TFHEDecimal::operator/(const TFHEDecimal &other) const {
        TFHEDecimal result;
        fhe_int64_div(data, other.data, &result.data);
        fhe_int64_scalar_shl_assign(data, scale);
#ifdef DEBUG
        result.ori = ori / other.ori;
#endif
        return result;
    }

    TFHEBool TFHEDecimal::operator==(const TFHEDecimal &other) const {
        TFHEBool result;
        fhe_int64_eq(data, other.data, &result.data);
#ifdef DEBUG
        result.ori = ori == other.ori;
#endif
        return result;
    }

    TFHEBool TFHEDecimal::operator!=(const TFHEDecimal &other) const {
        TFHEBool result;
        fhe_int64_ne(data, other.data, &result.data);
#ifdef DEBUG
        result.ori = ori != other.ori;
#endif
        return result;
    }

    TFHEBool TFHEDecimal::operator>(const TFHEDecimal &other) const {
        TFHEBool result;
        fhe_int64_gt(data, other.data, &result.data);
#ifdef DEBUG
        result.ori = ori > other.ori;
#endif
        return result;
    }

    TFHEBool TFHEDecimal::operator<(const TFHEDecimal &other) const {
        TFHEBool result;
        fhe_int64_lt(data, other.data, &result.data);
#ifdef DEBUG
        result.ori = ori < other.ori;
#endif
        return result;
    }

    TFHEBool TFHEDecimal::operator>=(const TFHEDecimal &other) const {
        TFHEBool result;
        fhe_int64_ge(data, other.data, &result.data);
#ifdef DEBUG
        result.ori = ori >= other.ori;
#endif
        return result;
    }

    TFHEBool TFHEDecimal::operator<=(const TFHEDecimal &other) const {
        TFHEBool result;
        fhe_int64_le(data, other.data, &result.data);
#ifdef DEBUG
        result.ori = ori <= other.ori;
#endif
        return result;
    }

    TFHEDecimal TFHEDecimal::operator-() const {
        TFHEDecimal result;
        fhe_int64_neg(data, &result.data);
#ifdef DEBUG
        result.ori = -ori;
#endif
        return result;
    }

    TFHEDecimal TFHEDecimal::operator+=(const TFHEDecimal &other) {
        fhe_int64_add_assign(data, other.data);
#ifdef DEBUG
        ori += other.ori;
#endif
        return *this;
    }

    TFHEDecimal TFHEDecimal::operator-=(const TFHEDecimal &other) {
        fhe_int64_sub_assign(data, other.data);
#ifdef DEBUG
        ori -= other.ori;
#endif
        return *this;
    }

    TFHEDecimal TFHEDecimal::operator*=(const TFHEDecimal &other) {
        fhe_int64_mul_assign(data, other.data);
        fhe_int64_scalar_shr_assign(data, scale);
#ifdef DEBUG
        ori *= other.ori;
#endif
        return *this;
    }

    TFHEDecimal TFHEDecimal::operator/=(const TFHEDecimal &other) {
        fhe_int64_div_assign(data, other.data);
        fhe_int64_scalar_shl_assign(data, scale);
#ifdef DEBUG
        ori /= other.ori;
#endif
        return *this;
    }

    TFHEDecimal TFHEDecimal::operator+=(double other) {
        int o = other * (1 << scale);
        fhe_int64_scalar_add_assign(data, o);
#ifdef DEBUG
        ori += other;
#endif
        return *this;
    }

    TFHEDecimal TFHEDecimal::operator-=(double other) {
        int o = other * (1 << scale);
        fhe_int64_scalar_sub_assign(data, o);
#ifdef DEBUG
        ori -= other;
#endif
        return *this;
    }

    TFHEDecimal TFHEDecimal::operator*=(double other) {
        int o = other * (1 << scale);
        fhe_int64_scalar_mul_assign(data, o);
        fhe_int64_scalar_shr_assign(data, scale);
#ifdef DEBUG
        ori *= other;
#endif
        return *this;
    }

    TFHEDecimal TFHEDecimal::operator/=(double other) {
        int o = other * (1 << scale);
        fhe_int64_scalar_div_assign(data, o);
        fhe_int64_scalar_shl_assign(data, scale);
#ifdef DEBUG
        ori /= other;
#endif
        return *this;
    }

    bool TFHEDecimal::eqTrivial(double other) const {
        if (this->isTrivial()) {
            return decrypt() == other;
        }
        return (*this == other).decrypt();
    }

    bool TFHEDecimal::gtTrivial(double other) const {
        if (this->isTrivial()) {
            return decrypt() > other;
        }
        return (*this > other).decrypt();
    }

    bool TFHEDecimal::geTrivial(double other) const {
        if (this->isTrivial()) {
            return decrypt() >= other;
        }
        return (*this >= other).decrypt();
    }

    bool TFHEDecimal::ltTrivial(double other) const {
        if (this->isTrivial()) {
            return decrypt() < other;
        }
        return (*this < other).decrypt();
    }

    bool TFHEDecimal::leTrivial(double other) const {
        if (this->isTrivial()) {
            return decrypt() <= other;
        }
        return (*this <= other).decrypt();
    }

    bool TFHEDecimal::neTrivial(double other) const {
        if (this->isTrivial()) {
            return decrypt() != other;
        }
        return (*this != other).decrypt();
    }

    TFHEDecimal TFHEDecimal::operator+(double other) const {
        TFHEDecimal result;
        int o = other * (1 << scale);
        fhe_int64_scalar_add(data, o, &result.data);
#ifdef DEBUG
        result.ori = ori + other;
#endif
        return result;
    }

    TFHEDecimal TFHEDecimal::operator-(double other) const {
        TFHEDecimal result;
        int o = other * (1 << scale);
        fhe_int64_scalar_sub(data, o, &result.data);
#ifdef DEBUG
        result.ori = ori - other;
#endif
        return result;
    }

    TFHEDecimal TFHEDecimal::operator*(double other) const {
        TFHEDecimal result;
        int o = other * (1 << scale);
        fhe_int64_scalar_mul(data, o, &result.data);
        fhe_int64_scalar_shr_assign(data, scale);
#ifdef DEBUG
        result.ori = ori * other;
#endif
        return result;
    }

    TFHEDecimal TFHEDecimal::operator/(double other) const {
        TFHEDecimal result;
        int o = other * (1 << scale);
        fhe_int64_scalar_div(data, o, &result.data);
        fhe_int64_scalar_shl_assign(data, scale);
#ifdef DEBUG
        result.ori = ori / other;
#endif
        return result;
    }

    TFHEDecimal TFHEDecimal::operator%(double other) const {
        TFHEDecimal result;
        int o = other * (1 << scale);
        fhe_int64_scalar_rem(data, o, &result.data);
#ifdef DEBUG
        result.ori = ori % other;
#endif
        return result;
    }

    TFHEBool TFHEDecimal::operator==(double other) const {
        TFHEBool result;
        int o = other * (1 << scale);
        fhe_int64_scalar_eq(data, o, &result.data);
#ifdef DEBUG
        result.ori = ori == other;
#endif
        return result;
    }

    TFHEBool TFHEDecimal::operator!=(double other) const {
        TFHEBool result;
        int o = other * (1 << scale);
        fhe_int64_scalar_ne(data, o, &result.data);
#ifdef DEBUG
        result.ori = ori != other;
#endif
        return result;
    }

    TFHEBool TFHEDecimal::operator>(double other) const {
        TFHEBool result;
        int o = other * (1 << scale);
        fhe_int64_scalar_gt(data, o, &result.data);
#ifdef DEBUG
        result.ori = ori > other;
#endif
        return result;
    }

    TFHEBool TFHEDecimal::operator<(double other) const {
        TFHEBool result;
        int o = other * (1 << scale);
        fhe_int64_scalar_lt(data, o, &result.data);
#ifdef DEBUG
        result.ori = ori < other;
#endif
        return result;
    }

    TFHEBool TFHEDecimal::operator>=(double other) const {
        TFHEBool result;
        int o = other * (1 << scale);
        fhe_int64_scalar_ge(data, o, &result.data);
#ifdef DEBUG
        result.ori = ori >= other;
#endif
        return result;
    }

    TFHEBool TFHEDecimal::operator<=(double other) const {
        TFHEBool result;
        int o = other * (1 << scale);
        fhe_int64_scalar_ge(data, o, &result.data);
#ifdef DEBUG
        result.ori = ori >= other;
#endif
        return result;
    }
    TFHEDecimal TFHEDecimal::max(const TFHEDecimal &a, const TFHEDecimal &b) {
        TFHEDecimal result;
        fhe_int64_max(a.data, b.data, &result.data);
#ifdef DEBUG
        result.ori = std::max(a.ori, b.ori);
#endif
        return result;
    }

    TFHEDecimal TFHEDecimal::min(const TFHEDecimal &a, const TFHEDecimal &b) {
        TFHEDecimal result;
        fhe_int64_min(a.data, b.data, &result.data);
#ifdef DEBUG
        result.ori = std::min(a.ori, b.ori);
#endif
        return result;
    }
}  // namespace SpatialFHE