package bragi;

import org.json.JSONObject;

import java.io.File;
import java.io.FileReader;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;

public class Settings {
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
        /* !!! Файл bragi.json в корневом каталоге репозитория - это шаблон для файла '~/.bragi' !!!*/
        try {  //Пытаемся прочитать файл конфигурации
            Path path = Paths.get(System.getProperty("user.home") + "/.bragi");  //Путь к файлу конфигурации
            String fileText = Files.readString(path);  //Считываем файл

            /* Переводим строку в JSON объект и инициализируем переменные */
            JSONObject jsonObject =  new JSONObject(fileText);
            prefix =  jsonObject.getString("prefix");
            botToken =  jsonObject.getString("botToken");
            deezerArl =  jsonObject.getString("deezerArl");
        } catch (Exception ignore) {  //При ошибке выбрасываем исключение
            throw new Exception("The configuration file cannot be read");
        }
    }
}
