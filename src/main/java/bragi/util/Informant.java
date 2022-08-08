package bragi.util;

import net.dv8tion.jda.api.EmbedBuilder;

import java.awt.*;

public class Informant {

    /* Метод для вывода информаци о треках, содеражит несколько перегрузок для вывода разного количества информации */
    public static EmbedBuilder getOutputInformation(String trackTitle, String trackDuration, boolean isPlayingNow) {
        /* Состояние трека может изменяться, поэтому мы вычисляем его */
        String trackState = isPlayingNow? String.format("Сейчас играет: **%s**", trackTitle) : String.format("Трек **%s** добавлен в плейлист", trackTitle);

        return new EmbedBuilder()
                .setColor(Color.decode("#FE2901"))
                .setDescription(String.format("%s\nПродолжительность: **%s**", trackState, trackDuration));
    }
    public static EmbedBuilder getOutputInformation(String trackTitle, String artistName, String trackDuration, boolean isPlayingNow) {
        /* Состояние трека может изменяться, поэтому мы вычисляем его */
        String trackState = isPlayingNow? String.format("Сейчас играет: **%s**", trackTitle) : String.format("Трек **%s** добавлен в плейлист", trackTitle);

        if (artistName == null)  //если у нас все-таки нет имени исполнителя
            return new EmbedBuilder()
                    .setColor(Color.decode("#FE2901"))
                    .setDescription(String.format("%s\nПродолжительность: **%s**", trackState, trackDuration));
        else  //Если у нас есть имя исполнителя
            return new EmbedBuilder()
                    .setColor(Color.decode("#FE2901"))
                    .setDescription(String.format("%s\nИсполнитель: **%s**\nПродолжительность: **%s**", trackState, artistName, trackDuration));
    }
}
