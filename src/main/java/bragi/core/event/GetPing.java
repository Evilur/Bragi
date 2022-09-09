package bragi.core.event;

import net.dv8tion.jda.api.EmbedBuilder;
import net.dv8tion.jda.api.entities.MessageEmbed;

import java.awt.*;
import java.time.OffsetDateTime;

import static java.lang.String.valueOf;

public class GetPing {
    /* Метод для вывода задержки бота */
    public static MessageEmbed run(OffsetDateTime timeCreated) {
        /* Высчитываем задержку */
        long creationTime = timeCreated.toInstant().toEpochMilli();  //Получаем время создание сообщения в миллисекундах
        long currentTime =  OffsetDateTime.now().toInstant().toEpochMilli();  //Получаем текущее время
        String ping = valueOf(currentTime - creationTime);  //Получаем задержку

        /* Создаем Embed и возвращаем его */
        return new EmbedBuilder()
                .setTitle("Задержка")
                .setDescription("**" + ping + " мс**")
                .setColor(Color.decode("#0BDA4D")).build();
    }
}
