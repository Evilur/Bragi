package bragi.core.event;

import bragi.core.Player;
import bragi.core.util.TrackInfo;
import net.dv8tion.jda.api.EmbedBuilder;

import java.awt.*;

public class GetPlaylist {
    public static EmbedBuilder run(Player player) {  //Метод для вывода списка воспроизведения
        /* Если плейлист пуст */
        if (player.getPlaylist().size() == 0) {
            return new EmbedBuilder()
                    .setColor(Color.decode("#0BDA4D"))
                    .setDescription("**В плейлисте нет треков для воспроизведения**");
        }

        /* Если плейлист не пуст, перебираем его циклом, форматируем и записываем результат в переменную */
        StringBuilder result = new StringBuilder();  //Сюда будем записывать результат

        /* Циклом перебираем плейлист и дабавляем это к результату */
        for (int i = 0; i < player.getPlaylist().size(); i++) {
            TrackInfo trackInfo = player.getPlaylist().get(i);
            result.append(String.format("%d. %s\n", i + 1, trackInfo.getTrackTitle()));
        }

        if (player.isLoopMode())  //Если включено повторение трека, заявляем об этом
            result.append("————————————————————\nВключено повторения треков");

        /* Возвращаем информацию о плейлисте */
        return new EmbedBuilder()
                .setColor(Color.decode("#0BDA4D"))
                .setTitle("**Текущий плейлист:**")
                .setDescription(String.format("**Общая продолжительность: %s\n————————————————————\n%s**", player.getTotalDuration(), result));
    }
}
