#pragma once

#include <seal/seal.h>

#include <string>
#include <variant>

namespace SpatialFHE {
    using PlainTextType = std::variant<int, std::string, seal::Plaintext>;

    class PlainText {
    private:
        PlainTextType data;
        std::string data_source;
    public:
        PlainText();
        PlainText(PlainText const &other);
        PlainText(PlainText &&other);
        PlainText(int const &value);
        PlainText(seal::Plaintext const &value);
        PlainText(std::string const &value);

        PlainText operator=(PlainText const &other);
        PlainText operator=(PlainText &&other);

        void setData(int const &value);
        void setData(seal::Plaintext const &value);
        void setData(std::string const &value);

        unsigned int size() const;

        std::string toString() const;

        ~PlainText();
    };

}  // namespace SpatialFHE
