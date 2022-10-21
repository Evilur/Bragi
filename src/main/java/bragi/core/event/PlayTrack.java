package bragi.core.event;

import bragi.core.Methods;
import net.dv8tion.jda.api.EmbedBuilder;
import net.dv8tion.jda.api.entities.MessageEmbed;
import net.dv8tion.jda.api.events.message.MessageReceivedEvent;

import java.awt.*;
import java.util.Objects;

public class PlayTrack {
    /* Метод для поиска музыки и ее воспроизведения */
    public static MessageEmbed run(String argument, MessageReceivedEvent event) {
        /* Если не были переданы аргументы, и не были прикреплены вложения */
        if (argument == null && event.getMessage().getAttachments().isEmpty()) {
            return new EmbedBuilder()
                    .setTitle("Ошибка!")
                    .setDescription("**После команды не было передано обязательных аргументов!**")
                    .setColor(Color.decode("#FE2901")).build();
        } else if (!Objects.requireNonNull(Objects.requireNonNull(event.getMember()).getVoiceState()).inAudioChannel()) { //Если участник не в голосовом канале, сообщим ему об этом и не выполняем код дальше
            return new EmbedBuilder()
                    .setDescription("**Вы должны находиться в голосовом канале**")
                    .setColor(Color.decode("#FE2901")).build();
        }

        assert argument != null;
        if (!event.getMessage().getAttachments().isEmpty()) {  //Если к сообщению были прикреплены вложения, то пытаемся их воспроизвести */
            return Methods.playTrackFromAttachment(event);
        } else {  //Иначе воспроизвводим трек из Deezer
            if (!argument.startsWith("https://") && !argument.startsWith("http://"))  {  //Если аргумент не содержит url
                return Methods.playDeezerTrackBySearchResults(argument, event).build();
            } else {
                return new EmbedBuilder()
                        .setColor(Color.decode("#FE2901"))
                        .setDescription("**Такая возможность еще не добавлена**").build();
            }
        }
    }
}
