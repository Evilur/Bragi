package bragi.core;

import bragi.core.util.TrackInfo;
import bragi.core.plugins.lavaplayer.GuildPlayer;
import net.dv8tion.jda.api.entities.Guild;

import java.util.ArrayList;

public final class Player {
    private final ArrayList<TrackInfo> playlist;  //Это список воспроизведения
    private final GuildPlayer instance;  //Это наш проигрыватель
    private int totalDuration;  //Общая продолжительность треков в плейлисте
    private boolean loopMode;  //Режим повторения

    public Player(Guild guild) {
        this.playlist = new ArrayList<>();  //Объявляем пустой плейлист
        this.totalDuration = 0;  //Общая длина треков пока равна нулю
        this.instance = new GuildPlayer(guild);  //Объявляем новый проигрыватель
        this.loopMode = false;  //Изначально режим повторения выключен
    }

    /* Метод для получения плейлиста */
    public ArrayList<TrackInfo> getPlaylist() {
        return this.playlist;
    }

    public String getTotalDuration() {
        int hours = this.totalDuration / 3600;
        int minutes = (this.totalDuration - hours * 3600) / 60;
        int seconds = this.totalDuration - hours * 3600 - minutes * 60;
        if (hours == 0)
            return String.format("%dм%dс", minutes, seconds);
        else
            return String.format("%dч%dм%dс", hours, minutes, seconds);
    }

    /** Метод для замены трека в плейлисте
     * @param trackNumber Номер трека в плейлисте
     * @param trackInfo Новый трек
     */
    public void replaceTrack(int trackNumber, TrackInfo trackInfo) {
        this.decreaseTotalDuration(this.playlist.get(trackNumber).getTrackDuration()); //Уменьшаем общ продолжительность
        this.increaseTotalDuration(trackInfo.getTrackDuration());  //Увеличиваем общую продолжительность
        this.playlist.set(trackNumber, trackInfo);
    }

    /* Метод для увеличения общей продолжительности треков */
    public void increaseTotalDuration(int value) {
        this.totalDuration += value;
    }

    /* Метод для уменьшения общей продолжительности треков */
    public void decreaseTotalDuration(int value) {
        this.totalDuration -= value;
    }

    /* Метод для получения экземпляра проигрывателя */
    public GuildPlayer getInstance() {
        return this.instance;
    }

    /* Метод для получения режима повторения */
    public boolean isLoopMode() {
        return this.loopMode;
    }

    /* Метод для переключения режима повторения */
    public void switchLoopMode() {
        this.loopMode = !this.loopMode;
    }
}
