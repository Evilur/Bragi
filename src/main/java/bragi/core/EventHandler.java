package bragi.core;

import net.dv8tion.jda.api.entities.AudioChannel;
import net.dv8tion.jda.api.entities.MessageChannel;
import net.dv8tion.jda.api.entities.MessageEmbed;
import net.dv8tion.jda.api.events.guild.voice.GuildVoiceUpdateEvent;
import net.dv8tion.jda.api.events.interaction.command.SlashCommandInteractionEvent;
import net.dv8tion.jda.api.events.message.MessageReceivedEvent;
import net.dv8tion.jda.api.hooks.ListenerAdapter;
import org.jetbrains.annotations.NotNull;
import bragi.core.event.*;

import java.util.Objects;

import static bragi.Bragi.Players;

public class EventHandler extends ListenerAdapter {
    @Override
    public void onMessageReceived (@NotNull MessageReceivedEvent event) {  //Обработчик событий, срабатывающий при отправке сообщения
        /* Если сообщение отправил бот или оно начинаемтся не с префикса, прерываем метод */
        if (event.getAuthor().isBot() || !event.getMessage().getContentDisplay().startsWith(Settings.getPrefix()))
            return;

        MessageChannel channel = event.getChannel();  //Получаем канал, из которого пришло сообщение
        String[] message = event.getMessage().getContentDisplay().substring(Settings.getPrefix().length()).split(" ", 2);  //Получаем аргументы в нормальном виде, без префикса
        String command = message[0].toLowerCase(); //Инициализируем переменную самой комманды
        String argument = message.length > 1 ? message[1] : null;  //Объявляем переменную аргумента комманды

        /* Если сервер еще не был инициализирован, инициализируем его */
        if (Players.get(event.getGuild()) == null)
            Players.put(event.getGuild(), new Player(event.getGuild()));

        /* Обрабатываем комманды */
        switch (command) {
            case "ping" -> //Проверяем задержку отправки сообщений
                    GetPing.run(event);
            case "join" -> //Подключаемся к голосовому каналу
                    JoinChannel.run(event);
            case "leave" -> //Покидаем голосовой канал
                    LeaveChannel.run(event);
            case "list", "l" ->  //Выводим состояние плейлиста
                    GetPlaylist.run(event);
            case "loop" -> //Переключаем режим повторения и выводим сообщение
                    SwitchLoopMode.run(event);
            case "skip", "s" -> //Удалисть из очереди один или несколько треков
                    SkipTracks.run(event, argument);
            case "play", "p" -> {  //Воспроизводим отдельный трек или добавляем его в очередь
                PlayTrack.run(event, argument);
            }
            case "next", "n" -> {  //Если нашелся неправильный трек, переходим к следующему результату
                MessageEmbed embed = GetNextTrack.run(event);  //Производим новый поиск и записываем сюда вывод
                channel.sendMessageEmbeds(embed).submit();  //Отправляем Embed в канал
            }
        }
    }

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
            case "play" ->  //Воспроизводим отдельный трек или добавляем его в очередь
                    System.out.println("Not ready yet");
            case "loop" ->  //Переключаем режим повторения и выводим сообщение
                    SwitchLoopMode.run(event);
            case "skip" ->  //Удалисть из очереди один или несколько треков
                    SkipTracks.run(event);
        }
    }

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
