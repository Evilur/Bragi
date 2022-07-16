package Bragi.LavaPlayer;

import Bragi.Methods;
import com.sedmelluq.discord.lavaplayer.player.AudioPlayer;
import com.sedmelluq.discord.lavaplayer.player.event.AudioEventAdapter;
import com.sedmelluq.discord.lavaplayer.track.AudioTrack;
import com.sedmelluq.discord.lavaplayer.track.AudioTrackEndReason;

public class TrackScheduler extends AudioEventAdapter {
    private final AudioPlayer player;

    /* Объявляем конструктор, который будет присваивать значение переменное "player" */
    public TrackScheduler (AudioPlayer player) {
        this.player = player;
    }

    /* С помощью этого метода будем воспроизводить треки */
    public void PlayTrack (AudioTrack track) {
        this.player.playTrack(track);
    }

    public void StopTrack () {
        this.player.stopTrack();
    }

    /* Это обработчик событий, который срабатывает при окончании трека */
    @Override
    public void onTrackEnd(AudioPlayer player, AudioTrack track, AudioTrackEndReason endReason) {
        /* Если причинойостановки трека является его замена, то скипать еще раз не нужно */
        if (!endReason.name().equals("REPLACED") && !endReason.name().equals("STOPPED"))
            Methods.SkipTracks(1, false);
    }
}