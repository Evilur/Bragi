package bragi;

import bragi.core.Settings;
import bragi.core.EventHandler;
import bragi.core.Player;
import bragi.core.source.deezer.DeezerMethods;
import net.dv8tion.jda.api.JDA;
import net.dv8tion.jda.api.JDABuilder;
import net.dv8tion.jda.api.entities.Guild;

import java.io.File;
import java.util.HashMap;
import java.util.Objects;

public class Bragi {
    public static HashMap<Guild, Player> Players = new HashMap<>();  //Инициализированные сервера
    public static JDA bot;  //Объект бота

    public static void main(String[] args) throws Exception {
        /* Очищаем мусор от предыдущей сессии */
        for (File file : Objects.requireNonNull(new File("/tmp").listFiles())) {
            if (file.getName().endsWith(".flac")) {
                if (!file.delete())
                    System.out.printf("Error while deleting file \"%s\"\n", file.getName());
            }
        }

        Settings.initialize();  //Инициализируем настройки
        bot = JDABuilder.createDefault(Settings.getBotToken()).build(); //Создаем бота и подключаем его на сервер
        DeezerMethods.initializeDeezer();  //Подключаемся к серверам Deezer и инициализируем клиент
        bot.addEventListener(new EventHandler());  //Добавляем обработчики событий
    }
}
