#ifndef FSMANAGER_H
#define FSMANAGER_H

#include <string>
#include <fstream>
#include <memory>

namespace SpatialFHE {

    class FSManager {
    protected:
        std::string path;
        std::ofstream output_stream;
        std::ifstream input_stream;

        static bool createParentDir(const std::string& destPath);
    public:
        explicit FSManager(const std::string& path);
        virtual ~FSManager() = default;

        virtual void OpenOutputStream() = 0;
        virtual std::ofstream &GetOutputStream() = 0;
        virtual void CloseOutputStream() = 0;

        virtual void OpenInputStream() = 0;
        virtual std::ifstream &GetInputStream() = 0;
        virtual void CloseInputStream() = 0;

        static std::shared_ptr<FSManager> createFSManager(const std::string& path);
    };

} // SpatialFHE

#endif //FSMANAGER_H
