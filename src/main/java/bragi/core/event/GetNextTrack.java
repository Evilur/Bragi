package bragi.core.event;

import bragi.core.Player;
import bragi.core.source.deezer.DeezerMethods;
import bragi.core.util.TrackInfo;
import net.dv8tion.jda.api.EmbedBuilder;
import net.dv8tion.jda.api.events.interaction.command.SlashCommandInteractionEvent;
import net.dv8tion.jda.api.events.message.MessageReceivedEvent;

import java.awt.*;
import java.util.ArrayList;
import java.util.Objects;

import static bragi.Bragi.Players;

/** Класс, с помощью которого мы будем получать следующий трек из поискового запроса */
public final class GetNextTrack {
    /** С помощью этого метода будем получать следующий трек из поискового запроса
     * @param event Событие получения сообщения
     */
    public static void run(MessageReceivedEvent event, String argument) {
        Player player = Players.get(event.getGuild());  //Экземпляр проигрывателя
        String state = "В плейлист добавлено";  //Состояние плеера
        ArrayList<TrackInfo> playlist = player.getPlaylist();  //Список треков в плейлисте

        if (playlist.isEmpty()) {  //Если плейлист пуст
            event.getChannel().sendMessage("**:x: В плейлисте нет песен**").submit();
            return;
        }

        /* Очищаем аргумент от не цифр */
        argument = argument == null ? "" : argument.replaceAll("[^0-9]", "");
        /* Пользователь может передать слишком большое число */
        int trackNumber = !argument.equals("") ? Integer.parseInt(argument) > playlist.size() ? 0 :
                Integer.parseInt(argument) - 1 : playlist.size() - 1;

        /* Получаем элемент трека в списке и записываем его в переменную */
        TrackInfo trackInfo = playlist.get(trackNumber);

        if (trackInfo.getSource().equals("Deezer")) {  //Если трек с Deezer
            /* Если больше не существует результатов поиска */
            if (Integer.parseInt(trackInfo.getNextTrackInSearchResults()) > trackInfo.getTotalOfSearchResults())
                event.getChannel().sendMessage(
                        "**:bangbang: По данному поисковому запросу больше не было найдено результатов**").submit();

            try {  //На всякий случай делаем еще одну проверку
                /* Заменяем предыдущий трек на новый */
                trackInfo = DeezerMethods.searchTrack(trackInfo.getSearchRequest(),
                        Integer.parseInt(trackInfo.getNextTrackInSearchResults()));
                player.replaceTrack(trackNumber, trackInfo);

                if (trackNumber == 0) {  //Если мы заменяем самый первый элемент, то нужно начать воспроизведение заново
                    player.getInstance().Play(trackInfo.getTrackIdentifier());
                    state = "Сейчас играет";
                }

                /* Выводим информацию о треке */
                event.getChannel().sendMessage(String.format("**:notes: %s: `%s`\n:watch: Продолжительность: `%s`**",
                        state, trackInfo.getTrackTitle(), trackInfo.getTrackDurationFormatted())).submit();
                event.getChannel().sendMessageEmbeds(new EmbedBuilder()
                        .setColor(Color.decode("#FE2901"))
                        .setImage(trackInfo.getAlbumCoverUrl())
                        .addField("Альбом", trackInfo.getAlbumTitle(), false)
                        .addField("Исполнитель",trackInfo.getArtistName(), false)
                        .setThumbnail(trackInfo.getArtistPictureUrl()).build()).submit();
            } catch (Exception ignore) {
                event.getChannel().sendMessage(
                        "**:bangbang: По данному поисковому запросу больше не было найдено результатов**").submit();
            }
        }
        else {  //Если же нет, то объявляем об этом
            event.getChannel().sendMessage("**:x: Эта песня не была найдена с помощью поискового запроса**").submit();
        }
    }
    /** С помощью этого метода будем получать следующий трек из поискового запроса
     * @param event Событие получения сообщения
     */
    public static void run(SlashCommandInteractionEvent event) {
        Player player = Players.get(event.getGuild());  //Экземпляр проигрывателя
        String state = "В плейлист добавлено";  //Состояние плеера
        ArrayList<TrackInfo> playlist = player.getPlaylist();  //Список треков в плейлисте

        if (playlist.isEmpty()) {  //Если плейлист пуст
            event.reply("**:x: В плейлисте нет песен**").submit();
            return;
        }

        /* Получаем номер трека, пользователь может передать слишком большое число */
        int argument = !event.getOptionsByName("number").isEmpty() ?
                Objects.requireNonNull(event.getOption("number")).getAsInt() : playlist.size();
        argument = argument < 0 ? -1 * argument : argument;
        int trackNumber = argument > playlist.size() ? 0 : argument - 1;

        /* Получаем элемент трека в списке и записываем его в переменную */
        TrackInfo trackInfo = playlist.get(trackNumber);

        if (trackInfo.getSource().equals("Deezer")) {  //Если трек с Deezer
            /* Если больше не существует результатов поиска */
            if (Integer.parseInt(trackInfo.getNextTrackInSearchResults()) > trackInfo.getTotalOfSearchResults())
                event.reply("**:bangbang: По данному поисковому запросу больше не было найдено результатов**")
                        .submit();

            try {  //На всякий случай делаем еще одну проверку
                /* Заменяем предыдущий трек на новый */
                trackInfo = DeezerMethods.searchTrack(trackInfo.getSearchRequest(),
                        Integer.parseInt(trackInfo.getNextTrackInSearchResults()));
                player.replaceTrack(trackNumber, trackInfo);

                if (trackNumber == 0) {  //Если мы заменяем самый первый элемент, то нужно начать воспроизведение заново
                    player.getInstance().Play(trackInfo.getTrackIdentifier());
                    state = "Сейчас играет";
                }

                /* Выводим информацию о треке */
                event.reply(String.format("**:notes: %s: `%s`\n:watch: Продолжительность: `%s`**",
                        state, trackInfo.getTrackTitle(), trackInfo.getTrackDurationFormatted())).submit();
                event.getChannel().sendMessageEmbeds(new EmbedBuilder()
                        .setColor(Color.decode("#FE2901"))
                        .setImage(trackInfo.getAlbumCoverUrl())
                        .addField("Альбом", trackInfo.getAlbumTitle(), false)
                        .addField("Исполнитель",trackInfo.getArtistName(), false)
                        .setThumbnail(trackInfo.getArtistPictureUrl()).build()).submit();
            } catch (Exception ignore) {
                event.reply("**:bangbang: По данному поисковому запросу больше не было найдено результатов**")
                        .submit();
            }
        }
        else {  //Если же нет, то объявляем об этом
            event.reply("**:x: Эта песня не была найдена с помощью поискового запроса**").submit();
        }
    }
}
