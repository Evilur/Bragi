package bragi.util;

import bragi.info.AlbumInfo;
import bragi.info.TrackInfo;
import bragi.Settings;
import org.json.JSONArray;
import org.json.JSONObject;
import org.jsoup.Jsoup;
import org.jsoup.nodes.Document;

import java.io.*;
import java.util.*;

public class DeezerMethods {
    /* Идентефикатор сессиипользователя */
    private static String sessionId;
    /* Лицензионныйтокен пользователя Deezer */
    private static String licenseToken;

    /* Метод для установки id сессии Deezer клиента */
    public static void setSessionId() {
        /* Объявляем перменные для создания запроса на сервер Deezer для вытаскивания токена лицензии и идентефикатора пользователя */
        String requestUrl = "https://www.deezer.com/ajax/gw-light.php?version=8.32.0&api_key=ZAIVAHCEISOHWAICUQUEXAEPICENGUAFAEZAIPHAELEEVAHPHUCUFONGUAPASUAY&output=3&input=3&buildId=ios12_universal&screenHeight=480&screenWidth=320&lang=en&method=deezer.getUserData&api_version=1.0&api_token";
        String requestBody = String.format("{arl=\"%s\"}", Settings.getDeezerArl());

        try {
            /* Делаем запрос для  вытягивания токена лицензии пользователя и идентефикатора пользователя */
            JSONObject jsonObject = deezerRequest(requestUrl, requestBody);

            /* Присваиваем переменным их законные значения */
            sessionId = jsonObject.getJSONObject("results").getString("SESSION_ID");
            licenseToken = jsonObject.getJSONObject("results").getJSONObject("USER").getJSONObject("OPTIONS").getString("license_token");
        } catch (Exception ignore) {    }
    }

    /* Метод для поиска трека в Deezer */
    public static TrackInfo searchTrack(String trackTitle, int trackIndex) throws Exception {
        /* Объявляем перменные для создания запроса на сервер Deezer */
        String requestUrl = String.format("https://api.deezer.com/1.0/gateway.php?api_key=ZAIVAHCEISOHWAICUQUEXAEPICENGUAFAEZAIPHAELEEVAHPHUCUFONGUAPASUAY&output=3&input=3&sid=%s&method=search.music", sessionId);
        String requestBody = String.format("{\"query\":\"%s\",\"nb\":1,\"output\":\"TRACK\",\"filter\":\"TRACK\",\"start\":%d}", trackTitle, trackIndex);

        /* Делаем запрос на сервер Deezer */
        JSONObject jsonObject = deezerRequest(requestUrl, requestBody);

        /* Получаем количество треков, найденных по поисковому запросу. Если не было найдено подходящих треков, выбрасываем исключение */
        int totalOfSearchResults = jsonObject.getJSONObject("results").getInt("total");
        if (totalOfSearchResults == 0)
            throw new IOException("No search results");

        JSONObject trackObject = jsonObject.getJSONObject("results").getJSONArray("data").getJSONObject(0);

        TrackInfo trackInfo = new TrackInfo();  //Сюда будем записывать результат

        /* Устанавливаем информацию о треке */
        trackInfo.setSource("Deezer");
        trackInfo.setTrackId(trackObject.getInt("SNG_ID"));
        trackInfo.setTrackTitle(trackObject.getString("SNG_TITLE"));
        trackInfo.setTrackIdentifier(trackObject.getInt("SNG_ID") + "|" + getTrackUrl(trackObject.getString("TRACK_TOKEN"), (byte) 2));
        trackInfo.setTrackDuration(trackObject.getInt("DURATION"));

        /* Устанавливаем информацию об альбоме */
        trackInfo.setAlbumId(trackObject.getInt("ALB_ID"));
        trackInfo.setAlbumTitle(trackObject.getString("ALB_TITLE"));
        trackInfo.setAlbumCoverUrl(String.format("https://e-cdns-images.dzcdn.net/images/cover/%s/1000x1000-000000-80-0-0.jpg", trackObject.getString("ALB_PICTURE")));

        /* Устанавливаем информацию об исполнителе */
        trackInfo.setArtistId(trackObject.getInt("ART_ID"));
        trackInfo.setArtistName(trackObject.getString("ART_NAME"));
        trackInfo.setArtistPictureUrl(String.format("https://e-cdns-images.dzcdn.net/images/artist/%s/1000x1000-000000-80-0-0.jpg", trackObject.getString("ART_PICTURE")));

        /* Объявляем некоторые перменные, которые в будущем помогут нам поменять результат поиска */
        trackInfo.setTotalOfSearchResults(totalOfSearchResults);
        trackInfo.setNextTrackInSearchResults(String.valueOf(jsonObject.getJSONObject("results").getInt("next")));
        trackInfo.setSearchRequest(trackTitle);

        return trackInfo;
    }

    public static AlbumInfo searchAlbum(String albumName, int albumIndex) throws IOException {
        /* Объявляем перменные для создания запроса на сервер Deezer */
        String requestUrl = String.format("https://api.deezer.com/1.0/gateway.php?api_key=ZAIVAHCEISOHWAICUQUEXAEPICENGUAFAEZAIPHAELEEVAHPHUCUFONGUAPASUAY&output=3&input=3&sid=%s&method=search.music", sessionId);
        String requestBody = String.format("{\"query\":\"%s\",\"nb\":1,\"output\":\"ALBUM\",\"filter\":\"ALBUM\",\"start\":%s}", albumName, albumIndex);

        /* Делаем запрос на сервер Deezer */
        JSONObject jsonObject = deezerRequest(requestUrl, requestBody);
        System.out.println(jsonObject);

        return null;
    }

    /* С помощью этого метода будем получать url закодированного трека */
    private static String getTrackUrl(String trackToken, byte trackQuality) throws IOException {
        /* Объявляем перменные для создания запроса на сервер Deezer */
        String requestUrl = String.format("https://media.deezer.com/v1/get_url?version=8.32.0&api_key=ZAIVAHCEISOHWAICUQUEXAEPICENGUAFAEZAIPHAELEEVAHPHUCUFONGUAPASUAY&output=3&input=3&buildId=ios12_universal&screenHeight=480&screenWidth=320&lang=en&sid=%s", sessionId);
        String requestBody = String.format("{\"license_token\":\"%s\",\"media\":[{\"type\":\"FULL\",\"formats\":[{\"format\":\"%s\",\"cipher\":\"BF_CBC_STRIPE\"}]}],\"track_tokens\":[\"%s\"]}", licenseToken, Quality.values()[trackQuality], trackToken);

        /* Получаем url трека в json массиве */
        JSONArray media = deezerRequest(requestUrl, requestBody)
                .getJSONArray("data").getJSONObject(0)
                .getJSONArray("media");
        System.out.println(Quality.values()[trackQuality]);

        if (!media.isEmpty()) {  //Проверяем, существует ли трек в таком качестве
            /* Если трек существует, возвращаем url */
            return media.getJSONObject(0).getJSONArray("sources")
                    .getJSONObject(0).getString("url");
        } else {  //Если не существует, снижаем планку качества
            return getTrackUrl(trackToken, (byte) (trackQuality - 1));
        }
    }

    /* Метод для парсинга JSON информации с Deezer */
    private static JSONObject deezerRequest(String url, String requestBody) throws IOException {
        /* Устанавливаем заголовки */
        Map<String, String> headers = new HashMap<>();
        headers.put("Accept", "application/json, text/plain, */*");
        headers.put("Content-Type", "text/plain;charset=UTF-8");
        headers.put("User-Agent", "Deezer/7.17.0.2 CFNetwork/1098.6 Darwin/19.0.0");
        headers.put("Cache-Control", "max-age=0");
        headers.put("Accept-Language", "en-US,en;q=0.9,en-US;q=0.8,en;q=0.7");
        headers.put("Accept-Charset", "utf-8,ISO-8859-1;q=0.8,*;q=0.7");
        headers.put("Cookie", "arl=" + Settings.getDeezerArl());
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
