//
// Created by ubuntu on 12/13/24.
//

#include <gtest/gtest.h>
#include <concretelang/Support/CompilerEngine.h>
#include <concretelang/Common/Keysets.h>
#include <concretelang/ServerLib/ServerLib.h>
#include <concretelang/ClientLib/ClientLib.h>




class ConcreteDemoSuite : public testing::Test {

};

TEST_F(ConcreteDemoSuite, LoadAdd) {
    std::string ARTIFACT_PATH = "/share/add-compile";
    // Get program info using the Library or unserialize yourself manually
    mlir::concretelang::CompilerEngine::Library lib(ARTIFACT_PATH);
    auto programInfo = lib.getProgramInfo().value();
    std::cout << "Get program info" << std::endl;

    auto secCsprng = concretelang::csprng::SecretCSPRNG(0);
    auto encCsprng = concretelang::csprng::EncryptionCSPRNG(0);
    // Generate keyset
    concretelang::keysets::Keyset keyset(Message<concreteprotocol::KeysetInfo>(programInfo.asReader().getKeyset()), secCsprng, encCsprng);
    std::cout << "Generate keyset" << std::endl;

    // Setup ServerProgram
    auto libPath = lib.getSharedLibraryPath();
    auto serverResult = concretelang::serverlib::ServerProgram::load(programInfo, libPath, false);
    if (!serverResult.has_value()) {
        std::cout << serverResult.error().mesg << std::endl;
    }
    concretelang::serverlib::ServerProgram server
        = concretelang::serverlib::ServerProgram::load(programInfo, lib.getSharedLibraryPath(), false).value();
    std::cout << "Setup ServerProgram" << std::endl;

    // Setup ClientProgram
    auto clientProgram = concretelang::clientlib::ClientProgram::createEncrypted(
    programInfo, keyset.client,
     std::make_shared<concretelang::csprng::EncryptionCSPRNG>(concretelang::csprng::EncryptionCSPRNG(0))).value();
    std::cout << "Setup ClientProgram" << std::endl;

    auto clientCircuit = clientProgram.getClientCircuit("add").value();
    std::cout << "Get ClientCircuit" << std::endl;
    // Encrypt
    auto arg0 = clientCircuit.prepareInput(Tensor<uint64_t>(1) ,0).value();
    auto arg1 = clientCircuit.prepareInput(Tensor<uint64_t>(2), 1).value();
    std::cout << "Encrypt" << std::endl;

    // Call server side
    auto serverCircuit = server.getServerCircuit("add").value();
    auto inputArguments = std::vector<TransportValue>({arg0, arg1});
    auto results = serverCircuit.call(keyset.server, inputArguments).value();
    std::cout << "Call server side" << std::endl;

    // Decrypt
    auto decrypted_0 = clientCircuit.processOutput(results[0], 0).value();
    std::cout << "Decrypt" << std::endl;

    // Check the result
    EXPECT_EQ(decrypted_0.getTensor<uint64_t>().value()[0], 3);

}