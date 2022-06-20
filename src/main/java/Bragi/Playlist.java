package Bragi;

import Bragi.APIObjectsInfo.TrackInfo;
import Bragi.LavaPlayer.Player;
import com.sedmelluq.discord.lavaplayer.track.AudioTrack;

import java.util.ArrayList;

public class Playlist {
    public static ArrayList<TrackInfo> list = new ArrayList<>();
    public static int totalDuration = 0;
    public static Player player;  //Это наш проигрыватель
}
