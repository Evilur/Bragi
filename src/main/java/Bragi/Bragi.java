package Bragi;

import net.dv8tion.jda.api.JDA;
import net.dv8tion.jda.api.JDABuilder;

public class Bragi {
    public static void main(String[] args) throws Exception {
        JDA bot = JDABuilder.createDefault(Settings.botToken).build(); //Создаем бота и подключаем его на сервер
        DeezerMethods.SetSessionId();  //Подключаемся к серверам Deezer и получаем Id сессии
        bot.addEventListener(new EventHandler());  //Добавляем обработчики событий
    }
}
