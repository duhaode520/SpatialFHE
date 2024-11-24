#pragma once

#include <seal/seal.h>
#include <phantom.h>

#include <string>
#include <variant>

namespace SpatialFHE {
    using PlainTextData = std::variant<int, std::string, seal::Plaintext, PhantomPlaintext>;

    class PlainText {
    private:
        PlainTextData data;
        std::string data_source;

    public:
        PlainText();
        PlainText(PlainText const &other);
        PlainText(PlainText &&other) noexcept ;
        explicit PlainText(int const &value);
        explicit PlainText(seal::Plaintext const &value);
        explicit PlainText(std::string const &value);
        explicit PlainText(PhantomPlaintext const &value);

        PlainText& operator=(PlainText const &other);
        PlainText& operator=(PlainText &&other) noexcept ;

        void setData(int const &value);
        void setData(seal::Plaintext const &value);
        void setData(std::string const &value);
        void setData(PhantomPlaintext const &value);

        [[nodiscard]] PlainTextData getData() const;

        [[nodiscard]] unsigned int size() const;

        [[nodiscard]] std::string toString() const;

        ~PlainText();
    };

}  // namespace SpatialFHE
