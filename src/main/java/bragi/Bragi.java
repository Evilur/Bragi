package bragi;

import bragi.util.DeezerMethods;
import bragi.util.EventHandler;
import bragi.util.Player;
import net.dv8tion.jda.api.JDA;
import net.dv8tion.jda.api.JDABuilder;
import net.dv8tion.jda.api.entities.Guild;

import java.io.File;
import java.util.HashMap;
import java.util.Objects;

public class Bragi {
    public static HashMap<Guild, Player> Players = new HashMap<>();  //Инициализированные сервера

    public static void main(String[] args) throws Exception {
        /* Очищаем мусор от предыдущей сессии */
        for (File file : Objects.requireNonNull(new File("/tmp").listFiles())) {
            if (file.getName().endsWith(".flac")) {
                if (!file.delete())
                    System.out.printf("Error while deleting file \"%s\"\n", file.getName());
            }
        }

        JDA bot = JDABuilder.createDefault(Settings.getBotToken()).build(); //Создаем бота и подключаем его на сервер
        DeezerMethods.setSessionId();  //Подключаемся к серверам Deezer и получаем Id сессии
        bot.addEventListener(new EventHandler());  //Добавляем обработчики событий
    }
}
