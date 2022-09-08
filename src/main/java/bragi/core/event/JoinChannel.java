package bragi.core.event;

import net.dv8tion.jda.api.entities.AudioChannel;
import net.dv8tion.jda.api.events.message.MessageReceivedEvent;
import net.dv8tion.jda.api.managers.AudioManager;

import java.util.Objects;

public class JoinChannel {
    public static boolean run(MessageReceivedEvent event) {  //Метод для присоединения к голосовому каналу
        /* Получаем голосовой канал и аудио-менеджер для подключения к нему */
        AudioChannel audioChannel = Objects.requireNonNull(Objects.requireNonNull(event.getMember()).getVoiceState()).getChannel();
        AudioManager audioManager = event.getGuild().getAudioManager();

        /* Пытаемся подключиться к каналу; если канал закрыт и не получается подключиться, выводим ошибку и выходим из метода*/
        try {
            audioManager.openAudioConnection(audioChannel);
            return true;
        } catch (Exception ignore) {
            return false;
        }
    }
}
