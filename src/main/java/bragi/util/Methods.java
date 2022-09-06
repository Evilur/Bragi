package bragi.util;

import bragi.info.TrackInfo;
import net.dv8tion.jda.api.EmbedBuilder;
import net.dv8tion.jda.api.entities.AudioChannel;
import net.dv8tion.jda.api.entities.Guild;
import net.dv8tion.jda.api.events.message.MessageReceivedEvent;
import net.dv8tion.jda.api.managers.AudioManager;
import org.jsoup.Jsoup;
import org.jsoup.nodes.Document;

import java.awt.*;
import java.io.File;
import java.io.IOException;
import java.net.URL;
import java.time.OffsetDateTime;
import java.util.ArrayList;
import java.util.Objects;

import org.json.JSONObject;

import static bragi.Bragi.Players;
import static java.lang.String.valueOf;

public class Methods {
    public static boolean joinChannel(MessageReceivedEvent event) {  //Метод для присоединения к каналу
        /* Получаем голосовой канал и аудио-менеджер для подключения к нему */
        AudioChannel audioChannel = Objects.requireNonNull(Objects.requireNonNull(event.getMember()).getVoiceState()).getChannel();
        AudioManager audioManager = event.getGuild().getAudioManager();

        /* Пытаемся подключиться к каналу; если канал закрыт и не получается подключиться, выводим ошибку и выходим из метода*/
        try {
            audioManager.openAudioConnection(audioChannel);
            return true;
        } catch (Exception ignore) {
            return false;
        }
    }
    public static EmbedBuilder playTrack(String argument, MessageReceivedEvent event) {  //Метод для поиска музыки и ее воспроизведения
        /* Если не были переданы аргументы, и не были прикреплены вложения */
        if (argument == null && event.getMessage().getAttachments().isEmpty()) {
            return new EmbedBuilder()
                    .setTitle("Ошибка!")
                    .setDescription("**После команды не было передано обязательных аргументов!**")
                    .setColor(Color.decode("#FE2901"));
        } else if (!Objects.requireNonNull(Objects.requireNonNull(event.getMember()).getVoiceState()).inAudioChannel()) { //Если участник не в голосовом канале, сообщим ему об этом и не выполняем код дальше
            return new EmbedBuilder()
                    .setDescription("**Вы должны находиться в голосовом канале**")
                    .setColor(Color.decode("#FE2901"));
        }

        assert argument != null;
        if (!event.getMessage().getAttachments().isEmpty()) {  //Если к сообщению были прикреплены вложения, то пытаемся их воспроизвести */
            return PlayerMethods.playTrackFromAttachment(event);
        } else {  //Иначе воспроизвводим трек из Deezer
            if (!argument.startsWith("https://") && !argument.startsWith("http://"))  {  //Если аргумент не содержит url
                return PlayerMethods.playDeezerTrackBySearchResults(argument, event);
            } else {
                return new EmbedBuilder()
                        .setColor(Color.decode("#FE2901"))
                        .setDescription("**Такая возможность еще не добавлена**");
            }
        }
    }

    public static EmbedBuilder getNextSong(MessageReceivedEvent event) {  //Метод, с помощью которого мы будем получать следующий трек из поискового запроса
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
                return PlayerMethods.playTrackOrAddItToPlaylist(newTrackInfo, event);
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

    public static EmbedBuilder playAlbum(String argument, MessageReceivedEvent event) {
        System.out.println();
        if (!argument.startsWith("https://") && !argument.startsWith("http://")) {  //Если аргумент не содержит url
            return  PlayerMethods.playDeezerAlbumBySearchResults(argument, event);
        } else {
            return new EmbedBuilder()
                    .setColor(Color.decode("#FE2901"))
                    .setDescription("**Такая возможность еще не добавлена**");
        }
    }

    public static EmbedBuilder getPing(net.dv8tion.jda.api.entities.Message message) {  //Метод для вывода задержки бота
        /* Высчитываем задержку */
        long creationTime = message.getTimeCreated().toInstant().toEpochMilli();  //Получаем время создание сообщения в миллисекундах
        long currentTime =  OffsetDateTime.now().toInstant().toEpochMilli();  //Получаем текущее время
        String ping = valueOf(currentTime - creationTime);  //Получаем задержку

        /* Создаем Embed */
        return new EmbedBuilder()
                .setTitle("Задержка")
                .setDescription("**" + ping + " мс**")
                .setColor(Color.decode("#0BDA4D"));
    }

    public static JSONObject getJsonObject(String url) throws IOException {  //Метод для парсинга JSON информации
        Document jsonDocument = Jsoup
                .connect(url)
                .ignoreContentType(true)
                .get();
        String jsonString = jsonDocument.body().html();  //Получаем содержимое тега body, что и является ответом сервера

        return new JSONObject(jsonString);  //Создаем JSON объект из JSON строки и возвращаем его
    }

    /* Метод для переключения режима повторения */
    public static EmbedBuilder switchLoopMode(Guild guild) {
        Players.get(guild).switchLoopMode();

        /* Возвращаем сообщение */
        if (Players.get(guild).isLoopMode())
            return new EmbedBuilder()
                    .setColor(Color.decode("#0BDA4D"))
                    .setDescription("**Повторение треков включено**");
        else
            return new EmbedBuilder()
                    .setColor(Color.decode("#0BDA4D"))
                    .setDescription("**Повторение треков выключено**");
    }

    /* Метод для пропуска определенного колиичества треков */
    public static void skipTracks(int numberOfTracks, boolean hardSkip, Guild guild) {
        /* Если пользователь хочет пропустить треков больше, чем существует в плейлисте, ограничим его хотения */
        if (numberOfTracks > Players.get(guild).getPlaylist().size())
            numberOfTracks = Players.get(guild).getPlaylist().size();
        else if (numberOfTracks < 1) {  //Если не передано число, присваиваем единицу, то есть убираем один трек
            numberOfTracks = 1;
        }

        /* Если не стоит повторение или трек пропускается вручную */
        if (!Players.get(guild).isLoopMode() || hardSkip) {  //Удаляем элементы
            Players.get(guild).decreaseTotalDuration(Players.get(guild).getPlaylist().get(0).getTrackDuration());  //Уменьшаем общую длину треков
            Players.get(guild).getPlaylist().subList(0, numberOfTracks).clear();  //Удаляем первые элементы из списка
        }

        /* Если в плейлисте есть треки */
        if (Players.get(guild).getPlaylist().size() > 0) {
            String url = Players.get(guild).getPlaylist().get(0).getTrackIdentifier();  //Получаем url трека
            Players.get(guild).getInstance().Play(url);  //Воспроизводим трек
        }
        else {  //Если треков в плейлисте нет
            Players.get(guild).getInstance().Stop();
        }
    }

    public static EmbedBuilder getPlaylist(Guild guild) {
        /* Если плейлист пуст */
        if (Players.get(guild).getPlaylist().size() == 0) {
            return new EmbedBuilder()
                    .setColor(Color.decode("#0BDA4D"))
                    .setDescription("**В плейлисте нет треков для воспроизведения**");
        }

        /* Если плейлист не пуст, перебираем его циклом, форматируем и записываем результат в переменную */
        StringBuilder result = new StringBuilder();  //Сюда будем записывать результат

        /* Циклом перебираем плейлист и дабавляем это к результату */
        for (int i = 0; i < Players.get(guild).getPlaylist().size(); i++) {
            TrackInfo trackInfo = Players.get(guild).getPlaylist().get(i);
            result.append(String.format("%d. %s\n", i + 1, trackInfo.getTrackTitle()));
        }

        if (Players.get(guild).isLoopMode())  //Если включено повторение трека, заявляем об этом
            result.append("————————————————————\nВключено повторения треков");

        /* Возвращаем информацию о плейлисте */
        return new EmbedBuilder()
                .setColor(Color.decode("#0BDA4D"))
                .setTitle("**Текущий плейлист:**")
                .setDescription(String.format("**Общая продолжительность: %s\n————————————————————\n%s**", Players.get(guild).getTotalDuration(), result));
    }
}
