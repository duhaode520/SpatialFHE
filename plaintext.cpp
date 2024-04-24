#include "plaintext.h"

namespace SpatialFHE {
    PlainText::PlainText() {}

    PlainText::PlainText(PlainText const &other) {
        this->data = other.data;
        this->data_source = other.data_source;
    }

    PlainText::PlainText(PlainText &&other) {
        this->data = other.data;
        this->data_source = other.data_source;
    }

    PlainText::PlainText(int const &value) {
        this->data = value;
        this->data_source = "int";
    }

    PlainText::PlainText(seal::Plaintext const &value) {
        this->data = value;
        this->data_source = "SEAL";
    }

    PlainText::PlainText(std::string const &value) {
        this->data = value;
        this->data_source = "string";
    }

    PlainText PlainText::operator=(PlainText const &other) {
        if (this != &other) {
            this->data = other.data;
            this->data_source = other.data_source;
        }
        return *this;
    }

    PlainText PlainText::operator=(PlainText &&other) {
        if (this != &other) {
            this->data = other.data;
            this->data_source = other.data_source;
        }
        return *this;
    }

    void PlainText::setData(int const &value) {
        this->data = value;
        this->data_source = "int";
    }

    void PlainText::setData(seal::Plaintext const &value) {
        this->data = value;
        this->data_source = "SEAL";
    }

    void PlainText::setData(std::string const &value) {
        this->data = value;
        this->data_source = "string";
    }

    unsigned int PlainText::size() const {
        if (std::holds_alternative<int>(this->data)) {
            return 1;
        } else if (std::holds_alternative<std::string>(this->data)) {
            return std::get<std::string>(this->data).size();
        } else if (std::holds_alternative<seal::Plaintext>(this->data)) {
            return std::get<seal::Plaintext>(this->data).coeff_count();
        } else {
            return -1;
        }
    }

    PlainText::~PlainText() {}
    
} // namespace SpatialFHE
