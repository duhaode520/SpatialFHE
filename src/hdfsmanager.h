#ifndef HDFSMANAGER_H
#define HDFSMANAGER_H

#include "fsmanager.h"
#include <iostream>
#include "hdfs/hdfs.h"

namespace SpatialFHE {

    class HDFSManager final : public FSManager {
    private:
        std::string host;
        int port{};

        void parsePath(const std::string& path);

        // HDFS functions
        hdfsFS hdfs{};

        void connect();
        void disconnect() const;

        bool fileExists(const std::string& path);
        hdfsFile openFile(const std::string& path, int mode);
        void closeFile(const hdfsFile& file);

        // upload local file to HDFS
        void upload(const std::string& srcPath, const std::string& destPath);

        // download file from HDFS to local
        void download(const std::string& srcPath, const std::string& destPath);

    public:
        explicit HDFSManager(const std::string& path);
        ~HDFSManager() override;

        void OpenOutputStream() override;
        std::ofstream &GetOutputStream() override;
        void CloseOutputStream() override;

        void OpenInputStream() override;
        std::ifstream &GetInputStream() override;
        void CloseInputStream() override;
    };

} // SpatialFHE

#endif //HDFSMANAGER_H
