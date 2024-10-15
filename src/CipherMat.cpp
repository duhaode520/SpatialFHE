//
// Created by ubuntu on 10/14/24.
//

#include "CipherMat.h"

namespace SpatialFHE {
    CipherMat::CipherMat(int width, int height, std::vector<CipherText> &data) {
        this->width = width;
        this->height = height;
        this->data = data;
    }

    CipherMat::CipherMat(CipherMat const &other) {
        this->width = other.width;
        this->height = other.height;
        this->data = other.data;
    }

    CipherMat::CipherMat(CipherMat &&other) noexcept {
        this->width = other.width;
        this->height = other.height;
        // move other.data
        this->data = std::move(other.data);
    }
    CipherMat &CipherMat::operator=(CipherMat const &other) {
        if (this != &other) {
            this->data = other.data;
            this->width = other.width;
            this->height = other.height;
        }
        return *this;
    }

    CipherMat &CipherMat::operator=(CipherMat &&other) noexcept {
        if (this != &other) {
            this->width = other.width;
            this->data = std::move(other.data);
            this->height = other.height;
        }
        return *this;
    }
    int CipherMat::getWidth() const {
        return width;
    }
    int CipherMat::getHeight() const {
        return height;
    }
    std::vector<CipherText> &CipherMat::getData() {
        return data;
    }
} // SpatialFHE