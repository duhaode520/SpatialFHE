package org.ade.SpatialFHE;

import com.google.gson.Gson;
import com.google.gson.JsonObject;
import org.ade.SpatialFHE.spatialfhe.HECrypto;
import org.ade.SpatialFHE.spatialfhe.SEALCrypto;
import org.ade.SpatialFHE.spatialfhe.SpatialFHEManager;
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

    private final SpatialFHEManager manager;
    private final String publicKeyPath;

    private FHEHelper(String publicKeyPath, String secretKeyPath, String confDir, boolean isInit, int maxThreadNum) {
        // load config
        this.publicKeyPath = publicKeyPath;
        try {
            Properties jniConfig = new Properties();
            jniConfig.load(Files.newInputStream(Paths.get(confDir).resolve(JNI_CONFIG_PATH)));
            System.load(jniConfig.getProperty("libPath"));
        } catch (Exception e) {
            logger.error("Error loading JNI library: {}. Stacktrace: {}", e.getMessage(), e);
        }

        String jsonConfig = loadConfig(confDir);
        manager = new SpatialFHEManager(publicKeyPath, secretKeyPath, jsonConfig, isInit, maxThreadNum);
    }

    private FHEHelper(String publicKeyPath, String secretKeyPath, String libPath, String jsonConfig, boolean isInit, int maxThreadNum) {
        this.publicKeyPath = publicKeyPath;
        try {
            System.load(libPath);
        } catch (Exception e) {
            logger.error("Error loading JNI library: {}. Stacktrace: {}", e.getMessage(), e);
        }
        manager = new SpatialFHEManager(publicKeyPath, secretKeyPath, jsonConfig, isInit, maxThreadNum);
    }

    public static FHEHelper getOrCreate(String publicKeyPath, String secretKeyPath, String confDir, boolean isInit, int maxThreadNum) {
        if (instance == null) {
            synchronized (FHEHelper.class) {
                if (instance == null) {
                    instance = new FHEHelper(publicKeyPath, secretKeyPath, confDir, isInit, maxThreadNum);
                }
            }
        }
        return instance;
    }

    public static FHEHelper getOrCreate(String publicKeyPath, String secretKeyPath, String libPath, String jsonConfig, boolean isInit, int maxThreadNum) {
        if (instance == null) {
            synchronized (FHEHelper.class) {
                if (instance == null) {
                    instance = new FHEHelper(publicKeyPath, secretKeyPath, libPath, jsonConfig, isInit, maxThreadNum);
                }
            }
        }
        return instance;
    }

    public static FHEHelper getInstance() {
        if (instance == null) {
            throw new NullPointerException("FHE Singleton is not initialized");
        }
        return instance;
    }

    public SpatialFHEManager getManager() {
        return manager;
    }

    public String getPublicKeyPath() {
        return publicKeyPath;
    }

    private String loadConfig(String confDir) {
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

    private HECrypto initCrypto(String publicKeyPath, String secretKeyPath, boolean isInit, String jsonConfig) {
        final HECrypto crypto;
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
        return crypto;
    }
}
