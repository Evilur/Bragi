package bragi.core.event;

import bragi.Bragi;
import bragi.core.Player;
import bragi.core.source.deezer.DeezerClient;
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
        EmbedBuilder embed = new EmbedBuilder();  //Инициализируем embed для вывода

        /* Очищаем аргумент от не цифр */
        argument = argument == null ? "" : argument.replaceAll("[^0-9]", "");
        /* Пользователь может передать слишком большое число */
        int trackNumber = !argument.equals("") ? Integer.parseInt(argument) > player.getPlaylist().size() ? 0 :
                Integer.parseInt(argument) - 1 : player.getPlaylist().size() - 1;

        String output = eval(player, trackNumber, embed);  //Получаем вывод

        /* Выводим информацию */
        event.getChannel().sendMessage(output).submit();
        event.getChannel().sendMessageEmbeds(embed.build()).submit();
    }
    /** С помощью этого метода будем получать следующий трек из поискового запроса
     * @param event Событие получения сообщения
     */
    public static void run(SlashCommandInteractionEvent event) {
        Player player = Bragi.Players.get(event.getGuild());  //Экземпляр проигрывателя
        EmbedBuilder embed = new EmbedBuilder();  //Инициализируем embed для вывода

        /* Получаем номер трека, пользователь может передать слишком большое число */
        int argument = !event.getOptionsByName("number").isEmpty() ?
                Objects.requireNonNull(event.getOption("number")).getAsInt() : player.getPlaylist().size();

        argument = argument < 0 ? -1 * argument : argument;  //Если аргумент меньше нуля
        int trackNumber = argument > player.getPlaylist().size() ? 0 : argument - 1;  //Номер трека для пропуска

        String output = eval(player, trackNumber, embed);  //Получаем вывод

        /* Выводим информацию */
        event.reply(output).submit();
        event.getChannel().sendMessageEmbeds(embed.build()).submit();
    }

    /** Метод для проведения операций по получению следующего трека
     * @param player Проигрыватель гильдии
     * @param trackNumber Трек, который надо заменить
     * @param embed Embed, куда будет записываться часть вывода
     * @return Строку с выводом
     */
    private static String eval(Player player, int trackNumber, EmbedBuilder embed) {
        String state = "В плейлист добавлено";  //Состояние плеера
        ArrayList<TrackInfo> playlist = player.getPlaylist();  //Список треков в плейлисте

        if (playlist.isEmpty()) {  //Если плейлист пуст
            return "**:x: В плейлисте нет песен**";
        }

        /* Получаем элемент трека в списке и записываем его в переменную */
        TrackInfo trackInfo = playlist.get(trackNumber);

        if (trackInfo.getSource().equals("Deezer")) {  //Если трек с Deezer
            /* Если больше не существует результатов поиска */
            if (Integer.parseInt(trackInfo.getNextTrackInSearchResults()) > trackInfo.getTotalOfSearchResults())
                return "**:bangbang: По данному поисковому запросу больше не было найдено результатов**";

            try {  //На всякий случай делаем еще одну проверку
                /* Заменяем предыдущий трек на новый */
                trackInfo = DeezerClient.searchTrack(trackInfo.getSearchRequest(),
                        Integer.parseInt(trackInfo.getNextTrackInSearchResults()));
                playlist.set(trackNumber, trackInfo);

                if (trackNumber == 0) {  //Если мы заменяем самый первый элемент, то нужно начать воспроизведение заново
                    player.getInstance().Play(trackInfo.getTrackIdentifier());
                    state = "Сейчас играет";
                }

                /* Возвращаем информацию о треке */
                embed.setColor(Color.decode("#FE2901"))
                        .setImage(trackInfo.getAlbumCoverUrl())
                        .addField("Альбом", trackInfo.getAlbumTitle(), false)
                        .addField("Исполнитель",trackInfo.getArtistName(), false)
                        .setThumbnail(trackInfo.getArtistPictureUrl());
                return String.format("**:notes: %s: `%s`\n:watch: Продолжительность: `%s`**",
                        state, trackInfo.getTrackTitle(), trackInfo.getTrackDurationFormatted());
            } catch (Exception ignore) {
                return "**:bangbang: По данному поисковому запросу больше не было найдено результатов**";
            }
        }
        else {  //Если же нет, то объявляем об этом
            return "**:x: Эта песня не была найдена с помощью поискового запроса**";
        }
    }
}
