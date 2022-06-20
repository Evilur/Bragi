package Bragi;

import net.dv8tion.jda.api.EmbedBuilder;
import net.dv8tion.jda.api.entities.MessageChannel;
import net.dv8tion.jda.api.events.message.MessageReceivedEvent;
import net.dv8tion.jda.api.hooks.ListenerAdapter;
import org.jetbrains.annotations.NotNull;

import java.awt.*;
import java.util.Objects;

public class EventHandler extends ListenerAdapter {
    @Override
    public void onMessageReceived (@NotNull MessageReceivedEvent event) {
        /* Если сообщение отправил бот или оно начинаемтся не с префикса, прерываем метод */
        if (event.getAuthor().isBot() || !event.getMessage().getContentDisplay().startsWith(Settings.prefix))
            return;

        MessageChannel channel = event.getChannel();  //Получаем канал, из которого пришло сообщение
        String[] message = event.getMessage().getContentDisplay().substring(Settings.prefix.length()).split(" ", 2);  //Получаем аргументы в нормальном виде, без префикса
        String command = message[0].toLowerCase(); //Инициализируем переменную самой комманды
        String argument = null;  //Объявляем переменную аргумента комманды

        /* Инициализируем переменную аргумента команды, если он есть */
        if (message.length > 1) {
            argument = message[1];
        }

        /* Обрабатываем комманды */
        switch (command) {
            case "ping" -> {  //Проверяем задержку отправки сообщений
                EmbedBuilder embed = Methods.GetPing(event.getMessage());  //Получаем Embed для вывода задерки в милисекундах
                channel.sendMessageEmbeds(embed.build()).submit();  //Отправляем Embed в канал
            }
            case "join" -> {
                if (!Objects.requireNonNull(Objects.requireNonNull(event.getMember()).getVoiceState()).inAudioChannel()) { //Если участник не в голосовом канале, сообщим ему об этом
                    /* Выводим информацию о том, что пользователь не в канале */
                    channel.sendMessageEmbeds(new EmbedBuilder()
                            .setDescription("**Вы должны находиться в голосовом канале**")
                            .setColor(Color.decode("#FE2901")).build()).submit();
                } else {  //Если пользователь в канале, то пытаемся подключиться
                    /* Если не удалось подключиться к каналу, выводим сообщение */
                    if (!Methods.JoinChannel(event)) {
                        channel.sendMessageEmbeds(new EmbedBuilder()
                                .setDescription("**Не удалось подключиться к голосовому каналу. Недостаточно прав**")
                                .setColor(Color.decode("#FE2901")).build()).submit();
                    }
                }
            }
            case "p" -> {  //Воспроизводим отдельный трек или добавляем его в очередь
                EmbedBuilder embed = Methods.PlaySong(argument, event);  //Производим запуск музыки и получаем данные для вывода в Embed
                channel.sendMessageEmbeds(embed.build()).submit();  //Отправляем Embed в канал
            }
            case "s" -> {  //Удалисть из очереди один или несколько треков
                try {
                    assert argument != null;
                    Methods.SkipTracks(Integer.parseInt(argument));
                }  catch (Exception ignore)  {
                    Methods.SkipTracks(1);
                }
            }
        }
    }
}
