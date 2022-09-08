package bragi.core.event;

import bragi.Bragi;
import bragi.core.Player;
import net.dv8tion.jda.api.entities.Guild;

public class SkipTracks {
    public static void run(int numberOfTracks, boolean hardSkip, Guild guild) {  //Метод для пропуска определенного колиичества треков
        Player player = Bragi.Players.get(guild);  //Получаем проигрыватель
        /* Если пользователь хочет пропустить треков больше, чем существует в плейлисте, ограничим его хотения */
        if (numberOfTracks > player.getPlaylist().size())
            numberOfTracks = player.getPlaylist().size();
        else if (numberOfTracks < 1) {  //Если не передано число, присваиваем единицу, то есть убираем один трек
            numberOfTracks = 1;
        }

        /* Если не стоит повторение или трек пропускается вручную */
        if (!player.isLoopMode() || hardSkip) {  //Удаляем элементы
            player.decreaseTotalDuration(player.getPlaylist().get(0).getTrackDuration());  //Уменьшаем общую длину треков
            player.getPlaylist().subList(0, numberOfTracks).clear();  //Удаляем первые элементы из списка
        }

        /* Если в плейлисте есть треки */
        if (player.getPlaylist().size() > 0) {
            String url = player.getPlaylist().get(0).getTrackIdentifier();  //Получаем url трека
            player.getInstance().Play(url);  //Воспроизводим трек
        }
        else {  //Если треков в плейлисте нет
            player.getInstance().Stop();
            LeaveChannel.run(guild.getAudioManager());
        }
    }
}
