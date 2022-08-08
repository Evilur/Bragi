package bragi.util;

import net.dv8tion.jda.api.EmbedBuilder;

import java.awt.*;

import static bragi.Bragi.Players;
import static java.lang.String.valueOf;

public class Informant {

    /* Метод для вывода информаци о треках, содеражит несколько перегрузок для вывода разного количества информации */
    public static EmbedBuilder getOutputInformation(String trackTitle, String trackDuration) {
        return new EmbedBuilder()
                .setColor(Color.decode("#FE2901"))
                .setDescription(String.format("Название: **%s**\nПродолжительность: **%s**", trackTitle, trackDuration));
    }
    public static EmbedBuilder getOutputInformation(String trackTitle, String artistName, String trackDuration) {
        if (artistName == null)  //если у нас все-таки нет имени исполнителя
            return new EmbedBuilder()
                    .setColor(Color.decode("#FE2901"))
                    .setDescription(String.format("Название: **%s**\nПродолжительность: **%s**", trackTitle, trackDuration));
        else  //Если у нас есть имя исполнителя
            return new EmbedBuilder()
                    .setColor(Color.decode("#FE2901"))
                    .setDescription(String.format("Название: **%s**\nИсполнитель: **%s**\nПродолжительность: **%s**", trackTitle, artistName, trackDuration));
    }
}
