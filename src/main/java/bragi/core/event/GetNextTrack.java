package bragi.core.event;

import bragi.core.source.deezer.DeezerMethods;
import bragi.core.util.TrackInfo;
import net.dv8tion.jda.api.EmbedBuilder;
import net.dv8tion.jda.api.events.message.MessageReceivedEvent;

import java.awt.*;
import java.io.File;
import java.util.ArrayList;

import static bragi.Bragi.Players;

public class GetNextTrack {
    public static EmbedBuilder run(MessageReceivedEvent event) {  //Метод, с помощью которого мы будем получать следующий трек из поискового запроса
        /* Получаем последний трек из списка */
        ArrayList<TrackInfo> list = Players.get(event.getGuild()).getPlaylist();

        if (list.size() == 0) {
            return new EmbedBuilder()
                    .setColor(Color.decode("#FE2901"))
                    .setDescription("**В плейлисте нет песен**");
        }

        TrackInfo trackInfo = list.get(list.size() - 1);  //Получаем последний элемент в списке и записываем его в переменную

        if (trackInfo.getSource().equals("Deezer")) {//Если трек с Deezer
            /* Если больше не существует результатов поиска */
            if (Integer.parseInt(trackInfo.getNextTrackInSearchResults()) > trackInfo.getTotalOfSearchResults()) {
                return new EmbedBuilder()
                        .setColor(Color.decode("#FE2901"))
                        .setDescription("**По данному поисковому запросу больше не было найдено результатов**");
            }

            try {
                /* Выполняем поиск трека, начиная не с первого элемента */
                TrackInfo newTrackInfo = DeezerMethods.searchTrack(trackInfo.getSearchRequest(), Integer.parseInt(trackInfo.getNextTrackInSearchResults()));

                list.remove(list.size() - 1);  //Удаляем последний элемент в списке
                new File(String.format("/tmp/%s.flac", trackInfo.getTrackId())).delete();  //Удаляем  временный файл, если он есть
                Players.get(event.getGuild()).decreaseTotalDuration(trackInfo.getTrackDuration());  //Уменьшаем общую продолжительность треков

                /* Добалвяем в очередь, или начинаем воспроизводить (в зависимости от состояния плейлиста) и возвращаем вывод*/
                return bragi.core.Methods.playTrackOrAddItToPlaylist(newTrackInfo, event);
            } catch (Exception ignore) {
                return new EmbedBuilder()
                        .setColor(Color.decode("#FE2901"))
                        .setDescription("**По данному поисковому запросу больше не было найдено результатов**");
            }
        }
        else {  //Если же нет, то выводим себе напоминание
            return new EmbedBuilder()
                    .setColor(Color.decode("#FE2901"))
                    .setDescription("**Эта песня не была найдена с помощью поискового запроса**");
        }
    }
}
