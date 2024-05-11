#pragma once

#include <seal/seal.h>

#include <algorithm>
#include <string>
#include <variant>


namespace SpatialFHE {
    using CipherTextType = std::variant<seal::Ciphertext, std::string>;

    class CipherText {
    private:
        /* data */
        CipherTextType data;
        std::string data_source;

    public:
        CipherText();
        CipherText(CipherText const &other);
        CipherText(CipherText &&other);

        CipherText(seal::Ciphertext const &ct);
        CipherText(const std::string &str);
        // CipherText(CipherTextType) 应该还有一个构造函数

        ~CipherText();

        void setData(seal::Ciphertext const &ct);
        void setData(const std::string &str);
        // void setData()

        unsigned int size() const;

        CipherText operator=(CipherText const &other);
        CipherText operator=(CipherText &&other);

        std::string toString();
    };

}  // namespace SpatialFHE
