package bragi.core.event;

import net.dv8tion.jda.api.events.message.MessageReceivedEvent;

public class LeaveChannel {
    public static boolean run(MessageReceivedEvent event) {  //Метод для покидания голосового канала
        try {
            event.getGuild().getAudioManager().closeAudioConnection();
            return true;
        } catch (Exception ignore) {
            return false;
        }
    }
}
