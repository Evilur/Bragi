package bragi.core.event;

import net.dv8tion.jda.api.entities.AudioChannel;
import net.dv8tion.jda.api.events.interaction.command.SlashCommandInteractionEvent;
import net.dv8tion.jda.api.events.message.MessageReceivedEvent;
import net.dv8tion.jda.api.managers.AudioManager;

import java.util.Objects;

/** Класс для присоединения к голосовому каналу */
public class JoinChannel {
    /** Метод для присоединения к голосовому каналу
     * @param event Событие получения сообщения
     * @return true, если удалось подключиться. В противном случае - false
     */
    public static boolean run(MessageReceivedEvent event) {
        /* Если участник не в голосовом канале, сообщим ему об этом */
        if (!Objects.requireNonNull(Objects.requireNonNull(event.getMember()).getVoiceState()).inAudioChannel()) {
            event.getChannel().sendMessage(":x:** Вы должны находиться в голосовом канале**").submit();
            return false;
        }

        try {  //Пытаемся подключиться к голосовому каналу
            /* //Если бот не в голосовом канале */
            if (!Objects.requireNonNull(Objects.requireNonNull(
                    event.getGuild()).getSelfMember().getVoiceState()).inAudioChannel()) {
                if (Objects.requireNonNull(event.getMember()).getVoiceState().getChannel() ==
                        Objects.requireNonNull(Objects.requireNonNull(event.getGuild())
                                .getSelfMember().getVoiceState()).getChannel()) {  //Если бот уже в этом канале
                    event.getChannel().sendMessage(":x:** Бот уже подключен к этому голосовому каналу**").submit();
                } else {
                    AudioChannel audioChannel = event.getMember().getVoiceState().getChannel();
                    AudioManager audioManager = Objects.requireNonNull(event.getGuild()).getAudioManager();
                    audioManager.openAudioConnection(audioChannel);

                    event.getChannel().sendMessage(String.format(":thumbsup:** Присоединился к каналу `%s`**",
                            Objects.requireNonNull(event.getMember().getVoiceState().getChannel()).getName())).submit();
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
     * @return true, если удалось подключиться. В противном случае - false
     */
    public static boolean run(SlashCommandInteractionEvent event) {
        /* Если участник не в голосовом канале, сообщим ему об этом */
        if (!Objects.requireNonNull(Objects.requireNonNull(event.getMember()).getVoiceState()).inAudioChannel()) {
            event.reply(":x:** Вы должны находиться в голосовом канале**").submit();
            return false;
        }

        try {  //Пытаемся подключиться к голосовому каналу
            /* //Если бот не в голосовом канале */
            if (!Objects.requireNonNull(Objects.requireNonNull(
                    event.getGuild()).getSelfMember().getVoiceState()).inAudioChannel()) {
                if (Objects.requireNonNull(event.getMember()).getVoiceState().getChannel() ==
                        Objects.requireNonNull(Objects.requireNonNull(event.getGuild())
                                .getSelfMember().getVoiceState()).getChannel()) {  //Если бот уже в этом канале
                    event.reply(":x:** Бот уже подключен к этому голосовому каналу**").submit();
                } else {
                    AudioChannel audioChannel = event.getMember().getVoiceState().getChannel();
                    AudioManager audioManager = Objects.requireNonNull(event.getGuild()).getAudioManager();
                    audioManager.openAudioConnection(audioChannel);

                    event.reply(String.format(":thumbsup:** Присоединился к каналу** `%s`",
                            Objects.requireNonNull(event.getMember().getVoiceState().getChannel()).getName())).submit();
                }
            }
            return true;
        } catch (Exception ignore) {
            event.reply(":x:** Не удалось подключиться к голосовому каналу. Недостаточно прав**").submit();
            return false;
        }
    }
}
