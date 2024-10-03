package org.ade.SpatialFHE;

import com.google.gson.Gson;
import com.google.gson.JsonObject;
import org.ade.SpatialFHE.spatialfhe.HECrypto;
import org.ade.SpatialFHE.spatialfhe.SEALCrypto;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.Properties;

public class FHEHelper {

    static volatile FHEHelper instance;
    private static final String JSON_CONFIG_PATH = "fhe.json";
    private static final String JNI_CONFIG_PATH = "jni.conf";
    private static final Logger logger = LoggerFactory.getLogger(FHEHelper.class);

    private final HECrypto crypto;
    private final String publicKeyPath;
    private final String confDir;

    private FHEHelper(String publicKeyPath, String secretKeyPath, String confDir, boolean isInit) {
        // load config
        Properties jniConfig = new Properties();
        this.publicKeyPath = publicKeyPath;
        this.confDir = confDir;
        try {
            jniConfig.load(Files.newInputStream(Paths.get(confDir).resolve(JNI_CONFIG_PATH)));
            System.load(jniConfig.getProperty("libPath"));
        } catch (Exception e) {
            logger.error("Error loading JNI library: {}. Stacktrace: {}", e.getMessage(), e);
        }

        String jsonConfig = loadConfig();
        crypto = new SEALCrypto(jsonConfig);
        try {
            if (isInit) {
                crypto.GenerateKeyPair(publicKeyPath, secretKeyPath);
            } else {
                if (secretKeyPath.isEmpty()) {
                    crypto.LoadPublicKey(publicKeyPath);
                } else {
                    crypto.LoadKeyPair(publicKeyPath, secretKeyPath);
                }
            }
        } catch (Exception e) {
            logger.error("Error init SEAL Crpyto: {}, Stacktrace: {}", e.getMessage(), e);
        }
    }

    public static FHEHelper getOrCreate(String publicKeyPath, String secretKeyPath, String confDir, boolean isInit) {
        if (instance == null) {
            synchronized (FHEHelper.class) {
                if (instance == null) {
                    instance = new FHEHelper(publicKeyPath, secretKeyPath, confDir, isInit);
                }
            }
        }
        return instance;
    }

    public HECrypto getCrypto() {
        return crypto;
    }

    public String getPublicKeyPath() {
        return publicKeyPath;
    }

    private String loadConfig() {
        // load json config from file
        Gson gson = new Gson();
        try {
             String fileString = new String(Files.readAllBytes(Paths.get(confDir).resolve(JSON_CONFIG_PATH)));
            JsonObject jsonObject = gson.fromJson(fileString, JsonObject.class);
            return jsonObject.toString();
        } catch (Exception e) {
            logger.error("Error load crypto configs: {}, Stacktrace: {}", e.getMessage(), e);
            return null;
        }
    }
}
