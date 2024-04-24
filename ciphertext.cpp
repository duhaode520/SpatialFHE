#include "ciphertext.h"

using namespace std;
namespace SpatialFHE {
    CipherText::CipherText() {}

    CipherText::CipherText(CipherText const &other) {
        this->data = other.data;
        this->data_source = other.data_source;
    }

    CipherText::CipherText(CipherText &&other) {
        this->data = other.data;
        this->data_source = other.data_source;
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

    CipherText SpatialFHE::CipherText::operator=(CipherText const &other) {
        if (this != &other) {
            this->data = other.data;
            this->data_source = other.data_source;
        }
        return *this;
    }

    CipherText SpatialFHE::CipherText::operator=(CipherText &&other) {
        if (this != &other) {
            this->data = other.data;
            this->data_source = other.data_source;
        }
        return *this;
    }

    CipherText::~CipherText() {}

}  // namespace SpatialFHE
