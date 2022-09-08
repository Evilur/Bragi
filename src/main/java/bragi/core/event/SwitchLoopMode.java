package bragi.core.event;

import net.dv8tion.jda.api.EmbedBuilder;
import net.dv8tion.jda.api.entities.Guild;

import java.awt.*;

import static bragi.Bragi.Players;

public class SwitchLoopMode {
    public static EmbedBuilder run(Guild guild) {  //Метод для переключения режима повторения
        Players.get(guild).switchLoopMode();

        /* Возвращаем сообщение */
        if (Players.get(guild).isLoopMode())
            return new EmbedBuilder()
                    .setColor(Color.decode("#0BDA4D"))
                    .setDescription("**Повторение треков включено**");
        else
            return new EmbedBuilder()
                    .setColor(Color.decode("#0BDA4D"))
                    .setDescription("**Повторение треков выключено**");
    }
}
