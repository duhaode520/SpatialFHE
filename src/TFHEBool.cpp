//
// Created by ubuntu on 1/7/25.
//

#include "TFHEBool.h"
#include "tfhe.h"
#include "TFHEInt32.h"

#include <c++/9/stdexcept>

namespace SpatialFHE {
    TFHEBool TFHEBool::tfhe_true;
    TFHEBool TFHEBool::tfhe_false;

    TFHEBool::TFHEBool(bool value) {
        data = nullptr;
        fhe_bool_try_encrypt_with_public_key_bool(value, context->getPublicKey(), &data);
    }
    TFHEBool::TFHEBool(FheBool *data) : data(data) {}
    TFHEBool::TFHEBool() : data(nullptr) {}
    TFHEBool::TFHEBool(const TFHEBool &other) {
        data = nullptr;
        fhe_bool_clone(other.data, &data);
    }
    TFHEBool::TFHEBool(TFHEBool &&other) noexcept {
        data = other.data;
        other.data = nullptr;
    }

    bool TFHEBool::isNull() const {
        return data == nullptr;
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
        other.data = nullptr;
        return *this;
    }

    TFHEBool::~TFHEBool() {
        if (data != nullptr) {
            fhe_bool_destroy(data);
        }
    }

    bool TFHEBool::decrypt()  {
        bool result;
        if (context->getClientKey() != nullptr) {
            fhe_bool_decrypt(data, context->getClientKey(), &result);
        } else {
            // TODO: Implement decrypting through OT.
            throw std::logic_error("Decrypting without local client key is not implemented");
        }
        return result;
    }

    TFHEBool TFHEBool::operator&(const TFHEBool &other) const {
        TFHEBool result;
        fhe_bool_bitand(this->data, other.data, &result.data);
        return result;
    }

    TFHEBool TFHEBool::operator|(const TFHEBool &other) const {
        TFHEBool result;
        fhe_bool_bitor(this->data, other.data, &result.data);
        return result;
    }

    TFHEBool TFHEBool::operator^(const TFHEBool &other) const {
        TFHEBool result;
        fhe_bool_bitxor(this->data, other.data, &result.data);
        return result;
    }

    TFHEBool TFHEBool::operator!() const {
        TFHEBool result;
        fhe_bool_not(this->data, &result.data);
        return result;
    }

    TFHEBool TFHEBool::operator==(const TFHEBool &other) const {
        TFHEBool result;
        fhe_bool_eq(this->data, other.data, &result.data);
        return result;
    }

    TFHEBool TFHEBool::operator!=(const TFHEBool &other) const {
        TFHEBool result;
        fhe_bool_ne(this->data, other.data, &result.data);
        return result;
    }

    TFHEBool TFHEBool::operator&&(const TFHEBool &other) const {
        return operator&(other);
    }

    TFHEBool TFHEBool::operator||(const TFHEBool &other) const {
        return operator|(other);
    }

    TFHEBool TFHEBool::operator&(bool other) const {
        TFHEBool result;
        fhe_bool_scalar_bitand(this->data, other, &result.data);
        return result;
    }

    TFHEBool TFHEBool::operator|(bool other) const {
        TFHEBool result;
        fhe_bool_scalar_bitor(this->data, other, &result.data);
        return result;
    }

    TFHEBool TFHEBool::operator^(bool other) const {
        TFHEBool result;
        fhe_bool_scalar_bitxor(this->data, other, &result.data);
        return result;
    }

    TFHEBool TFHEBool::operator==(bool other) const {
        TFHEBool result;
        fhe_bool_scalar_eq(this->data, other, &result.data);
        return result;
    }

    TFHEBool TFHEBool::operator!=(bool other) const {
        TFHEBool result;
        fhe_bool_scalar_ne(this->data, other, &result.data);
        return result;
    }

    TFHEBool TFHEBool::operator&&(bool other) const {
        return operator&(other);
    }

    TFHEBool TFHEBool::operator||(bool other) const {
        return operator|(other);
    }

    TFHEBool::operator TFHEInt32() const {
        TFHEInt32 result;
        fhe_bool_cast_into_fhe_int32(this->data, &result.data);
        return result;
    }
    void TFHEBool::init() {
        if (tfhe_true.isNull()) {
            tfhe_true = TFHEBool(true);
        }
        if (tfhe_true.isNull()) {
            tfhe_false = TFHEBool(false);
        }
    }
    void TFHEBool::registerContext(TFHEContext *ctx) {
        TFHERegisteredType::registerContext(ctx);
        init();
    }
} // SpatialFHE