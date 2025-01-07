//
// Created by ubuntu on 1/7/25.
//

#include "TFHEInt32.h"
#include "tfhe.h"

namespace SpatialFHE {
    TFHEInt32::TFHEInt32(PublicKey *public_key, int32_t value) {
        data = nullptr;
        fhe_int32_try_encrypt_with_public_key_i32(value, public_key, &data);
    }
    TFHEInt32::TFHEInt32(FheInt32 *data) : data(data){}
    TFHEInt32::TFHEInt32() : data(nullptr) {}
    TFHEInt32::TFHEInt32(const TFHEInt32 &other) {
        data = nullptr;
        fhe_int32_clone(other.data, &data);
    }
    TFHEInt32::TFHEInt32(TFHEInt32 &&other) noexcept {
        data = other.data;
    }
    TFHEInt32 &TFHEInt32::operator=(const TFHEInt32 &other) {
        if (this == &other)
            return *this;
        fhe_int32_clone(other.data, &data);
        return *this;
    }
    TFHEInt32 &TFHEInt32::operator=(TFHEInt32 &&other) noexcept {
        if (this == &other)
            return *this;
        data = other.data;
        return *this;
    }

    TFHEInt32::~TFHEInt32() {
        fhe_int32_destroy(data);
    }

    TFHEInt32 TFHEInt32::operator+(const TFHEInt32 &other) {
        TFHEInt32 result;
        fhe_int32_add(data, other.data, &result.data);
        return result;
    }

    TFHEInt32 TFHEInt32::operator-(const TFHEInt32 &other) {
        TFHEInt32 result;
        fhe_int32_sub(data, other.data, &result.data);
        return result;
    }

    TFHEInt32 TFHEInt32::operator*(const TFHEInt32 &other) {
        TFHEInt32 result;
        fhe_int32_mul(data, other.data, &result.data);
        return result;
    }

    TFHEInt32 TFHEInt32::operator/(const TFHEInt32 &other) {
        TFHEInt32 result;
        fhe_int32_div(data, other.data, &result.data);
        return result;
    }

    TFHEInt32 TFHEInt32::operator%(const TFHEInt32 &other) {
        TFHEInt32 result;
        fhe_int32_rem(data, other.data, &result.data);
        return result;
    }

    TFHEBool TFHEInt32::operator==(const TFHEInt32 &other) {
        TFHEBool result;
        fhe_int32_eq(data, other.data, &result.data);
        return result;
    }

    TFHEBool TFHEInt32::operator!=(const TFHEInt32 &other) {
        TFHEBool result;
        fhe_int32_ne(data, other.data, &result.data);
        return result;
    }

    TFHEBool TFHEInt32::operator>(const TFHEInt32 &other) {
        TFHEBool result;
        fhe_int32_gt(data, other.data, &result.data);
        return result;
    }

    TFHEBool TFHEInt32::operator<(const TFHEInt32 &other) {
        TFHEBool result;
        fhe_int32_lt(data, other.data, &result.data);
        return result;
    }

    TFHEBool TFHEInt32::operator>=(const TFHEInt32 &other) {
        TFHEBool result;
        fhe_int32_ge(data, other.data, &result.data);
        return result;
    }

    TFHEBool TFHEInt32::operator<=(const TFHEInt32 &other) {
        TFHEBool result;
        fhe_int32_le(data, other.data, &result.data);
        return result;
    }

    TFHEInt32 TFHEInt32::operator&(const TFHEInt32 &other) {
        TFHEInt32 result;
        fhe_int32_bitand(data, other.data, &result.data);
        return result;
    }

    TFHEInt32 TFHEInt32::operator|(const TFHEInt32 &other) {
        TFHEInt32 result;
        fhe_int32_bitor(data, other.data, &result.data);
        return result;
    }

    TFHEInt32 TFHEInt32::operator^(const TFHEInt32 &other) {
        TFHEInt32 result;
        fhe_int32_bitxor(data, other.data, &result.data);
        return result;
    }

    TFHEInt32 TFHEInt32::operator~() {
        TFHEInt32 result;
        fhe_int32_not(data, &result.data);
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
        return result;
    }

    TFHEInt32 TFHEInt32::min(const TFHEInt32 &a, const TFHEInt32 &b) {
        TFHEInt32 result;
        fhe_int32_min(a.data, b.data, &result.data);
        return result;
    }
} // SpatialFHE