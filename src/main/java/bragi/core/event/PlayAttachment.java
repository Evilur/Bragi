package bragi.core.event;

import bragi.Bragi;
import bragi.core.Methods;
import bragi.core.util.TrackInfo;
import net.dv8tion.jda.api.entities.Message.Attachment;
import net.dv8tion.jda.api.events.interaction.command.SlashCommandInteractionEvent;
import net.dv8tion.jda.api.events.message.MessageReceivedEvent;

import java.util.List;
import java.util.Objects;

/** Класс для воспроизведения аудио-вложений */
public class PlayAttachment {
    /** Метод для запуска воспроизведения аудио-вложений
     * @param event Событие получения сообщения
     */
    public static void run(MessageReceivedEvent event) {
        boolean needToJoin = !Objects.requireNonNull(Objects.requireNonNull(event.getGuild()).getSelfMember()
                .getVoiceState()).inAudioChannel();

        /* Если нужно присоединяться, а пользователь не в голосовом канале */
        if (needToJoin && !Objects.requireNonNull(Objects.requireNonNull(event.getMember()).getVoiceState())
                .inAudioChannel()) {
            event.getChannel().sendMessage(":x:** Вы должны находиться в голосовом канале**").submit();
            return;
        }

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
    }

    /** Метод для запуска воспроизведения аудио-вложений
     * @param event Событие получения команды
     */
    public static void run(SlashCommandInteractionEvent event) {
        boolean needToJoin = !Objects.requireNonNull(Objects.requireNonNull(event.getGuild()).getSelfMember()
                .getVoiceState()).inAudioChannel();

        /* Если нужно присоединяться, а пользователь не в голосовом канале */
        if (needToJoin && !Objects.requireNonNull(Objects.requireNonNull(event.getMember()).getVoiceState())
                .inAudioChannel()) {
            event.getChannel().sendMessage(":x:** Вы должны находиться в голосовом канале**").submit();
            return;
        }

        /* Получаем вложение */
        Attachment attachment = Objects.requireNonNull(event.getOption("audio")).getAsAttachment();

        /* Если было передано не аудио */
        if (!Objects.requireNonNull(attachment.getContentType()).contains("audio")) {
            event.reply("**:x: Это вложение не является аудио-файлом**").submit();
            return;
        }

        /* Пытаемся подключиться к голосовому каналу, если плейлист пуст */
        boolean alreadyReplied = false;
        if (needToJoin) {  //Если плейлист пуст
            if (!JoinChannel.run(event)) return;  //Если не удалось подключиться к голосовому каналу
            else alreadyReplied = true;  //В ином случае уже будет ответ
        }

        /* Создаем новый объект TrackInfo и присваиваем ему необходимые значения */
        TrackInfo trackInfo = Methods.getTrackInfo(attachment.getProxyUrl());
        trackInfo.setSource("Attachment");  //Устанавливаем информацию об источнике
        trackInfo.setTrackIdentifier(attachment.getProxyUrl());  //Получаем url вложения

        /* Если не удалось получить имя трека из метаданных, получаем его из имени вложения без расширения */
        if (trackInfo.getTrackTitle() == null)
            trackInfo.setTrackTitle(attachment.getFileName().replaceAll("\\..+$", ""));

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

        /* Выводим информацию */
        if (alreadyReplied) event.getChannel().sendMessage(result).submit();
        else event.reply(result).submit();
    }
}
