package Bragi.ObjectsInfo;

public class TrackInfo {
    /* Информация о самом треке */
    public int trackId;
    public String trackTitle;
    public String trackURL;
    public int trackDuration;
    public String trackDurationFormatted;

    /* Информация об альбоме этого трека */
    public int albumId;
    public String albumTitle;
    public String albumCoverUrl;

    /* Информация об исполнителе трека */
    public int artistId;
    public String artistName;
    public String artistPictureUrl;

    /* Ссылка на следующий трек в результатах поиск (на случай, если первый в поиске оказался не тем, который хотелось найти) */
    public String nextTrackInSearchResultsUrl;

    /* Поисковой запрос, с помощью которого нашли этот трек */
    public String searchRequest;
    /* Общее количество результатов поиска */
    public int totalOfSearchResults;

    /* Если значение false, то метод, выводящий информацию принебрегнет некоторыми деталями */
    public boolean richInformation = true;

    public void SetTrackInformation(int id, String title, String url) {  //Метод, с помощью которого будем устанавливать информацию о треке
        this.trackId = id;
        this.trackTitle = title;
        this.trackURL = url;
    }

    public void SetTrackDuration (int duration) {
        this.trackDuration = duration;

        /* Высчитываем  продолжительность и делаем ей приемлемый вид */
        int minutes = duration / 60;
        int seconds = duration - minutes * 60;
        this.trackDurationFormatted = String.format("%dм%dс", minutes, seconds);
    }

    public void SetAlbumInformation (int id, String title, String cover) {  //Метод, с помощью которого будем устанавливать информацию об альбоме
        this.albumId = id;
        this.albumTitle = title;
        this.albumCoverUrl = cover;
    }

    public void SetArtistInformation (int id, String name, String picture) {  //Метод, с помощью которого будем устанавливать информацию об исполнителе
        this.artistId = id;
        this.artistName = name;
        this.artistPictureUrl = picture;
    }
}
