#pragma once

#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <map>
#include "HECrypto.h"

void createTestParams(SpatialFHE::HECrypto::CryptoParams& params) {
    const std::string json = R"(
        {
            "CoeffModulusBits": [60, 30, 30, 30, 60],
            "SchemeType": "CKKS",
            "PolyModulusDegree": 8192,
            "PlaintextModulus": 0,
            "CoeffModulusPrimes": [0],
            "Scale": 1073741824
        })";
    rapidjson::Document doc;
    doc.Parse(json.c_str());
    for (auto& m : doc.GetObject()) {
        params[m.name.GetString()] = m.value;
    }
}



