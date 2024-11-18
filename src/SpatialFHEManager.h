//
// Created by ubuntu on 10/14/24.
//

#ifndef SPATIALFHEMANAGER_H
#define SPATIALFHEMANAGER_H
#include "BaseFHEManager.h"
#include "MatFHEManager.h"

namespace SpatialFHE {
    class SpatialFHEManager : public BaseFHEManager, public MatFHEManager {
    private:
        int max_thread_num;

        CipherMat parallelOp2(CipherMat const &a, CipherMat const &b,
            std::function<CipherText(CipherText const&, CipherText const&)> func) const;
        CipherMat parallelOp2(CipherMat const& a, std::vector<PlainText> const& plainVector,
            std::function<CipherText(CipherText const&, PlainText const&)> func) const;

    public:
        SpatialFHEManager();
        SpatialFHEManager(
            const std::string &publicKeyPath,
            const std::string &secretKeyPath,
            const std::string &paramsString,
            bool isInit,
            int max_thread_num);
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
