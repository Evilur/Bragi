package Bragi;

import Bragi.APIObjectsInfo.TrackInfo;
import org.json.JSONObject;
import org.jsoup.Jsoup;
import org.jsoup.nodes.Document;

import javax.crypto.Cipher;
import javax.crypto.spec.IvParameterSpec;
import javax.crypto.spec.SecretKeySpec;
import java.io.*;
import java.net.URL;
import java.util.*;

import static org.apache.commons.codec.digest.DigestUtils.md5Hex;

public class DeezerMethods {
    /* Идентефикатор сессиипользователя */
    private static String sessionId;
    /* Лицензионныйтокен пользователя Deezer */
    private static String licenseToken;

    /* Метод для установки id сессии Deezer клиента */
    public static void SetSessionId() {
        /* Объявляем перменные для создания запроса на сервер Deezer для вытаскивания токена лицензии и идентефикатора пользователя */
        String requestUrl = "https://www.deezer.com/ajax/gw-light.php?version=8.32.0&api_key=ZAIVAHCEISOHWAICUQUEXAEPICENGUAFAEZAIPHAELEEVAHPHUCUFONGUAPASUAY&output=3&input=3&buildId=ios12_universal&screenHeight=480&screenWidth=320&lang=en&method=deezer.getUserData&api_version=1.0&api_token";
        String requestBody = String.format("{arl=\"%s\"}", Settings.deezerToken);

        try {
            /* Делаем запрос для  вытягивания токена лицензии пользователя и идентефикатора пользователя */
            JSONObject jsonObject = DeezerRequest(requestUrl, requestBody);

            /* Присваиваем переменным их законные значения */
            sessionId = jsonObject.getJSONObject("results").getString("SESSION_ID");
            licenseToken = jsonObject.getJSONObject("results").getJSONObject("USER").getJSONObject("OPTIONS").getString("license_token");
        } catch (Exception ignore) {    }
    }

    /* Метод для поиска трека в Deezer */
    public static TrackInfo SearchTrack (String trackTitle, int trackIndex) throws IOException {
        /* Объявляем перменные для создания запроса на сервер Deezer */
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
                GetTrackUrl(trackObject.getString("TRACK_TOKEN"))
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

    /* С помощью этого метода будем получать url закодированного трека */
    private static String GetTrackUrl (String trackToken) {
        /* Объявляем перменные для создания запроса на сервер Deezer */
        String requestUrl = String.format("https://media.deezer.com/v1/get_url?version=8.32.0&api_key=ZAIVAHCEISOHWAICUQUEXAEPICENGUAFAEZAIPHAELEEVAHPHUCUFONGUAPASUAY&output=3&input=3&buildId=ios12_universal&screenHeight=480&screenWidth=320&lang=en&sid=%s", sessionId);
        String requestBody = String.format("{\"license_token\":\"%s\",\"media\":[{\"type\":\"FULL\",\"formats\":[{\"format\":\"FLAC\",\"cipher\":\"BF_CBC_STRIPE\"}]}],\"track_tokens\":[\"%s\"]}", licenseToken, trackToken);

        try {
            /* Получаем url трека в json объекте */
            JSONObject jsonObject = DeezerRequest(requestUrl, requestBody);

            /* Возвращаем url трека */
            return jsonObject
                    .getJSONArray("data").getJSONObject(0)
                    .getJSONArray("media").getJSONObject(0)
                    .getJSONArray("sources").getJSONObject(0)
                    .getString("url");
        } catch (Exception ignore) {
            return null;
        }
    }

    /* С помощью этого метода будем получать раскодированный трек */
    private static void GetTrackDecodedData (String trackUrl, int trackId) throws Exception {
        /* Получаем ключ для расшифровки трека */
        String salt = "g4el58wc0zvf9na1";  //Соль, которая будет применяться вдальнейшем для расшифровки
        String hash = md5Hex(String.valueOf(trackId));  //Получаем MD5 хэш-сумму идентефикатора трека
        String firstMD5Half = hash.substring(0, 16);  //Получаем первую половину хэш-суммы
        String secondMD5Half = hash.substring(16, 32);  //Получаем вторую половину хэш-суммы
        StringBuilder keyBuilder = new StringBuilder(new String());  //В этой переменной будем хранить значение ключа

        /* Собираем ключ */
        for (byte i = 0; i < 16; i++) {
            int charCode = salt.charAt(i) ^ (int)firstMD5Half.charAt(i) ^ (int)secondMD5Half.charAt(i);
            keyBuilder.append((char) charCode);
        }  /* На этом этапе мы получили ключ для расшифровки трека */

        /* Открываем поток для чтения закодированной части трека */
        URL url = new URL(trackUrl);
        BufferedInputStream inputStream = new BufferedInputStream(url.openStream());

        /* Здесь объявляем переменные для рыботы с чанками */
        int chunkSize = 2048;  //Это размер чанка в байтах
        byte[] chunk = new byte[chunkSize];  //Это сам чанк, с которым мы будем работать
        int intervalChunk = 3;

        /* Я сам от себя в шоке, как мне удалось написать следующий кусок кода */

        /* Работаем с шифром Blowfish */
        SecretKeySpec key = new SecretKeySpec(keyBuilder.toString().getBytes(), "Blowfish");  //Загружаем  наш ключ
        IvParameterSpec iv = new IvParameterSpec(new byte[] {0, 1, 2, 3, 4, 5, 6, 7});  //Устанавливаем iv пареметр
        Cipher cipher = Cipher.getInstance("Blowfish/CBC/NoPadding");  //Выбираем шифр
        cipher.init(Cipher.DECRYPT_MODE, key, iv);  //Инициализируем шифр с использованием ключа и iv параметра

        /* Пока поток не закончится, будем с ним работать */
        for (int i = 0, currentChunkSize = chunkSize; currentChunkSize == chunkSize; i++) {
            /* Подгружаем чанк вручную, потому что родной метод  работает как-то нестабильно (чанки различаются по размеру)*/
            for (int j = 0; j < chunkSize; j++)  {
                int value = inputStream.read();  //Получаем значение байта

                if (value != -1) {  //Если поток не закончился
                    chunk[j] = (byte)value;  //Кладем его в чанк
                    currentChunkSize = j + 1;  //Присваиваем значение текущего размера чанка
                } else  //Если поток закончился
                    break;
            }

            /* Получаетися так, что не все чанки нужно расшифровывать */
            byte[] decryptedData;
            if (i % intervalChunk == 0)
                decryptedData = cipher.doFinal(chunk);
            else
                decryptedData = chunk;
        }
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
        headers.put("Cookie", "arl=" + Settings.deezerToken);
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
