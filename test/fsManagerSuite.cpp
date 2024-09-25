//
// Created by ubuntu on 9/22/24.
//
#include <gtest/gtest.h>
#include <string>
#include <hdfs/hdfs.h>

// #define private public
#include "fsmanager.h"
#include "localfsmanager.h"
#include "hdfsmanager.h"
// #undef private
using namespace SpatialFHE;
using namespace std;

class FSManagerTest : public ::testing::Test {
};

TEST_F(FSManagerTest, Init) {
    string local_path = "test.txt";
    string hdfs_path = "hdfs://dhdmaster:8020/tmp/test.txt";
    shared_ptr<FSManager> local_fs_manager = FSManager::createFSManager(local_path);
    shared_ptr<FSManager> hdfs_fs_manager = FSManager::createFSManager(hdfs_path);
    // check local_fs_manager is LocalFSManager
    ASSERT_EQ(typeid(*local_fs_manager), typeid(LocalFSManager));
    // check hdfs_fs_manager is HDFSManager
    ASSERT_EQ(typeid(*hdfs_fs_manager), typeid(HDFSManager));
}

TEST_F(FSManagerTest, LocalIO) {
        string local_path = "test.txt";
        shared_ptr<FSManager> local_fs_manager = FSManager::createFSManager(local_path);
        // write
        string content = "hello world";
        local_fs_manager->OpenOutputStream();
        std::ofstream& out = local_fs_manager->GetOutputStream();
        out << content;
        local_fs_manager->CloseOutputStream();
        // read
        local_fs_manager->OpenInputStream();
        std::ifstream& in = local_fs_manager->GetInputStream();
        string read_content;
        // read line
        std::getline(in, read_content);
        local_fs_manager->CloseInputStream();
        ASSERT_EQ(content, read_content);
}

TEST_F(FSManagerTest, HDFSIO) {
    string hdfs_path = "hdfs://dhdmaster:8020/tmp/test.txt";
    shared_ptr<FSManager> hdfs_fs_manager = FSManager::createFSManager(hdfs_path);
    // write
    string content = "hello world";
    hdfs_fs_manager->OpenOutputStream();
    std::ofstream& out = hdfs_fs_manager->GetOutputStream();
    out << content;
    hdfs_fs_manager->CloseOutputStream();
    // read
    hdfs_fs_manager->OpenInputStream();
    std::ifstream& in = hdfs_fs_manager->GetInputStream();
    string read_content;
    std::getline(in, read_content);
    hdfs_fs_manager->CloseInputStream();
    ASSERT_EQ(content, read_content);
}