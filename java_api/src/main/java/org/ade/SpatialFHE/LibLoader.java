package org.ade.SpatialFHE;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.Properties;

public class LibLoader {
    private static final String JNI_CONFIG_PATH = "jni.conf";
    private static final Logger logger = LoggerFactory.getLogger(LibLoader.class);
    private static boolean loaded = false;

    /**
     * 从配置目录中读取JNI配置文件并加载库
     * @param confDir 配置目录路径
     * @return 是否成功加载库
     */
    public static boolean tryLoadFromConf(String confDir) {
        try {
            if (loaded) {
                return true;
            }
            Properties jniConfig = new Properties();
            jniConfig.load(Files.newInputStream(Paths.get(confDir).resolve(JNI_CONFIG_PATH)));
            System.load(jniConfig.getProperty("libPath"));
            loaded = true;
            return true;
        } catch (Exception e) {
            logger.error("Error loading JNI library from config: {}. Stacktrace: {}", e.getMessage(), e);
            return false;
        }
    }

    /**
     * 直接加载指定路径的库
     * @param libPath 库文件路径
     * @return 是否成功加载库
     */
    public static boolean tryLoad(String libPath) {
        try {
            if (loaded) {
                return true;
            }
            System.load(libPath);
            loaded = true;
            return true;
        } catch (Exception e) {
            logger.error("Error loading JNI library from path: {}. Stacktrace: {}", e.getMessage(), e);
            return false;
        }
    }
}
