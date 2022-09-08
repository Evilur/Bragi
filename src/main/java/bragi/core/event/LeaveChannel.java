package bragi.core.event;

import net.dv8tion.jda.api.managers.AudioManager;

public class LeaveChannel {
    public static boolean run(AudioManager audioManager) {  //Метод для покидания голосового канала
        try {
            audioManager.closeAudioConnection();
            return true;
        } catch (Exception ignore) {
            return false;
        }
    }
}
