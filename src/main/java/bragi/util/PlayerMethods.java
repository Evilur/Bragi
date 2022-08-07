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
import static java.lang.String.valueOf;

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
            if (Objects.equals(trackInfo.getTrackTitle(), "Unknown title"))  //Если не удалось получить имя трека из метаданных, получаем его из имени вложения без расширения
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

            /* Инициализируем Embed для вывода части данных */
            EmbedBuilder output = new EmbedBuilder()
                    .setColor(Color.decode("#FE2901"))
                    .setDescription(String.format("Сейчас играет: **%s**\nПродолжительность: **%s**",
                        trackInfo.getTrackTitle(), trackInfo.getTrackDurationFormatted()));

            /* Если у нас есть много информации для вывода, мы выводим часть сейчас и продолжаем выполнение кода */
            if (trackInfo.getAlbumTitle() != null && trackInfo.getArtistName() != null && trackInfo.getAlbumCoverUrl() != null && trackInfo.getArtistPictureUrl() != null) {
                event.getChannel().sendMessageEmbeds(output.build()).submit();
            }
            else  //Если информации больше нет, выходим их метода, вернув данные для вывода
                return output;
        } else {
            /* Просто добавляем трек в очередь плейлиста и **не** воспроизводим его */
            Players.get(event.getGuild()).getPlaylist().add(trackInfo);
            Players.get(event.getGuild()).increaseTotalDuration(trackInfo.getTrackDuration());

            /* Высчитываем общую продолжительность треков и приводим ее к приемлемому виду */
            String duration = Players.get(event.getGuild()).getTotalDuration();

            /* Необходимо правильно просклонять слово "треки" в русском языке */
            String playlistState;
            String numberOfTracks = valueOf(Players.get(event.getGuild()).getPlaylist().size());
            if (numberOfTracks.endsWith("1") && !numberOfTracks.endsWith("11"))
                playlistState = String.format("В плейлисте находится **%d трек** общей продолжительностью **%s**", Players.get(event.getGuild()).getPlaylist().size(), duration);
            else if ((numberOfTracks.endsWith("2") || numberOfTracks.endsWith("3") || numberOfTracks.endsWith("4")) && !(numberOfTracks.endsWith("12") || numberOfTracks.endsWith("13") || numberOfTracks.endsWith("14")))
                playlistState = String.format("В плейлисте находится **%d трека** общей продолжительностью **%s**", Players.get(event.getGuild()).getPlaylist().size(), duration);
            else
                playlistState = String.format("В плейлисте находится **%d треков** общей продолжительностью **%s**", Players.get(event.getGuild()).getPlaylist().size(), duration);

            /* Инициализируем Embed для вывода части данных */
            EmbedBuilder output = new EmbedBuilder()
                    .setColor(Color.decode("#FE2901"))
                    .setDescription(String.format("Трек **%s** добавлен в плейлист\nПродолжительность: **%s**\n%s",
                            trackInfo.getTrackTitle(), trackInfo.getTrackDurationFormatted(), playlistState));

            /* Если у нас есть много информации для вывода, мы выводим часть сейчас и продолжаем выполнение кода */
            if (trackInfo.getAlbumTitle() != null && trackInfo.getArtistName() != null && trackInfo.getAlbumCoverUrl() != null && trackInfo.getArtistPictureUrl() != null) {
                event.getChannel().sendMessageEmbeds(output.build()).submit();
            }
            else  //Если информации больше нет, выходим их метода, вернув данные для вывода
                return output;
        }

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
                int trackDuration = (int)(audioTrack.getDuration() / 1000);
                String trackTitle = audioTrack.getInfo().title;

                /* Устанавливаем полученную информацию */
                trackInfo.setTrackDuration(trackDuration);  //Устанавливаем длину трека
                trackInfo.setTrackTitle(trackTitle);
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
