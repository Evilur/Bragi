package bragi;

import bragi.core.Settings;
import bragi.core.EventHandler;
import bragi.core.Player;
import bragi.core.source.deezer.DeezerMethods;
import net.dv8tion.jda.api.JDA;
import net.dv8tion.jda.api.JDABuilder;
import net.dv8tion.jda.api.entities.Guild;
import net.dv8tion.jda.api.interactions.commands.OptionType;
import net.dv8tion.jda.api.interactions.commands.build.SubcommandData;

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
        DeezerMethods.initializeDeezer();  //Подключаемся к серверам Deezer и инициализируем клиент

        //region Инициализация бота
        bot = JDABuilder.createDefault(Settings.getBotToken()).build(); //Создаем бота и подключаем его на сервер
        bot.addEventListener(new EventHandler());  //Добавляем обработчики событий

        /* Добавляем слэш-команды */
        bot.upsertCommand("play", "Воспроизвести трек или добавить его в плейлист")
                .addSubcommands(
                        new SubcommandData("track", "Найти трек по запросу, воспроизвести его или добавить в плейлист")
                                .addOption(OptionType.STRING, "request", "Поисковой запрос", true),
                        new SubcommandData("attachment", "Найти трек во вложениях, воспроизвести его или добавить в плейлист")
                                .addOption(OptionType.ATTACHMENT, "attachment", "Вложение", true)
                ).submit();
        bot.upsertCommand("join", "Подключиться к голосовому каналу").submit();
        bot.upsertCommand("leave", "Покинуть голосовой канал").submit();
        bot.upsertCommand("ping", "Получить информацию о задержке").submit();
        //endregion
    }
}
