package Bragi;

import Bragi.ObjectsInfo.TrackInfo;
import Bragi.LavaPlayer.GuildPlayer;

import java.util.ArrayList;

public class Player {
    public static ArrayList<TrackInfo> playlist = new ArrayList<>();
    public static int totalDuration = 0;  //Общая продолжительность треков в плейлисте
    public static GuildPlayer player;  //Это наш проигрыватель
    public static boolean loopMode = false;  //Режим повторения
}
