package org.ade.SpatialFHE;

import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import static org.junit.jupiter.api.Assertions.*;
import org.ade.SpatialFHE.spatialfhe.*;

import java.io.IOException;

class FHEHelperTest {

    static String publicKeyPath, secretKeyPath;
    static final String serverURL = "127.0.0.1:8080";
    static boolean isInit;

    @BeforeEach
    void resetSingleton() {
        // Reset the singleton instance before each test
        FHEHelper.instance = null;
    }

    @AfterEach
    void clear() throws IOException {
        // Clear the keys after each test
        // check public key path not start with hdfs://
        if (publicKeyPath != null && !publicKeyPath.startsWith("hdfs://")) {
            // Delete the public key file
            deleteFile(publicKeyPath);
            // delete the relin key file
            deleteFile(publicKeyPath + ".relin");
            // delete the galois key file
            deleteFile(publicKeyPath + ".galois");

            // TFHE
            deleteFile(publicKeyPath + ".tfhe");
        }
        if (secretKeyPath != null) {
            deleteFile(secretKeyPath);
            deleteFile(secretKeyPath + ".tfhe");
            // if SECRET_KEY_PATH includes directory, delete the directory
            if (secretKeyPath.contains("/")) {
                java.io.File dir = new java.io.File(secretKeyPath.substring(0, secretKeyPath.lastIndexOf("/")));
                if (dir.exists()) {
                    if (dir.delete()) {
                        System.out.println("Deleted secret key directory: " + dir);
                    } else {
                        throw new IOException("Failed to delete secret key directory: " + dir);
                    }
                }
            }
        }
    }

    private static void deleteFile(String publicKeyPath) throws IOException {
        java.io.File file = new java.io.File(publicKeyPath);
        if (file.exists()) {
            if (file.delete()) {
                System.out.println("Deleted public key file: " + publicKeyPath);
            } else {
                throw new IOException("Failed to delete public key file: " + publicKeyPath);
            }
        }
    }

    void compute() {
        FHEHelper helper = FHEHelper.getOrCreate(publicKeyPath, secretKeyPath, "conf",serverURL,
            HECrypto.HELibrary.Phantom, isInit, 2);
        assertNotNull(helper);
        assertNotNull(helper.getManager());
        assertEquals(publicKeyPath, helper.getPublicKeyPath());
        SpatialFHEManager manager = helper.getManager();
        CipherText ct1 = manager.encodeAndEncrypt(1.0);
        CipherText ct2 = manager.encodeAndEncrypt(2.0);
        CipherText ct3 = manager.add(ct1, ct2);
        DoubleVector dv = new DoubleVector();
        manager.decodeAndDecrypt(dv, ct3);
//        assertEquals(3.0, dv.get(0), 1e-4);
    }


    @Test
    void localKeysTest() {
        publicKeyPath = "tmp/public.key";
        secretKeyPath = "tmp/secret.key";
        isInit = true;
        System.out.println("localKeysTest: publicKeyPath = "
             + publicKeyPath + ", secretKeyPath = " + secretKeyPath + ", isInit = " + isInit);
        compute();
        FHEHelper.instance = null;
        // load
        System.out.println("Clearing singleton instance and loading keys");
        isInit = false;
        compute();
    }

    @Test
    void HDFSKeysTest() {
        publicKeyPath = "hdfs://dhdmaster:8020/tmp/public.key";
        secretKeyPath = "tmp/secret.key";
        isInit = true;
        System.out.println("HDFSKeysTest: publicKeyPath = "
                + publicKeyPath + ", secretKeyPath = " + secretKeyPath + ", isInit = " + isInit);
        compute();
        FHEHelper.instance = null;
        // load
        System.out.println("Clearing singleton instance and loading keys");
        isInit = false;
        compute();
    }

    @Test
    void TFHETest() {
        publicKeyPath = "tmp/public.key";
        secretKeyPath = "tmp/secret.key";
        isInit = true;
        System.out.println("localKeysTest: publicKeyPath = "
                + publicKeyPath + ", secretKeyPath = " + secretKeyPath + ", isInit = " + isInit);
        compute();
        WKTReader reader = new WKTReader();
        String a = "LINESTRING(0 0, 900 900)";
        String b = "LINESTRING(0 900, 900 0)";
        TFHELineString la = reader.read(a).asLineString();
        TFHELineString lb = reader.read(b).asLineString();
        assertTrue(la.intersects(lb).decrypt());
    }


}