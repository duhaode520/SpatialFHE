#ifndef LOCALFSMANAGER_H
#define LOCALFSMANAGER_H
#include "FSManager.h"

namespace SpatialFHE {

    class LocalFSManager: public FSManager {
    public:
        explicit LocalFSManager(const std::string &path);

        ~LocalFSManager() override;

        void OpenOutputStream() override;
        std::ofstream &GetOutputStream() override;
        void CloseOutputStream() override;

        void OpenInputStream() override;
        std::ifstream &GetInputStream() override;
        void CloseInputStream() override;

    };

} // SpatialFHE

#endif //LOCALFSMANAGER_H
