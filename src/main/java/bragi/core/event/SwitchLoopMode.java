package bragi.core.event;

import bragi.core.Player;
import net.dv8tion.jda.api.EmbedBuilder;

import java.awt.*;

public class SwitchLoopMode {
    public static EmbedBuilder run(Player player) {  //Метод для переключения режима повторения
        player.switchLoopMode();

        /* Возвращаем сообщение */
        if (player.isLoopMode())
            return new EmbedBuilder()
                    .setColor(Color.decode("#0BDA4D"))
                    .setDescription("**Повторение треков включено**");
        else
            return new EmbedBuilder()
                    .setColor(Color.decode("#0BDA4D"))
                    .setDescription("**Повторение треков выключено**");
    }
}
