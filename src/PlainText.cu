#include "PlainText.h"

namespace SpatialFHE {
    PlainText::PlainText() = default;

    PlainText::PlainText(PlainText const &other) {
        this->data = other.data;
        this->data_source = other.data_source;
    }

    PlainText::PlainText(PlainText &&other)  noexcept {
        this->data = PlainTextData(other.data);
        this->data_source = std::string(other.data_source);
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

    PlainText::PlainText(PhantomPlaintext const &value) {
        this->data = value;
        this->data_source = "Phantom";
    }

    void PlainText::setData(PhantomPlaintext const &value) {
        this->data = value;
        this->data_source = "Phantom";
    }

    PlainText & PlainText::operator=(PlainText const &other) {
        if (this != &other) {
            this->data = other.data;
            this->data_source = other.data_source;
        }
        return *this;
    }

    PlainText& PlainText::operator=(PlainText &&other)  noexcept {
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

    PlainTextData PlainText::getData() const {
        return this->data;
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

    std::string PlainText::toString() const {
        if (std::holds_alternative<int>(this->data)) {
            return std::to_string(std::get<int>(this->data));
        } else if (std::holds_alternative<std::string>(this->data)) {
            return std::get<std::string>(this->data);
        } else if (std::holds_alternative<seal::Plaintext>(this->data)) {
            seal::Plaintext pt = std::get<seal::Plaintext>(this->data);
            return pt.to_string();
        } else {
            return "Unknown";
        }
    }

    PlainText::~PlainText() = default;
    
} // namespace SpatialFHE
