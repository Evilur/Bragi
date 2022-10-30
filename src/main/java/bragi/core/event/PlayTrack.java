package bragi.core.event;

import bragi.Bragi;
import bragi.core.Methods;
import bragi.core.Player;
import bragi.core.source.deezer.DeezerMethods;
import bragi.core.util.TrackInfo;
import net.dv8tion.jda.api.EmbedBuilder;
import net.dv8tion.jda.api.entities.Message;
import net.dv8tion.jda.api.events.interaction.command.SlashCommandInteractionEvent;
import net.dv8tion.jda.api.events.message.MessageReceivedEvent;

import java.awt.*;
import java.util.ArrayList;
import java.util.List;
import java.util.Objects;

/** Класс для воспроизведения музыки */
public class PlayTrack {
    /** Метод запуска воспроизведения музыки
     * @param event Событие полуения сообщения
     * @param argument Аргумент, полученный из сообщения
     */
    public static void run(MessageReceivedEvent event, String argument) {
        /* Если не были переданы аргументы, и не были прикреплены вложения */
        if (argument == null && event.getMessage().getAttachments().isEmpty()) {
            event.getChannel().sendMessage("**:x: После команды не было передано обязательных аргументов!**").submit();
            return;
        } else if (!Objects.requireNonNull(Objects.requireNonNull(event.getMember()).getVoiceState())
                .inAudioChannel()) {  //Если участник не в голосовом канале, сообщим ему это, и не выполняем код дальше
            event.getChannel().sendMessage("**:x: Вы должны находиться в голосовом канале**").submit();
            return;
        }

        Player player = Bragi.Players.get(event.getGuild());  //Экземпляр проигрывателя
        String state = "В плейлист добавлено";  //Состояние плеера

        if (!event.getMessage().getAttachments().isEmpty()) {  //Если были переданы вложения
            /* Получаем список треков из вложений */
            List<TrackInfo> trackInfoList = getTracksFromAttachments(event.getMessage().getAttachments());

            if (trackInfoList.isEmpty()) {
                event.getChannel().sendMessage("**:x: Среди вложений не было аудио**").submit();
                return;
            }

            /* Пытаемся подключиться к голосовому каналу, если плейлист пуст */
            if (player.getPlaylist().isEmpty()) {  //Если плейлист пуст
                if (JoinChannel.run(event)) //Если удалось подключиться к голосовому каналу
                    state = "Сейчас играет";
                else  //Если не удалось подключиться к голосовому каналу
                    return;
            }

            /* Пробегаем циклом по трекам и воспроизводим трек, либо добавляем его в плейлист, выводим результат */
            for (byte i = 0; i < trackInfoList.size(); i++) {
                try {
                    TrackInfo trackInfo = trackInfoList.get(i);  //Получаем объект с ифнормацией о треке

                    /* Добавляем трек в очередь или сразу воспроизводим его */
                    Methods.playTrackOrAddItToPlaylist(Bragi.Players.get(event.getGuild()), trackInfo);

                    /* Строка, в которую записываем результат */
                    String result = String
                            .format("**:notes: %s: `%s`\n:watch: Продолжительность: `%s`**",
                                    state, trackInfo.getTrackTitle(), trackInfo.getTrackDurationFormatted());

                    /* Если есть имя исполнителя */
                    result = trackInfo.getArtistName() != null ?
                            String.format("%s\n**:bust_in_silhouette: Исполнитель: `%s`**", result,
                                    trackInfo.getArtistName()) : result;

                    if (i + 1 != trackInfoList.size())  //Если трек не последний, необходимо добавить разделитель
                        result += "\n**────────────────────**";

                    event.getChannel().sendMessage(result).submit();  //Выводим информацию
                    state = "В плейлист добавлено";  //Обновляем состояние
                } catch (Exception ignore) {    }
            }
        } else {  //Если вложений нет, работаем с аргументом
            try {  //Пытаемся найти трек на сервере по запросу
                TrackInfo trackInfo = DeezerMethods.searchTrack(argument, 0);  //Получаем инфо трека
                try {
                    /* Пытаемся подключиться к голосовому каналу, если плейлист пуст */
                    if (player.getPlaylist().isEmpty()) {  //Если плейлист пуст
                        if (JoinChannel.run(event)) //Если удалось подключиться к голосовому каналу
                            state = "Сейчас играет";
                        else  //Если не удалось подключиться к голосовому каналу
                            return;
                    }

                    /* Добавляем трек в очередь или сразу воспроизводим его */
                    Methods.playTrackOrAddItToPlaylist(player, trackInfo);

                    /* Выводим информацию */
                    event.getChannel().sendMessage(String
                            .format("**:notes: %s: `%s`\n:watch: Продолжительность: `%s`**",
                            state, trackInfo.getTrackTitle(), trackInfo.getTrackDurationFormatted())).submit();
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
        if (!Objects.requireNonNull(Objects.requireNonNull(event.getMember()).getVoiceState())
                .inAudioChannel()) {  //Если участник не в голосовом канале, сообщим ему это, и не выполняем код дальше
            event.reply("**:x: Вы должны находиться в голосовом канале**").submit();
            return;
        }

        Player player = Bragi.Players.get(event.getGuild());  //Экземпляр проигрывателя
        String state = "В плейлист добавлено";  //Состояние плеера

        try {  //Пытаемся найти трек на сервере по запросу
            TrackInfo trackInfo = DeezerMethods.searchTrack(Objects.requireNonNull(event.getOption("query"))
                    .getAsString(), 0);  //Получаем инфо трека

            /* Пытаемся подключиться к голосовому каналу, если плейлист пуст */
            boolean alreadyReplied = false;
            if (player.getPlaylist().isEmpty()) {  //Если плейлист пуст
                state = "Сейчас играет";
                if (!JoinChannel.run(event)) //Если не удалось подключиться к голосовому каналу
                    return;
                else //В ином случае уже будет ответ
                    alreadyReplied = true;
            }

            /* Добавляем трек в очередь или сразу воспроизводим его */
            Methods.playTrackOrAddItToPlaylist(player, trackInfo);

            /* Выводим информацию */
            if (alreadyReplied)
                event.getChannel().sendMessage(String.format("**:notes: %s: `%s`\n:watch: Продолжительность: `%s`**",
                        state, trackInfo.getTrackTitle(), trackInfo.getTrackDurationFormatted())).submit();
            else
                event.reply(String.format("**:notes: %s: `%s`\n:watch: Продолжительность: `%s`**",
                        state, trackInfo.getTrackTitle(), trackInfo.getTrackDurationFormatted())).submit();
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

    /** Метод для воспроизведения треков из вложений
     * @param attachments Список вложений сообщения
     * @return Список объектов с информацией о треке
     */
    private static List<TrackInfo> getTracksFromAttachments(List<Message.Attachment> attachments) {
        List<Message.Attachment> audioAttachments = new ArrayList<>();  //Список аудио-вложений
        List<TrackInfo> result = new ArrayList<>(); //Список треков для возврата результата

        for (Message.Attachment attachment : attachments) {  //Проходимся циклом по вложениям
            /* Если вложение — это аудиофайл, добавляем его в список */
            if (Objects.requireNonNull(attachment.getContentType()).contains("audio"))
                audioAttachments.add(attachment);
        }

        for (Message.Attachment attachment : audioAttachments) {
            /* Создаем новый объект TrackInfo и присваиваем ему необходимые значения */
            TrackInfo trackInfo = Methods.getTrackInfo(attachment.getProxyUrl());
            trackInfo.setSource("Attachment");  //Устанавливаем информациб об источнике
            trackInfo.setTrackIdentifier(attachment.getProxyUrl());  //Получаем url вложения
            /* Если не удалось получить имя трека из метаданных, получаем его из имени вложения без расширения */
            if (trackInfo.getTrackTitle() == null)
                trackInfo.setTrackTitle(attachment.getFileName().replace("." +
                        Objects.requireNonNull(attachment.getFileExtension()), ""));
            result.add(trackInfo);  //Добавляем полученный трек в результаты
        }

        return result;  //Возвращаем результат
    }
}
