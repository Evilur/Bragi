package bragi.core.event;

import net.dv8tion.jda.api.entities.AudioChannel;
import net.dv8tion.jda.api.managers.AudioManager;

public class JoinChannel {
    /* Метод для присоединения к голосовому каналу */
    public static boolean run(AudioChannel audioChannel, AudioManager audioManager) {
        /* Пытаемся подключиться к каналу; если канал закрыт и не получается подключиться, возвращаем false */
        try {
            audioManager.openAudioConnection(audioChannel);
            return true;
        } catch (Exception ignore) {
            return false;
        }
    }
}
