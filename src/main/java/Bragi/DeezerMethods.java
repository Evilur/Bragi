package Bragi;

import Bragi.APIObjectsInfo.TrackInfo;
import org.json.JSONObject;
import org.jsoup.Jsoup;
import org.jsoup.nodes.Document;

import java.io.IOException;
import java.util.HashMap;
import java.util.Map;

public class DeezerMethods {
    private static String sessionId;

    /* Метод для установки id сессии Deezer клиента */
    public static void SetSessionId() {
        String requestUrl = "https://www.deezer.com/ajax/gw-light.php?method=deezer.ping&api_version=1.0&api_token";
        String requestBody = String.format("{arl=\"%s\"}", Settings.deezerToken);

        try {
            JSONObject jsonObject = DeezerRequest(requestUrl, requestBody);
            sessionId = jsonObject.getJSONObject("results").getString("SESSION");
            SearchTrack("Лесник", 0);
        } catch (Exception ignore) {
            System.out.println("Ошибка подключения к серверам Deezer");
        }
    }

    /* Метод для поиска трека в Deezer */
    public static TrackInfo SearchTrack (String trackTitle, int trackIndex) throws IOException {
        /* Объявляем перменные для запроса на сервер Deezer */
        String requestUrl = String.format("https://api.deezer.com/1.0/gateway.php?api_key=ZAIVAHCEISOHWAICUQUEXAEPICENGUAFAEZAIPHAELEEVAHPHUCUFONGUAPASUAY&output=3&input=3&sid=%s&method=search.music", sessionId);
        String requestBody = String.format("{\"query\":\"%s\",\"nb\":1,\"output\":\"TRACK\",\"filter\":\"TRACK\",\"start\":%d}", trackTitle, trackIndex);

        /* Делаем запрос на сервер Deezer */
        JSONObject jsonObject = DeezerRequest(requestUrl, requestBody);

        /* Получаем количество треков, найденных по поисковому запросу. Если не было найдено подходящих треков, выбрасываем исключение */
        int totalOfSearchResults = jsonObject.getJSONObject("results").getInt("total");
        if (totalOfSearchResults == 0)
            throw new IOException("No search results");

        JSONObject trackObject = jsonObject.getJSONObject("results").getJSONArray("data").getJSONObject(0);

        TrackInfo trackInfo = new TrackInfo();  //Сюда будем записывать результат
        trackInfo.SetTrackInformation(  //Устанавливаем информацию о треке
                trackObject.getInt("SNG_ID"),
                trackObject.getString("SNG_TITLE"),
                trackObject.getJSONArray("MEDIA").getJSONObject(0).getString("HREF")
        );
        trackInfo.SetAlbumInformation(  //Устанавливаем информацию об альбоме
                trackObject.getInt("ALB_ID"),
                trackObject.getString("ALB_TITLE"),
                String.format("https://e-cdns-images.dzcdn.net/images/cover/%s/1000x1000-000000-80-0-0.jpg", trackObject.getString("ALB_PICTURE"))
        );
        trackInfo.SetArtistInformation(  //Устанавливаем информацию об исполнителе
                trackObject.getInt("ART_ID"),
                trackObject.getString("ART_NAME"),
                String.format("https://e-cdns-images.dzcdn.net/images/artist/%s/1000x1000-000000-80-0-0.jpg", trackObject.getString("ART_PICTURE"))
        );
        trackInfo.SetTrackDuration(trackObject.getInt("DURATION"));  //Устанавливаем длину трека

        /* Объявляем некоторые перменные, которые в будущем помогут нам поменять результат поиска */
        trackInfo.totalOfSearchResults = totalOfSearchResults;
        trackInfo.nextTrackInSearchResultsUrl = String.valueOf(trackIndex + 1);
        trackInfo.searchRequest = trackTitle;

        return trackInfo;
    }

    /* Метод для парсинга JSON информации с Deezer */
    private static JSONObject DeezerRequest (String url, String requestBody) throws IOException {
        /* Устанавливаем заголовки */
        Map<String, String> headers = new HashMap<>();
        headers.put("Accept", "application/json, text/plain, */*");
        headers.put("Content-Type", "text/plain;charset=UTF-8");
        headers.put("User-Agent", "Deezer/7.17.0.2 CFNetwork/1098.6 Darwin/19.0.0");
        headers.put("Cache-Control", "max-age=0");
        headers.put("Accept-Language", "en-US,en;q=0.9,en-US;q=0.8,en;q=0.7");
        headers.put("Accept-Charset", "utf-8,ISO-8859-1;q=0.8,*;q=0.7");
        headers.put("Connection", "Close");

        Document jsonDocument = Jsoup
                .connect(url)
                .headers(headers)
                .ignoreContentType(true)
                .requestBody(requestBody)
                .post();
        String jsonString = jsonDocument.body().html();  //Получаем содержимое тега body, что и является ответом сервера

        return new JSONObject(jsonString);  //Создаем JSON объект из JSON строки и возвращаем его
    }
}
