package bragi.core.event;

import net.dv8tion.jda.api.EmbedBuilder;
import net.dv8tion.jda.api.entities.MessageEmbed;
import net.dv8tion.jda.api.managers.AudioManager;

import java.awt.*;

public class LeaveChannel {
    /* Метод для покидания голосового канала */
    public static MessageEmbed run(AudioManager audioManager) {
        audioManager.closeAudioConnection();
        return new EmbedBuilder().setDescription("**Голосовой канал покинут**")
                .setColor(Color.decode("#FE2901")).build();
    }
}
