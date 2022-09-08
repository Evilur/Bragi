package bragi.core.event;

import bragi.core.util.TrackInfo;
import net.dv8tion.jda.api.EmbedBuilder;
import net.dv8tion.jda.api.entities.Guild;

import java.awt.*;

import static bragi.Bragi.Players;

public class GetPlaylist {
    public static EmbedBuilder run(Guild guild) {  //Метод для вывода списка воспроизведения
        /* Если плейлист пуст */
        if (Players.get(guild).getPlaylist().size() == 0) {
            return new EmbedBuilder()
                    .setColor(Color.decode("#0BDA4D"))
                    .setDescription("**В плейлисте нет треков для воспроизведения**");
        }

        /* Если плейлист не пуст, перебираем его циклом, форматируем и записываем результат в переменную */
        StringBuilder result = new StringBuilder();  //Сюда будем записывать результат

        /* Циклом перебираем плейлист и дабавляем это к результату */
        for (int i = 0; i < Players.get(guild).getPlaylist().size(); i++) {
            TrackInfo trackInfo = Players.get(guild).getPlaylist().get(i);
            result.append(String.format("%d. %s\n", i + 1, trackInfo.getTrackTitle()));
        }

        if (Players.get(guild).isLoopMode())  //Если включено повторение трека, заявляем об этом
            result.append("————————————————————\nВключено повторения треков");

        /* Возвращаем информацию о плейлисте */
        return new EmbedBuilder()
                .setColor(Color.decode("#0BDA4D"))
                .setTitle("**Текущий плейлист:**")
                .setDescription(String.format("**Общая продолжительность: %s\n————————————————————\n%s**", Players.get(guild).getTotalDuration(), result));
    }
}
