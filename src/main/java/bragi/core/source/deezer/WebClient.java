package bragi.core.source.deezer;

import bragi.core.Settings;
import org.json.JSONObject;
import org.jsoup.Jsoup;
import org.jsoup.nodes.Document;

import java.io.IOException;
import java.util.HashMap;
import java.util.Map;

public final class WebClient {
    private final Map<String, String> headers = new HashMap<>();  //Стандартные веб-заголовки

    public WebClient() {
        /* Устанавливаем заголовки */
        headers.put("Accept", "application/json, text/plain, */*");
        headers.put("Content-Type", "text/plain;charset=UTF-8");
        headers.put("User-Agent", "Deezer/7.17.0.2 CFNetwork/1098.6 Darwin/19.0.0");
        headers.put("Cache-Control", "max-age=0");
        headers.put("Accept-Language", "en-US,en;q=0.9,en-US;q=0.8,en;q=0.7");
        headers.put("Accept-Charset", "utf-8,ISO-8859-1;q=0.8,*;q=0.7");
        headers.put("Cookie", "arl=" + Settings.getDeezerArl());
        headers.put("Connection", "Close");
    }

    /* Метод для парсинга JSON информации с Deezer */
    public JSONObject sendRequest(String url, String requestBody) throws IOException {
        Document jsonDocument = Jsoup
                .connect(url)
                .headers(this.headers)
                .ignoreContentType(true)
                .requestBody(requestBody)
                .post();
        /* Получаем содержимое тега body, что и является ответом сервера, и удаляем амперсанд */
        String jsonString = jsonDocument.body().html().replace("&amp;", "&");
        return new JSONObject(jsonString);  //Создаем JSON объект из JSON строки и возвращаем его
    }
}
