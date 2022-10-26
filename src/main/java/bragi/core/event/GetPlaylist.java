package bragi.core.event;

import bragi.Bragi;
import bragi.core.Player;
import net.dv8tion.jda.api.EmbedBuilder;
import net.dv8tion.jda.api.events.interaction.command.SlashCommandInteractionEvent;
import net.dv8tion.jda.api.events.message.MessageReceivedEvent;

import java.awt.*;

/** Класс для вывода списка воспроизведения */
public class GetPlaylist {
    /** Метод для запуска скрипта, выводящего информацию о плейлисте
     * @param event Событие получения сообщения
     */
    public static void run(MessageReceivedEvent event) {
        Player player = Bragi.Players.get(event.getGuild());  //Получаем прогрыватель сервера

        /* Если плейлист пуст */
        if (player.getPlaylist().size() == 0) {  //Сообщаем о том, что плейлист пуст
            event.getChannel().sendMessage("**:bangbang: В плейлисте нет треков для воспроизведения**").submit();
            return;
        }

        /* Если плейлист не пуст, выводим информацию о нем */
        event.getChannel().sendMessageEmbeds(new EmbedBuilder()
                .setColor(Color.decode("#0BDA4D"))
                .setTitle(String.format("**Текущий плейлист (%s):**", player.getTotalDuration()))
                .setDescription(eval(player)).build()).submit();
    }
    /** Метод для запуска скрипта, выводящего информацию о плейлисте
     * @param event Событие получения команды
     */
    public static void run(SlashCommandInteractionEvent event) {
        Player player = Bragi.Players.get(event.getGuild());  //Получаем прогрыватель сервера

        /* Если плейлист пуст */
        if (player.getPlaylist().size() == 0) {  //Сообщаем о том, что плейлист пуст
            event.reply("**:bangbang: В плейлисте нет треков для воспроизведения**").submit();
            return;
        }

        /* Если плейлист не пуст, выводим информацию о нем */
        event.replyEmbeds(new EmbedBuilder()
                .setColor(Color.decode("#0BDA4D"))
                .setTitle(String.format("**Текущий плейлист (%s):**", player.getTotalDuration()))
                .setDescription(eval(player)).build()).submit();
    }

    /** Метод для создания строки для вывода информации о плейлисте
     * @param player Экземпляр проигрывателя сервера
     * @return Информация о плейлисте в виде строки для вывода
     */
    private static String eval(Player player) {
        /* Перебираем плейлист циклом, форматируем и записываем результат в переменную */
        StringBuilder result = new StringBuilder();  //Сюда будем записывать результат

        /* Циклом перебираем плейлист и дабавляем это к результату */
        for (int i = 0; i < player.getPlaylist().size(); i++)
            result.append(String.format("%d. %s\n", i + 1, player.getPlaylist().get(i).getTrackTitle()));

        if (player.isLoopMode())  //Если включено повторение трека, объявляем об этом
            result.append("————————————————————\nВключено повторения треков");

        return "**" + result + "**";
    }
}
