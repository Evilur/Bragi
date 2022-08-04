package Bragi.LavaPlayer.AudioSourceManagers.Deezer;

import com.sedmelluq.discord.lavaplayer.container.MediaContainerDescriptor;
import com.sedmelluq.discord.lavaplayer.track.AudioTrackInfo;
import com.sedmelluq.discord.lavaplayer.track.DelegatedAudioTrack;
import com.sedmelluq.discord.lavaplayer.track.InternalAudioTrack;
import com.sedmelluq.discord.lavaplayer.track.playback.LocalAudioTrackExecutor;

import java.net.URL;

public class DeezerAudioTrack extends DelegatedAudioTrack {
    private final MediaContainerDescriptor containerTrackFactory;
    private final String trackId;
    private final String trackUrl;

    public DeezerAudioTrack(AudioTrackInfo trackInfo, MediaContainerDescriptor containerTrackFactory, String trackId, String trackUrl) {
        super(trackInfo);
        this.containerTrackFactory = containerTrackFactory;
        this.trackId = trackId;
        this.trackUrl = trackUrl;
    }

    /* С помощью этого начинается воспроизведение трека */
    @Override
    public void process(LocalAudioTrackExecutor localExecutor) throws Exception {
        /* Загружаем боту поток, сделанный своими лапками */
        try (DeezerAudioStream inputStream = new DeezerAudioStream(this.trackId, new URL(this.trackUrl))) {
            processDelegate((InternalAudioTrack) containerTrackFactory.createTrack(this.trackInfo, inputStream), localExecutor);
        }
    }
}