package bragi;

import bragi.core.Settings;
import bragi.core.EventHandler;
import bragi.core.Player;
import bragi.core.source.deezer.DeezerMethods;
import net.dv8tion.jda.api.JDA;
import net.dv8tion.jda.api.JDABuilder;
import net.dv8tion.jda.api.entities.Guild;
import net.dv8tion.jda.api.interactions.commands.OptionType;

import java.io.File;
import java.util.HashMap;
import java.util.Objects;

public final class Bragi {
    public static HashMap<Guild, Player> Players = new HashMap<>();  //Инициализированные сервера

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
        JDA bot = JDABuilder.createDefault(Settings.getBotToken()).build(); //Создаем экземпляр бота
        bot.addEventListener(new EventHandler());  //Добавляем обработчики событий

        /* Добавляем слэш-команды */
        bot.upsertCommand("ping", "Получить информацию о задержке").submit();
        bot.upsertCommand("join", "Подключиться к голосовому каналу").submit();
        bot.upsertCommand("leave", "Покинуть голосовой канал").submit();
        bot.upsertCommand("list", "Вывести состояние плейлиста").submit();
        bot.upsertCommand("loop", "Переключить режим повторения").submit();
        bot.upsertCommand("skip", "Пропустить трек")
                .addOption(OptionType.INTEGER, "quantity", "Количество пропускаемых треков").submit();
        bot.upsertCommand("play", "Воспроизвести трек")
                .addOption(OptionType.STRING, "query", "Запрос, по которому ищется трек", true)
                .submit();
        bot.upsertCommand("next", "Следующий результат поиска")
                .addOption(OptionType.INTEGER, "number", "Номер трека для нового поиска")
                .submit();
        //endregion
    }
}
