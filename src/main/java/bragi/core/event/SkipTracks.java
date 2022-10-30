package bragi.core.event;

import bragi.Bragi;
import bragi.core.Player;
import net.dv8tion.jda.api.entities.Guild;
import net.dv8tion.jda.api.events.interaction.command.SlashCommandInteractionEvent;
import net.dv8tion.jda.api.events.message.MessageReceivedEvent;

import java.util.Objects;

/** Класс для пропуска треков из плейлиста */
public class SkipTracks {
    /** Метод для запуска попуска треков из плейлиста
     * @param event Событие получения команды
     * @param argument Аргумент, полученный из сообщения
     */
    public static void run(MessageReceivedEvent event, String argument) {
        argument = argument == null ? "" :
                argument.replaceAll("[^0-9]", "");  //Очищаем аргумент от не цифр

        /* Получаем количество пропускаемых треков */
        int numberOfTracks = !argument.equals("") ? Integer.parseInt(argument) : 1;

        /* Пропускаем треки и выводим результат */
        try {
            event.getChannel().sendMessage(String.format("**:track_next: Треков пропущено: `%s`**",
                    eval(event.getGuild(), numberOfTracks, true))).submit();
        }
        catch (Exception ignore) {
            event.getChannel().sendMessage("**:bangbang: Плейлист пуст**").submit();
        }
    }
    /** Метод для запуска попуска треков из плейлиста
     * @param event Событие получения команды
     */
    public static void run(SlashCommandInteractionEvent event) {
        /* Получаем количество пропускаемых треков из опции */
        int numberOfTracks = event.getOption("quantity") == null ? 1 :
                Objects.requireNonNull(event.getOption("quantity")).getAsInt();

        /* Пропускаем треки и выводим результат */
        try {
            event.reply(String.format("**:track_next: Треков пропущено: `%s`**",
                    eval(event.getGuild(), numberOfTracks, true))).submit();
        }
        catch (Exception ignore) {
            event.reply("**:bangbang: Плейлист пуст**").submit();
        }
    }

    /** Метод для запуска попуска одного трека из плейлиста
     * @param guild Гильдия для пропуска треков
     */
    public static void run(Guild guild) {
        eval(guild, 1, false);
    }

    private static int eval(Guild guild, int numberOfTracks, boolean hardSkip) {
        Player player = Bragi.Players.get(guild);  //Получаем проигрыватель

        /* Если пользователь хочет пропустить треков больше, чем существует в плейлисте, ограничим его хотения */
        if (numberOfTracks > player.getPlaylist().size())
            numberOfTracks = player.getPlaylist().size();
        else if (numberOfTracks < 1)  //Если не передано число, присваиваем единицу, то есть убираем один трек
            numberOfTracks = 1;

        /* Если не стоит повторение или трек пропускается вручную */
        if (!player.isLoopMode() || hardSkip) {  //Удаляем элементы
            player.decreaseTotalDuration(player.getPlaylist().get(0).getTrackDuration()); //Уменьшаем общую длину треков
            player.getPlaylist().subList(0, numberOfTracks).clear();  //Удаляем первые элементы из списка
        }

        /* Если в плейлисте есть треки */
        if (player.getPlaylist().size() > 0) {
            String url = player.getPlaylist().get(0).getTrackIdentifier();  //Получаем url трека
            player.getInstance().Play(url);  //Воспроизводим трек
        }
        else {  //Если треков в плейлисте нет
            player.getInstance().Stop();  //Останавливаем проигрывание
            LeaveChannel.run(guild);  //Покидаем канал
        }

        return numberOfTracks;  //Возвращаем число пропущенных треков
    }
}
