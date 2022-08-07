package bragi.info;

public class TrackInfo {
    /* Информация о самом треке */
    private int trackId;
    private String trackTitle;
    private String trackIdentifier;
    private String source;
    private int trackDuration;

    /* Информация об альбоме этого трека */
    private int albumId;
    private String albumTitle;
    private String albumCoverUrl;

    /* Информация об исполнителе трека */
    private int artistId;
    private String artistName;
    private String artistPictureUrl;

    /* Ссылка на следующий трек в результатах поиск (на случай, если первый в поиске оказался не тем, который хотелось найти) */
    private String nextTrackInSearchResults;
    /* Поисковой запрос, с помощью которого нашли этот трек */
    private String searchRequest;
    /* Общее количество результатов поиска */
    private int totalOfSearchResults;

    //region Методы информации о треке
    public int getTrackId() {
        return this.trackId;
    }
    public void setTrackId(int value) {
        this.trackId = value;
    }

    public String getTrackTitle() {
        return this.trackTitle;
    }
    public void setTrackTitle(String value) {
        this.trackTitle = value;
    }

    public String getTrackIdentifier() {
        return this.trackIdentifier;
    }
    public void setTrackIdentifier(String value) {
        this.trackIdentifier = value;
    }

    public String getSource() {
        return this.source;
    }
    public void setSource(String value) {
        this.source = value;
    }

    /* Метод, с помощью которого мы будем получать длину трека в приемелемом виде */
    public String getTrackDurationFormatted() {
        int minutes = this.trackDuration / 60;
        int seconds = this.trackDuration - minutes * 60;
        return String.format("%dм%dс", minutes, seconds);
    }
    public int getTrackDuration() {
        return this.trackDuration;
    }
    public void setTrackDuration(int value) {
        this.trackDuration = value;
    }
    //endregion
    //region Методы информации об альбоме трека
    public int getAlbumId() {
        return this.albumId;
    }
    public void setAlbumId(int value) {
        this.albumId = value;
    }

    public String getAlbumTitle() {
        return this.albumTitle;
    }
    public void setAlbumTitle(String value) {
        this.albumTitle = value;
    }

    public String getAlbumCoverUrl() {
        return this.albumCoverUrl;
    }
    public void setAlbumCoverUrl(String value) {
        this.albumCoverUrl = value;
    }
    //endregion
    //region Методы информации об исполнителе трека
    public int getArtistId() {
        return this.artistId;
    }
    public void setArtistId(int value) {
        this.artistId = value;
    }

    public String getArtistName() {
        return this.artistName;
    }
    public void setArtistName(String value) {
        this.artistName = value;
    }

    public String getArtistPictureUrl() {
        return this.artistPictureUrl;
    }
    public void setArtistPictureUrl(String value) {
        this.artistPictureUrl = value;
    }
    //endregion
    //region остальные методы
    public String getNextTrackInSearchResults() {
        return this.nextTrackInSearchResults;
    }
    public void setNextTrackInSearchResults(String value) {
        this.nextTrackInSearchResults = value;
    }

    public String getSearchRequest() {
        return this.searchRequest;
    }
    public void setSearchRequest(String value) {
        this.searchRequest = value;
    }

    public int getTotalOfSearchResults() {
        return this.totalOfSearchResults;
    }
    public void setTotalOfSearchResults(int value) {
        this.totalOfSearchResults = value;
    }
    //endregion
}
