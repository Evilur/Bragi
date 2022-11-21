package bragi.core.event;

import net.dv8tion.jda.api.entities.AudioChannel;
import net.dv8tion.jda.api.events.interaction.command.SlashCommandInteractionEvent;
import net.dv8tion.jda.api.events.message.MessageReceivedEvent;
import net.dv8tion.jda.api.managers.AudioManager;

import java.util.Objects;

/** Класс для присоединения к голосовому каналу */
public final class JoinChannel {
    /** Метод для присоединения к голосовому каналу
     * @param event Событие получения сообщения
     * @return True, если удалось подключиться. В противном случае - false
     */
    public static boolean run(MessageReceivedEvent event, String argument) {
        AudioChannel audioChannel = Objects.requireNonNull(Objects.requireNonNull(event.getMember()).getVoiceState())
                .getChannel();  //Получаем голосовой канал, к которому нужно присоединиться

        if (argument != null) {  //Если бы передан аргумент
            try {
                /* Если в качестве аргумента был передан пользователь, получаем голосовой канал этого пользователя */
                if (!event.getMessage().getMentions().getMembers().isEmpty())
                    audioChannel = Objects.requireNonNull(event.getMessage().getMentions().getMembers().get(0)
                            .getVoiceState()).getChannel();  //Получаем голосовой канал
                else  //В ином случае, пытаемся найти пользователя по id
                    audioChannel = Objects.requireNonNull(Objects.requireNonNull(event.getGuild().getMemberByTag(argument))
                            .getVoiceState()).getChannel();  //Получаем голосовой канал
            } catch (Exception ignore) {
                event.getChannel().sendMessage(":x:** Этот пользователь не находится в голосовом канале**").submit();
                return false;
            }

            /* Если пользователь не в канале, выходим из метода */
            if (audioChannel == null) {
                event.getChannel().sendMessage(":x:** Этот пользователь не находится в голосовом канале**").submit();
                return false;
            }
        } else if (!Objects.requireNonNull(Objects.requireNonNull(event.getMember()).getVoiceState())
                .inAudioChannel()) {  //Если участник не в голосовом канале, сообщим ему об этом
            event.getChannel().sendMessage(":x:** Вы должны находиться в голосовом канале**").submit();
            return false;
        }

        try {  //Пытаемся подключиться к голосовому каналу
            /* //Если бот не в голосовом канале */
            if (!Objects.requireNonNull(Objects.requireNonNull(
                    event.getGuild()).getSelfMember().getVoiceState()).inAudioChannel()) {
                if (audioChannel == Objects.requireNonNull(Objects.requireNonNull(event.getGuild())
                                .getSelfMember().getVoiceState()).getChannel()) {  //Если бот уже в этом канале
                    event.getChannel().sendMessage(":x:** Бот уже подключен к этому голосовому каналу**").submit();
                } else {
                    AudioManager audioManager = Objects.requireNonNull(event.getGuild()).getAudioManager();
                    audioManager.openAudioConnection(audioChannel);

                    event.getChannel().sendMessage(String.format(":thumbsup:** Присоединился к каналу `%s`**",
                            Objects.requireNonNull(audioChannel).getName())).submit();
                }
            }
            return true;
        } catch (Exception ignore) {
            event.getChannel().sendMessage(":x:** Не удалось подключиться к голосовому каналу. Недостаточно прав**")
                    .submit();
            return false;
        }
    }
    /** Метод для присоединения к голосовому каналу
     * @param event Событие получения команды
     * @return True, если удалось подключиться. В противном случае - false
     */
    public static boolean run(SlashCommandInteractionEvent event) {
        AudioChannel audioChannel = Objects.requireNonNull(Objects.requireNonNull(event.getMember()).getVoiceState())
                .getChannel();  //Получаем голосовой канал, к которому нужно присоединиться

        /* Если в качестве параметра был передан пользователь, получаем голосовой канал этого пользователя */
        if (!event.getOptionsByName("user").isEmpty()) {
            audioChannel = Objects.requireNonNull(Objects.requireNonNull(Objects.requireNonNull(event.getGuild())
                            .getMemberById(Objects.requireNonNull(event.getOption("user")).getAsUser().getId()))
                    .getVoiceState()).getChannel();

            /* Если пользователь не в канале, выходим из метода */
            if (audioChannel == null) {
                event.reply(":x:** Этот пользователь не находится в голосовом канале**").submit();
                return false;
            }
        } else if (!Objects.requireNonNull(Objects.requireNonNull(event.getMember()).getVoiceState())
                .inAudioChannel()) {  //Если участник не в голосовом канале, сообщим ему об этом
            event.reply(":x:** Вы должны находиться в голосовом канале**").submit();
            return false;
        }

        try {  //Пытаемся подключиться к голосовому каналу
            /* //Если бот не в голосовом канале */
            if (!Objects.requireNonNull(Objects.requireNonNull(
                    event.getGuild()).getSelfMember().getVoiceState()).inAudioChannel()) {
                if (audioChannel == Objects.requireNonNull(Objects.requireNonNull(event.getGuild())
                                .getSelfMember().getVoiceState()).getChannel()) {  //Если бот уже в этом канале
                    event.reply(":x:** Бот уже подключен к этому голосовому каналу**").submit();
                } else {
                    AudioManager audioManager = Objects.requireNonNull(event.getGuild()).getAudioManager();
                    audioManager.openAudioConnection(audioChannel);

                    event.reply(String.format(":thumbsup:** Присоединился к каналу** `%s`",
                            Objects.requireNonNull(audioChannel).getName())).submit();
                }
            }
            return true;
        } catch (Exception ignore) {
            event.reply(":x:** Не удалось подключиться к голосовому каналу. Недостаточно прав**").submit();
            return false;
        }
    }
}
