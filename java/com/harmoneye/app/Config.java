package com.harmoneye.app;

import java.io.InputStream;
import java.util.Properties;

/**
 * Reads configuration from a Properties file which is typically located in a
 * "resources" source folder. The configuration can then be used by the various
 * experimental applications in order not to hard-wire some constants.
 */
public class Config {

    private static final String DEFAULT_FILE_NAME = "harmoneye.properties";

    private final Properties properties = new Properties();

    private Config(String propertiesFileName) {
        ClassLoader classLoader = Config.class.getClassLoader();
        InputStream stream = classLoader.getResourceAsStream(propertiesFileName);
        try {
            properties.load(stream);
        } catch (Exception ex) {
            throw new RuntimeException(ex);
        }
    }

    public static Config fromDefault() {
        return new Config(DEFAULT_FILE_NAME);
    }

    public static Config from(String propertiesFileName) {
        return new Config(propertiesFileName);
    }

    public String get(String key) {
        return properties.getProperty(key);
    }

    public String get(String key, String defaultValue) {
        return properties.getProperty(key, defaultValue);
    }
}