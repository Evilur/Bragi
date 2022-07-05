package Bragi.LavaPlayer;

import Bragi.Player;
import com.sedmelluq.discord.lavaplayer.player.AudioLoadResultHandler;
import com.sedmelluq.discord.lavaplayer.player.AudioPlayerManager;
import com.sedmelluq.discord.lavaplayer.player.DefaultAudioPlayerManager;
import com.sedmelluq.discord.lavaplayer.source.AudioSourceManagers;
import com.sedmelluq.discord.lavaplayer.tools.FriendlyException;
import com.sedmelluq.discord.lavaplayer.track.AudioPlaylist;
import com.sedmelluq.discord.lavaplayer.track.AudioTrack;
import net.dv8tion.jda.api.entities.Guild;

public class GuildPlayer {
    private final GuildMusicManager guildMusicManager;
    private final AudioPlayerManager audioPlayerManager;

    public GuildPlayer(Guild guild) {
        /* Создаем менеджер для проигрывания */
        this.audioPlayerManager = new DefaultAudioPlayerManager();
        this.guildMusicManager =  new GuildMusicManager(audioPlayerManager);

        /* Разрешаем аудио-менеджеру использовать и локальны, и удаленные файлы */
        AudioSourceManagers.registerRemoteSources(this.audioPlayerManager);
        AudioSourceManagers.registerLocalSource(this.audioPlayerManager);

        /* Привязываем менеждер к дискорду */
        AudioPlayerSendHandler sendHandler = guildMusicManager.getSendHandler();
        guild.getAudioManager().setSendingHandler(sendHandler);
    }

    public void Play (String trackUrl) {
        /* Объявляем обработчик состояния трека */
        AudioLoadResultHandler resultHandler = new AudioLoadResultHandler() {
            @Override
            public void trackLoaded(AudioTrack audioTrack) {
                guildMusicManager.scheduler.PlayTrack(audioTrack);  //Проигрываем трек при успешной загрузке

                /* Если не получилось получить продолжительность трека раньше */
                if (Player.playlist.get(0).trackDuration == 0) {
                    int duration = (int)(audioTrack.getDuration() / 1000);  //Получаем продолжительность трека из проигрывателя
                    Player.playlist.get(0).SetTrackDuration(duration);  //Присваиваем ее текщему треку
                    Player.totalDuration += duration;  //Также добавляем это к общей продолжительности
                }
            }

            /* Неиспользуемые, но обязательные методы, которые нельзя убирать */
            @Override
            public void loadFailed(FriendlyException e) {   }
            @Override
            public void playlistLoaded(AudioPlaylist audioPlaylist) {   }
            @Override
            public void noMatches() {   }
        };

        /* Загружаем трек */
        audioPlayerManager.loadItemOrdered(guildMusicManager, trackUrl, resultHandler);
    }
}
