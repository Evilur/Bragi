package bragi.core.event;

import net.dv8tion.jda.api.EmbedBuilder;
import net.dv8tion.jda.api.entities.MessageEmbed;
import net.dv8tion.jda.api.events.interaction.command.SlashCommandInteractionEvent;
import net.dv8tion.jda.api.events.message.MessageReceivedEvent;

import java.awt.*;
import java.time.OffsetDateTime;

import static java.lang.String.valueOf;

/** Класс для вычисления задержки сообщений */
public class GetPing {
    /** Метод для запуска вычисления задержки
     * @param event Событие получения сообщения
     */
    public static void run(MessageReceivedEvent event) {
        /* //Получаем время создания сообщения в миллисекундах */
        long creationTime = event.getMessage().getTimeCreated().toInstant().toEpochMilli();
        event.getChannel().sendMessageEmbeds(eval(creationTime)).submit();
    }
    /** Метод для запуска вычисления задержки
     * @param event Событие получения команды
     */
    public static void run(SlashCommandInteractionEvent event) {
        /* //Получаем время создания сообщения в миллисекундах */
        long creationTime = event.getTimeCreated().toInstant().toEpochMilli();
        event.replyEmbeds(eval(creationTime)).submit();  //Создаем Embed и отправляем его
    }

    /** Метод для вычисления задержки
     * @param creationTime Время получения сообщения в миллисекундах
     * @return Embed для вывода
     */
    private static MessageEmbed eval(long creationTime) {
        long currentTime =  OffsetDateTime.now().toInstant().toEpochMilli();  //Получаем текущее время
        String ping = valueOf(currentTime - creationTime);  //Получаем задержку

        /* Создаем Embed */
        return new EmbedBuilder()
                .setTitle("Задержка")
                .setDescription("**:hourglass: " + ping + " мс**")
                .setColor(Color.decode("#0BDA4D")).build();
    }
}
