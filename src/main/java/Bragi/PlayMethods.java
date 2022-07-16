package Bragi;

import Bragi.ObjectsInfo.TrackInfo;
import Bragi.LavaPlayer.GuildMusicManager;
import Bragi.LavaPlayer.GuildPlayer;
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

import static java.lang.String.valueOf;

public class PlayMethods {
    /* С помощью этого метода будем воспроизводить треки Deezer по поисковому запросу */
    public static EmbedBuilder PlayDeezerTrackBySearchResults (String templateName, MessageReceivedEvent event) {
        try {
            /* Выполняем поиск трека */
            TrackInfo trackInfo = DeezerMethods.SearchTrack(templateName, 0);

            /* Добалвяем в очередь, или начинаем воспроизводить (в зависимости от состояния плейлиста)*/
            return PlayTrackOrAddItToPlaylist(trackInfo, event);
        } catch (Exception ignore) {  //Если поиск не выдал результатов
            return new EmbedBuilder()
                    .setDescription("**Не удалось найти подходящую песню**")
                    .setColor(Color.decode("#FE2901"));
        }
    }

    /* С помощбю этого метода будем воспроизводить музыку из вложений */
    public static EmbedBuilder PlayTrackFromAttachment (MessageReceivedEvent event) {
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
            TrackInfo trackInfo = GetTrackInfo(attachment.getProxyUrl());
            trackInfo.trackURL = attachment.getProxyUrl();  //Получаем url вложения
            if (Objects.equals(trackInfo.trackTitle, "Unknown title"))  //Если не удалось получить имя трека из метаданных, получаем его из имени вложения без расширения
                trackInfo.trackTitle = attachment.getFileName().replace("." + Objects.requireNonNull(attachment.getFileExtension()), "");

            trackInfo.richInformation = false;  //У нас не полный набор информации, поэтому мы не можем осуществить стандартный вывод

            /* Если элемент последний в списке, то выходим из метода */
            if (i + 1 == audioAttachments.size())  {
                /* Добалвяем в очередь, или начинаем воспроизводить (в зависимости от состояния плейлиста)*/
                return PlayTrackOrAddItToPlaylist(trackInfo, event);
            } else {
                event.getChannel().sendMessageEmbeds(PlayTrackOrAddItToPlaylist(trackInfo, event).build()).submit();
            }
        }

        /* Сюда програма дойдет только в том случае, если не удалось запустить ни один аудио-файл */
        return new EmbedBuilder()
                .setColor(Color.decode("#FE2901"))
                .setDescription("**Не удалось найти аудио файл для воспроизведения среди прикрепленных файлов**");
    }

    private static EmbedBuilder PlayTrackOrAddItToPlaylist (TrackInfo trackInfo, MessageReceivedEvent event) {
        if (Player.playlist.size() < 1) {
            /* Пытаемся подключиться к голосовому каналу, если не получается, выходим из метода */
            if (!Methods.JoinChannel(event)) {
                return new EmbedBuilder()
                        .setColor(Color.decode("#FE2901"))
                        .setDescription("**Не удалось подключиться к голосовому каналу. Недостаточно прав**");
            }

            /* Добавляем трек в плейлист для дальнейшего воспроизведения */
            Player.playlist.add(trackInfo);
            Player.totalDuration += trackInfo.trackDuration;

            /* Объявляем проигрыватель и воспроизводим трек */
            Player.instance = new GuildPlayer(event.getGuild());
            Player.instance.Play(trackInfo.trackURL);

            /* Инициализируем Embed для вывода части данных */
            EmbedBuilder output = new EmbedBuilder()
                    .setColor(Color.decode("#FE2901"))
                    .setDescription(String.format("Сейчас играет: **%s**\nПродолжительность: **%s**",
                        trackInfo.trackTitle, trackInfo.trackDurationFormatted));

            /* Если у нас есть много информации для вывода, мы выводим часть сейчас и продолжаем выполнение кода */
            if (trackInfo.richInformation) {
                event.getChannel().sendMessageEmbeds(output.build()).submit();
            }
            else  //Если информации больше нет, выходим их метода, вернув данные для вывода
                return output;
        } else {
            /* Просто добавляем трек в очередь плейлиста и **не** воспроизводим его */
            Player.playlist.add(trackInfo);
            Player.totalDuration += trackInfo.trackDuration;

            /* Высчитываем общую продолжительность треков и приводим ее к приемлемому виду */
            int totalDuration = Player.totalDuration;
            String duration;
            int hours = totalDuration / 3600;
            int minutes = (totalDuration - hours * 3600) / 60;
            int seconds = totalDuration - hours * 3600 - minutes * 60;
            if (hours == 0)
                duration = String.format("%dм%dс", minutes, seconds);
            else
                duration = String.format("%dч%dм%dс", hours, minutes, seconds);

            /* Необходимо правильно просклонять слово "треки" в русском языке */
            String playlistState;
            String numberOfTracks = valueOf(Player.playlist.size());
            if (numberOfTracks.endsWith("1") && !numberOfTracks.endsWith("11"))
                playlistState = String.format("В плейлисте находится **%d трек** общей продолжительностью **%s**", Player.playlist.size(), duration);
            else if ((numberOfTracks.endsWith("2") || numberOfTracks.endsWith("3") || numberOfTracks.endsWith("4")) && !(numberOfTracks.endsWith("12") || numberOfTracks.endsWith("13") || numberOfTracks.endsWith("14")))
                playlistState = String.format("В плейлисте находится **%d трека** общей продолжительностью **%s**", Player.playlist.size(), duration);
            else
                playlistState = String.format("В плейлисте находится **%d треков** общей продолжительностью **%s**", Player.playlist.size(), duration);

            /* Инициализируем Embed для вывода части данных */
            EmbedBuilder output = new EmbedBuilder()
                    .setColor(Color.decode("#FE2901"))
                    .setDescription(String.format("Трек **%s** добавлен в плейлист\nПродолжительность: **%s**\n%s",
                            trackInfo.trackTitle, trackInfo.trackDurationFormatted, playlistState));

            /* Если у нас есть много информации для вывода, мы выводим часть сейчас и продолжаем выполнение кода */
            if (trackInfo.richInformation) {
                event.getChannel().sendMessageEmbeds(output.build()).submit();
            }
            else  //Если информации больше нет, выходим их метода, вернув данные для вывода
                return output;
        }

        return new EmbedBuilder()
                .setColor(Color.decode("#FE2901"))
                .setImage(trackInfo.albumCoverUrl)
                .addField("Альбом", trackInfo.albumTitle, false)
                .addField("Исполнитель",trackInfo.artistName, false)
                .setThumbnail(trackInfo.artistPictureUrl);
    }

    private static TrackInfo GetTrackInfo (String trackUrl) {
        TrackInfo trackInfo = new TrackInfo();  //Сюда будем складывать информацию о треке

        /* Объявляем обработчик состояния трека */
        AudioLoadResultHandler resultHandler = new AudioLoadResultHandler() {

            @Override
            public void trackLoaded(AudioTrack audioTrack) {
                /* При успешной загрузке получаем информацию о треке*/
                int trackDuration = (int)(audioTrack.getDuration() / 1000);
                String trackTitle = audioTrack.getInfo().title;

                /* Устанавливаем полученную информацию */
                trackInfo.SetTrackDuration(trackDuration);  //Устанавливаем длину трека
                trackInfo.trackTitle = trackTitle;
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
        audioPlayerManager.loadItemOrdered(new GuildMusicManager(audioPlayerManager), trackUrl, resultHandler);
        audioPlayerManager.shutdown();

        return trackInfo;
    }
}
