package bragi.core;

import bragi.core.util.TrackInfo;
import com.sedmelluq.discord.lavaplayer.player.AudioLoadResultHandler;
import com.sedmelluq.discord.lavaplayer.player.AudioPlayerManager;
import com.sedmelluq.discord.lavaplayer.player.DefaultAudioPlayerManager;
import com.sedmelluq.discord.lavaplayer.source.AudioSourceManagers;
import com.sedmelluq.discord.lavaplayer.tools.FriendlyException;
import com.sedmelluq.discord.lavaplayer.track.AudioPlaylist;
import com.sedmelluq.discord.lavaplayer.track.AudioTrack;

public final class Methods {
    /** Метод для добавления трека в плейлист или его воспроизведения, в зависимости от состояния плейлиста
     * @param player Экземпляр проигрывателя сервера
     * @param trackInfo Информация о треке
     */
    public static void playTrackOrAddItToPlaylist(Player player, TrackInfo trackInfo) {
        /* Если плейлист пуст */
        if (player.getPlaylist().size() == 0) {
            /* Добавляем трек в очередь для дальнейшего проигрывания */
            player.getPlaylist().add(trackInfo);
            player.increaseTotalDuration(trackInfo.getTrackDuration());

            player.getInstance().Play(trackInfo.getTrackIdentifier());  //Воспроизводим трек
        } else {
            /* Добавляем трек в очередь и не проигрываем его */
            player.getPlaylist().add(trackInfo);
            player.increaseTotalDuration(trackInfo.getTrackDuration());
        }
    }

    /** Метод для получения некоторой первичной информации о треке
     * @param trackUrl Url адрес трека
     * @return Первичную информацию
     */
    public static TrackInfo getTrackInfo(String trackUrl) {
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
