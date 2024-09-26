package org.ade.SpatialFHE;

import com.google.gson.Gson;
import com.google.gson.JsonObject;
import org.ade.SpatialFHE.spatialfhe.HECrypto;
import org.ade.SpatialFHE.spatialfhe.SEALCrypto;

import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.Properties;

public class FHEHelper {

    static volatile FHEHelper instance;
    static private final String JSON_CONFIG_PATH = "conf/fhe.json";
    static private final String JNI_CONFIG_PATH = "conf/jni.conf";

    private final HECrypto crypto;
    private final String publicKeyPath;
    private FHEHelper(String publicKeyPath, String secretKeyPath, boolean isInit) {
        // load config
        Properties jniConfig = new Properties();
        this.publicKeyPath = publicKeyPath;
        try {
            jniConfig.load(Files.newInputStream(Paths.get(JNI_CONFIG_PATH)));
            System.load(jniConfig.getProperty("libPath"));
        } catch (Exception e) {
            e.printStackTrace();
        }

        String jsonConfig = loadConfig();
        crypto = new SEALCrypto(jsonConfig);
        if (isInit) {
            crypto.GenerateKeyPair(publicKeyPath, secretKeyPath);
        } else {
            crypto.LoadKeyPair(publicKeyPath, secretKeyPath);
        }
    }

    public static FHEHelper getOrCreate(String publicKeyPath, String secretKeyPath, boolean isInit) {
        if (instance == null) {
            synchronized (FHEHelper.class) {
                if (instance == null) {
                    instance = new FHEHelper(publicKeyPath, secretKeyPath, isInit);
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
             String fileString = new String(Files.readAllBytes(Paths.get(JSON_CONFIG_PATH)));
            JsonObject jsonObject = gson.fromJson(fileString, JsonObject.class);
            return jsonObject.toString();
        } catch (Exception e) {
            e.printStackTrace();
            return null;
        }
    }
}
