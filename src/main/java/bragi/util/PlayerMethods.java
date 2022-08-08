package bragi.util;

import bragi.info.TrackInfo;
import com.sedmelluq.discord.lavaplayer.player.AudioLoadResultHandler;
import com.sedmelluq.discord.lavaplayer.player.AudioPlayerManager;
import com.sedmelluq.discord.lavaplayer.player.DefaultAudioPlayerManager;
import com.sedmelluq.discord.lavaplayer.source.AudioSourceManagers;
import com.sedmelluq.discord.lavaplayer.tools.FriendlyException;
import com.sedmelluq.discord.lavaplayer.track.AudioPlaylist;
import com.sedmelluq.discord.lavaplayer.track.AudioTrack;
import net.dv8tion.jda.api.EmbedBuilder;
import net.dv8tion.jda.api.entities.Message;
import net.dv8tion.jda.api.events.message.MessageReceivedEvent;

import java.awt.*;
import java.util.ArrayList;
import java.util.List;
import java.util.Objects;

import static bragi.Bragi.Players;

public class PlayerMethods {
    /* С помощью этого метода будем воспроизводить треки Deezer по поисковому запросу */
    public static EmbedBuilder playDeezerTrackBySearchResults(String templateName, MessageReceivedEvent event) {
        try {
            /* Выполняем поиск трека */
            TrackInfo trackInfo = DeezerMethods.searchTrack(templateName, 0);

            /* Добалвяем в очередь, или начинаем воспроизводить (в зависимости от состояния плейлиста)*/
            return playTrackOrAddItToPlaylist(trackInfo, event);
        } catch (Exception ignore) {  //Если поиск не выдал результатов
            return new EmbedBuilder()
                    .setDescription("**Не удалось найти подходящую песню**")
                    .setColor(Color.decode("#FE2901"));
        }
    }

    /* С помощбю этого метода будем воспроизводить музыку из вложений */
    public static EmbedBuilder playTrackFromAttachment(MessageReceivedEvent event) {
        List<Message.Attachment> attachments = event.getMessage().getAttachments();  //Получаем список вложений
        List<Message.Attachment> audioAttachments = new ArrayList<>();

        for (Message.Attachment attachment : attachments) {
            /* Если вложение — это аудиофайл, добавляем его в список */
            if (Objects.requireNonNull(attachment.getContentType()).contains("audio"))
                audioAttachments.add(attachment);
        }

        for (byte i = 0; i < audioAttachments.size(); i++) {
            Message.Attachment attachment = audioAttachments.get(i);

            /* Созаем новый объект TrackInfo и присваиваем ему необходимые значения */
            TrackInfo trackInfo = getTrackInfo(attachment.getProxyUrl());
            trackInfo.setSource("Attachment");  //Устанавливаем информациб об источнике
            trackInfo.setTrackIdentifier(attachment.getProxyUrl());  //Получаем url вложения
            if (trackInfo.getTrackTitle() == null)  //Если не удалось получить имя трека из метаданных, получаем его из имени вложения без расширения
                trackInfo.setTrackTitle(attachment.getFileName().replace("." + Objects.requireNonNull(attachment.getFileExtension()), ""));

            /* Если элемент последний в списке, то выходим из метода */
            if (i + 1 == audioAttachments.size())  {
                /* Добалвяем в очередь, или начинаем воспроизводить (в зависимости от состояния плейлиста)*/
                return playTrackOrAddItToPlaylist(trackInfo, event);
            } else {
                event.getChannel().sendMessageEmbeds(playTrackOrAddItToPlaylist(trackInfo, event).build()).submit();
            }
        }

        /* Сюда програма дойдет только в том случае, если не удалось запустить ни один аудио-файл */
        return new EmbedBuilder()
                .setColor(Color.decode("#FE2901"))
                .setDescription("**Не удалось найти аудио файл для воспроизведения среди прикрепленных файлов**");
    }

    private static EmbedBuilder playTrackOrAddItToPlaylist(TrackInfo trackInfo, MessageReceivedEvent event) {
        /* Если в плйлисте в данный момент нет треков */
        if (Players.get(event.getGuild()).getPlaylist().size() < 1) {
            /* Пытаемся подключиться к голосовому каналу, если не получается, выходим из метода */
            if (!Methods.joinChannel(event)) {
                return new EmbedBuilder()
                        .setColor(Color.decode("#FE2901"))
                        .setDescription("**Не удалось подключиться к голосовому каналу. Недостаточно прав**");
            }

            /* Добавляем трек в плейлист для дальнейшего воспроизведения */
            Players.get(event.getGuild()).getPlaylist().add(trackInfo);
            Players.get(event.getGuild()).increaseTotalDuration(trackInfo.getTrackDuration());

            /* Воспроизводим трек */
            Players.get(event.getGuild()).getInstance().Play(trackInfo.getTrackIdentifier());
        } else {
            /* Просто добавляем трек в очередь плейлиста и **не** воспроизводим его */
            Players.get(event.getGuild()).getPlaylist().add(trackInfo);
            Players.get(event.getGuild()).increaseTotalDuration(trackInfo.getTrackDuration());
        }

        /* В зависимости от того, из каких источников был получен трек, выводим разное количество информации */
        if (!trackInfo.getSource().equals("Attachment"))
            event.getChannel()
                    .sendMessageEmbeds(Informant.getOutputInformation(trackInfo.getTrackTitle(), trackInfo.getTrackDurationFormatted()).build()).submit();
        else  //Если трек получили из вложений, то возвращаем имеющуюся информацию
            return Informant.getOutputInformation(trackInfo.getTrackTitle(), trackInfo.getArtistName(), trackInfo.getTrackDurationFormatted());

        /* Соотвественно, если код дошел до сюда, у нас должна быть информацию для вывода */
        return new EmbedBuilder()
                .setColor(Color.decode("#FE2901"))
                .setImage(trackInfo.getAlbumCoverUrl())
                .addField("Альбом", trackInfo.getAlbumTitle(), false)
                .addField("Исполнитель",trackInfo.getArtistName(), false)
                .setThumbnail(trackInfo.getArtistPictureUrl());
    }

    private static TrackInfo getTrackInfo(String trackUrl) {
        TrackInfo trackInfo = new TrackInfo();  //Сюда будем складывать информацию о треке

        /* Объявляем обработчик состояния трека */
        AudioLoadResultHandler resultHandler = new AudioLoadResultHandler() {

            @Override
            public void trackLoaded(AudioTrack audioTrack) {
                /* При успешной загрузке получаем информацию о треке*/
                String trackTitle = audioTrack.getInfo().title;
                String artistName = audioTrack.getInfo().author;

                /* Устанавливаем полученную информацию */
                trackInfo.setTrackDuration((int)(audioTrack.getDuration() / 1000));  //Устанавливаем длину трека

                if (!trackTitle.contains("Unknown"))  //Если получилось узнать название трека
                    trackInfo.setTrackTitle(trackTitle);  //Устанавливаем его
                if (!artistName.contains("Unknown"))  //Если удалось узнать исполнителя трека
                    trackInfo.setArtistName(artistName);  //Устанавливаем его
            }

            /* Неиспользуемые, но обязательные методы, которые нельзя убирать */
            @Override
            public void loadFailed(FriendlyException e) {   }
            @Override
            public void playlistLoaded(AudioPlaylist audioPlaylist) {   }
            @Override
            public void noMatches() {   }
        };

        /* Загружаем трек в несуществующий проигрыватель и стразу выключаем его*/
        AudioPlayerManager audioPlayerManager = new DefaultAudioPlayerManager();
        AudioSourceManagers.registerRemoteSources(audioPlayerManager);
        audioPlayerManager.loadItem(trackUrl, resultHandler);
        audioPlayerManager.shutdown();

        return trackInfo;
    }
}
