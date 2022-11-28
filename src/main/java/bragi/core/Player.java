package bragi.core;

import bragi.core.util.TrackInfo;
import bragi.core.plugins.lavaplayer.GuildPlayer;
import net.dv8tion.jda.api.entities.Guild;

import java.util.ArrayList;

public final class Player {
    private final ArrayList<TrackInfo> playlist;  //Это список воспроизведения
    private final GuildPlayer instance;  //Это наш проигрыватель
    private boolean loopMode;  //Режим повторения

    public Player(Guild guild) {
        this.playlist = new ArrayList<>();  //Объявляем пустой плейлист
        this.instance = new GuildPlayer(guild);  //Объявляем новый проигрыватель
        this.loopMode = false;  //Изначально режим повторения выключен
    }

    /** Метод для получения плейлиста
     * @return Плейлист гильдии
     */
    public ArrayList<TrackInfo> getPlaylist() {
        return this.playlist;
    }

    /** Метод для получения общей продолжительности треков
     * @return Общую продолжительность треков
     */
    public String getTotalDuration() {
        /* Высчитываем общее время продолжительности треков */
        int totalDuration = 0;
        for (TrackInfo trackInfo : this.playlist)
            totalDuration += trackInfo.getTrackDuration();

        int hours = totalDuration / 3600;
        int minutes = (totalDuration - hours * 3600) / 60;
        int seconds = totalDuration - hours * 3600 - minutes * 60;

        if (hours == 0)
            return String.format("%dм%dс", minutes, seconds);
        else
            return String.format("%dч%dм%dс", hours, minutes, seconds);
    }

    /** Метод для получения экземпляра проигрывателя
     * @return Экземпляр проигрывателя
     */
    public GuildPlayer getInstance() {
        return this.instance;
    }

    /** Метод для получения режима повторения
     * @return Режим повторения треков
     */
    public boolean isLoopMode() {
        return this.loopMode;
    }

    /** Метод для переключения режима повторения */
    public void switchLoopMode() {
        this.loopMode = !this.loopMode;
    }
}
