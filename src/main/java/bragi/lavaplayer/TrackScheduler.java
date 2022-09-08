package bragi.lavaplayer;

import bragi.Bragi;
import bragi.core.event.SkipTracks;
import com.sedmelluq.discord.lavaplayer.player.AudioPlayer;
import com.sedmelluq.discord.lavaplayer.player.event.AudioEventAdapter;
import com.sedmelluq.discord.lavaplayer.track.AudioTrack;
import com.sedmelluq.discord.lavaplayer.track.AudioTrackEndReason;
import net.dv8tion.jda.api.entities.Guild;

public class TrackScheduler extends AudioEventAdapter {
    private final AudioPlayer player;
    private final Guild guild;

    /* Объявляем конструктор, который будет присваивать значение переменное "player" */
    public TrackScheduler (AudioPlayer player, Guild guild) {
        this.player = player;
        this.guild = guild;
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
        /* Если причиной остановки трека является его окончание, заменяем его на новый */
        if (endReason.name().equals("FINISHED"))
            SkipTracks.run(1, false, this.guild);
    }
}