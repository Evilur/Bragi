package bragi.core.event;

import bragi.Bragi;
import bragi.core.Player;
import net.dv8tion.jda.api.events.interaction.command.SlashCommandInteractionEvent;
import net.dv8tion.jda.api.events.message.MessageReceivedEvent;

/** Класс для переключения режима повторения трека */
public final class SwitchLoopMode {
    /** Метод для запуска смены переключения режима повторения трека
     * @param event Событие получения сообщения
     */
    public static void run(MessageReceivedEvent event) {
        Player player = Bragi.Players.get(event.getGuild());  //Получаем экземпляр проигрывателя
        player.switchLoopMode();  //Переключаем режим повторения

        /* Выводим сообщение */
        if (player.isLoopMode())
            event.getChannel().sendMessage("**:repeat: Повторение треков включено**").submit();
        else
            event.getChannel().sendMessage("**:repeat: Повторение треков выключено**").submit();
    }
    /** Метод для запуска смены переключения режима повторения трека
     * @param event Событие получения команды
     */
    public static void run(SlashCommandInteractionEvent event) {
        Player player = Bragi.Players.get(event.getGuild());  //Получаем экземпляр проигрывателя
        player.switchLoopMode();  //Переключаем режим повторения

        /* Выводим сообщение */
        if (player.isLoopMode())
            event.reply("**:repeat: Повторение треков включено**").submit();
        else
            event.reply("**:repeat: Повторение треков выключено**").submit();
    }
}
