package bragi.core.event;

import bragi.Bragi;
import bragi.core.Player;
import bragi.core.util.TrackInfo;
import net.dv8tion.jda.api.entities.Guild;
import net.dv8tion.jda.api.events.interaction.command.SlashCommandInteractionEvent;
import net.dv8tion.jda.api.events.message.MessageReceivedEvent;

import java.util.ArrayList;
import java.util.Objects;

/** Класс для удаления определенных треков из очереди */
public class RemoveTracks {
    /** Метод для запуска процесса удаления определенных треков из очереди
     * @param event Событие получения сообщения
     * @param argument Запрос, по которому будет производиться удаление
     */
    public static void run(MessageReceivedEvent event, String argument) {
        /* Если аргумент не был передан */
        if (argument == null) {
            event.getChannel().sendMessage("**:x: После команды не было передано обязательных аргументов!**").submit();
            return;
        } else if (Bragi.Players.get(event.getGuild()).getPlaylist().size() == 0) {  //Если плейлист пуст
            event.getChannel().sendMessage("**:bangbang: В плейлисте нет треков**").submit();
            return;
        }

        short quantity = eval(event.getGuild(), argument);

        if (quantity > 0)  //Если был пропущен хотя бы один трек
            event.getChannel().sendMessage(String.format("**:wastebasket: Треков удалено из плейлиста: `%s`**",
                    quantity)).submit();
        else
            event.getChannel().sendMessage("**:x: По данному запросу не удалось удалить ни один трек**").submit();
    }

    /** Метод для запуска процесса удаления определенных треков из очереди
     * @param event Событие получения команды
     */
    public static void run(SlashCommandInteractionEvent event) {
        if (Bragi.Players.get(event.getGuild()).getPlaylist().size() == 0) {  //Если плейлист пуст
            event.reply("**:bangbang: В плейлисте нет треков**").submit();
            return;
        }

        String argument = Objects.requireNonNull(event.getOption("query")).getAsString();  //Получаем аргумент
        short quantity = eval(event.getGuild(), argument);

        if (quantity > 0)  //Если был пропущен хотя бы один трек
            event.reply(String.format("**:wastebasket: Треков удалено из плейлиста: `%s`**", quantity)).submit();
        else
            event.reply("**:x: По данному запросу не удалось удалить ни один трек**").submit();
    }

    /** Метод для проведения процесса удаления треков из плейлиста
     * @param guild Гильдия, с плейлистом которой надо работать
     * @param argument Аргумент, содержащий запросы на удаление
     * @return Количество удаленных треков
     */
    private static short eval(Guild guild, String argument) {
        argument = argument.replaceAll("([^0-9-,]|^-+)", "")
                .replaceAll("-+", "-");  //Чистим строку от лишних символов
        String[] queries = argument.split(",");  //Получаем запросы, разделенные запятой
        Player player = Bragi.Players.get(guild);  //Получаем экземпляр проигрывателя
        ArrayList<TrackInfo> playlist = player.getPlaylist();  //Получаем ссылку на плейлист

        for (String query : queries) {
            /* Проверяем, является ли запрос промежутком */
            if (query.contains("-")) {
                /* Чистим и делим строку */
                query = query.replaceAll("-+", "-");  //Чистим строку
                String[] indexes = query.split("-");

                /* Получаем индексы */
                int i1 = Integer.parseInt(indexes[0]) - 1;
                int i2 = Integer.parseInt(indexes[1]) - 1;

                /* Обрабатываем индексы */
                if (i2 < i1) i1 = i1 + i2 - (i2 = i1);  //Если второй индекс меньше первого, меняем их местами
                if (i1 < 0) i1 = 0;  //Первый индекс не должен быть меньше нуля
                if (i2 < 0) i2 = 0;  //Второй индекс тоже не может быть меньше нуля
                if (i1 >= playlist.size()) i1 = playlist.size() - 1;  //Первый индекс меньше длины плейлиста
                if (i2 >= playlist.size()) i2 = playlist.size() - 1;  //Второй индекс тоже меньше длины плейлиста

                /* Циклом перебираем необходимые элементы плейлиста, и заменяем их null'ами */
                for ( ; i1 <= i2; i1++) {
                    //Уменьшаем общую продолжительность треков
                    if (playlist.get(i1) != null) player.decreaseTotalDuration(playlist.get(i1).getTrackDuration());
                    playlist.set(i1, null);  //Заменяем элемент на null
                }
            } else if (query.length() > 0) {  //Если не является промежутком, значит это просто число.
                int i = Integer.parseInt(query) - 1;  //Получаем индекс

                /* Обрабатываем индекс */
                if (i < 0) continue;  //Индекс не должен быть меньше нуля
                if (i >= playlist.size()) continue;  //Индекс должен быть меньше длины плейлиста

                //Уменьшаем общую продолжительность треков
                if (playlist.get(i) != null) player.decreaseTotalDuration(playlist.get(i).getTrackDuration());
                playlist.set(i, null);  //Заменяем элемент, который хотим удалить на null
            }
        }

        boolean needToPlayNext = playlist.get(0) == null;  //Нужно ли воспроизвести следующий трек из плейлиста
        short numberOfDeleted = 0;  //Количество удаленных треков

        /* Циклом пробегаемся по элементам плейлиста и удаляем все null'ы */
        for (TrackInfo trackInfo : playlist)
            if (trackInfo == null) numberOfDeleted++;

        /* Удаляем все null'ы */
        playlist.removeIf(Objects::isNull);

        /* Если необходимо воспроизвести следующий трек */
        if (needToPlayNext) {
            /* Если в плейлисте еще есть треки, то воспроизводим, в противном случае - просто покидаем канал */
            if (playlist.size() > 0) player.getInstance().Play(playlist.get(0).getTrackIdentifier());
            else LeaveChannel.run(guild);
        }

        return numberOfDeleted;  //Возвращаем количество удаленных треков
    }
}
