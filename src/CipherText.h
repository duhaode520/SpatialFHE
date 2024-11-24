#pragma once

#include <seal/seal.h>
#include "ciphertext.h"

#include <string>
#include <variant>


namespace SpatialFHE {
    using CipherTextData = std::variant<seal::Ciphertext, std::string, PhantomCiphertext>;

    class CipherText {
    private:
        /* data */
        CipherTextData data;
        std::string data_source;

    public:
        CipherText();
        CipherText(CipherText const &other);
        CipherText(CipherText &&other) noexcept ;

        explicit CipherText(seal::Ciphertext const &ct);
        explicit CipherText(const std::string &str);
        explicit CipherText(PhantomCiphertext const &ct);
        // CipherText(CipherTextType) 应该还有一个构造函数

        ~CipherText();

        void setData(seal::Ciphertext const &ct);
        void setData(const std::string &str);
        void setData(PhantomCiphertext const &ct);
        // void setData()

        [[nodiscard]] unsigned int size() const;

        [[nodiscard]] CipherTextData getData() const;

        CipherText& operator=(CipherText const &other);
        CipherText& operator=(CipherText &&other) noexcept ;

        std::string toString();
    };

}  // namespace SpatialFHE
