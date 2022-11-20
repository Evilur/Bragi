package bragi.core;

import net.dv8tion.jda.api.entities.AudioChannel;
import net.dv8tion.jda.api.events.guild.voice.GuildVoiceUpdateEvent;
import net.dv8tion.jda.api.events.interaction.command.SlashCommandInteractionEvent;
import net.dv8tion.jda.api.events.message.MessageReceivedEvent;
import net.dv8tion.jda.api.hooks.ListenerAdapter;
import org.jetbrains.annotations.NotNull;
import bragi.core.event.*;

import java.util.Objects;

import static bragi.Bragi.Players;

/** Класс, содержащий обработчики событий для бота */
public final class EventHandler extends ListenerAdapter {
    /** Обработчик события, срабатывающий при получении сообщения */
    @Override
    public void onMessageReceived (@NotNull MessageReceivedEvent event) {
        /* Если сообщение отправил бот или оно начинается не с префикса, прерываем метод */
        if (event.getAuthor().isBot() || !event.getMessage().getContentDisplay().startsWith(Settings.getPrefix()))
            return;

        String[] message = event.getMessage().getContentDisplay().substring(Settings.getPrefix().length())
                .split(" ", 2);  //Получаем аргументы в нормальном виде, без префикса
        String command = message[0].toLowerCase(); //Инициализируем переменную самой команды
        String argument = message.length > 1 ? message[1] : null;  //Объявляем переменную аргумента команды

        /* Если сервер еще не был инициализирован, инициализируем его */
        if (Players.get(event.getGuild()) == null)
            Players.put(event.getGuild(), new Player(event.getGuild()));

        /* Обрабатываем команды */
        switch (command) {
            case "ping" -> //Проверяем задержку отправки сообщений
                    GetPing.run(event);
            case "join" -> //Подключаемся к голосовому каналу
                    JoinChannel.run(event, argument);
            case "leave" -> //Покидаем голосовой канал
                    LeaveChannel.run(event);
            case "list", "l" ->  //Выводим состояние плейлиста
                    GetPlaylist.run(event);
            case "loop" -> //Переключаем режим повторения и выводим сообщение
                    SwitchLoopMode.run(event);
            case "skip", "s" -> //Удалить из очереди один или несколько треков
                    SkipTracks.run(event, argument);
            case "play", "p" -> //Воспроизводим отдельный трек или добавляем его в очередь
                    PlayTrack.run(event, argument);
            case "attachment", "att" -> //Воспроизводим аудио-вложения
                    PlayAttachment.run(event);
            case "next", "n" -> //Если нашелся неправильный трек, переходим к следующему результату
                    GetNextTrack.run(event, argument);
        }
    }

    /** Обработчик события, срабатывающий при получении команды */
    @Override
    public void onSlashCommandInteraction(@NotNull SlashCommandInteractionEvent event) {
        if (Objects.requireNonNull(event.getMember()).getUser().isBot())  //Если команду отправил бот, выходим из метода
            return;

        /* Если сервер еще не был инициализирован, инициализируем его */
        if (Players.get(event.getGuild()) == null)
            Players.put(event.getGuild(), new Player(event.getGuild()));

        switch (event.getName()) {
            case "ping" ->  //Проверяем задержку отправки сообщений
                    GetPing.run(event);
            case "join" ->  //Подключаемся к голосовому каналу
                    JoinChannel.run(event);
            case "leave" ->  //Покидаем голосовой канал
                    LeaveChannel.run(event);
            case "list" ->  //Выводим состояние плейлиста
                    GetPlaylist.run(event);
            case "loop" ->  //Переключаем режим повторения и выводим сообщение
                    SwitchLoopMode.run(event);
            case "skip" ->  //Удалить из очереди один или несколько треков
                    SkipTracks.run(event);
            case "play", "p" -> //Воспроизводим отдельный трек или добавляем его в очередь
                    PlayTrack.run(event);
            case "attachment" -> //Воспроизводим аудио-вложения
                    PlayAttachment.run(event);
            case "next", "n" -> //Если нашелся неправильный трек, переходим к следующему результату
                    GetNextTrack.run(event);
        }
    }

    /** Обработчик события, срабатывающий при изменении голосового канала */
    @Override
    public void onGuildVoiceUpdate(@NotNull GuildVoiceUpdateEvent event) {  //Этот обработчик событий будет покидать голосовой канал, если никого кроме бота там нет
        /* Канал, из которого ливнул пользователь */
        AudioChannel memberLeaveChannel = event.getChannelLeft();

        /* Если канал не существует (пользователь не ливал из канала, а присоединился) и если пользователь только один */
        if (memberLeaveChannel != null && memberLeaveChannel.getMembers().size() == 1) {
            /* Если последний участник канала - это наш бот, стоит прекратить воспроизведение и покинуть канал */
            if (memberLeaveChannel.getMembers().get(0).getUser().getId().equals(event.getGuild().getSelfMember().getId())) {
                // Здесь сделать паузу в произведении
                LeaveChannel.run(event.getGuild());
            }
        }
    }
}
