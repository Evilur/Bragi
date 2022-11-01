package bragi.core;

import org.json.JSONObject;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileReader;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.Properties;

public final class Settings {
    private static String prefix;
    private static String botToken;
    private static String deezerArl;

    /* Метод для получения токена бота */
    public static String getBotToken() {
        return botToken;
    }

    /* Метод для получения Deezer arl */
    public static String getDeezerArl() {
        return deezerArl;
    }

    /* Метод для получения префикса */
    public static String getPrefix() {
        return prefix;
    }

    /* Метод для инициализирования переменных */
    public static void initialize() throws Exception {
        /* !!! Файл bragi.ini в корневом каталоге репозитория - это шаблон для файла '~/.config/bragi.ini' !!! */
        try {  //Пытаемся прочитать файл конфигурации
            Path path = Paths.get(System.getProperty("user.home") + "/.config/bragi.ini");  //Путь к файлу конфигурации
            Properties config = new Properties();  //С помощью этого объекта будем считывать данные из конфига
            config.load(new FileReader(path.toFile()));  //Подгружаем наш файл

            /* Вытаскиваем необходимые нам данные */
            prefix = config.getProperty("prefix");
            botToken = config.getProperty("botToken");
            deezerArl = config.getProperty("deezerArl");
        } catch (Exception ignore) {  //При ошибке выбрасываем исключение
            throw new Exception("The configuration file cannot be read");
        }
    }
}
