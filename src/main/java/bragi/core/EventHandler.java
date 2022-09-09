package bragi.core;

import bragi.Bragi;
import net.dv8tion.jda.api.EmbedBuilder;
import net.dv8tion.jda.api.entities.AudioChannel;
import net.dv8tion.jda.api.entities.MessageChannel;
import net.dv8tion.jda.api.entities.MessageEmbed;
import net.dv8tion.jda.api.events.guild.voice.GuildVoiceUpdateEvent;
import net.dv8tion.jda.api.events.interaction.command.SlashCommandInteractionEvent;
import net.dv8tion.jda.api.events.message.MessageReceivedEvent;
import net.dv8tion.jda.api.hooks.ListenerAdapter;
import org.jetbrains.annotations.NotNull;
import bragi.core.event.*;

import java.awt.*;
import java.util.Objects;
import java.util.regex.Pattern;

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
            case "ping" -> {  //Проверяем задержку отправки сообщений
                MessageEmbed embed = GetPing.run(event.getMessage().getTimeCreated());  //Получаем Embed для вывода задерки в милисекундах
                channel.sendMessageEmbeds(embed).submit();  //Отправляем Embed в канал
            }
            case "join" -> {  //Подключаемся к голосовому каналу
                /* Если участник не в голосовом канале, сообщим ему об этом */
                if (!Objects.requireNonNull(Objects.requireNonNull(event.getMember()).getVoiceState()).inAudioChannel()) {
                    /* Выводим информацию о том, что пользователь не в канале */
                    channel.sendMessageEmbeds(new EmbedBuilder()
                            .setDescription("**Вы должны находиться в голосовом канале**")
                            .setColor(Color.decode("#FE2901")).build()).submit();
                } else {  //Если пользователь в канале, то пытаемся подключиться
                    /* Если не удалось подключиться к каналу, выводим сообщение */
                    if (!JoinChannel.run(event)) {
                        channel.sendMessageEmbeds(new EmbedBuilder()
                                .setDescription("**Не удалось подключиться к голосовому каналу. Недостаточно прав**")
                                .setColor(Color.decode("#FE2901")).build()).submit();
                    }
                }
            }
            case "leave" -> {  //Покидаем голосовой канал
                if (!LeaveChannel.run(event.getGuild().getAudioManager())) {  //Если не удалось покинуть голосовой канал
                    channel.sendMessageEmbeds(new EmbedBuilder()
                            .setDescription("**Не удалось покинуть голосовой канал**")
                            .setColor(Color.decode("#FE2901")).build()).submit();
                }
            }
            case "p" -> {  //Воспроизводим отдельный трек или добавляем его в очередь
                EmbedBuilder embed = PlayTrack.run(argument, event); //Производим запуск музыки и получаем данные для вывода в Embed
                channel.sendMessageEmbeds(embed.build()).submit();  //Отправляем Embed в канал
            }
            case "n" -> {  //Если нашелся неправильный трек, переходим к следующему результату
                EmbedBuilder embed = GetNextTrack.run(event);  //Производим новый поиск и записываем сюда вывод
                channel.sendMessageEmbeds(embed.build()).submit();  //Отправляем Embed в канал
            }
            case "s" -> {  //Удалисть из очереди один или несколько треков
                try {
                    /* Если аргумент содержит цифры */
                    int numberOfTracks = argument != null && Pattern.compile("[0-9]").matcher(argument).find() ?
                            Integer.parseInt(argument.replaceAll("[^0-9]", "")) : 1;  //Получаем количество пропускаемых треков
                    SkipTracks.run(numberOfTracks, true, event.getGuild());  //Пропускаем треки в нужном количестве
                } catch (Exception ignore) {    }
            }
            case "list" ->  {  //Выводим состояние плейлиста
                EmbedBuilder embed = GetPlaylist.run(Players.get(event.getGuild()));
                channel.sendMessageEmbeds(embed.build()).submit();
            }
            case "loop" -> {  //Переключаем режим повторения и выводим сообщение
                EmbedBuilder embed = SwitchLoopMode.run(Players.get(event.getGuild()));
                channel.sendMessageEmbeds(embed.build()).submit();
            }
        }
    }

    @Override
    public void onSlashCommandInteraction(@NotNull SlashCommandInteractionEvent event) {
        if (Objects.requireNonNull(event.getMember()).getUser().isBot())  //Если команду отправил бот, выходим из метода
            return;

        MessageChannel channel = event.getChannel();  //Получаем канал, из которого пришло сообщение
        String command = event.getName(); //Инициализируем переменную самой комманды

        /* Если сервер еще не был инициализирован, инициализируем его */
        if (Players.get(event.getGuild()) == null)
            Players.put(event.getGuild(), new Player(event.getGuild()));

        switch (command) {
            case "ping" -> {  //Проверяем задержку отправки сообщений
                MessageEmbed embed = GetPing.run(event.getTimeCreated());
                event.replyEmbeds(embed).submit();
            }
        }
    }

    @Override
    public void onGuildVoiceUpdate(@NotNull GuildVoiceUpdateEvent event) {  //Этот обработчик событий будет покидать голосовой канал, если никого кроме бота там нет
        /* Канал, из которого ливнул пользователь */
        AudioChannel memberLeaveChannel = event.getChannelLeft();

        /* Если канал не существует (пользователь не ливал из канала, а присоединился) и если пользователь только один */
        if (memberLeaveChannel != null && memberLeaveChannel.getMembers().size() == 1) {
            /* Если последний участник канала - это наш бот, стоит прекратить воспроизведение и покинуть канал */
            if (memberLeaveChannel.getMembers().get(0).getUser().getId().equals(Bragi.bot.getSelfUser().getId())) {
                SkipTracks.run(Players.get(event.getGuild()).getPlaylist().size(), true, event.getGuild());
                LeaveChannel.run(event.getGuild().getAudioManager());
            }
        }
    }
}
