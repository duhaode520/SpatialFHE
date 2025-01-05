//
// Created by ubuntu on 12/13/24.
//

#include <gtest/gtest.h>
#include <concretelang/Support/CompilerEngine.h>
#include <concretelang/Common/Keysets.h>
#include <concretelang/ServerLib/ServerLib.h>
#include <concretelang/ClientLib/ClientLib.h>
#include <tfhe.h>
#include <concrete-cpu.h>

class ConcreteDemoSuite : public testing::Test {};

TEST_F(ConcreteDemoSuite, LoadAdd) {
    std::string ARTIFACT_PATH = "/share/add-compile";
    // Get program info using the Library or unserialize yourself manually
    mlir::concretelang::CompilerEngine::Library lib(ARTIFACT_PATH);
    auto programInfo = lib.getProgramInfo().value();
    std::cout << "Get program info" << std::endl;

    auto secCsprng = concretelang::csprng::SecretCSPRNG(0);
    auto encCsprng = concretelang::csprng::EncryptionCSPRNG(0);
    // Generate keyset
    concretelang::keysets::Keyset keyset(
        Message<concreteprotocol::KeysetInfo>(programInfo.asReader().getKeyset()), secCsprng, encCsprng);
    std::cout << "Generate keyset" << std::endl;

    // Setup ServerProgram
    auto libPath = lib.getSharedLibraryPath();
    auto serverResult = concretelang::serverlib::ServerProgram::load(programInfo, libPath, false);
    if (!serverResult.has_value()) {
        std::cout << serverResult.error().mesg << std::endl;
    }
    concretelang::serverlib::ServerProgram server =
        concretelang::serverlib::ServerProgram::load(programInfo, lib.getSharedLibraryPath(), false).value();
    std::cout << "Setup ServerProgram" << std::endl;

    // Setup ClientProgram
    auto clientProgram =
        concretelang::clientlib::ClientProgram::createEncrypted(
            programInfo,
            keyset.client,
            std::make_shared<concretelang::csprng::EncryptionCSPRNG>(concretelang::csprng::EncryptionCSPRNG(0)))
            .value();
    std::cout << "Setup ClientProgram" << std::endl;

    auto clientCircuit = clientProgram.getClientCircuit("add").value();
    std::cout << "Get ClientCircuit" << std::endl;
    // Encrypt
    auto arg0 = clientCircuit.prepareInput(Tensor<uint64_t>(1), 0).value();
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

TEST_F(ConcreteDemoSuite, TFHE_RS_DEMO) {
    int ok = 0;
    // Prepare the config builder for the high level API and choose which types to enable
    ConfigBuilder *builder;
    Config *config;

    // Put the builder in a default state without any types enabled
    config_builder_default(&builder);
    // Use the small LWE key for encryption
    config_builder_default_with_small_encryption(&builder);
    // Populate the config
    config_builder_build(builder, &config);

    ClientKey *client_key = NULL;
    ServerKey *server_key = NULL;

    // Generate the keys using the config
    generate_keys(config, &client_key, &server_key);
    // Set the server key for the current thread
    set_server_key(server_key);

    FheUint128 *lhs = NULL;
    FheUint128 *rhs = NULL;
    FheUint128 *result = NULL;
    // A 128-bit unsigned integer containing value: 20 << 64 | 10
    U128 clear_lhs = {.w0 = 10, .w1 = 20};
    // A 128-bit unsigned integer containing value: 2 << 64 | 1
    U128 clear_rhs = {.w0 = 1, .w1 = 2};

    ok = fhe_uint128_try_encrypt_with_client_key_u128(clear_lhs, client_key, &lhs);
    assert(ok == 0);

    ok = fhe_uint128_try_encrypt_with_client_key_u128(clear_rhs, client_key, &rhs);
    assert(ok == 0);

    // Compute the subtraction
    ok = fhe_uint128_sub(lhs, rhs, &result);
    assert(ok == 0);

    U128 clear_result;
    // Decrypt
    ok = fhe_uint128_decrypt(result, client_key, &clear_result);
    assert(ok == 0);

    // Here the subtraction allows us to compare each word
    assert(clear_result.w0 == 9);
    assert(clear_result.w1 == 18);

    // Destroy the ciphertexts
    fhe_uint128_destroy(lhs);
    fhe_uint128_destroy(rhs);
    fhe_uint128_destroy(result);

    // Destroy the keys
    client_key_destroy(client_key);
    server_key_destroy(server_key);

    printf("FHE computation successful!\n");
}

TEST_F(ConcreteDemoSuite, SharedKeys) {
    // Generate the shared keys
    ShortintClientKey *shortint_client_key = NULL;
    ShortintServerKey *shortint_server_key = NULL;
    ShortintPBSParameters params = SHORTINT_PARAM_MESSAGE_2_CARRY_3_KS_PBS;
    int ok = 0;
    ok = shortint_gen_keys_with_parameters(params, &shortint_client_key, &shortint_server_key);
    assert(ok == 0);

    // get lwe_key from shortint_client_key
    uint64_t *lwe_key = NULL;
    ok = shortint_client_key_get_lwe_key(shortint_client_key, &lwe_key);
    assert(ok == 0);

    // change TFHE-rs keys to concrete keys
    std::string ARTIFACT_PATH = "/share/compute-share-compile";
    // Get program info using the Library or unserialize yourself manually
    mlir::concretelang::CompilerEngine::Library lib(ARTIFACT_PATH);
    auto programInfo = lib.getProgramInfo().value();
    std::cout << "Get program info" << std::endl;
    auto keyinfos = programInfo.asReader().getKeyset().getLweSecretKeys();
    auto keyinfo_reader = keyinfos[1];
    auto lwe_key_size = keyinfo_reader.getParams().getLweDimension();
    auto secCsprng = concretelang::csprng::SecretCSPRNG(0);
    auto encCsprng = concretelang::csprng::EncryptionCSPRNG(0);

    // build key_map from lwe_key
    auto lwe_key_map = std::map<uint32_t, LweSecretKey>();
    auto lwe_key_buffer = std::make_shared<std::vector<uint64_t>>(lwe_key, lwe_key + lwe_key_size);
    // build Message<LweSecreteKeyInfo> from keyinfo_reader
    capnp::MallocMessageBuilder message_builder;
    auto lweSecretKeyInfo = message_builder.initRoot<concreteprotocol::LweSecretKeyInfo>();
    // Copy the content from reader to lweSecretKeyInfo
    lweSecretKeyInfo.setParams(keyinfo_reader.getParams());
    lweSecretKeyInfo.setId(keyinfo_reader.getId());
    lwe_key_map.insert({1, LweSecretKey(lwe_key_buffer, Message<concreteprotocol::LweSecretKeyInfo>(lweSecretKeyInfo))});

    // Generate keyset
    concretelang::keysets::Keyset keyset(
        Message<concreteprotocol::KeysetInfo>(programInfo.asReader().getKeyset()), secCsprng, encCsprng, lwe_key_map);
    std::cout << "Generate keyset" << std::endl;

    // Setup ServerProgram
    auto libPath = lib.getSharedLibraryPath();
    auto serverResult = concretelang::serverlib::ServerProgram::load(programInfo, libPath, false);
    if (!serverResult.has_value()) {
        std::cout << serverResult.error().mesg << std::endl;
    }
    concretelang::serverlib::ServerProgram server =
        concretelang::serverlib::ServerProgram::load(programInfo, lib.getSharedLibraryPath(), false).value();
    std::cout << "Setup ServerProgram" << std::endl;

    // Setup ClientProgram
    auto clientProgram =
        concretelang::clientlib::ClientProgram::createEncrypted(
            programInfo,
            keyset.client,
            std::make_shared<concretelang::csprng::EncryptionCSPRNG>(concretelang::csprng::EncryptionCSPRNG(0)))
            .value();
    std::cout << "Setup ClientProgram" << std::endl;

    // clear
    shortint_destroy_client_key(shortint_client_key);
    shortint_destroy_server_key(shortint_server_key);
}