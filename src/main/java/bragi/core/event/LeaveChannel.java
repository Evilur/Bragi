package bragi.core.event;

import bragi.Bragi;
import net.dv8tion.jda.api.entities.Guild;
import net.dv8tion.jda.api.events.interaction.command.SlashCommandInteractionEvent;
import net.dv8tion.jda.api.events.message.MessageReceivedEvent;

import java.util.Objects;

/** Класс для покидания голосовомго канала */
public final class LeaveChannel {
    /** Метод для запуска операции покидания голосового канала
     * @param event Событие получения сообщения
     */
    public static void run(MessageReceivedEvent event) {
        /* Если пользователь в голосовом канале */
        if (Objects.requireNonNull(Objects.requireNonNull(
                event.getGuild()).getSelfMember().getVoiceState()).inAudioChannel()) {
            Objects.requireNonNull(event.getGuild()).getAudioManager().closeAudioConnection();  //Закрываем соединение
            event.getChannel().sendMessage(String.format(":door:**Голосовой канал **`%s`** покинут**",
                    Objects.requireNonNull(Objects.requireNonNull(Objects.requireNonNull(event.getGuild())
                            .getSelfMember().getVoiceState()).getChannel()).getName())).submit();  //Отправляем ответ

            Bragi.Players.get(event.getGuild()).getInstance().Stop();  //Останавливаем проигрывание
            Bragi.Players.get(event.getGuild()).getPlaylist().clear();  //Очищаем плейлист
        } else {  //Если пользователь не в голосовом канале
            /* Отправляем ответ */
            event.getChannel().sendMessage(":x: **Бот не подключен к голосовому каналу**").submit();
        }
    }
    /** Метод для запуска операции покидания голосового канала
     * @param event Событие получения команды
     */
    public static void run(SlashCommandInteractionEvent event) {
        /* Если пользователь в голосовом канале */
        if (Objects.requireNonNull(Objects.requireNonNull(
                event.getGuild()).getSelfMember().getVoiceState()).inAudioChannel()) {
            Objects.requireNonNull(event.getGuild()).getAudioManager().closeAudioConnection();  //Закрываем соединение
            event.reply(String.format(":door:**Голосовой канал **`%s`** покинут**",
                    Objects.requireNonNull(Objects.requireNonNull(Objects.requireNonNull(event.getGuild())
                            .getSelfMember().getVoiceState()).getChannel()).getName())).submit();  //Отправляем ответ

            Bragi.Players.get(event.getGuild()).getInstance().Stop();  //Останавливаем проигрывание
            Bragi.Players.get(event.getGuild()).getPlaylist().clear();  //Очищаем плейлист
        } else {  //Если пользователь не в голосовом канале
            event.reply(":x: **Бот не подключен к голосовому каналу**").submit();  //Отправляем ответ
        }
    }
    /** Метод для запуска операции покидания голосового канала
     * @param guild Гильдия, для которой нужно покинуть голосовой канал
     */
    public static void run(Guild guild) {
        guild.getAudioManager().closeAudioConnection();  //Закрываем соединение
    }
}
