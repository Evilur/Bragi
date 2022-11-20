package bragi.core.event;

import bragi.Bragi;
import bragi.core.Methods;
import bragi.core.Player;
import bragi.core.util.TrackInfo;
import net.dv8tion.jda.api.entities.Message.Attachment;
import net.dv8tion.jda.api.events.interaction.command.SlashCommandInteractionEvent;

import java.util.Objects;

/** Класс для воспроизведения аудио-вложений */
public class PlayAttachment {
    /** Метод для запуска воспроизведения аудио-вложений */
    public static void run(SlashCommandInteractionEvent event) {
        Player player = Bragi.Players.get(event.getGuild());  //Экземпляр проигрывателя
        String state = "В плейлист добавлено";  //Состояние плеера

        /* Получаем вложение */
        Attachment attachment = Objects.requireNonNull(event.getOption("audio")).getAsAttachment();

        /* Если было передано не аудио */
        if (!Objects.requireNonNull(attachment.getContentType()).contains("audio")) {
            event.reply("**:x: Это вложение не является аудио-файлом**").submit();
            return;
        }

        /* Пытаемся подключиться к голосовому каналу, если плейлист пуст */
        boolean alreadyReplied = false;
        if (player.getPlaylist().isEmpty()) {  //Если плейлист пуст
            state = "Сейчас играет";
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
                .format("**:notes: %s: `%s`\n:watch: Продолжительность: `%s`**",
                        state, trackInfo.getTrackTitle(), trackInfo.getTrackDurationFormatted());

        /* Если есть имя исполнителя */
        result = trackInfo.getArtistName() != null ?
                String.format("%s\n**:bust_in_silhouette: Исполнитель: `%s`**", result,
                        trackInfo.getArtistName()) : result;

        /* Выводим информацию */
        if (alreadyReplied) event.getChannel().sendMessage(result).submit();
        else event.reply(result).submit();
    }
}
