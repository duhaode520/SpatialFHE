#include "ciphertext.h"
#include <b64/encode.h>

using namespace std;
namespace SpatialFHE {
    CipherText::CipherText() = default;

    CipherText::CipherText(CipherText const &other) {
        this->data = other.data;
        this->data_source = other.data_source;
    }

    CipherText::CipherText(CipherText &&other) noexcept {
        this->data = CipherTextData(other.data);
        this->data_source = string(other.data_source);
    }

    CipherText::CipherText(seal::Ciphertext const &ct) {
        this->data = ct;
        this->data_source = "SEAL";
    }

    CipherText::CipherText(const string &str) {
        this->data = str;
        this->data_source = "string";
    }

    void CipherText::setData(seal::Ciphertext const &ct) {
        this->data = ct;
    }

    void CipherText::setData(const string &str) {
        this->data = str;
    }

    unsigned int CipherText::size() const {
        if (std::holds_alternative<seal::Ciphertext>(this->data)) {
            return std::get<seal::Ciphertext>(this->data).size();
        } else if (std::holds_alternative<string>(this->data)) {
            return std::get<string>(this->data).size();
        } else {
            return -1;
        }
    }

    CipherTextData CipherText::getData() const {
        return this->data;
    }

    CipherText& SpatialFHE::CipherText::operator=(CipherText const &other) {
        if (this != &other) {
            this->data = other.data;
            this->data_source = other.data_source;
        }
        return *this;
    }

    CipherText& SpatialFHE::CipherText::operator=(CipherText &&other) noexcept {
        if (this != &other) {
            this->data = other.data;
            this->data_source = other.data_source;
        }
        return *this;
    }

    string CipherText::toString() {
        if (holds_alternative<string>(this->data)) {
            return get<string>(this->data);
        } else if (holds_alternative<seal::Ciphertext>(this->data)) {
            stringstream ss;
            seal::Ciphertext ct = get<seal::Ciphertext>(this->data);
            ct.save(ss);
            base64::encoder b64Encoder;
            stringstream b64ss;
            b64Encoder.encode(ss, b64ss);
            return b64ss.str();
        } else {
            return "Unknown";
        }
    }

    CipherText::~CipherText() = default;

}  // namespace SpatialFHE
