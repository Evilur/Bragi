package bragi.info;

import java.util.ArrayList;

public class AlbumInfo {
    /* Информация об альбоме */
    private int albumId;
    private String albumTitle;
    private String albumCoverUrl;
    private int albumSize;
    private int albumDuration;

    /* Информация об исполнителе */
    private int artistId;
    private String artistName;
    private String artistPictureUrl;

    /* Списко треков альбома */
    private final ArrayList<TrackInfo> albumList = new ArrayList<>();

    //region Методы для дотсупа к полям информации об альбоме
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

    public int getAlbumSize() {
        return this.albumSize;
    }
    public void setAlbumSize(int value) {
        this.albumSize = value;
    }

    public int getAlbumDuration() {
        return this.albumDuration;
    }
    public void setAlbumDuration(int value) {
        this.albumDuration = value;
    }
    //endregion
    //region Методы для доступа к полям ифнормации об исполнителе
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
    //region Остальные методы
    public ArrayList<TrackInfo> getAlbumList() {
        return this.albumList;
    }
    //endregion
}
