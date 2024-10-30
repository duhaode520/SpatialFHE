//
// Created by ubuntu on 10/14/24.
//

#ifndef SPATIALFHEMANAGER_H
#define SPATIALFHEMANAGER_H
#include "BaseFHEManager.h"
#include "MatFHEManager.h"

namespace SpatialFHE {
    class SpatialFHEManager : public BaseFHEManager, public MatFHEManager {
    public:
        SpatialFHEManager();
        SpatialFHEManager(
            const std::string &publicKeyPath,
            const std::string &secretKeyPath,
            const std::string &paramsString,
            bool isInit);
        ~SpatialFHEManager() override;

        // MatFHEManger implementation
        CipherMat encryptMat(int weight, int height, const std::vector<double> &data) override;
        std::vector<double> decryptMat(CipherMat &cipher_mat) override;

        CipherMat addMat(CipherMat const &a, CipherMat const &b) override;
        CipherMat addMatPlain(CipherMat const &a, const std::vector<double> &plain) override;
        CipherMat subMat(CipherMat const &a, CipherMat const &b) override;
        CipherMat multiplyMat(CipherMat const &a, CipherMat const &b) override;
        CipherMat multiplyMatPlain(CipherMat const &a, const std::vector<double> &plain) override;

        static void validateSameSize(SpatialFHE::CipherMat const &a, SpatialFHE::CipherMat const &b);
    };
}  // namespace SpatialFHE

#endif  // SPATIALFHEMANAGER_H
