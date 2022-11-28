package bragi.core.event;

import bragi.Bragi;
import bragi.core.Player;
import bragi.core.util.TrackInfo;
import net.dv8tion.jda.api.EmbedBuilder;
import net.dv8tion.jda.api.events.interaction.command.SlashCommandInteractionEvent;
import net.dv8tion.jda.api.events.message.MessageReceivedEvent;

import java.awt.*;
import java.util.ArrayList;

/** Класс для вывода списка воспроизведения */
public final class GetPlaylist {
    /** Метод для запуска скрипта, выводящего информацию о плейлисте
     * @param event Событие получения сообщения
     */
    public static void run(MessageReceivedEvent event) {
        Player player = Bragi.Players.get(event.getGuild());  //Получаем экземпляр проигрывателя
        EmbedBuilder embed = new EmbedBuilder();  //Инициализируем embed для вывода

        String out = eval(player, embed);  //Получаем вывод

        if (out != null)  //Если строка вывода не равна null'у, то не выводим embed
            event.getChannel().sendMessage(out).submit();
        else  //В ином случае - выводим
            event.getChannel().sendMessageEmbeds(embed.build()).submit();
    }
    /** Метод для запуска скрипта, выводящего информацию о плейлисте
     * @param event Событие получения команды
     */
    public static void run(SlashCommandInteractionEvent event) {
        Player player = Bragi.Players.get(event.getGuild());  //Получаем экземпляр проигрывателя
        EmbedBuilder embed = new EmbedBuilder();  //Инициализируем embed для вывода

        String out = eval(player, embed);  //Получаем вывод

        if (out != null)  //Если строка вывода не равна null'у, то не выводим embed
            event.reply(out).submit();
        else  //В ином случае - выводим
            event.replyEmbeds(embed.build()).submit();
    }

    /** Метод для создания строки для вывода информации о плейлисте
     * @param player Экземпляр проигрывателя
     * @param embed embed для вывода информации
     * @return Информация о плейлисте в виде строки для вывода
     */
    private static String eval(Player player, EmbedBuilder embed) {
        ArrayList<TrackInfo> playlist = player.getPlaylist();  //Плейлист гильдии

        /* Если плейлист пуст */
        if (playlist.size() == 0)  //Сообщаем о том, что плейлист пуст
            return "**:bangbang: В плейлисте нет треков для воспроизведения**";

        /* Перебираем плейлист циклом, форматируем и записываем результат в переменную */
        StringBuilder result = new StringBuilder();  //Сюда будем записывать результат

        /* Циклом перебираем плейлист и добавляем это к результату */
        for (int i = 0; i < playlist.size(); i++)
            result.append(String.format("%d. %s\n", i + 1, playlist.get(i).getTrackTitle()));

        if (player.isLoopMode())  //Если включено повторение трека, объявляем об этом
            result.append("─────────────────────\n:repeat: Включено повторения треков");

        /* Создаем вывод */
        embed.setColor(Color.decode("#0BDA4D"))
                .setTitle(String.format("**Текущий плейлист (%s):**", player.getTotalDuration()))
                .setDescription("**" + result + "**").build();
        return null;
    }
}
