//
// Created by ubuntu on 10/14/24.
//

#ifndef CIPHERMAT_H
#define CIPHERMAT_H

#include "CipherText.h"
#include <vector>

namespace SpatialFHE {

    class CipherMat {
    private:
        int width, height;
        std::vector<CipherText> data;
    public:
        CipherMat(int width, int height, std::vector<CipherText>& data);
        CipherMat(CipherMat const &other);
        CipherMat(CipherMat &&other) noexcept ;
        ~CipherMat() = default;

        CipherMat& operator=(CipherMat const &other);
        CipherMat& operator=(CipherMat &&other) noexcept ;

        [[nodiscard]] int getWidth() const;
        [[nodiscard]] int getHeight() const;
        [[nodiscard]] const std::vector<CipherText> &getData() const;
    };

}
#endif
