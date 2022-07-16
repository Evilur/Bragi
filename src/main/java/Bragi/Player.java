package Bragi;

import Bragi.ObjectsInfo.TrackInfo;
import Bragi.LavaPlayer.GuildPlayer;

import java.util.ArrayList;

public class Player {
    public ArrayList<TrackInfo> playlist = new ArrayList<>();
    public int totalDuration = 0;  //Общая продолжительность треков в плейлисте
    public GuildPlayer instance;  //Это наш проигрыватель
    public boolean loopMode = false;  //Режим повторения
}
