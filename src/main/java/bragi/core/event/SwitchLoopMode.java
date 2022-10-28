package bragi.core.event;

import bragi.Bragi;
import bragi.core.Player;
import net.dv8tion.jda.api.EmbedBuilder;
import net.dv8tion.jda.api.events.interaction.command.SlashCommandInteractionEvent;
import net.dv8tion.jda.api.events.message.MessageReceivedEvent;

import java.awt.*;

/** Класс для переключения режима повторения трека */
public class SwitchLoopMode {
    /** Метод для запуска смены переключения режима повторения трека
     * @param event Событие полуения сообщения
     */
    public static void run(MessageReceivedEvent event) {  //Метод для переключения режима повторения
        Player player = Bragi.Players.get(event.getGuild());  //Получаем экзепмпляр проигррывателя
        player.switchLoopMode();  //Переключаем режим повторения

        /* Выводим сообщение */
        if (player.isLoopMode())
            event.getChannel().sendMessage("**:repeat: Повторение треков включено**").submit();
        else
            event.getChannel().sendMessage("**Повторение треков выключено**").submit();
    }
    /** Метод для запуска смены переключения режима повторения трека
     * @param event Событие полуения команды
     */
    public static void run(SlashCommandInteractionEvent event) {  //Метод для переключения режима повторения
        Player player = Bragi.Players.get(event.getGuild());  //Получаем экзепмпляр проигррывателя
        player.switchLoopMode();  //Переключаем режим повторения

        /* Выводим сообщение */
        if (player.isLoopMode())
            event.reply("**:repeat: Повторение треков включено**").submit();
        else
            event.reply("**Повторение треков выключено**").submit();
    }
}
