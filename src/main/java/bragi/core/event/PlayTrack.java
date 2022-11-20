package bragi.core.event;

import bragi.Bragi;
import bragi.core.Methods;
import bragi.core.Player;
import bragi.core.source.deezer.DeezerMethods;
import bragi.core.util.TrackInfo;
import net.dv8tion.jda.api.EmbedBuilder;
import net.dv8tion.jda.api.events.interaction.command.SlashCommandInteractionEvent;
import net.dv8tion.jda.api.events.message.MessageReceivedEvent;

import java.awt.*;
import java.util.List;
import java.util.Objects;

/** Класс для воспроизведения музыки */
public final class PlayTrack {
    /** Метод запуска воспроизведения музыки
     * @param event Событие получения сообщения
     * @param argument Аргумент, полученный из сообщения
     */
    public static void run(MessageReceivedEvent event, String argument) {
        /* Если не были переданы аргументы, и не были прикреплены вложения */
        if (argument == null && event.getMessage().getAttachments().isEmpty()) {
            event.getChannel().sendMessage("**:x: После команды не было передано обязательных аргументов!**").submit();
            return;
        }

        Player player = Bragi.Players.get(event.getGuild());  //Экземпляр проигрывателя
        boolean needToJoin = !Objects.requireNonNull(Objects.requireNonNull(event.getGuild()).getSelfMember()
                .getVoiceState()).inAudioChannel();

        /* Если нужно присоединяться, а пользователь не в голосовом канале */
        if (needToJoin && !Objects.requireNonNull(Objects.requireNonNull(event.getMember()).getVoiceState())
                .inAudioChannel()) {
            event.getChannel().sendMessage(":x:** Вы должны находиться в голосовом канале**").submit();
            return;
        }

        if (!event.getMessage().getAttachments().isEmpty()) {  //Если были переданы вложения
            /* Получаем список треков из вложений */
            List<TrackInfo> trackInfoList = Methods.getTracksFromAttachments(event.getMessage().getAttachments());

            if (trackInfoList.isEmpty()) {
                event.getChannel().sendMessage("**:x: Среди вложений не было аудио**").submit();
                return;
            }

            /* Пытаемся подключиться к голосовому каналу, если надо */
            if (needToJoin) {
                if (!JoinChannel.run(event, null)) return;  //Если не удалось подключиться к голосовому каналу
            }

            /* Пробегаем циклом по трекам и воспроизводим трек, либо добавляем его в плейлист, выводим результат */
            for (byte i = 0; i < trackInfoList.size(); i++) {
                try {
                    TrackInfo trackInfo = trackInfoList.get(i);  //Получаем объект с информацией о треке

                    /* Добавляем трек в очередь или сразу воспроизводим его */
                    Methods.playTrackOrAddItToPlaylist(Bragi.Players.get(event.getGuild()), trackInfo);

                    /* Строка, в которую записываем результат */
                    String result = String
                            .format("**:notes: В плейлист добавлено: `%s`\n:watch: Продолжительность: `%s`**",
                                    trackInfo.getTrackTitle(), trackInfo.getTrackDurationFormatted());

                    /* Если есть имя исполнителя */
                    result = trackInfo.getArtistName() != null ?
                            String.format("%s\n**:bust_in_silhouette: Исполнитель: `%s`**", result,
                                    trackInfo.getArtistName()) : result;

                    if (i + 1 != trackInfoList.size())  //Если трек не последний, необходимо добавить разделитель
                        result += "\n**────────────────────**";

                    event.getChannel().sendMessage(result).submit();  //Выводим информацию
                } catch (Exception ignore) {    }
            }
        } else {  //Если вложений нет, работаем с аргументом
            try {  //Пытаемся найти трек на сервере по запросу
                TrackInfo trackInfo = DeezerMethods.searchTrack(argument, 0);  //Получаем инфо трека
                try {
                    /* Пытаемся подключиться к голосовому каналу, если надо */
                    if (needToJoin) {
                        //Если не удалось подключиться к голосовому каналу, выходим из метода
                        if (!JoinChannel.run(event, null)) return;
                    }

                    /* Добавляем трек в очередь или сразу воспроизводим его */
                    Methods.playTrackOrAddItToPlaylist(player, trackInfo);

                    /* Выводим информацию */
                    event.getChannel().sendMessage(String
                            .format("**:notes: В плейлист добавлено: `%s`\n:watch: Продолжительность: `%s`**",
                            trackInfo.getTrackTitle(), trackInfo.getTrackDurationFormatted())).submit();
                    event.getChannel().sendMessageEmbeds(new EmbedBuilder()
                            .setColor(Color.decode("#FE2901"))
                            .setImage(trackInfo.getAlbumCoverUrl())
                            .addField("Альбом", trackInfo.getAlbumTitle(), false)
                            .addField("Исполнитель",trackInfo.getArtistName(), false)
                            .setThumbnail(trackInfo.getArtistPictureUrl()).build()).submit();
                } catch (Exception ignore) {    }
            } catch (Exception ignore) {  //Если трек мы не находим
                event.getChannel().sendMessage("**:x: Не удалось найти подходящую песню**").submit();
            }
        }
    }
    /** Метод запуска воспроизведения музыки
     * @param event Событие получения команды
     */
    public static void run(SlashCommandInteractionEvent event) {
        Player player = Bragi.Players.get(event.getGuild());  //Экземпляр проигрывателя
        boolean needToJoin = !Objects.requireNonNull(Objects.requireNonNull(event.getGuild()).getSelfMember()
                .getVoiceState()).inAudioChannel();

        /* Если нужно присоединяться, а пользователь не в голосовом канале */
        if (needToJoin && !Objects.requireNonNull(Objects.requireNonNull(event.getMember()).getVoiceState())
                .inAudioChannel()) {
            event.reply(":x:** Вы должны находиться в голосовом канале**").submit();
            return;
        }

        try {  //Пытаемся найти трек на сервере по запросу
            TrackInfo trackInfo = DeezerMethods.searchTrack(Objects.requireNonNull(event.getOption("query"))
                    .getAsString(), 0);  //Получаем инфо трека

            /* Пытаемся подключиться к голосовому каналу, если надо */
            boolean alreadyReplied = false;
            if (needToJoin) {
                if (!JoinChannel.run(event)) return;  //Если не удалось подключиться к голосовому каналу
                else alreadyReplied = true;  //В ином случае уже будет ответ
            }

            /* Добавляем трек в очередь или сразу воспроизводим его */
            Methods.playTrackOrAddItToPlaylist(player, trackInfo);

            /* Выводим информацию */
            if (alreadyReplied)
                event.getChannel().sendMessage(String.format(
                        "**:notes: В плейлист добавлено: `%s`\n:watch: Продолжительность: `%s`**",
                        trackInfo.getTrackTitle(), trackInfo.getTrackDurationFormatted())).submit();
            else
                event.reply(String.format("**:notes: В плейлист добавлено: `%s`\n:watch: Продолжительность: `%s`**",
                        trackInfo.getTrackTitle(), trackInfo.getTrackDurationFormatted())).submit();
            event.getChannel().sendMessageEmbeds(new EmbedBuilder()
                    .setColor(Color.decode("#FE2901"))
                    .setImage(trackInfo.getAlbumCoverUrl())
                    .addField("Альбом", trackInfo.getAlbumTitle(), false)
                    .addField("Исполнитель",trackInfo.getArtistName(), false)
                    .setThumbnail(trackInfo.getArtistPictureUrl()).build()).submit();
        } catch (Exception ignore) {  //Если трек мы не находим
            event.reply("**:x: Не удалось найти подходящую песню**").submit();
        }
    }
}
