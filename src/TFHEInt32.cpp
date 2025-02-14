//
// Created by ubuntu on 1/7/25.
//

#include "TFHEInt32.h"

#include <driver_types.h>

#include <c++/9/stdexcept>

#include "tfhe.h"

namespace SpatialFHE {
    TFHEInt32::TFHEInt32(int32_t value) {
        data = nullptr;
#ifdef DEBUG
        fhe_int32_try_encrypt_trivial_i32(value, &data);
        ori = value;
#else
        fhe_int32_try_encrypt_with_public_key_i32(value, context->getPublicKey(), &data);
#endif
    }

    TFHEInt32::TFHEInt32(FheInt32 *data) : data(data) {}

    TFHEInt32::TFHEInt32() : data(nullptr) {
#ifdef DEBUG
        ori = 0;
#endif
    }

    TFHEInt32::TFHEInt32(const TFHEInt32 &other) {
        data = nullptr;
        fhe_int32_clone(other.data, &data);
#ifdef DEBUG
        ori = other.ori;
#endif
    }

    TFHEInt32::TFHEInt32(TFHEInt32 &&other) noexcept {
        data = other.data;
        other.data = nullptr;
#ifdef DEBUG
        ori = other.ori;
#endif
    }

    TFHEInt32 &TFHEInt32::operator=(const TFHEInt32 &other) {
        if (this == &other)
            return *this;
        fhe_int32_clone(other.data, &data);
#ifdef DEBUG
        ori = other.ori;
#endif
        return *this;
    }

    TFHEInt32 &TFHEInt32::operator=(TFHEInt32 &&other) noexcept {
        if (this == &other)
            return *this;
        data = other.data;
        other.data = nullptr;
#ifdef DEBUG
        ori = other.ori;
#endif
        return *this;
    }

    bool TFHEInt32::isNull() const {
        return data == nullptr;
    }

    TFHEInt32::~TFHEInt32() {
        if (data != nullptr) {
            fhe_int32_destroy(data);
        }
    }

    int TFHEInt32::decrypt() {
        int result;
        if (context->getClientKey() != nullptr) {
#ifdef DEBUG
            fhe_int32_try_decrypt_trivial(data, &result);
#else
            fhe_int32_decrypt(data, context->getSecretKey(), &result);
#endif
        } else {
            throw std::logic_error("Remote decryption is not supported");
        }

        return result;
    }

    TFHEInt32 TFHEInt32::operator+(const TFHEInt32 &other) const {
        TFHEInt32 result;
        fhe_int32_add(data, other.data, &result.data);
#ifdef DEBUG
        result.ori = ori + other.ori;
#endif
        return result;
    }

    TFHEInt32 TFHEInt32::operator-(const TFHEInt32 &other) const {
        TFHEInt32 result;
        fhe_int32_sub(data, other.data, &result.data);
#ifdef DEBUG
        result.ori = ori - other.ori;
#endif
        return result;
    }

    TFHEInt32 TFHEInt32::operator*(const TFHEInt32 &other) const {
        TFHEInt32 result;
        fhe_int32_mul(data, other.data, &result.data);
#ifdef DEBUG
        result.ori = ori * other.ori;
#endif
        return result;
    }

    TFHEInt32 TFHEInt32::operator/(const TFHEInt32 &other) const {
        TFHEInt32 result;
        fhe_int32_div(data, other.data, &result.data);
#ifdef DEBUG
        result.ori = ori / other.ori;
#endif
        return result;
    }

    TFHEInt32 TFHEInt32::operator%(const TFHEInt32 &other) const {
        TFHEInt32 result;
        fhe_int32_rem(data, other.data, &result.data);
#ifdef DEBUG
        result.ori = ori % other.ori;
#endif
        return result;
    }

    TFHEBool TFHEInt32::operator==(const TFHEInt32 &other) const {
        TFHEBool result;
        fhe_int32_eq(data, other.data, &result.data);
#ifdef DEBUG
        result.ori = ori == other.ori;
#endif
        return result;
    }

    TFHEBool TFHEInt32::operator!=(const TFHEInt32 &other) const {
        TFHEBool result;
        fhe_int32_ne(data, other.data, &result.data);
#ifdef DEBUG
        result.ori = ori != other.ori;
#endif
        return result;
    }

    TFHEBool TFHEInt32::operator>(const TFHEInt32 &other) const {
        TFHEBool result;
        fhe_int32_gt(data, other.data, &result.data);
#ifdef DEBUG
        result.ori = ori > other.ori;
#endif
        return result;
    }

    TFHEBool TFHEInt32::operator<(const TFHEInt32 &other) const {
        TFHEBool result;
        fhe_int32_lt(data, other.data, &result.data);
#ifdef DEBUG
        result.ori = ori < other.ori;
#endif
        return result;
    }

    TFHEBool TFHEInt32::operator>=(const TFHEInt32 &other) const {
        TFHEBool result;
        fhe_int32_ge(data, other.data, &result.data);
#ifdef DEBUG
        result.ori = ori >= other.ori;
#endif
        return result;
    }

    TFHEBool TFHEInt32::operator<=(const TFHEInt32 &other) const {
        TFHEBool result;
        fhe_int32_le(data, other.data, &result.data);
#ifdef DEBUG
        result.ori = ori <= other.ori;
#endif
        return result;
    }

    TFHEInt32 TFHEInt32::operator&(const TFHEInt32 &other) const {
        TFHEInt32 result;
        fhe_int32_bitand(data, other.data, &result.data);
#ifdef DEBUG
        result.ori = ori & other.ori;
#endif
        return result;
    }

    TFHEInt32 TFHEInt32::operator|(const TFHEInt32 &other) const {
        TFHEInt32 result;
        fhe_int32_bitor(data, other.data, &result.data);
#ifdef DEBUG
        result.ori = ori | other.ori;
#endif
        return result;
    }

    TFHEInt32 TFHEInt32::operator^(const TFHEInt32 &other) const {
        TFHEInt32 result;
        fhe_int32_bitxor(data, other.data, &result.data);
#ifdef DEBUG
        result.ori = ori ^ other.ori;
#endif
        return result;
    }

    TFHEInt32 TFHEInt32::operator~() const {
        TFHEInt32 result;
        fhe_int32_not(data, &result.data);
#ifdef DEBUG
        result.ori = ~ori;
#endif
        return result;
    }

    TFHEInt32 TFHEInt32::operator+=(const TFHEInt32 &other) {
        fhe_int32_add_assign(data, other.data);
#ifdef DEBUG
        ori += other.ori;
#endif
        return *this;
    }

    TFHEInt32 TFHEInt32::operator-=(const TFHEInt32 &other) {
        fhe_int32_sub_assign(data, other.data);
#ifdef DEBUG
        ori -= other.ori;
#endif
        return *this;
    }

    TFHEInt32 TFHEInt32::operator*=(const TFHEInt32 &other) {
        fhe_int32_mul_assign(data, other.data);
#ifdef DEBUG
        ori *= other.ori;
#endif
        return *this;
    }

    TFHEInt32 TFHEInt32::operator/=(const TFHEInt32 &other) {
        fhe_int32_div_assign(data, other.data);
#ifdef DEBUG
        ori /= other.ori;
#endif
        return *this;
    }

    TFHEInt32 TFHEInt32::operator+=(int32_t other) {
        fhe_int32_scalar_add_assign(data, other);
#ifdef DEBUG
        ori += other;
#endif
        return *this;
    }

    TFHEInt32 TFHEInt32::operator-=(int32_t other) {
        fhe_int32_scalar_sub_assign(data, other);
#ifdef DEBUG
        ori -= other;
#endif
        return *this;
    }

    TFHEInt32 TFHEInt32::operator*=(int32_t other) {
        fhe_int32_scalar_mul_assign(data, other);
#ifdef DEBUG
        ori *= other;
#endif
        return *this;
    }

    TFHEInt32 TFHEInt32::operator/=(int32_t other) {
        fhe_int32_scalar_div_assign(data, other);
#ifdef DEBUG
        ori /= other;
#endif
        return *this;
    }

    TFHEInt32 TFHEInt32::operator+(int32_t other) const {
        TFHEInt32 result;
        fhe_int32_scalar_add(data, other, &result.data);
#ifdef DEBUG
        result.ori = ori + other;
#endif
        return result;
    }

    TFHEInt32 TFHEInt32::operator-(int32_t other) const {
        TFHEInt32 result;
        fhe_int32_scalar_sub(data, other, &result.data);
#ifdef DEBUG
        result.ori = ori - other;
#endif
        return result;
    }

    TFHEInt32 TFHEInt32::operator*(int32_t other) const {
        TFHEInt32 result;
        fhe_int32_scalar_mul(data, other, &result.data);
#ifdef DEBUG
        result.ori = ori * other;
#endif
        return result;
    }

    TFHEInt32 TFHEInt32::operator/(int32_t other) const {
        TFHEInt32 result;
        fhe_int32_scalar_div(data, other, &result.data);
#ifdef DEBUG
        result.ori = ori / other;
#endif
        return result;
    }

    TFHEInt32 TFHEInt32::operator%(int32_t other) const {
        TFHEInt32 result;
        fhe_int32_scalar_rem(data, other, &result.data);
#ifdef DEBUG
        result.ori = ori % other;
#endif
        return result;
    }

    TFHEBool TFHEInt32::operator==(int32_t other) const {
        TFHEBool result;
        fhe_int32_scalar_eq(data, other, &result.data);
#ifdef DEBUG
        result.ori = ori == other;
#endif
        return result;
    }

    TFHEBool TFHEInt32::operator!=(int32_t other) const {
        TFHEBool result;
        fhe_int32_scalar_ne(data, other, &result.data);
#ifdef DEBUG
        result.ori = ori != other;
#endif
        return result;
    }

    TFHEBool TFHEInt32::operator>(int32_t other) const {
        TFHEBool result;
        fhe_int32_scalar_gt(data, other, &result.data);
#ifdef DEBUG
        result.ori = ori > other;
#endif
        return result;
    }

    TFHEBool TFHEInt32::operator<(int32_t other) const {
        TFHEBool result;
        fhe_int32_scalar_lt(data, other, &result.data);
#ifdef DEBUG
        result.ori = ori < other;
#endif
        return result;
    }

    TFHEBool TFHEInt32::operator>=(int32_t other) const {
        TFHEBool result;
        fhe_int32_scalar_ge(data, other, &result.data);
#ifdef DEBUG
        result.ori = ori >= other;
#endif
        return result;
    }

    TFHEBool TFHEInt32::operator<=(int32_t other) const {
        TFHEBool result;
        fhe_int32_scalar_le(data, other, &result.data);
#ifdef DEBUG
        result.ori = ori <= other;
#endif
        return result;
    }

    TFHEInt32 TFHEInt32::operator&(int32_t other) const {
        TFHEInt32 result;
        fhe_int32_scalar_bitand(data, other, &result.data);
#ifdef DEBUG
        result.ori = ori & other;
#endif
        return result;
    }

    TFHEInt32 TFHEInt32::operator|(int32_t other) const {
        TFHEInt32 result;
        fhe_int32_scalar_bitor(data, other, &result.data);
#ifdef DEBUG
        result.ori = ori | other;
#endif
        return result;
    }

    TFHEInt32 TFHEInt32::operator^(int32_t other) const {
        TFHEInt32 result;
        fhe_int32_scalar_bitxor(data, other, &result.data);
#ifdef DEBUG
        result.ori = ori ^ other;
#endif
        return result;
    }

    TFHEInt32 TFHEInt32::operator<<(int32_t other) const {
        TFHEInt32 result;
        fhe_int32_scalar_shl(data, other, &result.data);
#ifdef DEBUG
        result.ori = ori << other;
#endif
        return result;
    }

    TFHEInt32 TFHEInt32::operator>>(int32_t other) const {
        TFHEInt32 result;
        fhe_int32_scalar_shr(data, other, &result.data);
#ifdef DEBUG
        result.ori = ori >> other;
#endif
        return result;
    }

    //
    // TFHEInt32 TFHEInt32::operator<<(const TFHEInt32 &other) {
    //     TFHEInt32 result;
    //     fhe_int32_shl(data, other.data, &result.data);
    //     return result;
    // }
    //
    // TFHEInt32 TFHEInt32::operator>>(const TFHEInt32 &other) {
    //     TFHEInt32 result;
    //     fhe_int32_shr(data, other.data, &result.data);
    //     return result;
    // }

    TFHEInt32 TFHEInt32::max(const TFHEInt32 &a, const TFHEInt32 &b) {
        TFHEInt32 result;
        fhe_int32_max(a.data, b.data, &result.data);
#ifdef DEBUG
        result.ori = std::max(a.ori, b.ori);
#endif
        return result;
    }

    TFHEInt32 TFHEInt32::min(const TFHEInt32 &a, const TFHEInt32 &b) {
        TFHEInt32 result;
        fhe_int32_min(a.data, b.data, &result.data);
#ifdef DEBUG
        result.ori = std::min(a.ori, b.ori);
#endif
        return result;
    }
}  // namespace SpatialFHE