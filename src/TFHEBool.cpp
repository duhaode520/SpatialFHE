//
// Created by ubuntu on 1/7/25.
//

#include "TFHEBool.h"

#include <c++/9/stdexcept>

#include "TFHEInt32.h"
#include "tfhe.h"

namespace SpatialFHE {
    TFHEBool TFHEBool::tfhe_true;
    TFHEBool TFHEBool::tfhe_false;

    std::vector<bool> TFHEBool::doDecrypt(std::vector<std::vector<uint8_t>> &data) {
        std::vector<FheBool *> fhe_data(data.size());
        for (int i = 0; i < data.size(); i++) {
            fhe_bool_deserialize(vectorToDynamicBufferView(data[i]), &fhe_data[i]);
        }
        std::vector<bool> result(data.size());
        for (int i = 0; i < data.size(); i++) {
            bool tmp;
            fhe_bool_decrypt(fhe_data[i], context->getClientKey(), &tmp);
            result[i] = tmp;
        }
        return result;
    }

    bool TFHEBool::remoteDecrypt() const {
        TFHEBool confused = !(*this);
        int choice = std::rand() % 2;

        std::vector<DynamicBuffer> buffers(2);
        fhe_bool_serialize(data, &(buffers[choice]));
        fhe_bool_serialize(confused.data, &(buffers[1 - choice]));
        std::vector<std::vector<uint8_t>> data(buffers.size());
        for (int i = 0; i < buffers.size(); i++) {
            data[i] = dynamicBufferToVector(buffers[i]);
        }

        auto result = context->getRpcClient()->call("decryptBool", data);
        auto decrypted = result.as<std::vector<bool>>();
        return decrypted[choice];
    }

    TFHEBool::TFHEBool(bool value, bool trivial) {
        data = nullptr;
#ifdef DEBUG
        this->trivial = true;
        ori = value;
#else
        this->trivial = trivial;
#endif
        if (trivial) {
            fhe_bool_try_encrypt_trivial_bool(value, &data);
        } else {
            fhe_bool_try_encrypt_with_public_key_bool(value, context->getPublicKey(), &data);
        }
    }

    TFHEBool::TFHEBool(FheBool *data) : data(data) {}

    TFHEBool::TFHEBool() : data(nullptr) {
#ifdef DEBUG
        this->trivial = true;
        ori = false;
#endif
    }

    TFHEBool::TFHEBool(const TFHEBool &other) {
        if (this == &other) {
            return;
        }

#ifdef DEBUG
        ori = other.ori;
#endif

        if (this == &tfhe_false || this == &tfhe_true) {
            return;
        }
        trivial = other.trivial;
        data = nullptr;

        fhe_bool_clone(other.data, &data);
    }

    TFHEBool::TFHEBool(TFHEBool &&other) noexcept {
        data = other.data;
#ifdef DEBUG
        ori = other.ori;
#endif
        if (&other != &tfhe_false && &other != &tfhe_true) {
            other.data = nullptr;
            trivial = other.trivial;
        }
    }

    bool TFHEBool::isNull() const {
        return data == nullptr;
    }

    TFHEBool &TFHEBool::operator=(const TFHEBool &other) {
        if (this == &other)
            return *this;
#ifdef DEBUG
        ori = other.ori;
#endif
        if (this == &tfhe_false || this == &tfhe_true) {
            return *this;
        }
        fhe_bool_clone(other.data, &data);
        trivial = other.trivial;
        return *this;
    }

    TFHEBool &TFHEBool::operator=(TFHEBool &&other) noexcept {
        if (this == &other)
            return *this;
        data = other.data;
#ifdef DEBUG
        ori = other.ori;
#endif
        if (&other != &tfhe_false && &other != &tfhe_true) {
            other.data = nullptr;
            trivial = other.trivial;
        }
        return *this;
    }

    TFHEBool::~TFHEBool() {
        if (data != nullptr) {
            fhe_bool_destroy(data);
        }
    }

    bool TFHEBool::decrypt() const {
        bool result;

        // quick return for static true and false
        if (this->data == tfhe_true.data) {
            return true;
        } else if (this->data == tfhe_false.data) {
            return false;
        }

        if (trivial) {
            fhe_bool_try_decrypt_trivial(data, &result);
        } else {
#ifdef DEBUG
            throw std::runtime_error("all should be trivial in debug");
#endif
            if (context->getClientKey() != nullptr) {
                fhe_bool_decrypt(data, context->getClientKey(), &result);
            }
            else {
                result = remoteDecrypt();
            }
        }
        return result;
    }

    TFHEBool TFHEBool::operator&(const TFHEBool &other) const {
        TFHEBool result;
        fhe_bool_bitand(this->data, other.data, &result.data);
#ifdef DEBUG
        result.ori = this->ori & other.ori;
#endif
        return result;
    }

    TFHEBool TFHEBool::operator|(const TFHEBool &other) const {
        TFHEBool result;
        fhe_bool_bitor(this->data, other.data, &result.data);
#ifdef DEBUG
        result.ori = this->ori | other.ori;
#endif
        return result;
    }

    TFHEBool TFHEBool::operator^(const TFHEBool &other) const {
        TFHEBool result;
        fhe_bool_bitxor(this->data, other.data, &result.data);
#ifdef DEBUG
        result.ori = this->ori ^ other.ori;
#endif
        return result;
    }

    TFHEBool TFHEBool::operator!() const {
        TFHEBool result;
        fhe_bool_not(this->data, &result.data);
#ifdef DEBUG
        result.ori = !this->ori;
#endif
        return result;
    }

    TFHEBool TFHEBool::operator==(const TFHEBool &other) const {
        TFHEBool result;
        fhe_bool_eq(this->data, other.data, &result.data);
#ifdef DEBUG
        result.ori = this->ori == other.ori;
#endif
        return result;
    }

    TFHEBool TFHEBool::operator!=(const TFHEBool &other) const {
        TFHEBool result;
        fhe_bool_ne(this->data, other.data, &result.data);
#ifdef DEBUG
        result.ori = this->ori != other.ori;
#endif
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
#ifdef DEBUG
        result.ori = this->ori & other;
#endif
        return result;
    }

    TFHEBool TFHEBool::operator|(bool other) const {
        TFHEBool result;
        fhe_bool_scalar_bitor(this->data, other, &result.data);
#ifdef DEBUG
        result.ori = this->ori | other;
#endif
        return result;
    }

    TFHEBool TFHEBool::operator^(bool other) const {
        TFHEBool result;
        fhe_bool_scalar_bitxor(this->data, other, &result.data);
#ifdef DEBUG
        result.ori = this->ori ^ other;
#endif
        return result;
    }

    TFHEBool TFHEBool::operator==(bool other) const {
        TFHEBool result;
        fhe_bool_scalar_eq(this->data, other, &result.data);
#ifdef DEBUG
        result.ori = this->ori == other;
#endif
        return result;
    }

    TFHEBool TFHEBool::operator!=(bool other) const {
        TFHEBool result;
        fhe_bool_scalar_ne(this->data, other, &result.data);
#ifdef DEBUG
        result.ori = this->ori != other;
#endif
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
#ifdef DEBUG
        result.ori = this->ori;
#endif
        return result;
    }

    void TFHEBool::init() {
        if (tfhe_true.isNull()) {
            tfhe_true = TFHEBool(true, true);
        }
        if (tfhe_false.isNull()) {
            tfhe_false = TFHEBool(false, true);
        }
        if (context->getRpcServer() != nullptr) {
            context->getRpcServer()->bind("decryptBool", &TFHEBool::doDecrypt);
        }
    }

    void TFHEBool::registerContext(TFHEContext *ctx) {
        TFHERegisteredType::registerContext(ctx);
        init();
    }

    bool TFHEBool::isLocalTrue(const TFHEBool &other) {
        return &tfhe_true == &other;
    }

    bool TFHEBool::isLocalFalse(const TFHEBool &other) {
        return &tfhe_false == &other;
    }
}  // namespace SpatialFHE