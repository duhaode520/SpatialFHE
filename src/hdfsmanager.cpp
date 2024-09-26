//
// Created by ubuntu on 9/19/24.
//

#include "hdfsmanager.h"
#include <unistd.h>
#include <sys/stat.h>

namespace SpatialFHE {
    void HDFSManager::parsePath(const std::string &path) {
        const std::string protocol = path.substr(0, path.find_first_of("://"));
        const std::string path_new = path.substr(protocol.size() + 3);

        const size_t pos = path_new.find_first_of(':');
        const std::string host_string = path_new.substr(0, pos);
        // 判断host是否为localhost
        if (host_string == "localhost") {
            this->host = "localhost";
            this->port = 0;
        } else {
            this->host = host_string;
            const std::string port_string = path_new.substr(pos + 1, path_new.find_first_of('/'));
            this->port = std::stoi(port_string);
        }

        this->path = path_new.substr(path_new.find_first_of('/'));
    }
    void HDFSManager::connect() {
        this->hdfs = hdfsConnect(this->host.c_str(), this->port);
        if (!this->hdfs) {
            std::cerr << "Failed to connect to " << this->host << ":" << this->port;
            exit(-1);
        }
    }

    void HDFSManager::disconnect() const {
        hdfsDisconnect(this->hdfs);
    }
    bool HDFSManager::fileExists(const std::string &path) {
        // check if the file exists
        return hdfsExists(this->hdfs, path.c_str()) == 0;
    }

    hdfsFile HDFSManager::openFile(const std::string &path, const int mode) {
        hdfsFile file = hdfsOpenFile(this->hdfs, path.c_str(), mode, 0, 0, 0);
        if (!file) {
            std::cerr << "Failed to open HDFS file " << path << std::endl;
        }
        return file;
    }

    void HDFSManager::closeFile(const hdfsFile &file) {
        hdfsFlush(this->hdfs, file);
        hdfsCloseFile(this->hdfs, file);
    }

    void HDFSManager::upload(const std::string &srcPath, const std::string &destPath) {
        std::ifstream srcFile(srcPath);
        if (!srcFile.is_open()) {
            std::cerr << "Failed to open local file:" << srcPath << std::endl;
            return;
        }
        // if the file exists, delete it
        if (fileExists(path)) {
            if (hdfsDelete(this->hdfs, path.c_str(), 0) != 0) {
                std::cerr << "Failed to delete existing HDFS file: " << path << std::endl;
                return;
            }
        }

        hdfsFile destFile = openFile(destPath, O_WRONLY | O_CREAT);
        char buffer[4096];
        while (srcFile.read(buffer, sizeof(buffer))) {
            size_t bytesRead = srcFile.gcount();
            if (hdfsWrite(hdfs, destFile, buffer, bytesRead) != bytesRead) {
                std::cerr << "Failed to write to HDFS file" << destPath << std::endl;
                closeFile(destFile);
                srcFile.close();
                return;
            }
        }

        size_t bytesRead = srcFile.gcount();
        if (hdfsWrite(hdfs, destFile, buffer, bytesRead) != bytesRead) {
            std::cerr << "Failed to write to HDFS file" << destPath << std::endl;
            closeFile(destFile);
            srcFile.close();
            return;
        }

        closeFile(destFile);
        srcFile.close();
    }

    void HDFSManager::download(const std::string &srcPath, const std::string &destPath) {
        // check if the local dir exists and recursively create it
        if (!createParentDir(destPath)) {
            return;
        }

        std::ofstream destFile = std::ofstream(destPath);
        if (!destFile.is_open()) {
            std::cerr << "Failed to open local file" << destPath << std::endl;
            return;
        }
        if (!fileExists(srcPath)) {
            std::cerr << "HDFS file does not exist: " << srcPath << std::endl;
            return;
        }

        hdfsFile srcFile = openFile(srcPath, O_RDONLY);

        char buffer[4096];
        int bytesRead = 4096;
        while (bytesRead == 4096) {
            bytesRead = hdfsRead(this->hdfs, srcFile, buffer, 4096);
            if (bytesRead < 0) {
                std::cerr << "Failed to read from HDFS file: " << srcPath << std::endl;
                closeFile(srcFile);
                destFile.close();
                return;
            }
            destFile.write(buffer, bytesRead);
        }

        closeFile(srcFile);
        destFile.close();
    }

    HDFSManager::HDFSManager(const std::string &path) : FSManager(path) {
        this->parsePath(path);
        connect();
    }
    HDFSManager::~HDFSManager() {
        if (this->hdfs) {
            disconnect();
        }
    }

    void HDFSManager::OpenOutputStream() {
        if (!createParentDir(this->path)) {
            return;
        }
        this->output_stream.open(this->path);
    }

    std::ofstream &HDFSManager::GetOutputStream() {
        return this->output_stream;
    }
    void HDFSManager::CloseOutputStream() {
        // 1. close the stream
        this->output_stream.flush();
        this->output_stream.close();
        // 2. upload the file
        upload(this->path, this->path);
        // 3. delete local file
        std::remove(this->path.c_str());
    }

    void HDFSManager::OpenInputStream() {
        // download the file
        download(this->path, this->path);
        // open the file
        this->input_stream.open(this->path);
    }

    std::ifstream &HDFSManager::GetInputStream() {
        return this->input_stream;
    }
    void HDFSManager::CloseInputStream() {
        // close the stream
        this->input_stream.close();
        // delete local file
        std::remove(this->path.c_str());
        // delete local dir if created
        const size_t pos = this->path.find_last_of('/');
        if (pos != std::string::npos) {
            const std::string dir = this->path.substr(0, pos);
            rmdir(dir.c_str());
        }
    }
}  // namespace SpatialFHE