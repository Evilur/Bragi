package Bragi;

import Bragi.APIObjectsInfo.TrackInfo;
import Bragi.LavaPlayer.GuildPlayer;

import java.util.ArrayList;

public class Player {
    public static ArrayList<TrackInfo> playlist = new ArrayList<>();
    public static int totalDuration = 0;
    public static GuildPlayer player;  //Это наш проигрыватель
}
