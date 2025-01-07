//
// Created by ubuntu on 1/7/25.
//

#include "TFHEBool.h"
#include "tfhe.h"
#include "TFHEInt32.h"

namespace SpatialFHE {
    TFHEBool::TFHEBool(PublicKey *public_key, bool value) {
        data = nullptr;
        fhe_bool_try_encrypt_with_public_key_bool(value, public_key, &data);
    }
    TFHEBool::TFHEBool(FheBool *data) : data(data) {}
    TFHEBool::TFHEBool() : data(nullptr) {}
    TFHEBool::TFHEBool(const TFHEBool &other) {
        data = nullptr;
        fhe_bool_clone(other.data, &data);
    }
    TFHEBool::TFHEBool(TFHEBool &&other)  noexcept {
        data = other.data;
    }
    TFHEBool &TFHEBool::operator=(const TFHEBool &other) {
        if (this == &other)
            return *this;
        fhe_bool_clone(other.data, &data);
        return *this;
    }
    TFHEBool &TFHEBool::operator=(TFHEBool &&other) noexcept {
        if (this == &other)
            return *this;
        data = other.data;
        return *this;
    }

    TFHEBool::~TFHEBool() {
        fhe_bool_destroy(data);
    }

    bool TFHEBool::decrypt(const ClientKey *client_key) const {
        bool result;
        fhe_bool_decrypt(data, client_key, &result);
        return result;
    }

    TFHEBool TFHEBool::operator&(const TFHEBool &other) {
        TFHEBool result;
        fhe_bool_bitand(this->data, other.data, &result.data);
        return result;
    }

    TFHEBool TFHEBool::operator|(const TFHEBool &other) {
        TFHEBool result;
        fhe_bool_bitor(this->data, other.data, &result.data);
        return result;
    }

    TFHEBool TFHEBool::operator^(const TFHEBool &other) {
        TFHEBool result;
        fhe_bool_bitxor(this->data, other.data, &result.data);
        return result;
    }

    TFHEBool TFHEBool::operator!() {
        TFHEBool result;
        fhe_bool_not(this->data, &result.data);
        return result;
    }

    TFHEBool TFHEBool::operator==(const TFHEBool &other) {
        TFHEBool result;
        fhe_bool_eq(this->data, other.data, &result.data);
        return result;
    }

    TFHEBool TFHEBool::operator!=(const TFHEBool &other) {
        TFHEBool result;
        fhe_bool_ne(this->data, other.data, &result.data);
        return result;
    }

    TFHEBool TFHEBool::operator&&(const TFHEBool &other) {
        return operator&(other);
    }

    TFHEBool TFHEBool::operator||(const TFHEBool &other) {
        return operator|(other);
    }

    TFHEBool::operator TFHEInt32() const {
        TFHEInt32 result;
        fhe_bool_cast_into_fhe_int32(this->data, &result.data);
        return result;
    }
} // SpatialFHE