package Bragi.LavaPlayer.AudioSourceManagers.Deezer;

import com.sedmelluq.discord.lavaplayer.container.*;
import com.sedmelluq.discord.lavaplayer.player.AudioPlayerManager;
import com.sedmelluq.discord.lavaplayer.source.ProbingAudioSourceManager;
import com.sedmelluq.discord.lavaplayer.tools.Units;
import com.sedmelluq.discord.lavaplayer.tools.io.HttpClientTools;
import com.sedmelluq.discord.lavaplayer.tools.io.HttpInterface;
import com.sedmelluq.discord.lavaplayer.tools.io.PersistentHttpStream;
import com.sedmelluq.discord.lavaplayer.tools.io.ThreadLocalHttpInterfaceManager;
import com.sedmelluq.discord.lavaplayer.track.AudioItem;
import com.sedmelluq.discord.lavaplayer.track.AudioReference;
import com.sedmelluq.discord.lavaplayer.track.AudioTrack;
import com.sedmelluq.discord.lavaplayer.track.AudioTrackInfo;
import com.sedmelluq.discord.lavaplayer.track.info.AudioTrackInfoBuilder;

import java.io.DataInput;
import java.io.DataOutput;
import java.net.URI;
import java.util.Objects;

import static com.sedmelluq.discord.lavaplayer.tools.io.HttpClientTools.getHeaderValue;

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
            /* Здесь небольшой костыль, который я возможно уберу вместе со следующим коммитом */
            PersistentHttpStream inputStream = new PersistentHttpStream(this.httpInterface, new URI("https://oplayer.ru/Vodka.flac"), Units.CONTENT_LENGTH_UNKNOWN);
            inputStream.checkStatusCode();

            MediaContainerHints hints = MediaContainerHints.from(getHeaderValue(inputStream.getCurrentResponse(), "Content-Type"), null);
            return new MediaContainerDetection(containerRegistry, reference, inputStream, hints).detectContainer();
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