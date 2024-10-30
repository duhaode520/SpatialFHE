//
// Created by ubuntu on 10/14/24.
//

#ifndef MATFHEMANAGER_H
#define MATFHEMANAGER_H
#include "CipherMat.h"

namespace SpatialFHE {
class MatFHEManager {
public:
    virtual CipherMat encryptMat(int width, int height, const std::vector<double> &data) = 0;
    virtual std::vector<double> decryptMat(CipherMat &cipher_mat) = 0;

    virtual CipherMat addMat(CipherMat const &a, CipherMat const &b) = 0;
    virtual CipherMat addMatPlain(CipherMat const& a, const std::vector<double> &plain) = 0;
    virtual CipherMat subMat(CipherMat const &a, CipherMat const &b) = 0;
    virtual CipherMat multiplyMat(CipherMat const& a, CipherMat const& b) = 0;
    virtual CipherMat multiplyMatPlain(CipherMat const& a, const std::vector<double> &plain) = 0;

    virtual ~MatFHEManager() = default;
};
}  // namespace SpatialFHE

#endif  // MATFHEMANAGER_H
