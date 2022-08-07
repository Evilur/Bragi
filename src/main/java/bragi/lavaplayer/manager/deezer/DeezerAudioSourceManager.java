package bragi.lavaplayer.manager.deezer;

import com.sedmelluq.discord.lavaplayer.container.*;
import com.sedmelluq.discord.lavaplayer.player.AudioPlayerManager;
import com.sedmelluq.discord.lavaplayer.source.ProbingAudioSourceManager;
import com.sedmelluq.discord.lavaplayer.tools.io.*;
import com.sedmelluq.discord.lavaplayer.track.AudioItem;
import com.sedmelluq.discord.lavaplayer.track.AudioReference;
import com.sedmelluq.discord.lavaplayer.track.AudioTrack;
import com.sedmelluq.discord.lavaplayer.track.AudioTrackInfo;
import com.sedmelluq.discord.lavaplayer.track.info.AudioTrackInfoBuilder;

import java.io.DataInput;
import java.io.DataOutput;
import java.net.URL;
import java.util.Objects;

public class DeezerAudioSourceManager extends ProbingAudioSourceManager {
    private final HttpInterface httpInterface = new ThreadLocalHttpInterfaceManager(
            HttpClientTools.createSharedCookiesHttpBuilder().setRedirectStrategy(new HttpClientTools.NoRedirectsStrategy()),
            HttpClientTools.DEFAULT_REQUEST_CONFIG).getInterface();
    private String trackId;
    private String trackUrl;

    public DeezerAudioSourceManager() {
        super(MediaContainerRegistry.DEFAULT_REGISTRY);
    }

    @Override
    public AudioItem loadItem(AudioPlayerManager manager, AudioReference reference) {
        if (IsDeezerTrack(reference.identifier)) {  //Если трек не с Deezer, значит этот менеджер нам не подходит
            AudioTrackInfo trackInfo = AudioTrackInfoBuilder.create(reference, null).build();

            /* Вытаскиваем из идентефикатора id и url трека */
            String[] arguments = trackInfo.identifier.split("\\|");  //Нулевым в массиве идет id трека, первым - его url
            this.trackId = arguments[0];
            this.trackUrl = arguments[1];
            
            MediaContainerDescriptor mediaContainerDescriptor = Objects.requireNonNull(GetMediaContainerDescriptor(reference)).getContainerDescriptor();
            return createTrack(trackInfo, mediaContainerDescriptor);
        } else {
            return null;
        }
    }

    private MediaContainerDetectionResult GetMediaContainerDescriptor(AudioReference reference) {
        try {
            SeekableInputStream inputStream = new DeezerAudioStream(this.trackId, new URL(this.trackUrl));
            MediaContainerHints hints = MediaContainerHints.from("audio/x-flac", "flac");
            return new MediaContainerDetection(this.containerRegistry, reference, inputStream, hints).detectContainer();
        } catch (Exception ignore) {
            return null;
        }
    }

    private static boolean IsDeezerTrack(String identifier) {
        /* Проверяем, с Deezer-а ли этот трек, или нет */
        return identifier.contains("cdnt-proxy");
    }

    @Override
    protected AudioTrack createTrack(AudioTrackInfo trackInfo, MediaContainerDescriptor containerDescriptor) {
        return new DeezerAudioTrack(trackInfo, containerDescriptor, this.trackId, this.trackUrl);
    }

    @Override
    public String getSourceName() {
        return "Deezer";
    }

    @Override
    public boolean isTrackEncodable(AudioTrack track) {
        return false;
    }

    @Override
    public AudioTrack decodeTrack(AudioTrackInfo trackInfo, DataInput input) {
        return null;
    }

    @Override
    public void encodeTrack(AudioTrack track, DataOutput output) {   }

    @Override
    public void shutdown() {    }
}