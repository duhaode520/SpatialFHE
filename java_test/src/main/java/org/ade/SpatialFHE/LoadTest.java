package org.ade.SpatialFHE;

import org.ade.SpatialFHE.spatialfhe.*;

import java.nio.file.Path;
import java.nio.file.Paths;

public class LoadTest {
    public static void main(String[] args) {
        // get current working directory
//        Path currentRelativePath = Paths.get("").toAbsolutePath();
//        System.load(currentRelativePath
//                .resolve("src").resolve("main")
//                .resolve("resources")
//                .resolve("SpatialFHE-linux-x86-64")
//                .resolve("libjniSpatialFHE.so").toString());
        System.load("/home/duhaode/pri_comp/SpatialFHE/java_test/src/main/resources/SpatialFHE-linux-x86-64/libjniSpatialFHE.so");
        SEALCrypto crypto = new SEALCrypto();
        String json = "{" +
                "\"CoeffModulusBits\": [60, 30, 30, 30, 60]," +
                "\"SchemeType\": \"CKKS\"," +
                "\"PolyModulusDegree\": 8192," +
                "\"PlaintextModulus\": 0," +
                "\"CoeffModulusPrimes\": [0]," +
                "\"ScaleFactor\": 30" +
                 "}";
        crypto.GenerateKeyPair(json, "public.key", "private.key");
        PlainText pt1 = crypto.Encode(1.0);
        PlainText pt2 = crypto.Encode(2.0);
        CipherText ct1 = crypto.Encrypt(pt1);
        CipherText ct2 = crypto.Encrypt(pt2);
        CipherText ct3 = crypto.Add(ct1, ct2);
        PlainText pt3 = crypto.Decrypt(ct3);
        DoubleVector dv = new DoubleVector();
        crypto.Decode(dv, pt3);
        System.out.println(dv.get(0));
    }
}
